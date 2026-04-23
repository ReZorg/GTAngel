using System.IO;
using System.IO.Compression;
using System.Text.Json;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Experience Replay Buffer for off-policy reinforcement learning algorithms (DQN, SAC, TD3).
/// Stores (state, action, reward, next_state, done) transitions with prioritized sampling.
///
/// Features:
///   - Circular buffer with configurable capacity (default 100K transitions)
///   - Prioritized Experience Replay (PER) with proportional prioritization
///   - Compressed frame storage (frames are large: 768*768*3*4 = 7MB each)
///   - Reservoir state storage (128+256+512 = 896 floats per step)
///   - Disk persistence for long training runs
///   - N-step returns for improved sample efficiency
///   - Hindsight Experience Replay (HER) for sparse reward environments
/// </summary>
public sealed class ExperienceReplayBuffer : IDisposable
{
    private readonly ILogger<ExperienceReplayBuffer> _logger;
    private readonly Random _rng;
    private bool _disposed;

    // Buffer storage
    private readonly Transition[] _buffer;
    private int _position;
    private int _count;
    private readonly int _capacity;

    // Prioritized replay
    private readonly float[] _priorities;
    private float _maxPriority = 1.0f;
    private readonly float _alpha; // Priority exponent (0 = uniform, 1 = full prioritization)
    private readonly float _beta;  // Importance sampling correction

    // N-step returns
    private readonly int _nStep;
    private readonly float _gamma; // Discount factor
    private readonly Queue<Transition> _nStepBuffer;

    // Statistics
    public int Count => _count;
    public int Capacity => _capacity;
    public float FillRatio => (float)_count / _capacity;
    public long TotalTransitionsAdded { get; private set; }
    public long TotalSamplesCalled { get; private set; }
    public double AverageReward { get; private set; }
    public double AveragePriority { get; private set; }

    // Disk persistence
    private string? _persistPath;
    private bool _autoPersist;

    /// <summary>
    /// Enable automatic persistence to disk every 10K transitions.
    /// </summary>
    public void EnableAutoPersist(string path)
    {
        _persistPath = path;
        _autoPersist = true;
        var dir = Path.GetDirectoryName(path);
        if (dir != null && !Directory.Exists(dir))
            Directory.CreateDirectory(dir);
        _logger.LogInformation("Auto-persist enabled: {Path}", path);
    }

    /// <summary>
    /// A single experience transition.
    /// </summary>
    public class Transition
    {
        /// <summary>Compressed frame data (PNG bytes) — null if frame storage disabled.</summary>
        public byte[]? FrameCompressed { get; set; }

        /// <summary>Game state feature vector [22-dim].</summary>
        public float[] GameState { get; set; } = Array.Empty<float>();

        /// <summary>Full ESN reservoir state [896-dim].</summary>
        public float[] ReservoirState { get; set; } = Array.Empty<float>();

        /// <summary>Action taken (discrete index or continuous vector).</summary>
        public int DiscreteAction { get; set; }
        public float[]? ContinuousAction { get; set; }

        /// <summary>Reward received.</summary>
        public float Reward { get; set; }

        /// <summary>Next state (game state after action).</summary>
        public float[] NextGameState { get; set; } = Array.Empty<float>();

        /// <summary>Next reservoir state.</summary>
        public float[] NextReservoirState { get; set; } = Array.Empty<float>();

        /// <summary>Whether the episode ended after this transition.</summary>
        public bool Done { get; set; }

        /// <summary>N-step discounted return (if using N-step).</summary>
        public float NStepReturn { get; set; }

        /// <summary>Timestamp of the transition.</summary>
        public long TimestampTicks { get; set; }

        /// <summary>Episode index.</summary>
        public int EpisodeId { get; set; }

        /// <summary>Step within the episode.</summary>
        public int StepInEpisode { get; set; }

        /// <summary>TD error for priority calculation.</summary>
        public float TdError { get; set; }

        /// <summary>Approximate memory size in bytes.</summary>
        public long ApproximateSize =>
            (FrameCompressed?.Length ?? 0) +
            GameState.Length * 4 +
            ReservoirState.Length * 4 +
            (ContinuousAction?.Length ?? 0) * 4 +
            NextGameState.Length * 4 +
            NextReservoirState.Length * 4 +
            64; // overhead
    }

    /// <summary>
    /// A batch of sampled transitions for training.
    /// </summary>
    public class SampleBatch
    {
        public Transition[] Transitions { get; set; } = Array.Empty<Transition>();
        public int[] Indices { get; set; } = Array.Empty<int>();
        public float[] Weights { get; set; } = Array.Empty<float>(); // Importance sampling weights
        public int BatchSize => Transitions.Length;
    }

    /// <summary>
    /// Create a new experience replay buffer.
    /// </summary>
    /// <param name="capacity">Maximum number of transitions to store.</param>
    /// <param name="alpha">PER priority exponent (0=uniform, 1=full priority).</param>
    /// <param name="beta">PER importance sampling correction.</param>
    /// <param name="nStep">N-step returns (1=standard, >1=multi-step).</param>
    /// <param name="gamma">Discount factor for N-step returns.</param>
    public ExperienceReplayBuffer(
        ILogger<ExperienceReplayBuffer> logger,
        int capacity = 100_000,
        float alpha = 0.6f,
        float beta = 0.4f,
        int nStep = 3,
        float gamma = 0.99f)
    {
        _logger = logger;
        _rng = new Random(42);
        _capacity = capacity;
        _alpha = alpha;
        _beta = beta;
        _nStep = nStep;
        _gamma = gamma;

        _buffer = new Transition[capacity];
        _priorities = new float[capacity];
        _nStepBuffer = new Queue<Transition>();

        _logger.LogInformation("Experience Replay Buffer initialized: capacity={Cap}, alpha={A}, beta={B}, n-step={N}",
            capacity, alpha, beta, nStep);
    }

    /// <summary>
    /// Add a transition to the buffer.
    /// </summary>
    public void Add(Transition transition)
    {
        transition.TimestampTicks = DateTime.UtcNow.Ticks;

        if (_nStep > 1)
        {
            _nStepBuffer.Enqueue(transition);
            if (_nStepBuffer.Count >= _nStep || transition.Done)
            {
                // Compute N-step return
                var nStepTransition = ComputeNStepReturn();
                if (nStepTransition != null)
                    AddToBuffer(nStepTransition);

                // Flush remaining on episode end
                if (transition.Done)
                {
                    while (_nStepBuffer.Count > 0)
                    {
                        var t = ComputeNStepReturn();
                        if (t != null) AddToBuffer(t);
                    }
                }
            }
        }
        else
        {
            AddToBuffer(transition);
        }

        // Update running average reward
        AverageReward = AverageReward * 0.999 + transition.Reward * 0.001;
    }

    /// <summary>
    /// Add a transition with frame compression.
    /// </summary>
    public void Add(float[] frame, float[] gameState, float[] reservoirState,
                    int action, float reward, float[] nextGameState, float[] nextReservoirState,
                    bool done, int episodeId, int stepInEpisode)
    {
        var transition = new Transition
        {
            FrameCompressed = CompressFrame(frame),
            GameState = gameState.ToArray(),
            ReservoirState = reservoirState.ToArray(),
            DiscreteAction = action,
            Reward = reward,
            NextGameState = nextGameState.ToArray(),
            NextReservoirState = nextReservoirState.ToArray(),
            Done = done,
            EpisodeId = episodeId,
            StepInEpisode = stepInEpisode,
        };

        Add(transition);
    }

    /// <summary>
    /// Sample a batch of transitions using prioritized replay.
    /// </summary>
    public SampleBatch Sample(int batchSize)
    {
        if (_count == 0) return new SampleBatch();

        batchSize = Math.Min(batchSize, _count);
        TotalSamplesCalled++;

        var indices = new int[batchSize];
        var weights = new float[batchSize];
        var transitions = new Transition[batchSize];

        if (_alpha > 0)
        {
            // Prioritized sampling
            SamplePrioritized(batchSize, indices, weights);
        }
        else
        {
            // Uniform sampling
            for (int i = 0; i < batchSize; i++)
            {
                indices[i] = _rng.Next(_count);
                weights[i] = 1.0f;
            }
        }

        for (int i = 0; i < batchSize; i++)
            transitions[i] = _buffer[indices[i]];

        return new SampleBatch
        {
            Transitions = transitions,
            Indices = indices,
            Weights = weights,
        };
    }

    /// <summary>
    /// Update priorities after training (based on TD errors).
    /// </summary>
    public void UpdatePriorities(int[] indices, float[] tdErrors)
    {
        for (int i = 0; i < indices.Length; i++)
        {
            if (indices[i] < _count)
            {
                float priority = Math.Abs(tdErrors[i]) + 1e-6f;
                _priorities[indices[i]] = priority;
                _maxPriority = Math.Max(_maxPriority, priority);

                if (_buffer[indices[i]] != null)
                    _buffer[indices[i]].TdError = tdErrors[i];
            }
        }

        // Update average priority
        float sum = 0;
        for (int i = 0; i < _count; i++) sum += _priorities[i];
        AveragePriority = sum / _count;
    }

    /// <summary>
    /// Get Hindsight Experience Replay (HER) transitions.
    /// Replays failed episodes with achieved goals as desired goals.
    /// </summary>
    public List<Transition> GetHerTransitions(int episodeId, float[] achievedGoal, int maxTransitions = 10)
    {
        var episodeTransitions = new List<Transition>();

        for (int i = 0; i < _count; i++)
        {
            if (_buffer[i]?.EpisodeId == episodeId)
                episodeTransitions.Add(_buffer[i]);
        }

        // Relabel rewards based on achieved goal
        var herTransitions = new List<Transition>();
        foreach (var t in episodeTransitions.Take(maxTransitions))
        {
            var herT = new Transition
            {
                FrameCompressed = t.FrameCompressed,
                GameState = t.GameState.ToArray(),
                ReservoirState = t.ReservoirState.ToArray(),
                DiscreteAction = t.DiscreteAction,
                NextGameState = t.NextGameState.ToArray(),
                NextReservoirState = t.NextReservoirState.ToArray(),
                Done = t.Done,
                EpisodeId = t.EpisodeId,
                StepInEpisode = t.StepInEpisode,
                // Relabel reward: +1 if close to achieved goal, 0 otherwise
                Reward = ComputeHerReward(t.NextGameState, achievedGoal),
            };
            herTransitions.Add(herT);
        }

        return herTransitions;
    }

    /// <summary>
    /// Save the buffer to disk for persistence across training sessions.
    /// </summary>
    public async Task SaveAsync(string path)
    {
        _persistPath = path;
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);

        using var fs = new FileStream(path, FileMode.Create, FileAccess.Write);
        using var gz = new GZipStream(fs, CompressionLevel.Fastest);
        using var writer = new BinaryWriter(gz);

        writer.Write(_count);
        writer.Write(_position);
        writer.Write(TotalTransitionsAdded);

        for (int i = 0; i < _count; i++)
        {
            var t = _buffer[i];
            if (t == null) continue;

            writer.Write(i);
            writer.Write(t.FrameCompressed?.Length ?? 0);
            if (t.FrameCompressed != null) writer.Write(t.FrameCompressed);

            WriteFloatArray(writer, t.GameState);
            WriteFloatArray(writer, t.ReservoirState);
            writer.Write(t.DiscreteAction);
            writer.Write(t.Reward);
            WriteFloatArray(writer, t.NextGameState);
            WriteFloatArray(writer, t.NextReservoirState);
            writer.Write(t.Done);
            writer.Write(t.NStepReturn);
            writer.Write(t.EpisodeId);
            writer.Write(t.StepInEpisode);
            writer.Write(_priorities[i]);
        }

        writer.Write(-1); // End marker

        _logger.LogInformation("Replay buffer saved: {Count} transitions to {Path}", _count, path);
    }

    /// <summary>
    /// Load the buffer from disk.
    /// </summary>
    public async Task LoadAsync(string path)
    {
        if (!File.Exists(path))
        {
            _logger.LogWarning("Replay buffer file not found: {Path}", path);
            return;
        }

        using var fs = new FileStream(path, FileMode.Open, FileAccess.Read);
        using var gz = new GZipStream(fs, CompressionMode.Decompress);
        using var reader = new BinaryReader(gz);

        _count = reader.ReadInt32();
        _position = reader.ReadInt32();
        TotalTransitionsAdded = reader.ReadInt64();

        while (true)
        {
            int idx = reader.ReadInt32();
            if (idx == -1) break;

            var t = new Transition();
            int frameLen = reader.ReadInt32();
            if (frameLen > 0) t.FrameCompressed = reader.ReadBytes(frameLen);

            t.GameState = ReadFloatArray(reader);
            t.ReservoirState = ReadFloatArray(reader);
            t.DiscreteAction = reader.ReadInt32();
            t.Reward = reader.ReadSingle();
            t.NextGameState = ReadFloatArray(reader);
            t.NextReservoirState = ReadFloatArray(reader);
            t.Done = reader.ReadBoolean();
            t.NStepReturn = reader.ReadSingle();
            t.EpisodeId = reader.ReadInt32();
            t.StepInEpisode = reader.ReadInt32();
            _priorities[idx] = reader.ReadSingle();

            _buffer[idx] = t;
        }

        _logger.LogInformation("Replay buffer loaded: {Count} transitions from {Path}", _count, path);
    }

    /// <summary>
    /// Get memory usage statistics.
    /// </summary>
    public (long totalBytes, long frameBytes, long stateBytes) GetMemoryUsage()
    {
        long frameBytes = 0, stateBytes = 0;
        for (int i = 0; i < _count; i++)
        {
            if (_buffer[i] == null) continue;
            frameBytes += _buffer[i].FrameCompressed?.Length ?? 0;
            stateBytes += (_buffer[i].GameState.Length + _buffer[i].ReservoirState.Length +
                          _buffer[i].NextGameState.Length + _buffer[i].NextReservoirState.Length) * 4;
        }
        return (frameBytes + stateBytes, frameBytes, stateBytes);
    }

    #region Private Methods

    private void AddToBuffer(Transition transition)
    {
        _buffer[_position] = transition;
        _priorities[_position] = _maxPriority; // New transitions get max priority
        _position = (_position + 1) % _capacity;
        _count = Math.Min(_count + 1, _capacity);
        TotalTransitionsAdded++;

        // Auto-persist every 10K transitions
        if (_autoPersist && _persistPath != null && TotalTransitionsAdded % 10_000 == 0)
        {
            _ = SaveAsync(_persistPath);
        }
    }

    private Transition? ComputeNStepReturn()
    {
        if (_nStepBuffer.Count == 0) return null;

        var first = _nStepBuffer.Dequeue();
        float nStepReturn = first.Reward;
        float discount = _gamma;

        foreach (var t in _nStepBuffer)
        {
            nStepReturn += discount * t.Reward;
            discount *= _gamma;
        }

        first.NStepReturn = nStepReturn;

        // Use the last transition's next state
        if (_nStepBuffer.Count > 0)
        {
            var last = _nStepBuffer.Last();
            first.NextGameState = last.NextGameState;
            first.NextReservoirState = last.NextReservoirState;
            first.Done = last.Done;
        }

        return first;
    }

    private void SamplePrioritized(int batchSize, int[] indices, float[] weights)
    {
        // Compute priority distribution
        float totalPriority = 0;
        for (int i = 0; i < _count; i++)
            totalPriority += (float)Math.Pow(_priorities[i], _alpha);

        float segmentSize = totalPriority / batchSize;
        float minProbability = (float)Math.Pow(
            _priorities.Take(_count).Min() + 1e-6f, _alpha) / totalPriority;
        float maxWeight = (float)Math.Pow(_count * minProbability, -_beta);

        for (int i = 0; i < batchSize; i++)
        {
            float target = (float)(_rng.NextDouble() * segmentSize + i * segmentSize);
            float cumSum = 0;
            int idx = 0;

            for (int j = 0; j < _count; j++)
            {
                cumSum += (float)Math.Pow(_priorities[j], _alpha);
                if (cumSum >= target)
                {
                    idx = j;
                    break;
                }
            }

            indices[i] = idx;

            // Importance sampling weight
            float probability = (float)Math.Pow(_priorities[idx], _alpha) / totalPriority;
            weights[i] = (float)Math.Pow(_count * probability, -_beta) / maxWeight;
        }
    }

    private static byte[]? CompressFrame(float[] frame)
    {
        if (frame == null || frame.Length == 0) return null;

        // Quantize to uint8 and compress with deflate
        var bytes = new byte[frame.Length];
        for (int i = 0; i < frame.Length; i++)
            bytes[i] = (byte)(Math.Clamp(frame[i], 0f, 1f) * 255);

        using var ms = new MemoryStream();
        using (var deflate = new DeflateStream(ms, CompressionLevel.Fastest))
        {
            deflate.Write(bytes, 0, bytes.Length);
        }
        return ms.ToArray();
    }

    private static float ComputeHerReward(float[] state, float[] goal)
    {
        if (state.Length == 0 || goal.Length == 0) return 0;

        float dist = 0;
        int len = Math.Min(state.Length, goal.Length);
        for (int i = 0; i < len; i++)
        {
            float diff = state[i] - goal[i];
            dist += diff * diff;
        }
        dist = (float)Math.Sqrt(dist / len);

        return dist < 0.1f ? 1.0f : 0.0f;
    }

    private static void WriteFloatArray(BinaryWriter writer, float[] arr)
    {
        writer.Write(arr.Length);
        foreach (var f in arr) writer.Write(f);
    }

    private static float[] ReadFloatArray(BinaryReader reader)
    {
        int len = reader.ReadInt32();
        var arr = new float[len];
        for (int i = 0; i < len; i++) arr[i] = reader.ReadSingle();
        return arr;
    }

    #endregion

    /// <summary>
    /// Clear the buffer.
    /// </summary>
    public void Clear()
    {
        Array.Clear(_buffer, 0, _buffer.Length);
        Array.Clear(_priorities, 0, _priorities.Length);
        _count = 0;
        _position = 0;
        _nStepBuffer.Clear();
        _logger.LogInformation("Replay buffer cleared");
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;

        if (_autoPersist && _persistPath != null && _count > 0)
        {
            SaveAsync(_persistPath).Wait();
        }

        _logger.LogInformation("ExperienceReplayBuffer disposed. Total transitions: {Total}, Current: {Count}",
            TotalTransitionsAdded, _count);
    }
}
