using GTAngel.Services;
using GTAngel.Models;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for GTAngelService — safety thresholds, state management, start/stop, events.
/// </summary>
public class GTAngelServiceTests : IDisposable
{
    private readonly GTAngelService _svc;

    public GTAngelServiceTests()
    {
        var engine = new TrainingEngine();
        _svc = new GTAngelService(NullLogger<GTAngelService>.Instance, engine);
    }

    // ── Constants ─────────────────────────────────────────────────────────────

    [Fact]
    public void CoherenceHaltThreshold_Is_0_15()
    {
        Assert.Equal(0.15, GTAngelService.CoherenceHaltThreshold);
    }

    [Fact]
    public void PropertyCoherenceMinimum_Is_0_60()
    {
        Assert.Equal(0.60, GTAngelService.PropertyCoherenceMinimum);
    }

    [Fact]
    public void MaxParameterDelta_Is_0_20()
    {
        Assert.Equal(0.20, GTAngelService.MaxParameterDelta);
    }

    // ── Initial state ─────────────────────────────────────────────────────────

    [Fact]
    public void State_Initially_IsNotRunning()
    {
        Assert.False(_svc.State.IsRunning);
    }

    [Fact]
    public void State_Initially_IsNotPaused()
    {
        Assert.False(_svc.State.IsPaused);
    }

    [Fact]
    public void State_Initially_IsNotHalted()
    {
        Assert.False(_svc.State.IsHalted);
    }

    [Fact]
    public void State_Initially_PhaseIsIdle()
    {
        Assert.Equal(GTAngelPhase.Idle, _svc.State.Phase);
    }

    [Fact]
    public void State_Initially_TotalExperimentsIsZero()
    {
        Assert.Equal(0, _svc.State.TotalExperiments);
    }

    [Fact]
    public void State_Initially_BestMetricIsZero()
    {
        Assert.Equal(0.0, _svc.State.BestMetric);
    }

    [Fact]
    public void State_KeepRatio_InitiallyZero()
    {
        Assert.Equal(0.0, _svc.State.KeepRatio);
    }

    // ── TogglePause ───────────────────────────────────────────────────────────

    [Fact]
    public void TogglePause_TogglesIsPaused()
    {
        bool before = _svc.State.IsPaused;
        _svc.TogglePause();
        Assert.NotEqual(before, _svc.State.IsPaused);
    }

    [Fact]
    public void TogglePause_Twice_RestoresOriginalState()
    {
        bool original = _svc.State.IsPaused;
        _svc.TogglePause();
        _svc.TogglePause();
        Assert.Equal(original, _svc.State.IsPaused);
    }

    // ── Start/Stop ────────────────────────────────────────────────────────────

    [Fact]
    public async Task StartAsync_SetsIsRunning()
    {
        var config = new GTAngelConfig { MaxExperiments = 1 };
        await _svc.StartAsync(config);

        Assert.True(_svc.State.IsRunning);

        await _svc.StopAsync();
    }

    [Fact]
    public async Task StartAsync_WhileAlreadyRunning_DoesNotThrow()
    {
        var config = new GTAngelConfig { MaxExperiments = 1 };
        await _svc.StartAsync(config);

        var ex = await Record.ExceptionAsync(() => _svc.StartAsync(config));
        Assert.Null(ex);

        await _svc.StopAsync();
    }

    [Fact]
    public async Task StopAsync_WhenNotRunning_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() => _svc.StopAsync());
        Assert.Null(ex);
    }

    [Fact]
    public async Task StopAsync_AfterStart_SetsIsRunningFalse()
    {
        var config = new GTAngelConfig { MaxExperiments = 1 };
        await _svc.StartAsync(config);
        await _svc.StopAsync();

        Assert.False(_svc.State.IsRunning);
    }

    // ── Events ────────────────────────────────────────────────────────────────

    [Fact]
    public async Task StartAsync_FiresOnStateUpdated()
    {
        bool fired = false;
        _svc.OnStateUpdated += _ => fired = true;

        var config = new GTAngelConfig { MaxExperiments = 1 };
        await _svc.StartAsync(config);
        await _svc.StopAsync();

        Assert.True(fired);
    }

    [Fact]
    public void TogglePause_FiresOnStateUpdated()
    {
        bool fired = false;
        _svc.OnStateUpdated += _ => fired = true;
        _svc.TogglePause();
        Assert.True(fired);
    }

    // ── GTAngelState helpers ──────────────────────────────────────────────────

    [Fact]
    public void State_KeepRatio_CalculatedCorrectly()
    {
        _svc.State.TotalExperiments = 10;
        _svc.State.KeptExperiments = 6;
        Assert.Equal(0.6, _svc.State.KeepRatio, precision: 10);
    }

    [Fact]
    public void State_Elapsed_IsZeroWhenNotRunning()
    {
        Assert.Equal(TimeSpan.Zero, _svc.State.Elapsed);
    }

    [Fact]
    public void State_RecentLogs_InitiallyEmpty()
    {
        Assert.Empty(_svc.State.RecentLogs);
    }

    // ── Dispose ───────────────────────────────────────────────────────────────

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var engine2 = new TrainingEngine();
        var svc2 = new GTAngelService(NullLogger<GTAngelService>.Instance, engine2);
        var ex = Record.Exception(() => svc2.Dispose());
        Assert.Null(ex);
    }

    public void Dispose() => _svc.Dispose();
}
