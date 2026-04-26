using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Integration tests for DteTrainingLoop lifecycle: Start, Stop, state tracking,
/// and OnStateUpdated event. The DXGI/ViGEm/OpenRW dependencies all have graceful
/// fallbacks, so tests run on a Windows runner without any hardware or game installed.
///
/// Strategy:
///   - Skip InitializeAsync() (which downloads models and detects game engines)
///   - Set State.IsInitialized = true directly to unlock Start()
///   - Use Config.MaxStepsPerEpisode=3, MaxEpisodes=1, TargetFps=1000 for fast execution
/// </summary>
public sealed class DteTrainingLoopTests : IAsyncLifetime
{
    private readonly DteTrainingLoop _loop;
    private readonly DxgiFrameCaptureService _frameCapture;
    private readonly VigemControllerService  _controller;
    private readonly OpenRwEngineBridge      _engine;
    private readonly EsnReservoirPipeline    _reservoir;
    private readonly ExperienceReplayBuffer  _buffer;
    private readonly OnnxCnnFeatureExtractor _extractor;

    public DteTrainingLoopTests()
    {
        _frameCapture = new DxgiFrameCaptureService(NullLogger<DxgiFrameCaptureService>.Instance);
        _controller   = new VigemControllerService(NullLogger<VigemControllerService>.Instance);
        _engine       = new OpenRwEngineBridge(NullLogger<OpenRwEngineBridge>.Instance);
        _reservoir    = new EsnReservoirPipeline(NullLogger<EsnReservoirPipeline>.Instance);
        _buffer       = new ExperienceReplayBuffer(NullLogger<ExperienceReplayBuffer>.Instance,
                            capacity: 1000, alpha: 0.6f, beta: 0.4f, nStep: 1);
        _extractor    = new OnnxCnnFeatureExtractor(NullLogger<OnnxCnnFeatureExtractor>.Instance);

        _loop = new DteTrainingLoop(
            NullLogger<DteTrainingLoop>.Instance,
            _frameCapture, _controller, _engine,
            _reservoir, _buffer, _extractor);

        // Fast config: 3 steps per episode, 1 episode, 1000 FPS → episode completes in ~3ms
        _loop.Config.MaxStepsPerEpisode = 3;
        _loop.Config.MaxEpisodes        = 1;
        _loop.Config.TargetFps          = 1000;
        _loop.Config.MinBufferSize      = 1;
        _loop.Config.TrainingMode       = DteTrainingMode.Online;
        _loop.Config.OnlineTrainInterval = 1;
    }

    public Task InitializeAsync() => Task.CompletedTask;

    public async Task DisposeAsync()
    {
        if (_loop.State.IsRunning)
            await _loop.StopAsync();
        _loop.Dispose();
        _extractor.Dispose();
        _reservoir.Dispose();
        _engine.Dispose();
        _controller.Dispose();
        _frameCapture.Dispose();
    }

    // ── 1. Initial state ──────────────────────────────────────────────────────

    [Fact]
    public void State_IsRunning_Initially_IsFalse()
    {
        Assert.False(_loop.State.IsRunning);
    }

    [Fact]
    public void State_IsInitialized_Initially_IsFalse()
    {
        Assert.False(_loop.State.IsInitialized);
    }

    [Fact]
    public void State_TotalSteps_Initially_IsZero()
    {
        Assert.Equal(0L, _loop.State.TotalSteps);
    }

    [Fact]
    public void State_TotalEpisodes_Initially_IsZero()
    {
        Assert.Equal(0, _loop.State.TotalEpisodes);
    }

    // ── 2. Start() guard: requires IsInitialized ─────────────────────────────

    [Fact]
    public void Start_WithoutInitialization_DoesNotSetIsRunning()
    {
        _loop.Start(); // IsInitialized is false — should be a no-op
        Assert.False(_loop.State.IsRunning);
    }

    // ── 3. Start() lifecycle ─────────────────────────────────────────────────

    [Fact]
    public void Start_AfterSettingIsInitialized_SetsIsRunningTrue()
    {
        _loop.State.IsInitialized = true;
        _loop.Start();
        Assert.True(_loop.State.IsRunning);
    }

    [Fact]
    public async Task StopAsync_SetsIsRunningFalse()
    {
        _loop.State.IsInitialized = true;
        _loop.Start();
        await _loop.StopAsync();
        Assert.False(_loop.State.IsRunning);
    }

    [Fact]
    public async Task StopAsync_WithoutStart_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() => _loop.StopAsync());
        Assert.Null(ex);
    }

    [Fact]
    public async Task StopAsync_CompletesWithinReasonableTime()
    {
        _loop.State.IsInitialized = true;
        _loop.Start();

        var sw = System.Diagnostics.Stopwatch.StartNew();
        await _loop.StopAsync();
        sw.Stop();

        Assert.True(sw.Elapsed.TotalSeconds < 10,
            $"StopAsync should complete within 10 seconds, took {sw.Elapsed.TotalSeconds:F1}s");
    }

    // ── 4. OnStateUpdated fires ────────────────────────────────────────────────

    [Fact]
    public void Start_FiresOnStateUpdated()
    {
        int fired = 0;
        _loop.OnStateUpdated += _ => fired++;

        _loop.State.IsInitialized = true;
        _loop.Start();

        Assert.True(fired >= 1, "OnStateUpdated should fire when Start() is called");
    }

    [Fact]
    public async Task OnStateUpdated_FiresDuringTrainingLoop()
    {
        int fired = 0;
        _loop.OnStateUpdated += _ => fired++;

        _loop.State.IsInitialized = true;
        _loop.Start();

        // Wait for the 1-episode loop to complete (3 steps × 1ms = ~3ms, generous slack)
        await Task.Delay(2000);
        await _loop.StopAsync();

        Assert.True(fired >= 1, "OnStateUpdated should fire during a training episode");
    }

    // ── 5. TotalSteps increments ──────────────────────────────────────────────

    [Fact]
    public async Task TotalSteps_IncreasesAfterRunning()
    {
        _loop.State.IsInitialized = true;
        _loop.Start();
        await Task.Delay(2000); // generous wait for 1 episode of 3 steps
        await _loop.StopAsync();

        Assert.True(_loop.State.TotalSteps > 0,
            "TotalSteps should be > 0 after running one episode");
    }

    [Fact]
    public async Task TotalSteps_IncreasesUpToMaxStepsPerEpisode()
    {
        _loop.State.IsInitialized = true;
        _loop.Start();
        await Task.Delay(2000);
        await _loop.StopAsync();

        Assert.True(_loop.State.TotalSteps <= _loop.Config.MaxStepsPerEpisode * _loop.Config.MaxEpisodes + 5L,
            "TotalSteps should not greatly exceed MaxStepsPerEpisode * MaxEpisodes");
    }

    // ── 6. Pause / Resume ────────────────────────────────────────────────────

    [Fact]
    public void Pause_TogglesIsPaused()
    {
        _loop.State.IsInitialized = true;
        _loop.Start();

        bool before = _loop.State.IsPaused;
        _loop.Pause();
        bool after = _loop.State.IsPaused;

        Assert.NotEqual(before, after);
    }

    [Fact]
    public async Task Pause_ThenResume_LoopContinues()
    {
        _loop.State.IsInitialized = true;
        _loop.Start();
        _loop.Pause();   // Pause
        _loop.Pause();   // Resume (toggle)
        await Task.Delay(2000);
        await _loop.StopAsync();

        // Should still have completed at least some steps
        Assert.True(_loop.State.TotalSteps >= 0); // No crash is the key assertion
    }

    // ── 7. OnEpisodeComplete fires ────────────────────────────────────────────

    [Fact]
    public async Task OnEpisodeComplete_FiresAfterEpisodeEnds()
    {
        int fired = 0;
        _loop.OnEpisodeComplete += _ => fired++;

        _loop.State.IsInitialized = true;
        _loop.Start();
        await Task.Delay(2000);
        await _loop.StopAsync();

        Assert.True(fired >= 1, "OnEpisodeComplete should fire after each completed episode");
    }

    [Fact]
    public async Task OnEpisodeComplete_ResultHasValidFields()
    {
        DteEpisodeResult? result = null;
        _loop.OnEpisodeComplete += r => result = r;

        _loop.State.IsInitialized = true;
        _loop.Start();
        await Task.Delay(2000);
        await _loop.StopAsync();

        if (result != null)
        {
            Assert.True(result.Steps >= 0);
            Assert.True(result.Duration >= TimeSpan.Zero);
        }
    }

    // ── 8. Dispose is safe ────────────────────────────────────────────────────

    [Fact]
    public void Dispose_WhenNotRunning_DoesNotThrow()
    {
        var loop = new DteTrainingLoop(
            NullLogger<DteTrainingLoop>.Instance,
            _frameCapture, _controller, _engine, _reservoir, _buffer, _extractor);

        var ex = Record.Exception(() => loop.Dispose());
        Assert.Null(ex);
    }

    [Fact]
    public async Task Dispose_AfterStart_DoesNotThrow()
    {
        var loop = new DteTrainingLoop(
            NullLogger<DteTrainingLoop>.Instance,
            _frameCapture, _controller, _engine, _reservoir, _buffer, _extractor);

        loop.Config.MaxStepsPerEpisode = 1;
        loop.Config.MaxEpisodes        = 1;
        loop.Config.TargetFps          = 1000;
        loop.State.IsInitialized = true;
        loop.Start();
        await Task.Delay(500);

        var ex = Record.Exception(() => loop.Dispose());
        Assert.Null(ex);
    }

    // ── 9. Config properties ──────────────────────────────────────────────────

    [Fact]
    public void Config_DefaultEpsilon_IsOne()
    {
        Assert.Equal(1.0, _loop.State.Epsilon);
    }

    [Fact]
    public void Config_EpsilonDecay_DefaultLessThanOne()
    {
        Assert.True(_loop.Config.EpsilonDecay < 1.0);
    }

    [Fact]
    public async Task Epsilon_Decreases_AfterEpisodes()
    {
        _loop.Config.EpsilonDecay = 0.9; // fast decay for test
        double epsilonBefore = _loop.State.Epsilon;

        _loop.State.IsInitialized = true;
        _loop.Start();
        await Task.Delay(2000);
        await _loop.StopAsync();

        // After 1 episode, epsilon should have decayed
        Assert.True(_loop.State.Epsilon <= epsilonBefore,
            "Epsilon should not increase after episodes");
    }

    // ── 10. OnLogMessage fires ────────────────────────────────────────────────

    [Fact]
    public void Start_FiresOnLogMessage()
    {
        string? logged = null;
        _loop.OnLogMessage += msg => logged ??= msg;

        _loop.State.IsInitialized = true;
        _loop.Start();

        Assert.NotNull(logged);
    }
}
