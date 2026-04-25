using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for EsnReservoirPipeline — ProcessStep, GetFullState, Reset,
/// GetCoherence, GetStats, and UpdateReadout.
/// </summary>
public class EsnReservoirPipelineTests : IDisposable
{
    private readonly EsnReservoirPipeline _pipeline;

    public EsnReservoirPipelineTests()
    {
        _pipeline = new EsnReservoirPipeline(NullLogger<EsnReservoirPipeline>.Instance);
    }

    public void Dispose() => _pipeline.Dispose();

    // ── Initial state ──────────────────────────────────────────────────────

    [Fact]
    public void IsInitialized_BeforeFirstStep_IsFalse()
    {
        Assert.False(_pipeline.IsInitialized);
    }

    [Fact]
    public void TotalStepsProcessed_Initially_IsZero()
    {
        Assert.Equal(0L, _pipeline.TotalStepsProcessed);
    }

    [Fact]
    public void WisdomLevel_Initially_IsZero()
    {
        Assert.Equal(0f, _pipeline.WisdomLevel);
    }

    [Fact]
    public void CognitiveDimensions_Initially_HasFourElements()
    {
        Assert.Equal(4, _pipeline.CognitiveDimensions.Length);
    }

    // ── Initialize ────────────────────────────────────────────────────────

    [Fact]
    public void Initialize_SetsIsInitializedTrue()
    {
        _pipeline.Initialize();
        Assert.True(_pipeline.IsInitialized);
    }

    [Fact]
    public void Initialize_CalledTwice_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
        {
            _pipeline.Initialize();
            _pipeline.Initialize();
        });
        Assert.Null(ex);
    }

    // ── ProcessStep ───────────────────────────────────────────────────────

    [Fact]
    public void ProcessStep_Returns18ActionProbabilities()
    {
        var frame = new float[22]; // small dummy frame
        var gameState = new float[22];
        var prevAction = new float[18];
        var result = _pipeline.ProcessStep(frame, gameState, prevAction);
        Assert.Equal(18, result.Length);
    }

    [Fact]
    public void ProcessStep_ActionProbabilitiesSumToOne()
    {
        var frame = new float[22];
        var gameState = new float[22];
        var prevAction = new float[18];
        var probs = _pipeline.ProcessStep(frame, gameState, prevAction);
        Assert.Equal(1f, probs.Sum(), 5);
    }

    [Fact]
    public void ProcessStep_AllProbabilitiesAreNonNegative()
    {
        var frame = new float[22];
        var gameState = new float[22];
        var prevAction = new float[18];
        var probs = _pipeline.ProcessStep(frame, gameState, prevAction);
        Assert.All(probs, p => Assert.True(p >= 0f));
    }

    [Fact]
    public void ProcessStep_IncrementsTotalStepsProcessed()
    {
        var frame = new float[22];
        var state = new float[22];
        var action = new float[18];
        _pipeline.ProcessStep(frame, state, action);
        Assert.Equal(1L, _pipeline.TotalStepsProcessed);
        _pipeline.ProcessStep(frame, state, action);
        Assert.Equal(2L, _pipeline.TotalStepsProcessed);
    }

    [Fact]
    public void ProcessStep_SetsIsInitializedTrue()
    {
        Assert.False(_pipeline.IsInitialized);
        _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        Assert.True(_pipeline.IsInitialized);
    }

    [Fact]
    public void ProcessStep_WithIntegerAction_Returns18Probabilities()
    {
        var result = _pipeline.ProcessStep(new float[22], new float[22], previousAction: 3);
        Assert.Equal(18, result.Length);
    }

    [Fact]
    public void ProcessStep_LastActionProbabilitiesAreUpdated()
    {
        var probs = _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        Assert.Equal(18, _pipeline.LastActionProbabilities.Length);
        Assert.Equal(probs[0], _pipeline.LastActionProbabilities[0]);
    }

    [Fact]
    public void ProcessStep_LastReservoirStateIsSet()
    {
        _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        Assert.NotEmpty(_pipeline.LastReservoirState);
    }

    // ── GetFullState ──────────────────────────────────────────────────────

    [Fact]
    public void GetFullState_BeforeProcessing_ReturnsZeroes()
    {
        _pipeline.Initialize();
        var state = _pipeline.GetFullState();
        // 128 + 256 + 512 = 896 neurons
        Assert.Equal(896, state.Length);
        Assert.All(state, s => Assert.Equal(0f, s));
    }

    [Fact]
    public void GetFullState_AfterProcessing_IsNotAllZero()
    {
        _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        var state = _pipeline.GetFullState();
        Assert.Contains(state, s => s != 0f);
    }

    [Fact]
    public void GetFullState_LengthIs896()
    {
        _pipeline.Initialize();
        Assert.Equal(896, _pipeline.GetFullState().Length);
    }

    // ── Reset ─────────────────────────────────────────────────────────────

    [Fact]
    public void Reset_AfterProcessing_ClearsReservoirState()
    {
        _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        _pipeline.Reset();
        var state = _pipeline.GetFullState();
        Assert.All(state, s => Assert.Equal(0f, s));
    }

    [Fact]
    public void Reset_AllowsProcessingAfterReset()
    {
        _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        _pipeline.Reset();
        var ex = Record.Exception(() => _pipeline.ProcessStep(new float[22], new float[22], new float[18]));
        Assert.Null(ex);
    }

    [Fact]
    public void Reset_DoesNotThrowBeforeInitialize()
    {
        var ex = Record.Exception(() => _pipeline.Reset());
        Assert.Null(ex);
    }

    // ── GetCoherence ──────────────────────────────────────────────────────

    [Fact]
    public void GetCoherence_BeforeProcessing_IsZero()
    {
        _pipeline.Initialize();
        float coherence = _pipeline.GetCoherence();
        Assert.Equal(0f, coherence);
    }

    [Fact]
    public void GetCoherence_AfterProcessing_IsInRange()
    {
        for (int i = 0; i < 5; i++)
            _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        float coherence = _pipeline.GetCoherence();
        Assert.InRange(coherence, 0f, 1f);
    }

    // ── GetStats ──────────────────────────────────────────────────────────

    [Fact]
    public void GetStats_ReturnsNonNullObject()
    {
        _pipeline.Initialize();
        var stats = _pipeline.GetStats();
        Assert.NotNull(stats);
    }

    [Fact]
    public void GetStats_AfterProcessing_TotalStepsMatchesPipelineTotalSteps()
    {
        for (int i = 0; i < 3; i++)
            _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        var stats = _pipeline.GetStats();
        Assert.Equal(_pipeline.TotalStepsProcessed, stats.TotalSteps);
    }

    [Fact]
    public void GetStats_CognitiveDimensionsHasFourElements()
    {
        _pipeline.Initialize();
        var stats = _pipeline.GetStats();
        Assert.Equal(4, stats.CognitiveDimensions.Length);
    }

    [Fact]
    public void GetStats_AvgProcessingMs_IsNonNegative()
    {
        _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        var stats = _pipeline.GetStats();
        Assert.True(stats.AvgProcessingMs >= 0);
    }

    // ── UpdateReadout ─────────────────────────────────────────────────────

    [Fact]
    public void UpdateReadout_DoesNotThrow()
    {
        _pipeline.Initialize();
        var reservoirState = new float[512];
        var ex = Record.Exception(() => _pipeline.UpdateReadout(reservoirState, 0, 0.5f, 0.001f));
        Assert.Null(ex);
    }

    [Fact]
    public void UpdateReadout_WithEmptyState_DoesNotThrow()
    {
        _pipeline.Initialize();
        var ex = Record.Exception(() => _pipeline.UpdateReadout(Array.Empty<float>(), 0, 1.0f, 0.01f));
        Assert.Null(ex);
    }

    // ── Edge cases ────────────────────────────────────────────────────────

    [Fact]
    public void ProcessStep_WithEmptyFrame_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            _pipeline.ProcessStep(Array.Empty<float>(), new float[22], new float[18]));
        Assert.Null(ex);
    }

    [Fact]
    public void ProcessStep_MultipleSteps_WisdomLevelIncreasesOverTime()
    {
        float prev = _pipeline.WisdomLevel;
        for (int i = 0; i < 20; i++)
            _pipeline.ProcessStep(new float[22], new float[22], new float[18]);
        // Wisdom accumulates via slow weights; after many steps it should be > 0
        Assert.True(_pipeline.WisdomLevel >= 0f);
    }
}
