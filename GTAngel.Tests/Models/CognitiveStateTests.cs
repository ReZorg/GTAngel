using GTAngel.Models;
using Xunit;

namespace GTAngel.Tests.Models;

public class CognitiveStateTests
{
    // ── CycleStepNames ─────────────────────────────────────────────────────

    [Fact]
    public void CycleStepNames_HasExactly12Entries()
    {
        Assert.Equal(12, CognitiveState.CycleStepNames.Length);
    }

    [Theory]
    [InlineData(0, "Perception")]
    [InlineData(1, "Attention")]
    [InlineData(2, "Recognition")]
    [InlineData(3, "Comprehension")]
    [InlineData(4, "Evaluation")]
    [InlineData(5, "Planning")]
    [InlineData(6, "Intention")]
    [InlineData(7, "Execution")]
    [InlineData(8, "Monitoring")]
    [InlineData(9, "Learning")]
    [InlineData(10, "Consolidation")]
    [InlineData(11, "Reflection")]
    public void CycleStepNames_CorrectNameAtEachIndex(int index, string expected)
    {
        Assert.Equal(expected, CognitiveState.CycleStepNames[index]);
    }

    // ── CycleStepName computed property ────────────────────────────────────

    [Theory]
    [InlineData(0, "Perception")]
    [InlineData(6, "Intention")]
    [InlineData(11, "Reflection")]
    public void CycleStepName_ReturnsCorrectNameForStep(int step, string expected)
    {
        var state = new CognitiveState { CurrentCycleStep = step };
        Assert.Equal(expected, state.CycleStepName);
    }

    [Fact]
    public void CycleStepName_WrapsAround_WhenStepExceeds11()
    {
        // step 12 → index 0 → "Perception"
        var state = new CognitiveState { CurrentCycleStep = 12 };
        Assert.Equal("Perception", state.CycleStepName);
    }

    [Fact]
    public void CycleStepName_WrapsAround_AtStep24()
    {
        // step 24 → index 0 → "Perception"
        var state = new CognitiveState { CurrentCycleStep = 24 };
        Assert.Equal("Perception", state.CycleStepName);
    }

    // ── CycleProgress ──────────────────────────────────────────────────────

    [Fact]
    public void CycleProgress_AtStep0_IsZero()
    {
        var state = new CognitiveState { CurrentCycleStep = 0 };
        Assert.Equal(0.0, state.CycleProgress);
    }

    [Fact]
    public void CycleProgress_AtStep6_IsHalf()
    {
        var state = new CognitiveState { CurrentCycleStep = 6 };
        Assert.Equal(0.5, state.CycleProgress, precision: 10);
    }

    [Fact]
    public void CycleProgress_AtStep11_IsNearlyOne()
    {
        var state = new CognitiveState { CurrentCycleStep = 11 };
        Assert.Equal(11.0 / 12.0, state.CycleProgress, precision: 10);
    }

    // ── Defaults ───────────────────────────────────────────────────────────

    [Fact]
    public void CognitiveState_Default_HasExplorationMode()
    {
        var state = new CognitiveState();
        Assert.Equal(CognitiveMode.Exploration, state.Mode);
    }

    [Fact]
    public void CognitiveState_Default_StreamsAreInitialized()
    {
        var state = new CognitiveState();
        Assert.NotNull(state.SensoryStream);
        Assert.NotNull(state.CognitiveStream);
        Assert.NotNull(state.AffectiveStream);
        Assert.Equal("Sensory", state.SensoryStream.Name);
        Assert.Equal("Cognitive", state.CognitiveStream.Name);
        Assert.Equal("Affective", state.AffectiveStream.Name);
    }

    [Fact]
    public void CognitiveState_Default_TimestampIsRecent()
    {
        var before = DateTime.UtcNow.AddSeconds(-1);
        var state = new CognitiveState();
        var after = DateTime.UtcNow.AddSeconds(1);
        Assert.InRange(state.Timestamp, before, after);
    }

    // ── ConsciousnessStream ────────────────────────────────────────────────

    [Fact]
    public void ConsciousnessStream_DefaultCoherenceHistoryIsEmpty()
    {
        var stream = new ConsciousnessStream();
        Assert.Empty(stream.CoherenceHistory);
    }

    [Fact]
    public void ConsciousnessStream_DefaultReservoirStateIsEmpty()
    {
        var stream = new ConsciousnessStream();
        Assert.Empty(stream.ReservoirState);
    }
}
