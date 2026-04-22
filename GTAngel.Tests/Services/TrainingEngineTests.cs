using GTA3DE.Wpf.Models;
using GTA3DE.Wpf.Services;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for TrainingEngine — focuses on synchronous logic:
/// property coherence computation, Stop(), IsRunning flag, and
/// state inspection after construction.
/// </summary>
public class TrainingEngineTests
{
    private readonly TrainingEngine _engine = new();

    // ── Initial state ──────────────────────────────────────────────────────

    [Fact]
    public void IsRunning_AfterConstruction_IsFalse()
    {
        Assert.False(_engine.IsRunning);
    }

    [Fact]
    public void Properties_AfterConstruction_Has15Entries()
    {
        Assert.Equal(15, _engine.Properties.Count);
    }

    [Fact]
    public void Properties_AfterConstruction_AllHaveScoresBetween0And1()
    {
        Assert.All(_engine.Properties, p =>
        {
            Assert.InRange(p.Score, 0.0, 1.0);
        });
    }

    [Fact]
    public void EpisodeHistory_AfterConstruction_IsEmpty()
    {
        Assert.Empty(_engine.EpisodeHistory);
    }

    [Fact]
    public void Experiments_AfterConstruction_IsEmpty()
    {
        Assert.Empty(_engine.Experiments);
    }

    [Fact]
    public void Stats_AfterConstruction_HasZeroEpisodes()
    {
        Assert.Equal(0, _engine.Stats.TotalEpisodes);
    }

    [Fact]
    public void CurrentEpisode_AfterConstruction_IsNull()
    {
        Assert.Null(_engine.CurrentEpisode);
    }

    // ── ComputeOverallPropertyCoherence ────────────────────────────────────

    [Fact]
    public void ComputeOverallPropertyCoherence_IsAverageOfPropertyScores()
    {
        // All properties are initialized to a value; avg should be in (0,1)
        double coherence = _engine.ComputeOverallPropertyCoherence();
        Assert.InRange(coherence, 0.0, 1.0);
    }

    [Fact]
    public void ComputeOverallPropertyCoherence_Equals_AverageOfAllScores()
    {
        double expected = _engine.Properties.Average(p => p.Score);
        double actual = _engine.ComputeOverallPropertyCoherence();
        Assert.Equal(expected, actual, precision: 10);
    }

    // ── Stop ──────────────────────────────────────────────────────────────

    [Fact]
    public void Stop_WhenNotRunning_DoesNotThrow()
    {
        var ex = Record.Exception(() => _engine.Stop());
        Assert.Null(ex);
    }

    [Fact]
    public void Stop_WhenNotRunning_LeavesIsRunningFalse()
    {
        _engine.Stop();
        Assert.False(_engine.IsRunning);
    }

    // ── ESN exposure ──────────────────────────────────────────────────────

    [Fact]
    public void ESN_IsNotNull()
    {
        Assert.NotNull(_engine.ESN);
    }

    [Fact]
    public void Config_IsNotNull()
    {
        Assert.NotNull(_engine.Config);
    }

    [Fact]
    public void CognitiveState_IsNotNull()
    {
        Assert.NotNull(_engine.CognitiveState);
    }

    // ── Event wiring ──────────────────────────────────────────────────────

    [Fact]
    public void Events_CanBeSubscribedAndUnsubscribed()
    {
        Action<GTA3DE.Wpf.Models.CognitiveState>? cognitiveStateHandler = _ => { };
        _engine.OnCognitiveStateChanged += cognitiveStateHandler;
        _engine.OnCognitiveStateChanged -= cognitiveStateHandler;
        // No assertion needed — just verifying no exception is thrown
    }
}
