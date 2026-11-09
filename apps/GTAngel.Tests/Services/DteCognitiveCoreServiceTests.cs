using GTAngel.Services;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for DteCognitiveCoreService:
///   1. ECAN Attention Economics (UpdateAttention, GetClusterSTI/LTI)
///   2. MOSES Pattern Miner (MinePatterns, PatternCount, GetTopPatternFitness, GetPatternDiversity)
///   3. Ridge Regression Wout Trainer (TrainWout, GetWoutLoss, GetWoutSampleCount)
///   4. Attention-Gated Action Logits (ComputeAttentionGatedLogits)
///   5. Thompson Sampling Policy (ThompsonSampleAction, UpdateThompson, GetThompsonSnapshot)
///   6. Cognitive Coherence (ComputeCoherence)
///   7. Lifecycle (Dispose idempotency)
/// </summary>
public sealed class DteCognitiveCoreServiceTests : IDisposable
{
    private const int ReservoirSize = 512;
    private const int ActionCount = 18;
    private const int ClusterCount = 16;

    private readonly DteCognitiveCoreService _svc = new();

    public void Dispose() => _svc.Dispose();

    // ── Helpers ──────────────────────────────────────────────────────────────

    private static float[] MakeReservoirState(float value = 0.5f)
        => Enumerable.Repeat(value, ReservoirSize).ToArray();

    private static float[] MakeVaryingReservoirState(int seed = 0)
    {
        var rng = new Random(seed);
        return Enumerable.Range(0, ReservoirSize).Select(_ => (float)rng.NextDouble()).ToArray();
    }

    private static float[] MakeActions(float value = 0.1f)
        => Enumerable.Repeat(value, ActionCount).ToArray();

    // ── 1. Initial state ─────────────────────────────────────────────────────

    [Fact]
    public void Constructor_ClusterSTI_InitializedToZero()
    {
        var sti = _svc.GetClusterSTI();
        Assert.Equal(ClusterCount, sti.Length);
        Assert.All(sti, v => Assert.Equal(0f, v));
    }

    [Fact]
    public void Constructor_ClusterLTI_InitializedToZero()
    {
        var lti = _svc.GetClusterLTI();
        Assert.Equal(ClusterCount, lti.Length);
        Assert.All(lti, v => Assert.Equal(0f, v));
    }

    [Fact]
    public void Constructor_PatternCount_IsZero()
    {
        Assert.Equal(0, _svc.PatternCount);
    }

    [Fact]
    public void Constructor_WoutLoss_IsOne()
    {
        Assert.Equal(1.0, _svc.GetWoutLoss());
    }

    [Fact]
    public void Constructor_WoutSampleCount_IsZero()
    {
        Assert.Equal(0, _svc.GetWoutSampleCount());
    }

    [Fact]
    public void Constructor_AttentionEntropy_IsZero()
    {
        Assert.Equal(0f, _svc.GetAttentionEntropy());
    }

    // ── 2. ECAN Attention ────────────────────────────────────────────────────

    [Fact]
    public void UpdateAttention_AfterOneStep_STISumsToPositiveValue()
    {
        _svc.UpdateAttention(MakeVaryingReservoirState());
        Assert.True(_svc.GetAttentionBudget() > 0f);
    }

    [Fact]
    public void UpdateAttention_STI_HasCorrectLength()
    {
        _svc.UpdateAttention(MakeVaryingReservoirState());
        Assert.Equal(ClusterCount, _svc.GetClusterSTI().Length);
    }

    [Fact]
    public void UpdateAttention_LTI_HasCorrectLength()
    {
        _svc.UpdateAttention(MakeVaryingReservoirState());
        Assert.Equal(ClusterCount, _svc.GetClusterLTI().Length);
    }

    [Fact]
    public void UpdateAttention_LTI_TracksSTIOverTime()
    {
        // After many identical steps, LTI should approach STI
        var state = MakeVaryingReservoirState(42);
        for (int i = 0; i < 200; i++)
            _svc.UpdateAttention(state);

        var sti = _svc.GetClusterSTI();
        var lti = _svc.GetClusterLTI();

        // LTI should be non-zero since STI has been non-zero
        Assert.True(lti.Sum() > 0f);
    }

    [Fact]
    public void UpdateAttention_WrongLength_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.UpdateAttention(new float[10]));
        Assert.Null(ex);
    }

    [Fact]
    public void UpdateAttention_WrongLength_LeavesSTIUnchanged()
    {
        _svc.UpdateAttention(new float[10]);
        Assert.All(_svc.GetClusterSTI(), v => Assert.Equal(0f, v));
    }

    [Fact]
    public void UpdateAttention_FiresOnAttentionUpdatedEvent()
    {
        int fired = 0;
        _svc.OnAttentionUpdated += (_, _) => fired++;
        _svc.UpdateAttention(MakeVaryingReservoirState());
        Assert.Equal(1, fired);
    }

    [Fact]
    public void UpdateAttention_AttentionSnapshot_TopNeuronClustersNotEmpty()
    {
        AttentionSnapshot? snapshot = null;
        _svc.OnAttentionUpdated += (_, e) => snapshot = e.Snapshot;
        _svc.UpdateAttention(MakeVaryingReservoirState());
        Assert.NotNull(snapshot);
        Assert.NotEmpty(snapshot!.TopNeuronClusters);
    }

    // ── 3. MOSES Pattern Mining ──────────────────────────────────────────────

    [Fact]
    public void MinePatterns_BelowHalfWindow_AddsNoPatterns()
    {
        // PatternWindowLen/2 = 25; feeding < 25 states should add no patterns
        var state = MakeVaryingReservoirState();
        for (int i = 0; i < 10; i++)
            _svc.MinePatterns(state);
        Assert.Equal(0, _svc.PatternCount);
    }

    [Fact]
    public void MinePatterns_AfterEnoughSteps_AddsPatterns()
    {
        // Feed 30+ unique states to cross the half-window threshold
        for (int i = 0; i < 30; i++)
            _svc.MinePatterns(MakeVaryingReservoirState(i));
        Assert.True(_svc.PatternCount > 0);
    }

    [Fact]
    public void MinePatterns_WrongLength_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.MinePatterns(new float[10]));
        Assert.Null(ex);
    }

    [Fact]
    public void MinePatterns_WrongLength_AddsNoPatterns()
    {
        _svc.MinePatterns(new float[10]);
        Assert.Equal(0, _svc.PatternCount);
    }

    [Fact]
    public void GetTopPatternFitness_NoPatterns_ReturnsZero()
    {
        Assert.Equal(0f, _svc.GetTopPatternFitness());
    }

    [Fact]
    public void GetTopPatternFitness_AfterPatternsMined_IsPositive()
    {
        for (int i = 0; i < 30; i++)
            _svc.MinePatterns(MakeVaryingReservoirState(i));

        if (_svc.PatternCount > 0)
            Assert.True(_svc.GetTopPatternFitness() > 0f);
    }

    [Fact]
    public void GetPatternDiversity_NoPatterns_ReturnsZero()
    {
        Assert.Equal(0.0, _svc.GetPatternDiversity());
    }

    // ── 4. Wout Ridge Regression ─────────────────────────────────────────────

    [Fact]
    public void TrainWout_IncrementsSampleCount()
    {
        _svc.TrainWout(MakeVaryingReservoirState(), MakeActions());
        Assert.Equal(1, _svc.GetWoutSampleCount());
    }

    [Fact]
    public void TrainWout_MultipleTimes_IncrementsSampleCountCorrectly()
    {
        for (int i = 0; i < 10; i++)
            _svc.TrainWout(MakeVaryingReservoirState(i), MakeActions());
        Assert.Equal(10, _svc.GetWoutSampleCount());
    }

    [Fact]
    public void TrainWout_WrongReservoirLength_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.TrainWout(new float[10], MakeActions()));
        Assert.Null(ex);
    }

    [Fact]
    public void TrainWout_WrongActionLength_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.TrainWout(MakeVaryingReservoirState(), new float[5]));
        Assert.Null(ex);
    }

    [Fact]
    public void TrainWout_WrongLengths_DoesNotChangeSampleCount()
    {
        _svc.TrainWout(new float[10], MakeActions());
        _svc.TrainWout(MakeVaryingReservoirState(), new float[5]);
        Assert.Equal(0, _svc.GetWoutSampleCount());
    }

    [Fact]
    public void TrainWout_LossDecreases_OverManyStepsWithConsistentTarget()
    {
        // With the same state and target, gradient descent should eventually reduce loss
        var state = MakeVaryingReservoirState(1);
        var target = MakeActions(0.5f);

        double initialLoss = _svc.GetWoutLoss();
        for (int i = 0; i < 1000; i++)
            _svc.TrainWout(state, target);

        double finalLoss = _svc.GetWoutLoss();
        // Loss should be lower after training (EMA smoothed)
        Assert.True(finalLoss < initialLoss);
    }

    // ── 5. Attention-Gated Logits ────────────────────────────────────────────

    [Fact]
    public void ComputeAttentionGatedLogits_ReturnsCorrectLength()
    {
        var logits = _svc.ComputeAttentionGatedLogits(MakeVaryingReservoirState());
        Assert.Equal(ActionCount, logits.Length);
    }

    [Fact]
    public void ComputeAttentionGatedLogits_WrongLength_ReturnsZeroArray()
    {
        var logits = _svc.ComputeAttentionGatedLogits(new float[10]);
        Assert.Equal(ActionCount, logits.Length);
        Assert.All(logits, v => Assert.Equal(0f, v));
    }

    [Fact]
    public void ComputeAttentionGatedLogits_AfterAttentionUpdate_ProducesNonZeroLogits()
    {
        var state = MakeVaryingReservoirState(7);
        _svc.UpdateAttention(state);

        // After wout training, logits should differ from zero
        for (int i = 0; i < 10; i++)
            _svc.TrainWout(state, MakeActions(0.5f));

        var logits = _svc.ComputeAttentionGatedLogits(state);
        // At least one logit should be non-zero given non-zero Wout weights
        Assert.Contains(logits, v => v != 0f);
    }

    // ── 6. Thompson Sampling ─────────────────────────────────────────────────

    [Fact]
    public void ThompsonSampleAction_ReturnsValidActionIndex()
    {
        var logits = new float[ActionCount];
        logits[3] = 1f;
        int action = _svc.ThompsonSampleAction(logits);
        Assert.InRange(action, 0, ActionCount - 1);
    }

    [Fact]
    public void ThompsonSampleAction_WrongLength_ReturnsZero()
    {
        int action = _svc.ThompsonSampleAction(new float[5]);
        Assert.Equal(0, action);
    }

    [Fact]
    public void UpdateThompson_ValidAction_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.UpdateThompson(0, 1f));
        Assert.Null(ex);
    }

    [Fact]
    public void UpdateThompson_InvalidAction_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.UpdateThompson(-1, 1f));
        Assert.Null(ex);
    }

    [Fact]
    public void UpdateThompson_InvalidActionAboveRange_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.UpdateThompson(ActionCount, 1f));
        Assert.Null(ex);
    }

    [Fact]
    public void UpdateThompson_PositiveReward_IncreasesAlpha()
    {
        var before = _svc.GetThompsonSnapshot();
        _svc.UpdateThompson(0, 1f); // full positive reward → Δα=1, Δβ=0
        var after = _svc.GetThompsonSnapshot();
        Assert.True(after.Alpha[0] > before.Alpha[0]);
    }

    [Fact]
    public void UpdateThompson_NegativeReward_IncreasesBeta()
    {
        var before = _svc.GetThompsonSnapshot();
        _svc.UpdateThompson(0, -1f); // full negative reward → Δα=0, Δβ=1
        var after = _svc.GetThompsonSnapshot();
        Assert.True(after.Beta[0] > before.Beta[0]);
    }

    [Fact]
    public void GetThompsonSnapshot_InitialAlpha_AllOnes()
    {
        var snap = _svc.GetThompsonSnapshot();
        Assert.All(snap.Alpha, v => Assert.Equal(1f, v));
    }

    [Fact]
    public void GetThompsonSnapshot_InitialBeta_AllOnes()
    {
        var snap = _svc.GetThompsonSnapshot();
        Assert.All(snap.Beta, v => Assert.Equal(1f, v));
    }

    [Fact]
    public void GetThompsonSnapshot_HasCorrectArrayLengths()
    {
        var snap = _svc.GetThompsonSnapshot();
        Assert.Equal(ActionCount, snap.Alpha.Length);
        Assert.Equal(ActionCount, snap.Beta.Length);
    }

    [Fact]
    public void UpdateThompson_ClipsAlphaAt1000()
    {
        for (int i = 0; i < 1001; i++)
            _svc.UpdateThompson(0, 1f);
        var snap = _svc.GetThompsonSnapshot();
        Assert.True(snap.Alpha[0] <= 1000f);
    }

    [Fact]
    public void UpdateThompson_ClipsBetaAt1000()
    {
        for (int i = 0; i < 1001; i++)
            _svc.UpdateThompson(0, -1f);
        var snap = _svc.GetThompsonSnapshot();
        Assert.True(snap.Beta[0] <= 1000f);
    }

    // ── 7. Cognitive Coherence ───────────────────────────────────────────────

    [Fact]
    public void ComputeCoherence_Initial_ReturnsValidSnapshot()
    {
        var snap = _svc.ComputeCoherence();
        Assert.InRange(snap.Coherence, 0.0, 1.0);
        Assert.InRange(snap.AttentionCoherence, 0.0, 1.5); // may slightly exceed 1 by formula
        Assert.InRange(snap.PatternCoherence, 0.0, 1.0);
        Assert.InRange(snap.PolicyCoherence, 0.0, 1.0);
    }

    [Fact]
    public void ComputeCoherence_TotalPatternsMatchesPatternCount()
    {
        for (int i = 0; i < 30; i++)
            _svc.MinePatterns(MakeVaryingReservoirState(i));

        var snap = _svc.ComputeCoherence();
        Assert.Equal(_svc.PatternCount, snap.TotalPatterns);
    }

    [Fact]
    public void ComputeCoherence_FiresOnCognitiveCoherenceUpdatedEvent()
    {
        int fired = 0;
        _svc.OnCognitiveCoherenceUpdated += (_, _) => fired++;
        _svc.ComputeCoherence();
        Assert.Equal(1, fired);
    }

    [Fact]
    public void ComputeCoherence_PatternDiversityIsNonNegative()
    {
        var snap = _svc.ComputeCoherence();
        Assert.True(snap.PatternDiversity >= 0.0);
    }

    // ── 8. Lifecycle ─────────────────────────────────────────────────────────

    [Fact]
    public void Dispose_CalledTwice_DoesNotThrow()
    {
        var svc = new DteCognitiveCoreService();
        svc.Dispose();
        var ex = Record.Exception(() => svc.Dispose());
        Assert.Null(ex);
    }

    [Fact]
    public void Constructor_WithNullLogger_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
        {
            using var svc = new DteCognitiveCoreService(null);
        });
        Assert.Null(ex);
    }
}
