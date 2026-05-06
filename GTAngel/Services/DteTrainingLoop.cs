using System.Collections.Concurrent;
using System.Diagnostics;
using System.IO;
using System.Text.Json;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Deep Tree Echo Training Loop — the central orchestrator that connects:
///   1. OpenRW/re3 Engine Bridge (game environment)
///   2. DXGI Frame Capture (visual perception)
///   3. ONNX CNN Feature Extractor (visual encoding)
///   4. ESN Reservoir Pipeline (cognitive processing)
///   5. ViGEm Controller (action execution)
///   6. Reward Shaping (reward computation)
///   7. Experience Replay Buffer (memory)
///
/// Implements the full perception-action-learning cycle:
///   Frame → CNN → ESN → Action → Reward → Buffer → Train → Repeat
///
/// Training modes:
///   - Online: Learn from each step immediately (DQN-style)
///   - Batch: Collect episodes, train on batches (PPO-style)
///   - Hybrid: Online + periodic batch updates (SAC-style)
/// </summary>
public sealed class DteTrainingLoop : IDisposable
{
    private readonly ILogger<DteTrainingLoop> _logger;
    private readonly DxgiFrameCaptureService _frameCapture;
    private readonly VigemControllerService _controller;
    private readonly OpenRwEngineBridge _engine;
    private readonly EsnReservoirPipeline _reservoir;
    private readonly ExperienceReplayBuffer _replayBuffer;
    private readonly OnnxCnnFeatureExtractor _featureExtractor;

    // ── KSM Cycle 5: DTE Cognitive Core ──────────────────────────────────
    private DteCognitiveCoreService? _cognitiveCore;

    private CancellationTokenSource? _cts;
    private Task? _trainingTask;
    private bool _disposed;

    // Training configuration
    public DteTrainingConfig Config { get; set; } = new();

    // Training state
    public DteTrainingState State { get; } = new();

    // Events for UI binding
    public event Action<DteTrainingState>? OnStateUpdated;
    public event Action<DteEpisodeResult>? OnEpisodeComplete;
    public event Action<float[]>? OnFrameCaptured;
    public event Action<string>? OnLogMessage;

    /// <summary>
    /// Set the DTE Cognitive Core service.
    /// Call after DI construction to wire in ECAN/MOSES/Thompson.
    /// </summary>
    public void SetCognitiveCoreService(DteCognitiveCoreService svc)
    {
        _cognitiveCore = svc;
        _logger.LogInformation("DteTrainingLoop: DteCognitiveCoreService wired in.");
    }

    public DteTrainingLoop(
        ILogger<DteTrainingLoop> logger,
        DxgiFrameCaptureService frameCapture,
        VigemControllerService controller,
        OpenRwEngineBridge engine,
        EsnReservoirPipeline reservoir,
        ExperienceReplayBuffer replayBuffer,
        OnnxCnnFeatureExtractor featureExtractor)
    {
        _logger = logger;
        _frameCapture = frameCapture;
        _controller = controller;
        _engine = engine;
        _reservoir = reservoir;
        _replayBuffer = replayBuffer;
        _featureExtractor = featureExtractor;
    }

    /// <summary>
    /// Initialize all subsystems and prepare for training.
    /// </summary>
    public async Task InitializeAsync()
    {
        Log("Initializing DTE Training Loop...");

        // 1. Initialize feature extractor
        Log("  Loading CNN feature extractor...");
        var modelPath = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "AngelClaw", "models", "resnet18-v1-7.onnx");

        if (!File.Exists(modelPath))
        {
            Log("  Downloading ResNet-18 model...");
            modelPath = await _featureExtractor.DownloadModelAsync("resnet18");
        }

        _featureExtractor.Initialize(modelPath, 512);
        Log($"  CNN: {_featureExtractor.ModelName} ({_featureExtractor.OutputDimension}-dim)");

        // 2. Initialize ESN reservoir
        Log("  Initializing ESN reservoir...");
        _reservoir.Initialize();
        Log("  ESN: 3-layer (128/256/512), spectral radius 0.90/0.95/0.99");

        // 3. Initialize frame capture
        Log("  Initializing frame capture...");
        _frameCapture.Initialize();
        Log($"  Capture: {(_frameCapture.IsDxgiMode ? "DXGI Desktop Duplication" : "GDI BitBlt")}");

        // 4. Initialize controller
        Log("  Initializing virtual controller...");
        _controller.Initialize();
        Log($"  Controller: {(_controller.IsVigemAvailable ? "ViGEm Xbox 360" : "Keyboard Injection")}");

        // 5. Detect game engines
        Log("  Detecting game engines...");
        _engine.DetectEngines();
        Log($"  Engine: {_engine.DetectedEngine.ToString()}");

        // 6. Configure replay buffer persistence
        var bufferPath = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "AngelClaw", "replay_buffer.bin");
        _replayBuffer.EnableAutoPersist(bufferPath);

        // Try to load existing buffer
        if (File.Exists(bufferPath))
        {
            await _replayBuffer.LoadAsync(bufferPath);
            Log($"  Replay buffer loaded: {_replayBuffer.Count} transitions");
        }

        State.IsInitialized = true;
        Log("DTE Training Loop initialized.");
        OnStateUpdated?.Invoke(State);
    }

    /// <summary>
    /// Start the training loop.
    /// </summary>
    public void Start()
    {
        if (!State.IsInitialized)
        {
            Log("ERROR: Not initialized. Call InitializeAsync() first.");
            return;
        }

        if (State.IsRunning)
        {
            Log("Training already running.");
            return;
        }

        _cts = new CancellationTokenSource();
        State.IsRunning = true;
        State.StartTime = DateTime.UtcNow;

        _trainingTask = Task.Run(() => TrainingLoopAsync(_cts.Token));
        Log("Training started.");
        OnStateUpdated?.Invoke(State);
    }

    /// <summary>
    /// Pause the training loop.
    /// </summary>
    public void Pause()
    {
        State.IsPaused = !State.IsPaused;
        Log(State.IsPaused ? "Training paused." : "Training resumed.");
        OnStateUpdated?.Invoke(State);
    }

    /// <summary>
    /// Stop the training loop.
    /// </summary>
    public async Task StopAsync()
    {
        if (!State.IsRunning) return;

        _cts?.Cancel();
        if (_trainingTask != null)
            await _trainingTask;

        State.IsRunning = false;
        Log("Training stopped.");

        // Save state
        await SaveCheckpointAsync();
        OnStateUpdated?.Invoke(State);
    }

    /// <summary>
    /// The main training loop.
    /// </summary>
    private async Task TrainingLoopAsync(CancellationToken ct)
    {
        var stepwatch = new Stopwatch();
        var rewardShaper = new RewardShaper();

        try
        {
            // Launch game if not running
            if (_engine.DetectedEngine != OpenRwEngineBridge.EngineType.None && !_engine.IsRunning)
            {
                Log("Launching game engine...");
                await _engine.LaunchAsync();
                await Task.Delay(3000, ct); // Wait for game to start

                // Attach frame capture to game window
                var gameProcess = _engine.GameProcess;
                if (gameProcess != null)
                {
                    _frameCapture.AttachToProcess(gameProcess.Id);
                    _frameCapture.StartCapture();
                    _controller.SetTargetWindow(_engine.GameWindowHandle);
                }
            }

            while (!ct.IsCancellationRequested)
            {
                if (State.IsPaused)
                {
                    await Task.Delay(100, ct);
                    continue;
                }

                // ═══ Run one episode ═══
                await RunEpisodeAsync(rewardShaper, stepwatch, ct);

                State.TotalEpisodes++;

                // Check if we've reached the episode limit
                if (Config.MaxEpisodes > 0 && State.TotalEpisodes >= Config.MaxEpisodes)
                {
                    Log($"Reached episode limit: {Config.MaxEpisodes}");
                    break;
                }

                // Periodic batch training
                if (Config.TrainingMode != DteTrainingMode.Online &&
                    State.TotalEpisodes % Config.BatchTrainInterval == 0 &&
                    _replayBuffer.Count >= Config.MinBufferSize)
                {
                    await BatchTrainAsync(ct);
                }

                // Periodic checkpoint
                if (State.TotalEpisodes % Config.CheckpointInterval == 0)
                {
                    await SaveCheckpointAsync();
                }
            }
        }
        catch (OperationCanceledException)
        {
            Log("Training loop cancelled.");
        }
        catch (Exception ex)
        {
            Log($"Training error: {ex.Message}");
            _logger.LogError(ex, "Training loop error");
        }
    }

    /// <summary>
    /// Run a single training episode.
    /// </summary>
    private async Task RunEpisodeAsync(RewardShaper rewardShaper, Stopwatch stepwatch, CancellationToken ct)
    {
        // Reset environment
        if (_engine.IsRunning)
            await _engine.ResetAsync();

        _reservoir.Reset();
        rewardShaper.Reset();

        float episodeReward = 0;
        int stepCount = 0;
        bool done = false;
        int prevAction = 0;
        float[] prevGameState = new float[22];

        var episodeId = State.TotalEpisodes;

        while (!done && stepCount < Config.MaxStepsPerEpisode && !ct.IsCancellationRequested)
        {
            stepwatch.Restart();

            // ── 1. Perceive: Capture frame ──
            float[] frame;
            if (_frameCapture.IsCapturing)
            {
                frame = _frameCapture.CaptureFrame();
            }
            else
            {
                // Simulation mode: generate synthetic frame
                frame = GenerateSyntheticFrame(stepCount);
            }

            OnFrameCaptured?.Invoke(frame);

            // ── 2. Encode: CNN feature extraction ──
            float[] visualFeatures = _featureExtractor.Extract(frame);

            // ── 3. Read game state (proprioception) ──
            float[] gameState;
            if (_engine.IsRunning)
            {
                var state = await _engine.StepAsync();
                gameState = state?.ToFeatureVector() ?? GenerateSyntheticState(stepCount);
            }
            else
            {
                gameState = GenerateSyntheticState(stepCount);
            }

            // ── 4. Think: ESN reservoir processing ──
            float[] actionProbabilities = _reservoir.ProcessStep(
                visualFeatures, gameState, prevAction);

            // ── 5. Decide: Select action ──
            // Phase 2.2: Per-step cognitive core integration (ECAN attention → Thompson sampling)
            int action;
            if (_cognitiveCore != null)
            {
                var execState = _reservoir.LastReservoirState;  // executive layer (512-dim)
                _cognitiveCore.UpdateAttention(execState);
                _cognitiveCore.MinePatterns(execState);
                // Feed ECAN STI back into ESN as top-down modulation
                _reservoir.SetTopDownModulation(_cognitiveCore.GetClusterSTI());
                var logits = _cognitiveCore.ComputeAttentionGatedLogits(execState);
                action = _cognitiveCore.ThompsonSampleAction(logits);
            }
            else
            {
                action = SelectAction(actionProbabilities);
            }

            // ── 6. Act: Execute action via controller ──
            _controller.ExecuteDiscreteAction((VigemControllerService.DiscreteAction)action);

            // ── 7. Observe: Compute reward ──
            float reward = rewardShaper.ComputeReward(prevGameState, gameState, action, stepCount);
            done = rewardShaper.IsTerminal(gameState);

            // Phase 2.2: Update Thompson belief and periodic Wout training
            if (_cognitiveCore != null)
            {
                var execState = _reservoir.LastReservoirState;
                _cognitiveCore.UpdateThompson(action, reward);

                if (State.TotalSteps % 32 == 0 && execState.Length >= 512)
                {
                    var targetActions = new float[18];
                    if (action >= 0 && action < 18) targetActions[action] = 1f;
                    _cognitiveCore.TrainWout(execState[^512..], targetActions);
                    var coherence = _cognitiveCore.ComputeCoherence();
                    State.CognitiveCoherence = coherence.Coherence;
                    OnStateUpdated?.Invoke(State);
                }
            }

            // ── 8. Remember: Store transition ──
            _replayBuffer.Add(
                frame, gameState, _reservoir.GetFullState(),
                action, reward,
                gameState, _reservoir.GetFullState(), // next state will be overwritten
                done, episodeId, stepCount);

            // ── 9. Learn (online mode) ──
            if (Config.TrainingMode != DteTrainingMode.Batch &&
                _replayBuffer.Count >= Config.MinBufferSize &&
                stepCount % Config.OnlineTrainInterval == 0)
            {
                var batch = _replayBuffer.Sample(Config.BatchSize);
                float[] tdErrors = ComputeTdErrors(batch);
                _replayBuffer.UpdatePriorities(batch.Indices, tdErrors);
                UpdateReadoutWeights(batch, tdErrors);
            }

            // Update state
            episodeReward += reward;
            prevAction = action;
            Array.Copy(gameState, prevGameState, Math.Min(gameState.Length, prevGameState.Length));
            stepCount++;
            State.TotalSteps++;

            stepwatch.Stop();
            State.StepsPerSecond = 1000.0 / Math.Max(1, stepwatch.ElapsedMilliseconds);

            // Throttle to target FPS
            int targetMs = 1000 / Config.TargetFps;
            int elapsed = (int)stepwatch.ElapsedMilliseconds;
            if (elapsed < targetMs)
                await Task.Delay(targetMs - elapsed, ct);
        }

        // Episode complete
        var result = new DteEpisodeResult
        {
            EpisodeId = episodeId,
            TotalReward = episodeReward,
            Steps = stepCount,
            Duration = TimeSpan.FromSeconds(stepCount / Math.Max(1, State.StepsPerSecond)),
            AverageReward = stepCount > 0 ? episodeReward / stepCount : 0,
            Epsilon = State.Epsilon,
            ReservoirCoherence = _reservoir.GetCoherence(),
            WisdomLevel = _reservoir.WisdomLevel,
        };

        State.EpisodeHistory.Add(result);
        if (State.EpisodeHistory.Count > 1000)
            State.EpisodeHistory.RemoveAt(0);

        State.BestEpisodeReward = Math.Max(State.BestEpisodeReward, episodeReward);
        State.AverageEpisodeReward = State.AverageEpisodeReward * 0.99f + episodeReward * 0.01f;

        // Decay epsilon
        State.Epsilon = Math.Max(Config.EpsilonMin,
            State.Epsilon * Config.EpsilonDecay);

        Log($"Episode {episodeId}: reward={episodeReward:F2}, steps={stepCount}, " +
            $"ε={State.Epsilon:F3}, wisdom={_reservoir.WisdomLevel:F3}");

        OnEpisodeComplete?.Invoke(result);
        OnStateUpdated?.Invoke(State);
    }

    /// <summary>
    /// Select an action using epsilon-greedy exploration.
    /// </summary>
    private int SelectAction(float[] probabilities)
    {
        var rng = Random.Shared;

        // Epsilon-greedy
        if (rng.NextDouble() < State.Epsilon)
        {
            return rng.Next(probabilities.Length);
        }

        // Greedy: pick highest probability
        int bestAction = 0;
        float bestProb = probabilities[0];
        for (int i = 1; i < probabilities.Length; i++)
        {
            if (probabilities[i] > bestProb)
            {
                bestProb = probabilities[i];
                bestAction = i;
            }
        }

        return bestAction;
    }

    /// <summary>
    /// Batch training on replay buffer samples.
    /// </summary>
    private async Task BatchTrainAsync(CancellationToken ct)
    {
        Log($"Batch training: {Config.BatchTrainEpochs} epochs, batch size {Config.BatchSize}");

        for (int epoch = 0; epoch < Config.BatchTrainEpochs && !ct.IsCancellationRequested; epoch++)
        {
            var batch = _replayBuffer.Sample(Config.BatchSize);
            float[] tdErrors = ComputeTdErrors(batch);
            _replayBuffer.UpdatePriorities(batch.Indices, tdErrors);
            UpdateReadoutWeights(batch, tdErrors);

            State.TrainingLoss = tdErrors.Select(e => e * e).Average();

            // KSM Cycle 5: MOSES pattern mining + Wout training after each batch
            if (_cognitiveCore != null)
            {
                for (int i = 0; i < batch.BatchSize; i++)
                {
                    var t = batch.Transitions[i];
                    if (t.ReservoirState.Length >= 512)
                    {
                        // Mine patterns from executive reservoir state
                        _cognitiveCore.MinePatterns(t.ReservoirState[^512..]);

                        // Train Wout: use action one-hot as target
                        var targetActions = new float[18];
                        if (t.DiscreteAction >= 0 && t.DiscreteAction < 18)
                            targetActions[t.DiscreteAction] = 1f;
                        _cognitiveCore.TrainWout(t.ReservoirState[^512..], targetActions);

                        // Update Thompson sampling with observed reward
                        _cognitiveCore.UpdateThompson(t.DiscreteAction, t.Reward);
                    }
                }

                // Emit cognitive coherence snapshot
                var coherence = _cognitiveCore.ComputeCoherence();
                State.CognitiveCoherence = coherence.Coherence;
                OnStateUpdated?.Invoke(State);
            }
        }

        Log($"  Loss: {State.TrainingLoss:F4}");
        await Task.CompletedTask;
    }

    /// <summary>
    /// Compute TD errors for a batch of transitions.
    /// </summary>
    private float[] ComputeTdErrors(ExperienceReplayBuffer.SampleBatch batch)
    {
        var tdErrors = new float[batch.BatchSize];
        float gamma = Config.Gamma;

        for (int i = 0; i < batch.BatchSize; i++)
        {
            var t = batch.Transitions[i];

            // Simple TD(0) error: δ = r + γ * V(s') - V(s)
            float currentValue = EstimateValue(t.ReservoirState);
            float nextValue = t.Done ? 0 : EstimateValue(t.NextReservoirState);
            float target = t.Reward + gamma * nextValue;

            tdErrors[i] = target - currentValue;
        }

        return tdErrors;
    }

    /// <summary>
    /// Estimate state value from reservoir state (linear readout).
    /// </summary>
    private float EstimateValue(float[] reservoirState)
    {
        if (reservoirState.Length == 0) return 0;

        // Simple linear value estimate from reservoir state
        float sum = 0;
        for (int i = 0; i < Math.Min(reservoirState.Length, 100); i++)
            sum += reservoirState[i];
        return sum / Math.Max(1, Math.Min(reservoirState.Length, 100));
    }

    /// <summary>
    /// Update ESN readout weights using TD errors.
    /// </summary>
    private void UpdateReadoutWeights(ExperienceReplayBuffer.SampleBatch batch, float[] tdErrors)
    {
        // Ridge regression update on readout weights
        float lr = Config.LearningRate;

        for (int i = 0; i < batch.BatchSize; i++)
        {
            var t = batch.Transitions[i];
            float weight = batch.Weights[i]; // Importance sampling weight
            float error = tdErrors[i] * weight;

            // Update reservoir readout via gradient
            _reservoir.UpdateReadout(t.ReservoirState, t.DiscreteAction, error, lr);
        }
    }

    #region Synthetic Data (Simulation Mode)

    private static float[] GenerateSyntheticFrame(int step)
    {
        var rng = new Random(step);
        var frame = new float[768 * 768 * 3];

        // Generate a simple gradient pattern that changes with step
        for (int y = 0; y < 768; y++)
        {
            for (int x = 0; x < 768; x++)
            {
                int idx = (y * 768 + x) * 3;
                float t = step * 0.01f;
                frame[idx] = (float)(0.5 + 0.5 * Math.Sin(x * 0.02 + t));     // R
                frame[idx + 1] = (float)(0.5 + 0.5 * Math.Sin(y * 0.02 + t)); // G
                frame[idx + 2] = (float)(0.5 + 0.5 * Math.Sin((x + y) * 0.01 + t)); // B
            }
        }

        return frame;
    }

    private static float[] GenerateSyntheticState(int step)
    {
        var rng = new Random(step);
        var state = new float[22];

        // Simulate player moving through the city
        float t = step * 0.1f;
        state[0] = (float)(100 * Math.Sin(t * 0.1));  // x
        state[1] = (float)(100 * Math.Cos(t * 0.1));  // y
        state[2] = 10f;                                 // z
        state[3] = t % 360;                             // heading
        state[4] = (float)Math.Sin(t) * 10;            // vx
        state[5] = (float)Math.Cos(t) * 10;            // vy
        state[6] = 0;                                   // vz
        state[7] = 100 - step * 0.1f;                  // health
        state[8] = 0;                                   // armor
        state[9] = 1;                                   // weapon
        state[10] = 0;                                  // wanted
        state[11] = 0;                                  // in_vehicle
        state[12] = 0;                                  // veh_health
        state[13] = 0;                                  // veh_speed
        state[14] = 1000 + step;                        // money

        return state;
    }

    #endregion

    #region Checkpointing

    public async Task SaveCheckpointAsync()
    {
        var checkpointDir = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "AngelClaw", "checkpoints");
        Directory.CreateDirectory(checkpointDir);

        var checkpointPath = Path.Combine(checkpointDir, $"checkpoint_{State.TotalEpisodes}.json");

        var checkpoint = new
        {
            State.TotalEpisodes,
            State.TotalSteps,
            State.Epsilon,
            State.BestEpisodeReward,
            State.AverageEpisodeReward,
            State.TrainingLoss,
            Config,
            ReplayBufferCount = _replayBuffer.Count,
            FeatureExtractor = _featureExtractor.ModelName,
            Timestamp = DateTime.UtcNow,
        };

        var json = JsonSerializer.Serialize(checkpoint, new JsonSerializerOptions { WriteIndented = true });
        await File.WriteAllTextAsync(checkpointPath, json);

        // Save replay buffer
        var bufferPath = Path.Combine(checkpointDir, "replay_buffer.bin");
        await _replayBuffer.SaveAsync(bufferPath);

        // Save projection weights
        var projPath = Path.Combine(checkpointDir, "projection_weights.bin");
        await _featureExtractor.SaveProjectionAsync(projPath);

        Log($"Checkpoint saved: {checkpointPath}");
    }

    public async Task LoadCheckpointAsync(string path)
    {
        if (!File.Exists(path)) return;

        var json = await File.ReadAllTextAsync(path);
        var doc = JsonDocument.Parse(json);
        var root = doc.RootElement;

        if (root.TryGetProperty("TotalEpisodes", out var ep)) State.TotalEpisodes = ep.GetInt32();
        if (root.TryGetProperty("TotalSteps", out var st)) State.TotalSteps = st.GetInt64();
        if (root.TryGetProperty("Epsilon", out var eps)) State.Epsilon = eps.GetDouble();
        if (root.TryGetProperty("BestEpisodeReward", out var best)) State.BestEpisodeReward = best.GetSingle();

        Log($"Checkpoint loaded: episode {State.TotalEpisodes}, steps {State.TotalSteps}");
    }

    #endregion

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
        _trainingTask?.Wait(5000);
        _cts?.Dispose();

        _logger.LogInformation("DteTrainingLoop disposed. Episodes: {Ep}, Steps: {St}",
            State.TotalEpisodes, State.TotalSteps);
    }
}

#region Configuration and State Classes

public enum DteTrainingMode
{
    Online,  // Learn from each step (DQN)
    Batch,   // Collect episodes, train on batches (PPO)
    Hybrid,  // Online + periodic batch updates (SAC)
}

public class DteTrainingConfig
{
    // General
    public DteTrainingMode TrainingMode { get; set; } = DteTrainingMode.Hybrid;
    public int MaxEpisodes { get; set; } = 0; // 0 = unlimited
    public int MaxStepsPerEpisode { get; set; } = 2000;
    public int TargetFps { get; set; } = 15;

    // Exploration
    public double EpsilonStart { get; set; } = 1.0;
    public double EpsilonMin { get; set; } = 0.05;
    public double EpsilonDecay { get; set; } = 0.9995;

    // Learning
    public float LearningRate { get; set; } = 0.001f;
    public float Gamma { get; set; } = 0.99f;
    public int BatchSize { get; set; } = 32;
    public int MinBufferSize { get; set; } = 1000;

    // Online training
    public int OnlineTrainInterval { get; set; } = 4; // Train every N steps

    // Batch training
    public int BatchTrainInterval { get; set; } = 10; // Train every N episodes
    public int BatchTrainEpochs { get; set; } = 5;

    // Checkpointing
    public int CheckpointInterval { get; set; } = 50; // Save every N episodes
}

public class DteTrainingState
{
    public bool IsInitialized { get; set; }
    public bool IsRunning { get; set; }
    public bool IsPaused { get; set; }
    public DateTime StartTime { get; set; }

    public int TotalEpisodes { get; set; }
    public long TotalSteps { get; set; }
    public double Epsilon { get; set; } = 1.0;

    public float BestEpisodeReward { get; set; }
    public float AverageEpisodeReward { get; set; }
    public float TrainingLoss { get; set; }
    public double StepsPerSecond { get; set; }

    // KSM Cycle 5: DTE Cognitive Core telemetry
    public double CognitiveCoherence { get; set; }

    public List<DteEpisodeResult> EpisodeHistory { get; set; } = new();
}

public class DteEpisodeResult
{
    public int EpisodeId { get; set; }
    public float TotalReward { get; set; }
    public int Steps { get; set; }
    public TimeSpan Duration { get; set; }
    public float AverageReward { get; set; }
    public double Epsilon { get; set; }
    public float ReservoirCoherence { get; set; }
    public float WisdomLevel { get; set; }
}

#endregion
