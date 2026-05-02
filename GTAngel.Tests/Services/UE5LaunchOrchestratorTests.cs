using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for UE5LaunchOrchestrator — initial state, properties, stage enum,
/// stop when idle, send-command guard when pipe is absent.
/// </summary>
public class UE5LaunchOrchestratorTests : IDisposable
{
    private readonly AppConfiguration _config =
        new(NullLogger<AppConfiguration>.Instance);

    private readonly UE5LaunchOrchestrator _svc;

    public UE5LaunchOrchestratorTests()
    {
        _svc = new UE5LaunchOrchestrator(
            NullLogger<UE5LaunchOrchestrator>.Instance, _config);
    }

    // ── Initial state ─────────────────────────────────────────────────────────

    [Fact]
    public void CurrentStage_Initially_IsIdle()
    {
        Assert.Equal(UE5LaunchStage.Idle, _svc.CurrentStage);
    }

    [Fact]
    public void IsReady_Initially_IsFalse()
    {
        Assert.False(_svc.IsReady);
    }

    [Fact]
    public void UEProcess_Initially_IsNull()
    {
        Assert.Null(_svc.UEProcess);
    }

    [Fact]
    public void EnginePath_ReflectsConfigPath()
    {
        Assert.False(string.IsNullOrEmpty(_svc.EnginePath));
    }

    // ── Stage enum values ─────────────────────────────────────────────────────

    [Fact]
    public void Stages_AreDistinctIntegers()
    {
        var stages = Enum.GetValues<UE5LaunchStage>();
        var distinct = stages.Distinct().ToArray();
        Assert.Equal(stages.Length, distinct.Length);
    }

    [Fact]
    public void UE5LaunchStage_Ready_Exists()
    {
        Assert.True(Enum.IsDefined(typeof(UE5LaunchStage), UE5LaunchStage.Ready));
    }

    [Fact]
    public void UE5LaunchStage_Failed_Exists()
    {
        Assert.True(Enum.IsDefined(typeof(UE5LaunchStage), UE5LaunchStage.Failed));
    }

    // ── Stop when idle ────────────────────────────────────────────────────────

    [Fact]
    public void Stop_WhenIdle_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.Stop());
        Assert.Null(ex);
    }

    [Fact]
    public void Stop_WhenIdle_StageRemainsIdle()
    {
        _svc.Stop();
        Assert.Equal(UE5LaunchStage.Idle, _svc.CurrentStage);
    }

    // ── SendCommandAsync when pipe is not connected ───────────────────────────

    [Fact]
    public async Task SendCommandAsync_WhenPipeNotConnected_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() =>
            _svc.SendCommandAsync("PING", "{}"));
        Assert.Null(ex);
    }

    [Fact]
    public async Task SendCommandAsync_WithExtra_WhenPipeNotConnected_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() =>
            _svc.SendCommandAsync("CMD", "{}", "extra"));
        Assert.Null(ex);
    }

    // ── LaunchAsync fails when engine path absent ─────────────────────────────

    [Fact]
    public async Task LaunchAsync_WhenEnginePathAbsent_ReturnsFailure()
    {
        // The default engine path does not exist on a CI machine, so
        // the Validating stage should fail quickly.
        using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(5));
        var result = await _svc.LaunchAsync(cts.Token);

        Assert.False(result.Success);
    }

    [Fact]
    public async Task LaunchAsync_WhenEnginePathAbsent_FailsAtValidatingStage()
    {
        using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(5));
        var result = await _svc.LaunchAsync(cts.Token);

        Assert.Equal(UE5LaunchStage.Validating, result.FailedAtStage);
    }

    [Fact]
    public async Task LaunchAsync_ReturnsResultWithElapsedTime()
    {
        using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(5));
        var result = await _svc.LaunchAsync(cts.Token);

        Assert.True(result.Duration >= TimeSpan.Zero);
    }

    [Fact]
    public async Task LaunchAsync_CanBeCancelledExternally()
    {
        using var cts = new CancellationTokenSource();
        cts.CancelAfter(10); // Cancel almost immediately

        var result = await _svc.LaunchAsync(cts.Token);

        // Either cancelled or validation failed — both are acceptable
        Assert.False(result.Success);
    }

    // ── Events ────────────────────────────────────────────────────────────────

    [Fact]
    public async Task LaunchAsync_FiresOnStageChangedEvent()
    {
        var stages = new List<UE5LaunchStage>();
        _svc.OnStageChanged += (s, _) => stages.Add(s);

        using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(5));
        await _svc.LaunchAsync(cts.Token);

        Assert.NotEmpty(stages);
    }

    [Fact]
    public async Task LaunchAsync_FiresOnLaunchCompleteEvent()
    {
        UE5LaunchResult? completedResult = null;
        _svc.OnLaunchComplete += r => completedResult = r;

        using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(5));
        await _svc.LaunchAsync(cts.Token);

        // OnLaunchComplete is fired on failure too — confirm it fired
        Assert.NotNull(completedResult);
    }

    // ── UE5LaunchResult record ────────────────────────────────────────────────

    [Fact]
    public void UE5LaunchResult_Can_BeConstructed()
    {
        var result = new UE5LaunchResult(true, UE5LaunchStage.Ready, "OK", TimeSpan.FromSeconds(1));
        Assert.True(result.Success);
        Assert.Equal(UE5LaunchStage.Ready, result.FailedAtStage);
        Assert.Equal("OK", result.Message);
    }

    // ── Dispose ───────────────────────────────────────────────────────────────

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var config2 = new AppConfiguration(NullLogger<AppConfiguration>.Instance);
        var svc2 = new UE5LaunchOrchestrator(
            NullLogger<UE5LaunchOrchestrator>.Instance, config2);
        var ex = Record.Exception(() => svc2.Dispose());
        Assert.Null(ex);
    }

    public void Dispose() => _svc.Dispose();
}
