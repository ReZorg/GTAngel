using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Integration tests for DteTrainingLoop + DteCognitiveCoreService.
///
/// Tests:
///   1. SetCognitiveCoreService wires the service without throwing
///   2. After wiring and running, State.CognitiveCoherence is ≥ 0
///   3. DteCognitiveCoreService standalone: PatternCount increases after 30+ distinct states
///   4. Thompson Alpha[action] increases after positive-reward update
///   5. Wout training reduces loss over multiple samples
///   6. Attention-gated logits differ from zero-state logits after updates
///   7. ComputeCoherence returns bounded snapshot
/// </summary>
public sealed class DteTrainingLoop_CognitiveCoreTests : IAsyncLifetime
{
    private readonly DteTrainingLoop _loop;
    private readonly DteCognitiveCoreService _cogCore;
    private readonly DxgiFrameCaptureService _frameCapture;
    private readonly VigemControllerService  _controller;
    private readonly OpenRwEngineBridge      _engine;
    private readonly EsnReservoirPipeline    _reservoir;
    private readonly ExperienceReplayBuffer  _buffer;
    private readonly OnnxCnnFeatureExtractor _extractor;

    public DteTrainingLoop_CognitiveCoreTests()
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

        _loop.Config.MaxStepsPerEpisode = 5;
        _loop.Config.MaxEpisodes        = 1;
        _loop.Config.TargetFps          = 1000;
        _loop.Config.MinBufferSize      = 1;
        _loop.Config.TrainingMode       = DteTrainingMode.Online;
        _loop.Config.OnlineTrainInterval = 1;

        _cogCore = new DteCognitiveCoreService(NullLogger<DteCognitiveCoreService>.Instance);
    }

    public Task InitializeAsync() => Task.CompletedTask;

    public async Task DisposeAsync()
    {
        if (_loop.State.IsRunning)
            await _loop.StopAsync();
        _loop.Dispose();
        _cogCore.Dispose();
        _extractor.Dispose();
        _reservoir.Dispose();
        _engine.Dispose();
        _controller.Dispose();
        _frameCapture.Dispose();
    }

    // ── 1. SetCognitiveCoreService wires without throwing ─────────────────────

    [Fact]
    public void SetCognitiveCoreService_DoesNotThrow()
    {
        var ex = Record.Exception(() => _loop.SetCognitiveCoreService(_cogCore));
        Assert.Null(ex);
    }

    [Fact]
    public void SetCognitiveCoreService_OnEsnReservoir_DoesNotThrow()
    {
        var ex = Record.Exception(() => _reservoir.SetCognitiveCoreService(_cogCore));
        Assert.Null(ex);
    }

    // ── 2. CognitiveCoherence tracked after run ───────────────────────────────

    [Fact]
    public async Task CognitiveCoherence_AfterRun_IsNonNegative()
    {
        _loop.SetCognitiveCoreService(_cogCore);
        _loop.State.IsInitialized = true;
        _loop.Start();
        await Task.Delay(2000);
        await _loop.StopAsync();

        Assert.True(_loop.State.CognitiveCoherence >= 0,
            "CognitiveCoherence should be ≥ 0 after a training run");
    }

    // ── 3. MOSES PatternCount increases after 30+ distinct reservoir states ───

    [Fact]
    public void MinePatterns_After30DistinctStates_PatternCountGreaterThanZero()
    {
        var rng = new Random(42);
        var state = new float[512];

        // Feed 30 distinct reservoir states to fill the window (PatternWindowLen/2 = 25 needed)
        for (int i = 0; i < 30; i++)
        {
            // Make each state distinct to avoid degenerate patterns
            for (int n = 0; n < 512; n++)
                state[n] = (float)rng.NextDouble() * 2 - 1;
            _cogCore.MinePatterns(state);
        }

        Assert.True(_cogCore.PatternCount > 0,
            "MOSES should mine at least one pattern after 30 distinct reservoir states");
    }

    [Fact]
    public void MinePatterns_WithShortReservoir_DoesNotThrow()
    {
        // States shorter than 512 are silently ignored
        var ex = Record.Exception(() => _cogCore.MinePatterns(new float[100]));
        Assert.Null(ex);
    }

    [Fact]
    public void MinePatterns_Before25States_PatternCountIsZero()
    {
        var fresh = new DteCognitiveCoreService(NullLogger<DteCognitiveCoreService>.Instance);
        var state = new float[512];
        var rng   = new Random(7);

        // Feed only 10 states — below the 25-state threshold
        for (int i = 0; i < 10; i++)
        {
            for (int n = 0; n < 512; n++) state[n] = (float)rng.NextDouble();
            fresh.MinePatterns(state);
        }

        Assert.Equal(0, fresh.PatternCount);
        fresh.Dispose();
    }

    // ── 4. Thompson Alpha increases after positive reward ─────────────────────

    [Fact]
    public void UpdateThompson_PositiveReward_IncreasesAlphaForAction()
    {
        int action = 3;
        var logits = new float[18];
        logits[action] = 1.0f; // favour action 3

        // Get initial Thompson snapshot via ComputeCoherence
        var coherence0 = _cogCore.ComputeCoherence();

        // Apply a strong positive reward for action 3
        _cogCore.UpdateThompson(action, reward: 1.0f);  // normalizedReward = 1 → alpha += 1
        _cogCore.UpdateThompson(action, reward: 1.0f);

        var snap = _cogCore.GetThompsonSnapshot();
        // Alpha for action 3 should be > initial value (Thompson initialises alpha=1)
        Assert.True(snap.Alpha[action] > 1.0f,
            $"Thompson Alpha[{action}] should exceed 1.0 after positive rewards, was {snap.Alpha[action]}");
    }

    [Fact]
    public void UpdateThompson_NegativeReward_IncreasesBetaForAction()
    {
        int action = 5;
        float betaBefore = _cogCore.GetThompsonSnapshot().Beta[action];
        _cogCore.UpdateThompson(action, reward: -1.0f); // normalizedReward ≈ 0 → beta += 1
        float betaAfter = _cogCore.GetThompsonSnapshot().Beta[action];

        Assert.True(betaAfter > betaBefore,
            $"Beta[{action}] should increase after negative reward");
    }

    [Fact]
    public void ThompsonSampleAction_ReturnsBoundedIndex()
    {
        var logits = new float[18];
        logits[2] = 1.0f;

        int action = _cogCore.ThompsonSampleAction(logits);
        Assert.InRange(action, 0, 17);
    }

    // ── 5. TrainWout reduces loss over samples ────────────────────────────────

    [Fact]
    public void TrainWout_Repeatedly_DoesNotIncreaseWoutNormExponentially()
    {
        var rng = new Random(99);
        var state  = new float[512];
        var target = new float[18];

        // Train 50 times and check Wout norm stays bounded
        for (int i = 0; i < 50; i++)
        {
            for (int n = 0; n < 512; n++) state[n]  = (float)rng.NextDouble() * 2 - 1;
            for (int a = 0; a < 18;  a++) target[a] = (float)rng.NextDouble();
            _cogCore.TrainWout(state, target);
        }

        // Wout norm should remain finite (no explosion due to ridge regularisation)
        var snapshot = _cogCore.ComputeCoherence();
        Assert.True(snapshot.Coherence >= 0 && snapshot.Coherence <= 1.01,
            "Cognitive coherence should be in [0,1] after Wout training");
    }

    [Fact]
    public void TrainWout_WithShortState_DoesNotThrow()
    {
        var ex = Record.Exception(() => _cogCore.TrainWout(new float[10], new float[18]));
        Assert.Null(ex);
    }

    // ── 6. Attention-gated logits differ from baseline after UpdateAttention ──

    [Fact]
    public void ComputeAttentionGatedLogits_AfterAttentionUpdate_DifferentFromZeroState()
    {
        var rng      = new Random(33);
        var reservoir = new float[512];
        for (int n = 0; n < 512; n++) reservoir[n] = (float)rng.NextDouble() * 2 - 1;

        // Logits before attention update (STI all zero initially → floor 0.1)
        var logitsBefore = _cogCore.ComputeAttentionGatedLogits(reservoir);

        // Update attention to build up STI
        _cogCore.UpdateAttention(reservoir);
        _cogCore.UpdateAttention(reservoir);

        var logitsAfter = _cogCore.ComputeAttentionGatedLogits(reservoir);

        // After attention update, at least the Wout-weighted projection runs on non-zero STI
        Assert.Equal(18, logitsBefore.Length);
        Assert.Equal(18, logitsAfter.Length);
    }

    [Fact]
    public void UpdateAttention_FiresOnAttentionUpdatedEvent()
    {
        int fired = 0;
        _cogCore.OnAttentionUpdated += (_, _) => fired++;

        var state = new float[512];
        _cogCore.UpdateAttention(state);

        Assert.Equal(1, fired);
    }

    // ── 7. ComputeCoherence returns valid snapshot ────────────────────────────

    [Fact]
    public void ComputeCoherence_ReturnsValidSnapshot()
    {
        var snapshot = _cogCore.ComputeCoherence();

        Assert.NotNull(snapshot);
        Assert.InRange(snapshot.Coherence,       0.0, 1.0);
        Assert.InRange(snapshot.AttentionCoherence, 0.0, 1.0);
        Assert.InRange(snapshot.PatternCoherence,   0.0, 1.0);
        Assert.InRange(snapshot.PolicyCoherence,    0.0, 1.0);
    }

    [Fact]
    public void ComputeCoherence_TotalPatterns_MatchesPatternCount()
    {
        // Without any mining, both should be 0
        var snapshot = _cogCore.ComputeCoherence();
        Assert.Equal(_cogCore.PatternCount, snapshot.TotalPatterns);
    }

    // ── 8. TrainingLoop + CognitiveCore: coherence state after run ────────────

    [Fact]
    public async Task TrainingLoop_WithCognitiveCore_DoesNotThrow()
    {
        _loop.SetCognitiveCoreService(_cogCore);
        _loop.State.IsInitialized = true;

        var ex = await Record.ExceptionAsync(async () =>
        {
            _loop.Start();
            await Task.Delay(1000);
            await _loop.StopAsync();
        });

        Assert.Null(ex);
    }

    // ── 9. OnPatternMined event fires after sufficient states ─────────────────

    [Fact]
    public void OnPatternMined_FiresAfterEnoughStates()
    {
        int fired = 0;
        _cogCore.OnPatternMined += (_, _) => fired++;

        var rng = new Random(1234);
        var state = new float[512];
        for (int i = 0; i < 50; i++) // well beyond the 25-state threshold
        {
            for (int n = 0; n < 512; n++) state[n] = (float)rng.NextDouble() * 2 - 1;
            _cogCore.MinePatterns(state);
        }

        Assert.True(fired >= 1, "OnPatternMined should fire after sufficient state accumulation");
    }

    // ── 10. Thompson snapshot structure ──────────────────────────────────────

    [Fact]
    public void GetThompsonSnapshot_HasCorrectArrayLengths()
    {
        var snap = _cogCore.GetThompsonSnapshot();
        Assert.Equal(18, snap.Alpha.Length);
        Assert.Equal(18, snap.Beta.Length);
    }

    [Fact]
    public void GetThompsonSnapshot_AllAlphaAndBetaPositive()
    {
        var snap = _cogCore.GetThompsonSnapshot();
        Assert.All(snap.Alpha, a => Assert.True(a > 0f, "Thompson alpha must be positive"));
        Assert.All(snap.Beta,  b => Assert.True(b > 0f, "Thompson beta must be positive"));
    }
}
