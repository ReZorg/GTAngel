using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// 768-frame-focused tests for EsnReservoirPipeline — additive to the existing
/// EsnReservoirPipelineTests suite. Exercises the full visual perception pathway
/// with a real 768×768×3 input and validates the embodied-cognition property that
/// proprioceptive state (game state) influences the reservoir independently of
/// the visual input.
///
/// Covered:
///   1. ProcessStep with a 768×768×3 = 1,769,472-element frame produces 18 action probs
///   2. LastReservoirState.Length == 512 (executive layer) after a 768-frame step
///   3. GetFullState().Length == 896 (128+256+512) after a 768-frame step
///   4. Same 768 frame + different 22-dim game states → different reservoir states
///   5. All action probs are in [0,1] and sum to ≈ 1 (Softmax output)
///   6. Reset after 768-frame processing clears state correctly
///   7. Multiple consecutive 768-frame steps keep reservoir stable (no NaN/Inf)
///   8. ProcessStep overload with integer previous action works with 768-frame
/// </summary>
public sealed class EsnReservoirPipeline_768FrameTests : IDisposable
{
    private readonly EsnReservoirPipeline _esn;

    public EsnReservoirPipeline_768FrameTests()
    {
        _esn = new EsnReservoirPipeline(NullLogger<EsnReservoirPipeline>.Instance);
        _esn.Initialize();
    }

    public void Dispose() => _esn.Dispose();

    private const int FrameSize = 768 * 768 * 3; // 1,769,472

    private static float[] MakeFrame(int seed = 0)
    {
        var frame = new float[FrameSize];
        var rng   = new Random(seed);
        for (int i = 0; i < FrameSize; i++)
            frame[i] = (float)rng.NextDouble();
        return frame;
    }

    private static float[] MakeGameState(float x = 0, float y = 0, float health = 100)
    {
        var s = new float[22];
        s[0] = x / 3000f;
        s[1] = y / 3000f;
        s[7] = health / 100f;
        return s;
    }

    private static float[] MakePrevAction(int actionIdx = 0)
    {
        var a = new float[18];
        if (actionIdx >= 0 && actionIdx < 18) a[actionIdx] = 1f;
        return a;
    }

    // ── 1. 768-frame → 18 action probabilities ────────────────────────────────

    [Fact]
    public void ProcessStep_With768Frame_Returns18ActionProbabilities()
    {
        var frame = MakeFrame(seed: 1);
        var probs = _esn.ProcessStep(frame, MakeGameState(), MakePrevAction());
        Assert.Equal(18, probs.Length);
    }

    [Fact]
    public void ProcessStep_With768Frame_ActionProbsAllInUnitRange()
    {
        var probs = _esn.ProcessStep(MakeFrame(seed: 2), MakeGameState(), MakePrevAction());
        Assert.All(probs, p => Assert.InRange(p, 0f, 1f));
    }

    [Fact]
    public void ProcessStep_With768Frame_ActionProbsSumToApproximatelyOne()
    {
        var probs = _esn.ProcessStep(MakeFrame(seed: 3), MakeGameState(), MakePrevAction());
        float sum = probs.Sum();
        Assert.True(Math.Abs(sum - 1f) < 0.01f,
            $"Softmax probabilities should sum to ~1, actual sum = {sum}");
    }

    // ── 2. LastReservoirState size after 768-frame step ───────────────────────

    [Fact]
    public void LastReservoirState_After768FrameStep_IsSize512()
    {
        _esn.ProcessStep(MakeFrame(seed: 4), MakeGameState(), MakePrevAction());
        Assert.Equal(512, _esn.LastReservoirState.Length);
    }

    [Fact]
    public void LastReservoirState_After768FrameStep_ContainsNoNaN()
    {
        _esn.ProcessStep(MakeFrame(seed: 5), MakeGameState(), MakePrevAction());
        Assert.All(_esn.LastReservoirState, v => Assert.False(float.IsNaN(v)));
    }

    [Fact]
    public void LastReservoirState_After768FrameStep_ContainsNoInfinity()
    {
        _esn.ProcessStep(MakeFrame(seed: 6), MakeGameState(), MakePrevAction());
        Assert.All(_esn.LastReservoirState, v => Assert.False(float.IsInfinity(v)));
    }

    // ── 3. GetFullState size after 768-frame step ─────────────────────────────

    [Fact]
    public void GetFullState_After768FrameStep_Is896Elements()
    {
        // Executive(512) + Cognitive(256) + Sensory(128) = 896
        _esn.ProcessStep(MakeFrame(seed: 7), MakeGameState(), MakePrevAction());
        Assert.Equal(896, _esn.GetFullState().Length);
    }

    [Fact]
    public void GetFullState_After768FrameStep_ContainsNoNaN()
    {
        _esn.ProcessStep(MakeFrame(seed: 8), MakeGameState(), MakePrevAction());
        Assert.All(_esn.GetFullState(), v => Assert.False(float.IsNaN(v)));
    }

    // ── 4. Proprioceptive differentiation ────────────────────────────────────

    [Fact]
    public void ProcessStep_SameFrame_DifferentGameStates_ProduceDifferentReservoirStates()
    {
        var frame     = MakeFrame(seed: 10); // same visual input
        var prevAction = MakePrevAction();

        // State A: player at origin, full health
        _esn.Reset();
        _esn.ProcessStep(frame, MakeGameState(x: 0, y: 0, health: 100), prevAction);
        var stateA = _esn.LastReservoirState.ToArray();

        // State B: player far away, low health
        _esn.Reset();
        _esn.ProcessStep(frame, MakeGameState(x: 2000, y: 1500, health: 20), prevAction);
        var stateB = _esn.LastReservoirState.ToArray();

        // The reservoir must be sensitive to proprioceptive input (game state)
        bool anyDifferent = stateA.Zip(stateB, (a, b) => Math.Abs(a - b) > 1e-6f).Any(d => d);
        Assert.True(anyDifferent,
            "Same 768-frame with different game states must produce different reservoir states");
    }

    [Fact]
    public void ProcessStep_DifferentFrames_SameGameState_ProduceDifferentReservoirStates()
    {
        var gameState  = MakeGameState(x: -500, y: 200);
        var prevAction = MakePrevAction(actionIdx: 1);

        _esn.Reset();
        _esn.ProcessStep(MakeFrame(seed: 100), gameState, prevAction);
        var stateA = _esn.LastReservoirState.ToArray();

        _esn.Reset();
        _esn.ProcessStep(MakeFrame(seed: 200), gameState, prevAction);
        var stateB = _esn.LastReservoirState.ToArray();

        bool anyDifferent = stateA.Zip(stateB, (a, b) => Math.Abs(a - b) > 1e-6f).Any(d => d);
        Assert.True(anyDifferent,
            "Different 768-frames with same game state should produce different reservoir states");
    }

    // ── 5. Integer action overload with 768-frame ────────────────────────────

    [Fact]
    public void ProcessStep_IntActionOverload_With768Frame_Returns18Probs()
    {
        var probs = _esn.ProcessStep(MakeFrame(seed: 20), MakeGameState(), previousAction: 5);
        Assert.Equal(18, probs.Length);
    }

    [Fact]
    public void ProcessStep_IntActionOverload_AllProbsInRange()
    {
        var probs = _esn.ProcessStep(MakeFrame(seed: 21), MakeGameState(), previousAction: 3);
        Assert.All(probs, p => Assert.InRange(p, 0f, 1f));
    }

    // ── 6. Reset after 768-frame processing ──────────────────────────────────

    [Fact]
    public void Reset_AfterMultiple768FrameSteps_ClearsExecutiveState()
    {
        // Process several steps to build up state
        for (int i = 0; i < 5; i++)
            _esn.ProcessStep(MakeFrame(seed: i + 30), MakeGameState(x: i * 100), MakePrevAction());

        _esn.Reset();

        // After reset, reservoir state should be near zero
        var fullState = _esn.GetFullState();
        float maxAbs  = fullState.Max(v => Math.Abs(v));
        Assert.Equal(0f, maxAbs, precision: 5);
    }

    [Fact]
    public void Reset_DoesNotClearWisdomLevel()
    {
        // Wisdom (slow weights) should persist across episodes
        for (int i = 0; i < 5; i++)
            _esn.ProcessStep(MakeFrame(seed: i + 40), MakeGameState(), MakePrevAction());

        float wisdomBefore = _esn.WisdomLevel;
        _esn.Reset();
        float wisdomAfter  = _esn.WisdomLevel;

        // Wisdom accumulates from slow weights; reset doesn't clear SlowWeights
        // so WisdomLevel should be unchanged (or at most rounded differently)
        Assert.Equal(wisdomBefore, wisdomAfter, precision: 5);
    }

    // ── 7. Stability over multiple steps ─────────────────────────────────────

    [Fact]
    public void MultipleConsecutive768Steps_ReservoirStaysStable()
    {
        for (int i = 0; i < 20; i++)
        {
            var probs = _esn.ProcessStep(MakeFrame(seed: i + 50), MakeGameState(x: i * 50), MakePrevAction(i % 18));

            // Each step must produce valid probabilities
            Assert.Equal(18, probs.Length);
            Assert.All(probs, p => Assert.False(float.IsNaN(p)));
            Assert.All(_esn.LastReservoirState, v => Assert.False(float.IsNaN(v)));
        }
    }

    [Fact]
    public void MultipleConsecutive768Steps_WisdomLevelIncreasesOrStaysConstant()
    {
        float wisdom0 = _esn.WisdomLevel;
        for (int i = 0; i < 15; i++)
            _esn.ProcessStep(MakeFrame(seed: i + 60), MakeGameState(x: i * 30), MakePrevAction());

        Assert.True(_esn.WisdomLevel >= wisdom0,
            "WisdomLevel should not decrease after processing steps");
    }

    // ── 8. TotalStepsProcessed counter ───────────────────────────────────────

    [Fact]
    public void TotalStepsProcessed_IncreasesAfterEach768FrameStep()
    {
        long before = _esn.TotalStepsProcessed;
        _esn.ProcessStep(MakeFrame(seed: 70), MakeGameState(), MakePrevAction());
        Assert.Equal(before + 1, _esn.TotalStepsProcessed);
    }

    [Fact]
    public void TotalStepsProcessed_IncreasesByNForNSteps()
    {
        long before = _esn.TotalStepsProcessed;
        const int n = 5;
        for (int i = 0; i < n; i++)
            _esn.ProcessStep(MakeFrame(seed: i + 80), MakeGameState(), MakePrevAction());
        Assert.Equal(before + n, _esn.TotalStepsProcessed);
    }

    // ── 9. Coherence stays in [0,1] after 768-frame processing ───────────────

    [Fact]
    public void GetCoherence_After768FrameSteps_IsInUnitRange()
    {
        for (int i = 0; i < 10; i++)
            _esn.ProcessStep(MakeFrame(seed: i + 90), MakeGameState(x: i * 100), MakePrevAction());

        float coherence = _esn.GetCoherence();
        Assert.InRange(coherence, 0f, 1f);
    }

    // ── 10. UpdateReadout doesn't break subsequent steps ─────────────────────

    [Fact]
    public void UpdateReadout_ThenProcessStep_ProducesValidProbs()
    {
        // Run a step to build reservoir state
        _esn.ProcessStep(MakeFrame(seed: 100), MakeGameState(), MakePrevAction());
        var reservoir = _esn.GetFullState();

        // Apply a readout weight update (simulate TD learning)
        _esn.UpdateReadout(reservoir, action: 3, tdError: 0.5f, learningRate: 0.001f);

        // Next step should still work correctly
        var probs = _esn.ProcessStep(MakeFrame(seed: 101), MakeGameState(x: 100), MakePrevAction(3));
        Assert.Equal(18, probs.Length);
        Assert.All(probs, p => Assert.InRange(p, 0f, 1f));
    }
}
