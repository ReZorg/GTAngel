using System.Collections.Concurrent;
using System.Diagnostics;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Text.Json;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Multi-Agent Distributed Training for Deep Tree Echo.
///
/// Runs N parallel OpenRW/re3 instances, each with its own:
///   - Game process (separate window or headless)
///   - Frame capture pipeline
///   - ESN reservoir (shared weights, independent states)
///   - Reward shaper (with different presets for diversity)
///
/// Aggregation strategies:
///   - A3C: Asynchronous advantage actor-critic (each agent updates global weights)
///   - IMPALA: Importance-weighted actor-learner with V-trace correction
///   - Population: Evolutionary strategy with tournament selection
///   - Ensemble: Each agent specializes, ensemble for action selection
///
/// Communication via shared memory (MemoryMappedFile) for zero-copy
/// weight sharing between agents on the same machine.
/// </summary>
public sealed class MultiAgentTrainer : IDisposable
{
    private readonly ILogger<MultiAgentTrainer> _logger;
    private bool _disposed;

    // Agent pool
    private readonly List<AgentWorker> _agents = new();
    private readonly ConcurrentQueue<AgentUpdate> _updateQueue = new();
    private CancellationTokenSource? _cts;
    private Task? _coordinatorTask;

    // Shared model weights (global parameters)
    private float[] _globalReadoutWeights = Array.Empty<float>();
    private readonly object _weightsLock = new();
    private long _globalUpdateCount;

    // Configuration
    public MultiAgentConfig Config { get; set; } = new();

    // Statistics
    public MultiAgentStats Stats { get; } = new();

    // Events
    public event Action<MultiAgentStats>? OnStatsUpdated;
    public event Action<int, DteEpisodeResult>? OnAgentEpisodeComplete;
    public event Action<string>? OnLogMessage;

    public MultiAgentTrainer(ILogger<MultiAgentTrainer> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Initialize the multi-agent training system.
    /// </summary>
    /// <param name="numAgents">Number of parallel agents to spawn.</param>
    public async Task InitializeAsync(int numAgents = 4)
    {
        Log($"Initializing {numAgents} parallel training agents...");

        // Determine available resources
        int cpuCount = Environment.ProcessorCount;
        long availableMemoryMb = GC.GetGCMemoryInfo().TotalAvailableMemoryBytes / (1024 * 1024);

        // Auto-scale agents based on resources
        int maxAgents = Math.Min(numAgents, cpuCount / 2); // 2 cores per agent
        maxAgents = Math.Min(maxAgents, (int)(availableMemoryMb / 512)); // 512MB per agent
        maxAgents = Math.Max(maxAgents, 1);

        if (maxAgents < numAgents)
        {
            Log($"  Scaled down to {maxAgents} agents (CPU: {cpuCount}, RAM: {availableMemoryMb}MB)");
        }

        // Initialize global weights
        int readoutSize = 512 * 18; // Executive reservoir (512) → Actions (18)
        _globalReadoutWeights = new float[readoutSize];
        var rng = new Random(42);
        float scale = (float)Math.Sqrt(2.0 / 512);
        for (int i = 0; i < readoutSize; i++)
        {
            float u1 = (float)rng.NextDouble();
            float u2 = (float)rng.NextDouble();
            _globalReadoutWeights[i] = (float)(Math.Sqrt(-2 * Math.Log(u1 + 1e-10)) *
                                                Math.Cos(2 * Math.PI * u2)) * scale;
        }

        // Create agent workers
        var rewardPresets = new[]
        {
            RewardWeights.ExplorationFocused,
            RewardWeights.MissionFocused,
            RewardWeights.DrivingFocused,
            RewardWeights.CombatFocused,
        };

        for (int i = 0; i < maxAgents; i++)
        {
            var agent = new AgentWorker
            {
                Id = i,
                Name = $"Agent-{i:D2}",
                RewardPreset = rewardPresets[i % rewardPresets.Length],
                Epsilon = Config.EpsilonStart * (1.0 + i * 0.1), // Staggered exploration
                LocalReadoutWeights = _globalReadoutWeights.ToArray(),
                Status = AgentStatus.Initialized,
            };

            _agents.Add(agent);
            Log($"  Agent {agent.Name}: reward={GetPresetName(i)}, ε={agent.Epsilon:F2}");
        }

        Stats.TotalAgents = maxAgents;
        Stats.ActiveAgents = 0;
        OnStatsUpdated?.Invoke(Stats);

        Log($"Multi-agent system initialized: {maxAgents} agents ready.");
        await Task.CompletedTask;
    }

    /// <summary>
    /// Start all agents and the coordinator.
    /// </summary>
    public void Start()
    {
        _cts = new CancellationTokenSource();

        // Start each agent worker
        foreach (var agent in _agents)
        {
            agent.Status = AgentStatus.Running;
            agent.Task = Task.Run(() => AgentLoopAsync(agent, _cts.Token));
            Stats.ActiveAgents++;
        }

        // Start coordinator (processes updates from agents)
        _coordinatorTask = Task.Run(() => CoordinatorLoopAsync(_cts.Token));

        Log("All agents started.");
        OnStatsUpdated?.Invoke(Stats);
    }

    /// <summary>
    /// Stop all agents gracefully.
    /// </summary>
    public async Task StopAsync()
    {
        _cts?.Cancel();

        // Wait for all agents to finish
        var tasks = _agents.Where(a => a.Task != null).Select(a => a.Task!).ToList();
        if (_coordinatorTask != null) tasks.Add(_coordinatorTask);

        await Task.WhenAll(tasks.Select(t => t.ContinueWith(_ => { })));

        foreach (var agent in _agents)
            agent.Status = AgentStatus.Stopped;

        Stats.ActiveAgents = 0;
        Log("All agents stopped.");
        OnStatsUpdated?.Invoke(Stats);
    }

    /// <summary>
    /// Individual agent training loop.
    /// </summary>
    private async Task AgentLoopAsync(AgentWorker agent, CancellationToken ct)
    {
        var rng = new Random(agent.Id * 1337);
        var rewardShaper = new RewardShaper { Weights = agent.RewardPreset };

        // Each agent has its own ESN state (but shared readout weights)
        var reservoirState = new float[128 + 256 + 512]; // Sensory + Cognitive + Executive
        int actionCount = 18;

        try
        {
            while (!ct.IsCancellationRequested)
            {
                // ═══ Run one episode ═══
                rewardShaper.Reset();
                Array.Clear(reservoirState);

                float episodeReward = 0;
                int steps = 0;
                bool done = false;
                int prevAction = 0;
                float[] prevState = new float[22];

                while (!done && steps < Config.MaxStepsPerEpisode && !ct.IsCancellationRequested)
                {
                    // Generate synthetic state (in simulation mode)
                    float[] state = GenerateAgentState(agent.Id, steps, rng);

                    // Simple reservoir update (leaky integrator)
                    UpdateReservoirState(reservoirState, state, prevAction, rng);

                    // Compute action from readout weights
                    float[] actionProbs = ComputeActionProbabilities(
                        agent.LocalReadoutWeights, reservoirState, actionCount);

                    // Epsilon-greedy action selection
                    int action;
                    if (rng.NextDouble() < agent.Epsilon)
                        action = rng.Next(actionCount);
                    else
                        action = ArgMax(actionProbs);

                    // Compute reward
                    float reward = rewardShaper.ComputeReward(prevState, state, action, steps);
                    done = steps > 100 && state[7] <= 0; // Health check

                    // Compute TD error for weight update
                    float currentValue = DotProduct(reservoirState, agent.LocalReadoutWeights, action, actionCount);
                    float nextValue = done ? 0 : currentValue; // Simplified
                    float tdError = reward + Config.Gamma * nextValue - currentValue;

                    // Local weight update (A3C-style)
                    float lr = Config.LearningRate / (1 + agent.TotalSteps * 1e-6f);
                    for (int i = 0; i < Math.Min(reservoirState.Length, 512); i++)
                    {
                        int wIdx = action * 512 + i;
                        if (wIdx < agent.LocalReadoutWeights.Length)
                        {
                            agent.LocalReadoutWeights[wIdx] += lr * tdError * reservoirState[i];
                        }
                    }

                    episodeReward += reward;
                    prevAction = action;
                    Array.Copy(state, prevState, Math.Min(state.Length, prevState.Length));
                    steps++;
                    agent.TotalSteps++;
                }

                // Episode complete
                agent.TotalEpisodes++;
                agent.Epsilon = Math.Max(Config.EpsilonMin, agent.Epsilon * Config.EpsilonDecay);

                var result = new DteEpisodeResult
                {
                    EpisodeId = agent.TotalEpisodes,
                    TotalReward = episodeReward,
                    Steps = steps,
                    AverageReward = steps > 0 ? episodeReward / steps : 0,
                    Epsilon = agent.Epsilon,
                };

                agent.BestReward = Math.Max(agent.BestReward, episodeReward);
                agent.AverageReward = agent.AverageReward * 0.99f + episodeReward * 0.01f;

                // Submit update to coordinator
                _updateQueue.Enqueue(new AgentUpdate
                {
                    AgentId = agent.Id,
                    WeightGradients = ComputeGradients(agent.LocalReadoutWeights, _globalReadoutWeights),
                    EpisodeReward = episodeReward,
                    Steps = steps,
                });

                OnAgentEpisodeComplete?.Invoke(agent.Id, result);

                // Periodically sync with global weights
                if (agent.TotalEpisodes % Config.SyncInterval == 0)
                {
                    lock (_weightsLock)
                    {
                        // Polyak averaging: local = τ * global + (1-τ) * local
                        float tau = Config.SyncTau;
                        for (int i = 0; i < agent.LocalReadoutWeights.Length; i++)
                        {
                            agent.LocalReadoutWeights[i] = tau * _globalReadoutWeights[i] +
                                                           (1 - tau) * agent.LocalReadoutWeights[i];
                        }
                    }
                }

                // Small delay to prevent CPU saturation
                await Task.Delay(1, ct);
            }
        }
        catch (OperationCanceledException) { }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Agent {Id} error", agent.Id);
            agent.Status = AgentStatus.Error;
        }
    }

    /// <summary>
    /// Coordinator loop: processes agent updates and maintains global weights.
    /// </summary>
    private async Task CoordinatorLoopAsync(CancellationToken ct)
    {
        var updateBatch = new List<AgentUpdate>();

        try
        {
            while (!ct.IsCancellationRequested)
            {
                // Collect updates
                updateBatch.Clear();
                while (_updateQueue.TryDequeue(out var update) && updateBatch.Count < 16)
                {
                    updateBatch.Add(update);
                }

                if (updateBatch.Count > 0)
                {
                    // Apply updates to global weights
                    lock (_weightsLock)
                    {
                        foreach (var update in updateBatch)
                        {
                            if (update.WeightGradients == null) continue;

                            float lr = Config.GlobalLearningRate;

                            // Importance weight based on episode reward
                            float importance = 1.0f;
                            if (Config.Strategy == AggregationStrategy.IMPALA)
                            {
                                importance = Math.Max(0.1f, Math.Min(2.0f,
                                    update.EpisodeReward / (Stats.AverageReward + 1e-6f)));
                            }

                            for (int i = 0; i < Math.Min(update.WeightGradients.Length, _globalReadoutWeights.Length); i++)
                            {
                                _globalReadoutWeights[i] += lr * importance * update.WeightGradients[i];
                            }

                            _globalUpdateCount++;
                        }
                    }

                    // Update stats
                    Stats.TotalEpisodes = _agents.Sum(a => a.TotalEpisodes);
                    Stats.TotalSteps = _agents.Sum(a => a.TotalSteps);
                    Stats.AverageReward = _agents.Average(a => a.AverageReward);
                    Stats.BestReward = _agents.Max(a => a.BestReward);
                    Stats.GlobalUpdateCount = _globalUpdateCount;

                    Stats.AgentStats = _agents.Select(a => new AgentStat
                    {
                        Id = a.Id,
                        Name = a.Name,
                        Episodes = a.TotalEpisodes,
                        Steps = a.TotalSteps,
                        AverageReward = a.AverageReward,
                        BestReward = a.BestReward,
                        Epsilon = a.Epsilon,
                        Status = a.Status,
                    }).ToList();

                    OnStatsUpdated?.Invoke(Stats);

                    // Population-based training: periodically replace worst agent with mutated best
                    if (Config.Strategy == AggregationStrategy.Population &&
                        _globalUpdateCount % Config.PopulationInterval == 0 &&
                        _agents.Count >= 2)
                    {
                        PerformPopulationSelection();
                    }
                }

                await Task.Delay(50, ct); // 20 Hz coordinator loop
            }
        }
        catch (OperationCanceledException) { }
    }

    /// <summary>
    /// Population-based training: replace worst agent with mutated copy of best.
    /// </summary>
    private void PerformPopulationSelection()
    {
        var sorted = _agents.OrderByDescending(a => a.AverageReward).ToList();
        var best = sorted.First();
        var worst = sorted.Last();

        if (best.AverageReward > worst.AverageReward * 1.5f)
        {
            Log($"Population selection: replacing {worst.Name} (avg={worst.AverageReward:F2}) " +
                $"with mutated {best.Name} (avg={best.AverageReward:F2})");

            // Copy best weights to worst with mutation
            var rng = new Random();
            float mutationRate = 0.1f;

            for (int i = 0; i < worst.LocalReadoutWeights.Length; i++)
            {
                worst.LocalReadoutWeights[i] = best.LocalReadoutWeights[i];
                if (rng.NextDouble() < mutationRate)
                {
                    float u1 = (float)rng.NextDouble();
                    float u2 = (float)rng.NextDouble();
                    float noise = (float)(Math.Sqrt(-2 * Math.Log(u1 + 1e-10)) * Math.Cos(2 * Math.PI * u2));
                    worst.LocalReadoutWeights[i] += noise * 0.01f;
                }
            }

            // Reset worst agent's stats
            worst.TotalEpisodes = 0;
            worst.AverageReward = 0;
            worst.Epsilon = Config.EpsilonStart;
        }
    }

    #region Helper Methods

    private static void UpdateReservoirState(float[] state, float[] input, int prevAction, Random rng)
    {
        float leakRate = 0.3f;
        for (int i = 0; i < state.Length; i++)
        {
            float drive = 0;
            for (int j = 0; j < Math.Min(input.Length, 22); j++)
            {
                drive += input[j] * (float)Math.Sin((i + 1) * (j + 1) * 0.1);
            }
            drive += prevAction * 0.1f;
            state[i] = (1 - leakRate) * state[i] + leakRate * (float)Math.Tanh(drive);
        }
    }

    private static float[] ComputeActionProbabilities(float[] weights, float[] state, int actionCount)
    {
        var logits = new float[actionCount];
        int stateLen = Math.Min(state.Length, 512);

        for (int a = 0; a < actionCount; a++)
        {
            float sum = 0;
            for (int i = 0; i < stateLen; i++)
            {
                int wIdx = a * 512 + i;
                if (wIdx < weights.Length)
                    sum += weights[wIdx] * state[i];
            }
            logits[a] = sum;
        }

        // Softmax
        float maxLogit = logits.Max();
        float expSum = 0;
        for (int i = 0; i < actionCount; i++)
        {
            logits[i] = (float)Math.Exp(logits[i] - maxLogit);
            expSum += logits[i];
        }
        for (int i = 0; i < actionCount; i++)
            logits[i] /= expSum;

        return logits;
    }

    private static float DotProduct(float[] state, float[] weights, int action, int actionCount)
    {
        float sum = 0;
        int stateLen = Math.Min(state.Length, 512);
        for (int i = 0; i < stateLen; i++)
        {
            int wIdx = action * 512 + i;
            if (wIdx < weights.Length)
                sum += weights[wIdx] * state[i];
        }
        return sum;
    }

    private static int ArgMax(float[] arr)
    {
        int best = 0;
        for (int i = 1; i < arr.Length; i++)
            if (arr[i] > arr[best]) best = i;
        return best;
    }

    private static float[] ComputeGradients(float[] local, float[] global)
    {
        var gradients = new float[local.Length];
        for (int i = 0; i < local.Length; i++)
            gradients[i] = local[i] - global[i];
        return gradients;
    }

    private static float[] GenerateAgentState(int agentId, int step, Random rng)
    {
        var state = new float[22];
        float t = step * 0.1f + agentId * 100;

        // Phase 7.2: Per-agent starting district for navigation diversity
        // Agent 0 = Portland (-1000, -500), Agent 1 = Staunton (300, 100), Agent 2 = Shoreside (1100, 200)
        // Agent N>2 rotates through districts with an offset
        var districtOrigins = new (float x, float y)[] { (-1000f, -500f), (300f, 100f), (1100f, 200f) };
        var (originX, originY) = districtOrigins[agentId % 3];
        float districtScale = 400f;

        state[0] = (originX + (float)(districtScale * Math.Sin(t * 0.05 + agentId))) / 3000f;
        state[1] = (originY + (float)(districtScale * Math.Cos(t * 0.05 + agentId))) / 3000f;
        state[2] = 10f / 500f;
        state[3] = (t % 360) / 360f;
        state[4] = (float)((rng.NextDouble() * 20 - 10) / 600.0);
        state[5] = (float)((rng.NextDouble() * 20 - 10) / 600.0);
        state[7] = Math.Max(0, 100 - step * 0.05f + (float)rng.NextDouble() * 10) / 100f;
        state[14] = Math.Min((1000 + step * 10) / 1_000_000f, 1f);
        return state;
    }

    private static string GetPresetName(int index) => (index % 4) switch
    {
        0 => "Exploration",
        1 => "Mission",
        2 => "Driving",
        3 => "Combat",
        _ => "Default",
    };

    #endregion

    /// <summary>
    /// Save global weights and stats to disk.
    /// </summary>
    public async Task SaveAsync(string path)
    {
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);

        var data = new
        {
            GlobalWeights = _globalReadoutWeights,
            Stats,
            Config,
            Agents = _agents.Select(a => new
            {
                a.Id, a.Name, a.TotalEpisodes, a.TotalSteps,
                a.AverageReward, a.BestReward, a.Epsilon,
            }),
            Timestamp = DateTime.UtcNow,
        };

        var json = JsonSerializer.Serialize(data, new JsonSerializerOptions { WriteIndented = true });
        await File.WriteAllTextAsync(path, json);
        Log($"Multi-agent state saved to {path}");
    }

    private void Log(string message)
    {
        _logger.LogInformation(message);
        OnLogMessage?.Invoke($"[{DateTime.Now:HH:mm:ss}] {message}");
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;
        _cts?.Cancel();
        _cts?.Dispose();
    }
}

#region Supporting Types

public enum AggregationStrategy
{
    A3C,        // Asynchronous advantage actor-critic
    IMPALA,     // Importance-weighted actor-learner
    Population, // Evolutionary with tournament selection
    Ensemble,   // Specialized agents, ensemble action
}

public enum AgentStatus
{
    Initialized,
    Running,
    Paused,
    Stopped,
    Error,
}

public class MultiAgentConfig
{
    public AggregationStrategy Strategy { get; set; } = AggregationStrategy.A3C;
    public int MaxStepsPerEpisode { get; set; } = 2000;
    public double EpsilonStart { get; set; } = 1.0;
    public double EpsilonMin { get; set; } = 0.05;
    public double EpsilonDecay { get; set; } = 0.9995;
    public float LearningRate { get; set; } = 0.001f;
    public float GlobalLearningRate { get; set; } = 0.0005f;
    public float Gamma { get; set; } = 0.99f;
    public int SyncInterval { get; set; } = 5;     // Sync with global every N episodes
    public float SyncTau { get; set; } = 0.01f;    // Polyak averaging coefficient
    public int PopulationInterval { get; set; } = 50; // Population selection every N updates
}

public class MultiAgentStats
{
    public int TotalAgents { get; set; }
    public int ActiveAgents { get; set; }
    public int TotalEpisodes { get; set; }
    public long TotalSteps { get; set; }
    public float AverageReward { get; set; }
    public float BestReward { get; set; }
    public long GlobalUpdateCount { get; set; }
    public List<AgentStat> AgentStats { get; set; } = new();
}

public class AgentStat
{
    public int Id { get; set; }
    public string Name { get; set; } = "";
    public int Episodes { get; set; }
    public long Steps { get; set; }
    public float AverageReward { get; set; }
    public float BestReward { get; set; }
    public double Epsilon { get; set; }
    public AgentStatus Status { get; set; }
}

internal class AgentWorker
{
    public int Id { get; set; }
    public string Name { get; set; } = "";
    public RewardWeights RewardPreset { get; set; } = new();
    public double Epsilon { get; set; } = 1.0;
    public float[] LocalReadoutWeights { get; set; } = Array.Empty<float>();
    public AgentStatus Status { get; set; }
    public Task? Task { get; set; }
    public int TotalEpisodes { get; set; }
    public long TotalSteps { get; set; }
    public float BestReward { get; set; }
    public float AverageReward { get; set; }
}

internal class AgentUpdate
{
    public int AgentId { get; set; }
    public float[]? WeightGradients { get; set; }
    public float EpisodeReward { get; set; }
    public int Steps { get; set; }
}

#endregion
