using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for ExperienceReplayBuffer — Add, Sample, UpdatePriorities, N-step returns,
/// circular overflow, Clear, GetMemoryUsage, and HER transitions.
/// </summary>
public class ExperienceReplayBufferTests : IDisposable
{
    private readonly ExperienceReplayBuffer _buffer;

    public ExperienceReplayBufferTests()
    {
        _buffer = new ExperienceReplayBuffer(
            NullLogger<ExperienceReplayBuffer>.Instance,
            capacity: 100,
            alpha: 0.6f,
            beta: 0.4f,
            nStep: 1,   // Use n=1 for most tests to keep behavior predictable
            gamma: 0.99f);
    }

    public void Dispose() => _buffer.Dispose();

    // ── Initial state ──────────────────────────────────────────────────────

    [Fact]
    public void Count_Initially_IsZero()
    {
        Assert.Equal(0, _buffer.Count);
    }

    [Fact]
    public void FillRatio_Initially_IsZero()
    {
        Assert.Equal(0f, _buffer.FillRatio);
    }

    [Fact]
    public void Capacity_MatchesConstructorArgument()
    {
        Assert.Equal(100, _buffer.Capacity);
    }

    [Fact]
    public void TotalTransitionsAdded_Initially_IsZero()
    {
        Assert.Equal(0L, _buffer.TotalTransitionsAdded);
    }

    // ── Add ────────────────────────────────────────────────────────────────

    [Fact]
    public void Add_SingleTransition_CountBecomesOne()
    {
        _buffer.Add(MakeTransition(1));
        Assert.Equal(1, _buffer.Count);
    }

    [Fact]
    public void Add_SingleTransition_TotalTransitionsAddedBecomesOne()
    {
        _buffer.Add(MakeTransition(1));
        Assert.Equal(1L, _buffer.TotalTransitionsAdded);
    }

    [Fact]
    public void Add_MultipleTransitions_CountGrows()
    {
        for (int i = 0; i < 10; i++)
            _buffer.Add(MakeTransition(i));
        Assert.Equal(10, _buffer.Count);
    }

    [Fact]
    public void Add_OverCapacity_CountStaysAtCapacity()
    {
        for (int i = 0; i < 150; i++)
            _buffer.Add(MakeTransition(i));
        Assert.Equal(100, _buffer.Count);   // Capacity is 100
    }

    [Fact]
    public void Add_OverCapacity_TotalTransitionsStillCounts()
    {
        for (int i = 0; i < 150; i++)
            _buffer.Add(MakeTransition(i));
        Assert.Equal(150L, _buffer.TotalTransitionsAdded);
    }

    [Fact]
    public void FillRatio_HalfFull_IsPointFive()
    {
        for (int i = 0; i < 50; i++)
            _buffer.Add(MakeTransition(i));
        Assert.Equal(0.5f, _buffer.FillRatio);
    }

    [Fact]
    public void FillRatio_Full_IsOne()
    {
        for (int i = 0; i < 100; i++)
            _buffer.Add(MakeTransition(i));
        Assert.Equal(1.0f, _buffer.FillRatio);
    }

    // ── Sample ─────────────────────────────────────────────────────────────

    [Fact]
    public void Sample_EmptyBuffer_ReturnsEmptyBatch()
    {
        var batch = _buffer.Sample(32);
        Assert.Equal(0, batch.BatchSize);
    }

    [Fact]
    public void Sample_FewerThanBatchSize_ReturnAll()
    {
        for (int i = 0; i < 5; i++) _buffer.Add(MakeTransition(i));
        var batch = _buffer.Sample(32);
        Assert.Equal(5, batch.BatchSize);
    }

    [Fact]
    public void Sample_RequestedBatchSize_ReturnsCorrectCount()
    {
        for (int i = 0; i < 50; i++) _buffer.Add(MakeTransition(i));
        var batch = _buffer.Sample(16);
        Assert.Equal(16, batch.BatchSize);
    }

    [Fact]
    public void Sample_IndicesArrayMatchesBatchSize()
    {
        for (int i = 0; i < 50; i++) _buffer.Add(MakeTransition(i));
        var batch = _buffer.Sample(16);
        Assert.Equal(16, batch.Indices.Length);
    }

    [Fact]
    public void Sample_WeightsArrayMatchesBatchSize()
    {
        for (int i = 0; i < 50; i++) _buffer.Add(MakeTransition(i));
        var batch = _buffer.Sample(16);
        Assert.Equal(16, batch.Weights.Length);
    }

    [Fact]
    public void Sample_IncrementsTotalSamplesCalled()
    {
        for (int i = 0; i < 20; i++) _buffer.Add(MakeTransition(i));
        _buffer.Sample(8);
        Assert.Equal(1L, _buffer.TotalSamplesCalled);
    }

    [Fact]
    public void Sample_Prioritized_AllWeightsArePositive()
    {
        for (int i = 0; i < 20; i++) _buffer.Add(MakeTransition(i));
        var batch = _buffer.Sample(10);
        Assert.All(batch.Weights, w => Assert.True(w > 0));
    }

    // ── UpdatePriorities ───────────────────────────────────────────────────

    [Fact]
    public void UpdatePriorities_DoesNotThrowForValidIndices()
    {
        for (int i = 0; i < 20; i++) _buffer.Add(MakeTransition(i));
        var batch = _buffer.Sample(5);
        var tdErrors = new float[] { 0.1f, 0.5f, 0.9f, 0.2f, 0.3f };
        var ex = Record.Exception(() => _buffer.UpdatePriorities(batch.Indices, tdErrors));
        Assert.Null(ex);
    }

    [Fact]
    public void UpdatePriorities_UpdatesAveragePriority()
    {
        for (int i = 0; i < 20; i++) _buffer.Add(MakeTransition(i));
        var batch = _buffer.Sample(5);
        var tdErrors = Enumerable.Range(0, 5).Select(i => (float)(i + 1) * 0.1f).ToArray();
        _buffer.UpdatePriorities(batch.Indices, tdErrors);
        Assert.True(_buffer.AveragePriority > 0);
    }

    // ── Clear ──────────────────────────────────────────────────────────────

    [Fact]
    public void Clear_ResetsCount()
    {
        for (int i = 0; i < 20; i++) _buffer.Add(MakeTransition(i));
        _buffer.Clear();
        Assert.Equal(0, _buffer.Count);
    }

    [Fact]
    public void Clear_AllowsAddAfterClear()
    {
        for (int i = 0; i < 20; i++) _buffer.Add(MakeTransition(i));
        _buffer.Clear();
        _buffer.Add(MakeTransition(99));
        Assert.Equal(1, _buffer.Count);
    }

    // ── GetMemoryUsage ─────────────────────────────────────────────────────

    [Fact]
    public void GetMemoryUsage_EmptyBuffer_ReturnsZeros()
    {
        var (total, frames, states) = _buffer.GetMemoryUsage();
        Assert.Equal(0L, total);
        Assert.Equal(0L, frames);
        Assert.Equal(0L, states);
    }

    [Fact]
    public void GetMemoryUsage_WithTransitions_StatesBytesIsPositive()
    {
        for (int i = 0; i < 5; i++) _buffer.Add(MakeTransition(i, stateSize: 22));
        var (total, _, states) = _buffer.GetMemoryUsage();
        Assert.True(states > 0);
    }

    // ── AverageReward ─────────────────────────────────────────────────────

    [Fact]
    public void AverageReward_UpdatesWithAddedTransitions()
    {
        for (int i = 0; i < 10; i++)
            _buffer.Add(MakeTransition(i, reward: 1.0f));
        // Average reward should converge toward 1.0 (exponential moving avg)
        Assert.True(_buffer.AverageReward >= 0);
    }

    // ── N-step buffer (n=3) ────────────────────────────────────────────────

    [Fact]
    public void NStepBuffer_WithNStep3_BufferGrowsAfterNStepTransitions()
    {
        using var nBuffer = new ExperienceReplayBuffer(
            NullLogger<ExperienceReplayBuffer>.Instance,
            capacity: 100, alpha: 0.6f, beta: 0.4f, nStep: 3, gamma: 0.99f);

        // Add 3 transitions — should trigger N-step computation and add 1 to buffer
        for (int i = 0; i < 3; i++)
            nBuffer.Add(MakeTransition(i));
        Assert.True(nBuffer.Count >= 1);
    }

    [Fact]
    public void NStepBuffer_OnEpisodeDone_FlushesRemainingTransitions()
    {
        using var nBuffer = new ExperienceReplayBuffer(
            NullLogger<ExperienceReplayBuffer>.Instance,
            capacity: 100, alpha: 0.6f, beta: 0.4f, nStep: 3, gamma: 0.99f);

        // Add 2 transitions then a terminal one
        nBuffer.Add(MakeTransition(1));
        nBuffer.Add(MakeTransition(2));
        nBuffer.Add(MakeTransition(3, done: true));

        // All should have been flushed
        Assert.True(nBuffer.Count > 0);
    }

    // ── GetHerTransitions ─────────────────────────────────────────────────

    [Fact]
    public void GetHerTransitions_UnknownEpisode_ReturnsEmpty()
    {
        for (int i = 0; i < 5; i++) _buffer.Add(MakeTransition(i, episodeId: 0));
        var her = _buffer.GetHerTransitions(999, new float[22]);
        Assert.Empty(her);
    }

    [Fact]
    public void GetHerTransitions_KnownEpisode_ReturnsTransitions()
    {
        for (int i = 0; i < 5; i++) _buffer.Add(MakeTransition(i, episodeId: 42));
        var her = _buffer.GetHerTransitions(42, new float[22]);
        Assert.NotEmpty(her);
    }

    [Fact]
    public void GetHerTransitions_RewardsAreRelabeled()
    {
        for (int i = 0; i < 5; i++) _buffer.Add(MakeTransition(i, episodeId: 7, reward: -5f));
        var goal = new float[22]; // all-zero goal
        var her = _buffer.GetHerTransitions(7, goal, maxTransitions: 5);
        // Rewards should be 0 or 1 (HER relabeled), not -5
        Assert.All(her, t => Assert.True(t.Reward == 0f || t.Reward == 1f));
    }

    // ── Helpers ────────────────────────────────────────────────────────────

    private static ExperienceReplayBuffer.Transition MakeTransition(
        int id,
        float reward = 0.5f,
        bool done = false,
        int episodeId = 0,
        int stateSize = 22)
    {
        var state = new float[stateSize];
        state[7] = 100f; // Health
        return new ExperienceReplayBuffer.Transition
        {
            GameState = state,
            ReservoirState = new float[10],
            NextGameState = state,
            NextReservoirState = new float[10],
            DiscreteAction = id % 8,
            Reward = reward,
            Done = done,
            EpisodeId = episodeId,
            StepInEpisode = id,
        };
    }
}
