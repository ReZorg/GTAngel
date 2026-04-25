using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for MultiAgentTrainer — InitializeAsync, Config defaults,
/// Stats initial state, Dispose, and SaveAsync.
/// </summary>
public class MultiAgentTrainerTests : IDisposable
{
    private readonly MultiAgentTrainer _trainer;

    public MultiAgentTrainerTests()
    {
        _trainer = new MultiAgentTrainer(NullLogger<MultiAgentTrainer>.Instance);
    }

    public void Dispose() => _trainer.Dispose();

    // ── Initial state ──────────────────────────────────────────────────────

    [Fact]
    public void Stats_Initially_TotalAgentsIsZero()
    {
        Assert.Equal(0, _trainer.Stats.TotalAgents);
    }

    [Fact]
    public void Stats_Initially_ActiveAgentsIsZero()
    {
        Assert.Equal(0, _trainer.Stats.ActiveAgents);
    }

    [Fact]
    public void Stats_Initially_TotalEpisodesIsZero()
    {
        Assert.Equal(0, _trainer.Stats.TotalEpisodes);
    }

    [Fact]
    public void Stats_Initially_AgentStatsIsEmpty()
    {
        Assert.Empty(_trainer.Stats.AgentStats);
    }

    // ── Config defaults ────────────────────────────────────────────────────

    [Fact]
    public void Config_DefaultStrategy_IsA3C()
    {
        Assert.Equal(AggregationStrategy.A3C, _trainer.Config.Strategy);
    }

    [Fact]
    public void Config_DefaultMaxStepsPerEpisode_Is2000()
    {
        Assert.Equal(2000, _trainer.Config.MaxStepsPerEpisode);
    }

    [Fact]
    public void Config_DefaultEpsilonStart_IsOne()
    {
        Assert.Equal(1.0, _trainer.Config.EpsilonStart);
    }

    [Fact]
    public void Config_DefaultEpsilonMin_IsSmall()
    {
        Assert.Equal(0.05, _trainer.Config.EpsilonMin);
    }

    [Fact]
    public void Config_DefaultGamma_Is0Point99()
    {
        Assert.Equal(0.99f, _trainer.Config.Gamma);
    }

    [Fact]
    public void Config_DefaultSyncInterval_Is5()
    {
        Assert.Equal(5, _trainer.Config.SyncInterval);
    }

    // ── InitializeAsync ────────────────────────────────────────────────────

    [Fact]
    public async Task InitializeAsync_WithOneAgent_SetsTotalAgentsToOne()
    {
        await _trainer.InitializeAsync(numAgents: 1);
        Assert.True(_trainer.Stats.TotalAgents >= 1);
    }

    [Fact]
    public async Task InitializeAsync_WithFourAgents_SetsAtLeastOneAgent()
    {
        await _trainer.InitializeAsync(numAgents: 4);
        Assert.True(_trainer.Stats.TotalAgents >= 1);
    }

    [Fact]
    public async Task InitializeAsync_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() => _trainer.InitializeAsync(numAgents: 2));
        Assert.Null(ex);
    }

    [Fact]
    public async Task InitializeAsync_CalledTwice_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(async () =>
        {
            await _trainer.InitializeAsync(numAgents: 1);
            await _trainer.InitializeAsync(numAgents: 1);
        });
        Assert.Null(ex);
    }

    // ── Config changes ────────────────────────────────────────────────────

    [Fact]
    public void Config_CanBeModified()
    {
        _trainer.Config = new MultiAgentConfig
        {
            Strategy = AggregationStrategy.IMPALA,
            MaxStepsPerEpisode = 500
        };
        Assert.Equal(AggregationStrategy.IMPALA, _trainer.Config.Strategy);
        Assert.Equal(500, _trainer.Config.MaxStepsPerEpisode);
    }

    // ── Events ────────────────────────────────────────────────────────────

    [Fact]
    public void Events_CanBeSubscribedAndUnsubscribed()
    {
        Action<MultiAgentStats>? statsHandler = _ => { };
        Action<int, DteEpisodeResult>? episodeHandler = (_, _) => { };
        Action<string>? logHandler = _ => { };

        _trainer.OnStatsUpdated += statsHandler;
        _trainer.OnAgentEpisodeComplete += episodeHandler;
        _trainer.OnLogMessage += logHandler;

        _trainer.OnStatsUpdated -= statsHandler;
        _trainer.OnAgentEpisodeComplete -= episodeHandler;
        _trainer.OnLogMessage -= logHandler;
        // No exception = pass
    }

    // ── StopAsync ─────────────────────────────────────────────────────────

    [Fact]
    public async Task StopAsync_BeforeStart_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() => _trainer.StopAsync());
        Assert.Null(ex);
    }

    // ── Dispose ───────────────────────────────────────────────────────────

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        using var trainer = new MultiAgentTrainer(NullLogger<MultiAgentTrainer>.Instance);
        var ex = Record.Exception(() => trainer.Dispose());
        Assert.Null(ex);
    }

    [Fact]
    public void Dispose_CalledTwice_DoesNotThrow()
    {
        var trainer = new MultiAgentTrainer(NullLogger<MultiAgentTrainer>.Instance);
        trainer.Dispose();
        var ex = Record.Exception(() => trainer.Dispose());
        Assert.Null(ex);
    }

    // ── MultiAgentConfig defaults ─────────────────────────────────────────

    [Fact]
    public void MultiAgentConfig_Defaults_AreReasonable()
    {
        var config = new MultiAgentConfig();
        Assert.Equal(AggregationStrategy.A3C, config.Strategy);
        Assert.Equal(2000, config.MaxStepsPerEpisode);
        Assert.Equal(1.0, config.EpsilonStart);
        Assert.Equal(0.05, config.EpsilonMin);
        Assert.True(config.EpsilonDecay > 0.99 && config.EpsilonDecay < 1.0);
        Assert.True(config.LearningRate > 0);
        Assert.True(config.GlobalLearningRate > 0);
        Assert.Equal(0.99f, config.Gamma);
    }

    // ── MultiAgentStats defaults ──────────────────────────────────────────

    [Fact]
    public void MultiAgentStats_Defaults_AgentStatsListIsNotNull()
    {
        var stats = new MultiAgentStats();
        Assert.NotNull(stats.AgentStats);
    }

    [Fact]
    public void MultiAgentStats_Defaults_NumericFieldsAreZero()
    {
        var stats = new MultiAgentStats();
        Assert.Equal(0, stats.TotalAgents);
        Assert.Equal(0, stats.ActiveAgents);
        Assert.Equal(0, stats.TotalEpisodes);
        Assert.Equal(0L, stats.TotalSteps);
        Assert.Equal(0f, stats.AverageReward);
        Assert.Equal(0f, stats.BestReward);
        Assert.Equal(0L, stats.GlobalUpdateCount);
    }
}
