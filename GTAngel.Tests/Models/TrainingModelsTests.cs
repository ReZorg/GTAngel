using GTA3DE.Wpf.Models;
using Xunit;

namespace GTAngel.Tests.Models;

public class TrainingModelsTests
{
    // ── TrainingEpisode ────────────────────────────────────────────────────

    [Fact]
    public void TrainingEpisode_AverageReward_ZeroSteps_ReturnsZero()
    {
        var ep = new TrainingEpisode { Steps = 0, TotalReward = 10.0 };
        Assert.Equal(0.0, ep.AverageReward);
    }

    [Fact]
    public void TrainingEpisode_AverageReward_CalculatesCorrectly()
    {
        var ep = new TrainingEpisode { Steps = 10, TotalReward = 50.0 };
        Assert.Equal(5.0, ep.AverageReward);
    }

    [Fact]
    public void TrainingEpisode_Duration_ReturnsPositiveWhenComplete()
    {
        var ep = new TrainingEpisode
        {
            StartTime = DateTime.UtcNow.AddSeconds(-5),
            EndTime = DateTime.UtcNow
        };
        Assert.True(ep.Duration.TotalSeconds > 0);
    }

    [Fact]
    public void TrainingEpisode_Duration_FallsBackToNowWhenNoEndTime()
    {
        var ep = new TrainingEpisode
        {
            StartTime = DateTime.UtcNow.AddSeconds(-3),
            EndTime = null
        };
        Assert.True(ep.Duration.TotalSeconds >= 0);
    }

    [Fact]
    public void TrainingEpisode_DefaultTerminationReason_IsRunning()
    {
        var ep = new TrainingEpisode();
        Assert.Equal(EpisodeTermination.Running, ep.TerminationReason);
    }

    [Fact]
    public void TrainingEpisode_DefaultAutonomyLevel_IsReactive()
    {
        var ep = new TrainingEpisode();
        Assert.Equal(AutonomyLevel.Reactive, ep.AutonomyLevelReached);
    }

    // ── AutogenesisExperiment ─────────────────────────────────────────────

    [Fact]
    public void AutogenesisExperiment_MetricDelta_IsCorrect()
    {
        var exp = new AutogenesisExperiment { PrimaryMetric = 0.8, BaselineMetric = 0.5 };
        Assert.Equal(0.3, exp.MetricDelta, precision: 10);
    }

    [Fact]
    public void AutogenesisExperiment_MetricImproved_TrueWhenPrimaryIsHigher()
    {
        var exp = new AutogenesisExperiment { PrimaryMetric = 0.9, BaselineMetric = 0.5 };
        Assert.True(exp.MetricImproved);
    }

    [Fact]
    public void AutogenesisExperiment_MetricImproved_FalseWhenPrimaryIsLower()
    {
        var exp = new AutogenesisExperiment { PrimaryMetric = 0.3, BaselineMetric = 0.5 };
        Assert.False(exp.MetricImproved);
    }

    [Fact]
    public void AutogenesisExperiment_MetricImproved_FalseWhenEqual()
    {
        var exp = new AutogenesisExperiment { PrimaryMetric = 0.5, BaselineMetric = 0.5 };
        Assert.False(exp.MetricImproved);
    }

    [Fact]
    public void AutogenesisExperiment_KsmStepName_WrapsCorrectly()
    {
        // Step 12 wraps to step 0 → "Observe"
        var exp = new AutogenesisExperiment { KsmStep = 12 };
        Assert.Equal("Observe", exp.KsmStepName);
    }

    [Theory]
    [InlineData(0, "Observe")]
    [InlineData(1, "Diagnose")]
    [InlineData(2, "Hypothesize")]
    [InlineData(3, "Design")]
    [InlineData(4, "Implement")]
    [InlineData(5, "Test")]
    [InlineData(6, "Measure")]
    [InlineData(7, "Evaluate")]
    [InlineData(8, "Integrate")]
    [InlineData(9, "Consolidate")]
    [InlineData(10, "Reflect")]
    [InlineData(11, "Evolve")]
    public void AutogenesisExperiment_KsmStepNames_AreCorrect(int step, string expected)
    {
        Assert.Equal(expected, AutogenesisExperiment.KsmStepNames[step]);
    }

    [Fact]
    public void AutogenesisExperiment_KsmStepNames_HasExactly12()
    {
        Assert.Equal(12, AutogenesisExperiment.KsmStepNames.Length);
    }

    [Fact]
    public void AutogenesisExperiment_DefaultPropertyScores_IsEmptyDict()
    {
        var exp = new AutogenesisExperiment();
        Assert.NotNull(exp.PropertyScores);
        Assert.Empty(exp.PropertyScores);
    }

    // ── AlexanderProperty ──────────────────────────────────────────────────

    [Fact]
    public void AlexanderProperty_CreateAll_Returns15Properties()
    {
        var props = AlexanderProperty.CreateAll();
        Assert.Equal(15, props.Count);
    }

    [Fact]
    public void AlexanderProperty_CreateAll_IndicesAreSequential()
    {
        var props = AlexanderProperty.CreateAll();
        for (int i = 0; i < props.Count; i++)
            Assert.Equal(i, props[i].Index);
    }

    [Fact]
    public void AlexanderProperty_CreateAll_NamesAreNotEmpty()
    {
        var props = AlexanderProperty.CreateAll();
        Assert.All(props, p => Assert.NotEmpty(p.Name));
    }

    [Fact]
    public void AlexanderProperty_CreateAll_DescriptionsAreNotEmpty()
    {
        var props = AlexanderProperty.CreateAll();
        Assert.All(props, p => Assert.NotEmpty(p.Description));
    }

    [Theory]
    [InlineData(0, "Levels of Scale")]
    [InlineData(1, "Strong Centers")]
    [InlineData(2, "Boundaries")]
    [InlineData(7, "Deep Interlock & Ambiguity")]
    [InlineData(14, "Not-Separateness")]
    public void AlexanderProperty_CreateAll_HasCorrectNameAtIndex(int index, string name)
    {
        var props = AlexanderProperty.CreateAll();
        Assert.Equal(name, props[index].Name);
    }

    [Fact]
    public void AlexanderProperty_Delta_IsScoreMinusPreviousScore()
    {
        var prop = new AlexanderProperty { Score = 0.8, PreviousScore = 0.6 };
        Assert.Equal(0.2, prop.Delta, precision: 10);
    }

    [Fact]
    public void AlexanderProperty_NegativeDelta_WhenScoreDecreased()
    {
        var prop = new AlexanderProperty { Score = 0.4, PreviousScore = 0.6 };
        Assert.True(prop.Delta < 0);
    }

    // ── TrainingStats ──────────────────────────────────────────────────────

    [Fact]
    public void TrainingStats_KeepRatio_ZeroExperiments_ReturnsZero()
    {
        var stats = new TrainingStats { ExperimentsRun = 0, ExperimentsKept = 0 };
        Assert.Equal(0.0, stats.KeepRatio);
    }

    [Fact]
    public void TrainingStats_KeepRatio_HalfKept()
    {
        var stats = new TrainingStats { ExperimentsRun = 10, ExperimentsKept = 5 };
        Assert.Equal(0.5, stats.KeepRatio);
    }

    [Fact]
    public void TrainingStats_KeepRatio_AllKept()
    {
        var stats = new TrainingStats { ExperimentsRun = 10, ExperimentsKept = 10 };
        Assert.Equal(1.0, stats.KeepRatio);
    }

    [Fact]
    public void TrainingStats_DefaultListsAreNotNull()
    {
        var stats = new TrainingStats();
        Assert.NotNull(stats.RewardHistory);
        Assert.NotNull(stats.CoherenceHistory);
    }

    // ── TrainingConfig ─────────────────────────────────────────────────────

    [Fact]
    public void TrainingConfig_Defaults_AreReasonable()
    {
        var config = new TrainingConfig();
        Assert.Equal(AutonomyLevel.Autonomous, config.TargetLevel);
        Assert.Equal(50, config.MaxExperiments);
        Assert.Equal(0.15, config.MinCoherence);
        Assert.Equal(0.60, config.MinPropertyCoherence);
        Assert.Equal(0.3, config.ExplorationRate);
        Assert.Equal(EGameGenre.OpenWorld, config.Genre);
        Assert.Equal(ETrainingMode.Supervised, config.TrainingMode);
    }
}
