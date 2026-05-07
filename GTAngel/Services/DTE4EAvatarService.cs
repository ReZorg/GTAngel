using System.Collections.Concurrent;
using System.IO;
using Microsoft.Extensions.Logging;
using GTAngel.Interop;
using GTAngel.Models;

namespace GTAngel.Services;

/// <summary>
/// DTE 4E Embodied Cognition Avatar Service for GTAngel.
///
/// Implements the 4E Cognition framework:
///   Embodied  — avatar has a physical body in the UE5 world (UAvatar3DComponent)
///   Embedded  — avatar is embedded in Liberty City environment (UVirtualEnvironmentManager)
///   Enacted   — avatar acts through motor control (UE5 Enhanced Input → virtual player controls)
///   Extended  — avatar's mind extends into the DTE ESN reservoir + NeurochemicalSystem
///
/// Architecture:
///   WPF (GTAngelService) ←→ DTE4EAvatarService ←→ UE5ProcessManager (UE5)
///                                    ↕
///                          EsnReservoirPipeline (ESN)
///                                    ↕
///                          AvatarExplorationPolicy (RL)
///                                    ↕
///                          768×768 ML Vision frames
///
/// UE5 Cognitive Modules used (from E:\u9n\UnrealEngine\Source\):
///   Avatar/Avatar3DComponent         — skeletal mesh + facial/gesture/aura/viz
///   Neurochemical/NeurochemicalSystem — curiosity, endorphin, chaos, homeostasis
///   Personality/SuperHotGirlPersonality — avatar personality traits
///   Environment/VirtualEnvironmentManager — dynamic lighting + particles
///   Live2DCubism/Live2DCubismAvatarComponent — 2D expression overlay
/// </summary>
public class DTE4EAvatarService : IDisposable
{
    private readonly ILogger<DTE4EAvatarService> _logger;
    private readonly UE5ProcessManager _ue5;
    private readonly EsnReservoirPipeline _esn;
    private readonly MlVisionCaptureService? _mlVision;       // KSM Cycle 1: real 768×768 frames
    private readonly AvatarEmbodimentService? _embodiment;    // KSM Cycle 3: FACS+IK+Neuro+Personality
    private readonly GameWorldNavigationService? _navigation;   // KSM Cycle 4: POI-directed navigation
    private Ue5PlayerAiBridgeService? _playerAiBridge;          // KSM Cycle 6: Player↔AI Bridge
    private DteCognitiveCoreService? _cognitiveCore;            // KSM Cycle 5: ECAN attention / MOSES patterns
    private float[] _previousAction = new float[18]; // 18-dim action history for ESN
    private readonly AvatarExplorationPolicy _policy;
    private readonly ConcurrentQueue<AvatarObservation> _observationQueue = new();
    private CancellationTokenSource? _cts;
    private Task? _explorationLoopTask;

    // ── State ────────────────────────────────────────────────────────────────
    public bool IsRunning { get; private set; }
    public AvatarCognitiveState CognitiveState { get; private set; } = new();
    public AvatarObservation? LastObservation { get; private set; }
    public int TotalStepsTaken { get; private set; }
    public float TotalReward { get; private set; }
    public float ExplorationCoverage { get; private set; }

    // ── Events ───────────────────────────────────────────────────────────────
    public event EventHandler<AvatarCognitiveState>? CognitiveStateUpdated;
    public event EventHandler<AvatarObservation>?    ObservationReceived;
    public event EventHandler<AvatarAction>?         ActionDispatched;
    public event EventHandler<string>?               ExplorationLog;

    // ── UE5 Module paths (from E:\u9n\UnrealEngine\Source\) ─────────────────
    public const string UE5EnginePath       = @"E:\u9n\UnrealEngine";
    public const string AvatarModulePath    = @"E:\u9n\UnrealEngine\Source\Avatar";
    public const string NeuroModulePath     = @"E:\u9n\UnrealEngine\Source\Neurochemical";
    public const string PersonalityPath     = @"E:\u9n\UnrealEngine\Source\Personality";
    public const string EnvironmentPath     = @"E:\u9n\UnrealEngine\Source\Environment";
    public const string Live2DPath          = @"E:\u9n\UnrealEngine\Source\Live2DCubism";
    public const string AssetMgmtPath       = @"E:\u9n\UnrealEngine\Source\AssetManagement";

    // ── ML Vision ────────────────────────────────────────────────────────────
    public const int MLVisionWidth  = 768;
    public const int MLVisionHeight = 768;

    // ── Exploration parameters ───────────────────────────────────────────────
    private const float ExplorationStepIntervalMs = 250f;  // 4 Hz decision rate
    private const int   MaxExplorationSteps       = 10_000;
    /// <summary>
    /// Phase 5.3: Max perception+ESN latency (ms) before FACS/IK is skipped
    /// for the step. The 250ms step budget needs ~75ms for the rest of the
    /// pipeline (action dispatch, reward, navigation, logging), so we cap
    /// perception+ESN at 175ms — anything above that and embodiment is
    /// skipped to keep the step rate stable.
    /// </summary>
    private const double MaxPerceptionLatencyMs   = 175.0;
    private const float RewardDiscoveryBonus      = 1.0f;
    private const float RewardMovementPenalty     = -0.01f;
    private const float RewardIdlePenalty         = -0.1f;

    public DTE4EAvatarService(
        ILogger<DTE4EAvatarService> logger,
        UE5ProcessManager ue5,
        EsnReservoirPipeline esn,
        MlVisionCaptureService? mlVision = null,
        AvatarEmbodimentService? embodiment = null,
        GameWorldNavigationService? navigation = null)
    {
        _logger   = logger;
        _ue5      = ue5;
        _esn      = esn;
        _mlVision    = mlVision;
        _embodiment  = embodiment;
        _navigation  = navigation;
        _policy      = new AvatarExplorationPolicy(logger, navigation);

        // Wire up UE5 observation events
        _ue5.AvatarObservationReceived += OnAvatarObservation;

        _logger.LogInformation("DTE 4E Avatar Service initialized");
        _logger.LogInformation("  UE5 Avatar module:        {Path}", AvatarModulePath);
        _logger.LogInformation("  UE5 Neurochemical module: {Path}", NeuroModulePath);
        _logger.LogInformation("  UE5 Personality module:   {Path}", PersonalityPath);
        _logger.LogInformation("  UE5 Environment module:   {Path}", EnvironmentPath);
        _logger.LogInformation("  ML Vision: {W}×{H}", MLVisionWidth, MLVisionHeight);
    }

    // ── Start / Stop ─────────────────────────────────────────────────────────

    /// <summary>
    /// Start the 4E embodied cognition exploration loop.
    /// The avatar will explore Liberty City like a human player would.
    /// </summary>
    public async Task StartExplorationAsync()
    {
        if (IsRunning)
        {
            _logger.LogWarning("Avatar exploration already running");
            return;
        }

        _cts = new CancellationTokenSource();
        IsRunning = true;
        TotalStepsTaken = 0;
        TotalReward = 0f;
        ExplorationCoverage = 0f;

        // Initialize avatar in UE5
        await InitializeUE5AvatarAsync();

        // Start the exploration loop
        _explorationLoopTask = Task.Run(() => ExplorationLoopAsync(_cts.Token));

        _logger.LogInformation("DTE 4E Avatar exploration started");
        ExplorationLog?.Invoke(this, "🌟 GTAngel Avatar awakened — beginning 4E embodied exploration of Liberty City");
    }

    public async Task StopExplorationAsync()
    {
        if (!IsRunning) return;

        _cts?.Cancel();
        if (_explorationLoopTask != null)
            await _explorationLoopTask.WaitAsync(TimeSpan.FromSeconds(5));

        IsRunning = false;
        _logger.LogInformation("DTE 4E Avatar exploration stopped. Steps: {Steps}, Reward: {Reward:F2}",
            TotalStepsTaken, TotalReward);
        ExplorationLog?.Invoke(this, $"🛑 Exploration stopped. Steps: {TotalStepsTaken}, Total reward: {TotalReward:F2}");
    }

    // ── UE5 Avatar Initialization ─────────────────────────────────────────────

    /// <summary>
    /// Initialize the UE5 avatar with all cognitive modules.
    /// Activates: Avatar3DComponent, NeurochemicalSystem, SuperHotGirlPersonality,
    ///            VirtualEnvironmentManager, Live2DCubismAvatarComponent
    /// </summary>
    private async Task InitializeUE5AvatarAsync()
    {
        _logger.LogInformation("Initializing UE5 DTE Avatar with cognitive modules...");

        // Spawn the avatar actor with Avatar3DComponent
        await _ue5.SendAvatarActionAsync(new AvatarAction
        {
            InputAction = "IA_SpawnAvatar",
            Source      = "Script",
            Magnitude   = 1.0f
        });

        // Activate NeurochemicalSystem (curiosity, endorphin, chaos, homeostasis)
        await SendUE5ModuleCommandAsync("NeurochemicalSystem", "Activate", new
        {
            CuriosityBaseline    = 0.7f,
            EndorphinBaseline    = 0.5f,
            ChaosIntensityMax    = 0.8f,
            HomeostasisTarget    = 0.6f,
            AbundanceThreshold   = 0.4f,
            ScarcityThreshold    = 0.2f
        });

        // Apply SuperHotGirlPersonality traits
        await SendUE5ModuleCommandAsync("SuperHotGirlPersonality", "Apply", new
        {
            Confidence  = 0.8f,
            Charm       = 0.9f,
            Playfulness = 0.7f,
            Wit         = 0.8f,
            Sass        = 0.6f
        });

        // Initialize VirtualEnvironmentManager (Lumen lighting + Niagara particles)
        await SendUE5ModuleCommandAsync("VirtualEnvironmentManager", "Initialize", new
        {
            UseLumen   = true,
            UseNiagara = true,
            TimeOfDay  = 14.0f  // 2 PM Liberty City time
        });

        // Enable Live2D expression overlay
        await SendUE5ModuleCommandAsync("Live2DCubismAvatarComponent", "Enable", new
        {
            ExpressionMode = "Cognitive",
            BlendWeight    = 0.6f
        });

        // Enable ML Vision capture at 768×768
        await _ue5.RequestMLVisionFrameAsync();

        ExplorationLog?.Invoke(this, "✅ UE5 Avatar initialized: Avatar3D + NeurochemicalSystem + Personality + Environment + Live2D");
        _logger.LogInformation("UE5 Avatar initialization complete");
    }

    // ── Exploration Loop ──────────────────────────────────────────────────────

    /// <summary>
    /// The main 4E embodied cognition exploration loop.
    /// Runs at 4 Hz: Observe → Process (ESN) → Decide (Policy) → Act (Enhanced Input) → Reward
    /// </summary>
    private async Task ExplorationLoopAsync(CancellationToken ct)
    {
        _logger.LogInformation("4E exploration loop started at {Hz} Hz", 1000f / ExplorationStepIntervalMs);

        while (!ct.IsCancellationRequested && TotalStepsTaken < MaxExplorationSteps)
        {
            try
            {
                var stepStart = DateTime.UtcNow;

                // ── 1. OBSERVE ────────────────────────────────────────────
                // Request a 768×768 ML vision frame from UE5
                await _ue5.RequestMLVisionFrameAsync();

                // Get the latest observation (may be from previous frame)
                var obs = LastObservation;
                if (obs == null)
                {
                    await Task.Delay((int)ExplorationStepIntervalMs, ct);
                    continue;
                }

                ObservationReceived?.Invoke(this, obs);

                // ── 2. PROCESS (ESN Reservoir) ────────────────────────────
                // Feed the observation into the DTE ESN reservoir
                // ProcessStep(frame[768*768*3], gameState[22], prevAction[18])
                // We pass an empty frame (visual processing handled by UE5 side)
                // and encode the observation as the 22-dim game state vector
                var percepStart = DateTime.UtcNow;
                var gameState = EncodeObservationForESN(obs);  // 22-dim, navigation+neuro integrated
                // KSM Cycle 1: use real 768×768 frames from MlVisionCaptureService
                // (falls back to Array.Empty if capture service not yet started)
                var visionFrame = (_mlVision?.IsCapturing == true)
                    ? _mlVision.GetLatestFrame()
                    : Array.Empty<float>();
                var actionProbs = _esn.ProcessStep(visionFrame, gameState, _previousAction);
                var esnState = _esn.LastReservoirState;

                // Phase 5.3: Token-bucket — measure perception+ESN latency
                // BEFORE the cognitive core update so the governor only
                // reflects the pipeline its name implies. (UpdateAttention /
                // MinePatterns latency is governed separately by the rate
                // limiter inside DteCognitiveCoreService.)
                var percepElapsedMs = (DateTime.UtcNow - percepStart).TotalMilliseconds;
                bool skipEmbodimentThisStep = percepElapsedMs > MaxPerceptionLatencyMs;

                // KSM Cycle 5: advance ECAN attention + MOSES pattern state on the executive
                // reservoir, then feed the cluster STI back into the ESN as top-down modulation.
                // The training-loop variant does the same per-step; the exploration path needs
                // it too so attention gating and pattern mining aren't frozen at zero here.
                if (_cognitiveCore != null && esnState.Length > 0)
                {
                    _cognitiveCore.UpdateAttention(esnState);
                    _cognitiveCore.MinePatterns(esnState);
                    _esn.SetTopDownModulation(_cognitiveCore.GetClusterSTI());
                }

                // ── 3. UPDATE COGNITIVE STATE ─────────────────────────────
                UpdateCognitiveState(obs, esnState);

                // ── 4. DECIDE (Exploration Policy) ────────────────────────
                // The policy uses ESN state + neurochemical state to decide action
                var action = _policy.SelectAction(CognitiveState, esnState);

                // ── 5. ACT (UE5 Enhanced Input) ───────────────────────────
                // KSM Cycle 6: Arbitrate input before executing
                AvatarAction finalAction = action;
                if (_playerAiBridge != null)
                {
                    AvatarAction? humanAction = null;
                    if (_playerAiBridge.CurrentMode == PlayerAiBridgeMode.HumanOnly || _playerAiBridge.CurrentMode == PlayerAiBridgeMode.Arbitrated)
                    {
                        // Dummy human passthrough (in reality, read from gamepad/keyboard)
                        humanAction = new AvatarAction { InputAction = "IA_Move", AxisX = 0, AxisY = 0, Magnitude = 0, Source = "Human" };
                    }
                    
                    finalAction = _playerAiBridge.ArbitrateInput(humanAction, action);
                }

                // Send the action to UE5 via the Enhanced Input System
                await _ue5.SendAvatarActionAsync(finalAction);
                ActionDispatched?.Invoke(this, finalAction);

                // Update previous action one-hot for next ESN step
                UpdatePreviousAction(action, actionProbs);

                // ── 6. REWARD ─────────────────────────────────────────────
                var reward = ComputeReward(obs, action);
                TotalReward += reward;
                TotalStepsTaken++;

                // Update exploration coverage (unique cells visited)
                UpdateExplorationCoverage(obs);

                // ── 6c. NAVIGATE (KSM Cycle 4: POI-directed navigation) ──────────
                _navigation?.UpdatePosition(obs.Position);

                // ── 6b. EMBODY (KSM Cycle 3: FACS+IK+Neuro+Personality) ──────────────
                // Phase 5.3: Skip FACS/IK if perception+ESN took > 200ms (rate governor)
                if (_embodiment != null && !skipEmbodimentThisStep)
                {
                    // Synthesize emotional state from ESN output + neurochemical state
                    var neuroState = _embodiment.CurrentNeuro;
                    var emotionalState = _embodiment.SynthesizeEmotionalState(esnState, neuroState);

                    // Compute FACS Action Units from emotional state
                    var aus = _embodiment.ComputeFACSActionUnits(emotionalState);

                    // Send FACS commands + ExpressionSynthesizer + PhysicsDeformer to UE5
                    _ = _embodiment.SendFACSCommandsAsync(aus);

                    // Send IK pose blend from ESN action dims [12..17]
                    _ = _embodiment.SendIKPoseBlendAsync(actionProbs);

                    // Apply SuperHotGirlPersonality traits based on cognitive state
                    _ = _embodiment.ApplyPersonalityTraitsAsync(
                        CognitiveState.AutonomyScore,
                        CognitiveState.Coherence,
                        neuroState);
                }

                // ── 7. LOG (periodic) ─────────────────────────────────────
                if (TotalStepsTaken % 100 == 0)
                {
                    ExplorationLog?.Invoke(this,
                        $"Step {TotalStepsTaken}: pos=({obs.Position[0]:F0},{obs.Position[1]:F0},{obs.Position[2]:F0}) " +
                        $"action={action.InputAction} reward={reward:F3} total={TotalReward:F1} " +
                        $"curiosity={CognitiveState.Curiosity:F2} coverage={ExplorationCoverage:P0}");
                }

                // ── Maintain step rate ─────────────────────────────────────
                var elapsed = (DateTime.UtcNow - stepStart).TotalMilliseconds;
                var delay   = Math.Max(0, ExplorationStepIntervalMs - elapsed);
                if (delay > 0) await Task.Delay((int)delay, ct);
            }
            catch (OperationCanceledException) { break; }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error in 4E exploration loop at step {Step}", TotalStepsTaken);
                await Task.Delay(1000, ct);
            }
        }

        _logger.LogInformation("Exploration loop ended. Steps: {Steps}", TotalStepsTaken);
    }

    // ── Observation Processing ────────────────────────────────────────────────

    private void OnAvatarObservation(object? sender, AvatarObservation obs)
    {
        LastObservation = obs;
        _observationQueue.Enqueue(obs);

        // KSM Cycle 6: Observation Fusion
        if (_mlVision != null && _playerAiBridge != null)
        {
            var features = _mlVision.GetLatestFeatures();
            _playerAiBridge.ProcessObservation(obs, features);

            // Phase 4.1: Feed fused observation norm back into ESN cognitive input scaling
            // High fusion norm (strong ML feature signal) → amplify cognitive layer drive
            float fusionNorm = 0f;
            if (features.Length > 0)
            {
                foreach (var f in features) fusionNorm += f * f;
                fusionNorm = MathF.Sqrt(fusionNorm);
            }
            float scale = 1.0f + Math.Clamp(fusionNorm / 10f, 0f, 1f) * 0.5f; // 1.0 .. 1.5
            _esn.SetObservationFusionScale(scale);
        }

        // Keep queue bounded
        while (_observationQueue.Count > 100)
            _observationQueue.TryDequeue(out _);
    }

    /// <summary>
    /// Encode an AvatarObservation into a float vector for the ESN reservoir.
    /// Output: 22-dim game state vector (padded from 16 dims to match ESN input)
    /// Layout: position(3) + rotation(3) + velocity(3) + neurochemical(6) + perceived(1) +
    ///         navDirX(1) + navDirY(1) + embodimentNeuro(4)
    /// </summary>
    private float[] EncodeObservationForESN(AvatarObservation obs)
    {
        var neuro = obs.NeurochemicalState ?? new NeurochemicalSnapshot();

        // Phase 1.2: Navigation direction toward current POI target (dims 16..17).
        // Capture NextPOI into a local first so the null check and the field reads
        // see the same value — _navigation is a singleton whose NextPOI can be
        // updated from the navigation loop concurrently with this encode call.
        float navDirX = 0f, navDirY = 0f;
        var nextPoi = _navigation?.NextPOI;
        if (nextPoi != null && nextPoi.Position.Length >= 2 && obs.Position.Length >= 2)
        {
            float dx = nextPoi.Position[0] - obs.Position[0];
            float dy = nextPoi.Position[1] - obs.Position[1];
            float dist = MathF.Sqrt(dx * dx + dy * dy) + 1e-6f;
            navDirX = dx / dist;
            navDirY = dy / dist;
        }

        // Phase 3.3: Embodiment neurochemical feedback (dims 18..21)
        // Prefer live readback from AvatarEmbodimentService over obs snapshot
        var embNeuro = _embodiment?.CurrentNeuro;
        float embCuriosity   = embNeuro?.Curiosity   ?? neuro.Curiosity;
        float embEndorphin   = embNeuro?.Endorphin   ?? neuro.Endorphin;
        float embChaos       = embNeuro?.Chaos       ?? neuro.ChaosIntensity;
        float embHomeostasis = embNeuro?.Homeostasis ?? neuro.Homeostasis;

        return new float[]
        {
            // Spatial (normalized to Liberty City bounds ~3000 UU)
            obs.Position[0] / 3000f,
            obs.Position[1] / 3000f,
            obs.Position[2] / 500f,
            // Rotation (normalized to [-1,1])
            obs.Rotation[0] / 180f,
            obs.Rotation[1] / 180f,
            obs.Rotation[2] / 180f,
            // Velocity (normalized to max ~600 UU/s)
            obs.Velocity[0] / 600f,
            obs.Velocity[1] / 600f,
            obs.Velocity[2] / 600f,
            // Neurochemical state (from UE5 observation)
            neuro.Curiosity,
            neuro.Endorphin,
            neuro.ChaosIntensity,
            neuro.Homeostasis,
            neuro.Abundance,
            neuro.Scarcity,
            // Perceived objects count (normalized)
            Math.Min(obs.PerceivedObjects.Length / 10f, 1f),
            // Phase 1.2: Navigation direction toward POI target (normalized 2D unit vector)
            navDirX,
            navDirY,
            // Phase 3.3: Embodiment neurochemical feedback loop (live readback from service)
            embCuriosity,
            embEndorphin,
            embChaos,
            embHomeostasis
        };
    }

    private void UpdateCognitiveState(AvatarObservation obs, float[] esnState)
    {
        var neuro = obs.NeurochemicalState ?? new NeurochemicalSnapshot();

        CognitiveState = new AvatarCognitiveState
        {
            // 4E dimensions
            EmbodiedPosition = obs.Position,
            EmbeddedContext  = obs.PerceivedObjects.Length,
            EnactedVelocity  = obs.Velocity,
            ExtendedESNNorm  = esnState.Length > 0 ? esnState.Average() : 0f,

            // Neurochemical
            Curiosity      = neuro.Curiosity,
            Endorphin      = neuro.Endorphin,
            ChaosIntensity = neuro.ChaosIntensity,
            Homeostasis    = neuro.Homeostasis,

            // Exploration
            TotalSteps      = TotalStepsTaken,
            TotalReward     = TotalReward,
            Coverage        = ExplorationCoverage,

            // Timestamp
            Timestamp = obs.Timestamp,

            // KSM Cycle 3: Autonomy and Coherence
            AutonomyScore = Math.Min(1f, ExplorationCoverage * 2f + (TotalReward > 0 ? 0.1f : 0f)),
            Coherence     = esnState.Length > 0
                ? Math.Min(1f, (float)esnState.Average() + neuro.Homeostasis * 0.3f)
                : neuro.Homeostasis
        };

        CognitiveStateUpdated?.Invoke(this, CognitiveState);
    }

    private float ComputeReward(AvatarObservation obs, AvatarAction action)
    {
        float reward = 0f;

        // Movement reward: reward for moving (human-like exploration)
        var speed = MathF.Sqrt(
            obs.Velocity[0] * obs.Velocity[0] +
            obs.Velocity[1] * obs.Velocity[1]);
        reward += speed > 10f ? 0.1f : RewardIdlePenalty;

        // Discovery reward: reward for seeing new objects
        reward += obs.PerceivedObjects.Length * 0.05f;

        // Curiosity bonus: high curiosity drives exploration
        var neuro = obs.NeurochemicalState;
        if (neuro != null)
            reward += neuro.Curiosity * 0.2f;

        // Small cost for each action (efficiency)
        reward += RewardMovementPenalty;

        return reward;
    }

    private readonly HashSet<(int, int)> _visitedCells = new();

    /// <summary>
    /// Update the previous action one-hot encoding for the ESN.
    /// Maps AvatarAction InputAction name to one of 18 discrete action slots.
    /// </summary>
    private void UpdatePreviousAction(AvatarAction action, float[] actionProbs)
    {
        Array.Clear(_previousAction);
        // Map Enhanced Input action names to discrete action indices
        var idx = action.InputAction switch
        {
            "IA_Move"      => 0,  // Forward
            "IA_MoveBack"  => 1,  // Backward
            "IA_StrafeL"   => 2,  // Strafe left
            "IA_StrafeR"   => 3,  // Strafe right
            "IA_Look"      => 4,  // Look
            "IA_Jump"      => 5,  // Jump
            "IA_Sprint"    => 6,  // Sprint
            "IA_Interact"  => 7,  // Interact
            "IA_Camera"    => 8,  // Camera
            "IA_Attack"    => 9,  // Attack
            "IA_Aim"       => 10, // Aim
            "IA_Crouch"    => 11, // Crouch
            "IA_Enter"     => 12, // Enter vehicle
            "IA_Exit"      => 13, // Exit vehicle
            "IA_Accelerate"=> 14, // Vehicle accelerate
            "IA_Brake"     => 15, // Vehicle brake
            "IA_Steer"     => 16, // Vehicle steer
            _              => 17  // Other
        };
        if (idx < _previousAction.Length)
            _previousAction[idx] = 1.0f;
    }

    private void UpdateExplorationCoverage(AvatarObservation obs)
    {
        // Discretize Liberty City into 50×50 UU cells
        var cellX = (int)(obs.Position[0] / 50f);
        var cellY = (int)(obs.Position[1] / 50f);
        _visitedCells.Add((cellX, cellY));

        // Liberty City is roughly 3000×3000 UU → 60×60 = 3600 cells
        ExplorationCoverage = Math.Min(_visitedCells.Count / 3600f, 1f);
    }

    // ── UE5 Module Commands ───────────────────────────────────────────────────

    private async Task SendUE5ModuleCommandAsync(string module, string command, object parameters)
    {
        var msg = new UEMessage
        {
            Action = $"Module.{module}.{command}",
            Key    = module,
            Value  = command,
            Extras = new[] { System.Text.Json.JsonSerializer.Serialize(parameters) }
        };
        await _ue5.SendAvatarActionAsync(new AvatarAction
        {
            InputAction = $"IA_Module_{module}_{command}",
            Source      = "Script",
            Magnitude   = 1.0f
        });
        _logger.LogDebug("UE5 module command: {Module}.{Command}", module, command);
    }

    public void SetPlayerAiBridge(Ue5PlayerAiBridgeService bridge)
    {
        _playerAiBridge = bridge;
    }

    /// <summary>
    /// KSM Cycle 5: wire in the DTE Cognitive Core so the 4E exploration loop
    /// advances ECAN attention / MOSES pattern state and feeds STI back into the
    /// ESN as top-down modulation. Without this, ComputeAttentionGatedLogits
    /// runs against a frozen-zero STI and effectively disables attention gating.
    /// </summary>
    public void SetCognitiveCoreService(DteCognitiveCoreService svc)
    {
        _cognitiveCore = svc;
        _logger.LogInformation("DTE4EAvatarService: DteCognitiveCoreService wired in — ECAN/MOSES advanced from exploration loop.");
    }

    public void Dispose()
    {
        _cts?.Cancel();
        _cts?.Dispose();
        _ue5.AvatarObservationReceived -= OnAvatarObservation;
    }
}

// ── Avatar Cognitive State ────────────────────────────────────────────────────

/// <summary>
/// The 4E cognitive state of the DTE avatar at a given moment.
/// Combines embodied (physical), embedded (environmental), enacted (motor),
/// and extended (ESN reservoir) dimensions.
/// </summary>
public class AvatarCognitiveState
{
    // 4E dimensions
    public float[] EmbodiedPosition  { get; set; } = new float[3];
    public int     EmbeddedContext   { get; set; }
    public float[] EnactedVelocity   { get; set; } = new float[3];
    public float   ExtendedESNNorm   { get; set; }

    // Neurochemical (from UE5 NeurochemicalSystem)
    public float Curiosity      { get; set; }
    public float Endorphin      { get; set; }
    public float ChaosIntensity { get; set; }
    public float Homeostasis    { get; set; }

    // Exploration metrics
    public int   TotalSteps  { get; set; }
    public float TotalReward { get; set; }
    public float Coverage    { get; set; }
    public double Timestamp  { get; set; }

    // KSM Cycle 3: Autonomy and Coherence (for personality trait applicator)
    public float AutonomyScore { get; set; }   // 0..1 (derived from Coverage + TotalReward)
    public float Coherence     { get; set; }   // 0..1 (derived from ESN norm + homeostasis)
}

// ── Avatar Exploration Policy ─────────────────────────────────────────────────

/// <summary>
/// Human-like exploration policy for the DTE avatar.
/// Uses a combination of curiosity-driven exploration, waypoint navigation,
/// and ESN reservoir state to select actions that mimic human player behavior.
///
/// UE5 Enhanced Input Actions used:
///   IA_Move  — 2D movement (WASD equivalent)
///   IA_Look  — camera look (mouse equivalent)
///   IA_Jump  — jump
///   IA_Sprint — sprint
///   IA_Interact — interact with objects
///   IA_Camera — switch camera perspective
/// </summary>
public class AvatarExplorationPolicy
{
    private readonly ILogger _logger;
    private readonly Random _rng = new();

    // Human-like behavior parameters
    private const float WaypointReachThreshold = 100f;  // UU
    private float[]? _currentWaypoint;
    private int _waypointStepsRemaining;
    private float _explorationTemperature = 0.7f;  // curiosity-modulated

    // Human-like action distribution (approximate)
    private static readonly string[] HumanActions = {
        "IA_Move", "IA_Move", "IA_Move", "IA_Move",  // 40% moving
        "IA_Look", "IA_Look",                          // 20% looking
        "IA_Move", "IA_Sprint",                        // 10% sprinting
        "IA_Jump",                                     // 10% jumping
        "IA_Interact",                                 // 10% interacting
        "IA_Camera",                                   // 10% camera
    };

    private readonly GameWorldNavigationService? _navigation;

    public AvatarExplorationPolicy(ILogger logger, GameWorldNavigationService? navigation = null)
    {
        _logger = logger;
        _navigation = navigation;
    }

    /// <summary>
    /// Select the next action for the avatar.
    /// Implements curiosity-driven human-like exploration:
    ///   - High curiosity → explore new areas (random waypoints)
    ///   - High endorphin → stay in interesting areas (local exploration)
    ///   - High chaos → erratic movement (human-like unpredictability)
    ///   - High homeostasis → return to safe areas
    /// </summary>
    public AvatarAction SelectAction(AvatarCognitiveState state, float[] esnState)
    {
        // Update exploration temperature based on neurochemical state
        _explorationTemperature = 0.3f + state.Curiosity * 0.7f;

        // Decide action type based on neurochemical state
        var actionType = SelectActionType(state);

        return actionType switch
        {
            "explore"   => CreateExploreAction(state),
            "look"      => CreateLookAction(state),
            "sprint"    => CreateSprintAction(state),
            "jump"      => CreateJumpAction(),
            "interact"  => CreateInteractAction(state),
            "camera"    => CreateCameraAction(),
            _           => CreateMoveAction(state)
        };
    }

    private string SelectActionType(AvatarCognitiveState state)
    {
        // Curiosity drives exploration
        if (state.Curiosity > 0.8f && _rng.NextDouble() < 0.4f) return "explore";

        // Chaos drives erratic behavior (human-like)
        if (state.ChaosIntensity > 0.7f && _rng.NextDouble() < 0.3f) return "jump";

        // Endorphin drives interaction
        if (state.Endorphin > 0.7f && _rng.NextDouble() < 0.2f) return "interact";

        // Default: human-like action distribution
        var idx = _rng.Next(HumanActions.Length);
        return HumanActions[idx] switch
        {
            "IA_Move"     => "move",
            "IA_Look"     => "look",
            "IA_Sprint"   => "sprint",
            "IA_Jump"     => "jump",
            "IA_Interact" => "interact",
            "IA_Camera"   => "camera",
            _             => "move"
        };
    }

    private AvatarAction CreateExploreAction(AvatarCognitiveState state)
    {
        // KSM Cycle 4: POI-directed navigation replaces random waypoints
        if (_navigation != null)
        {
            var direction = _navigation.GetNextWaypointDirection(state.EmbodiedPosition);
            if (direction.Length >= 2)
            {
                return new AvatarAction
                {
                    InputAction   = "IA_Move",
                    AxisX         = direction[0],
                    AxisY         = direction[1],
                    Magnitude     = _explorationTemperature,
                    HoldDuration  = 1.0f + (float)(_rng.NextDouble() * 2f),
                    Source        = "ML_Nav"
                };
            }
        }

        // Fallback: random exploration waypoint
        var angle = (float)(_rng.NextDouble() * Math.PI * 2);
        return new AvatarAction
        {
            InputAction   = "IA_Move",
            AxisX         = MathF.Cos(angle),
            AxisY         = MathF.Sin(angle),
            Magnitude     = _explorationTemperature,
            HoldDuration  = 1.0f + (float)(_rng.NextDouble() * 2f),
            Source        = "ML"
        };
    }

    private AvatarAction CreateMoveAction(AvatarCognitiveState state)
    {
        // Human-like movement: mostly forward with slight lateral variation
        var lateralBias = (float)((_rng.NextDouble() - 0.5) * 0.4);
        return new AvatarAction
        {
            InputAction  = "IA_Move",
            AxisX        = lateralBias,
            AxisY        = 0.8f + (float)(_rng.NextDouble() * 0.2),
            Magnitude    = 0.7f + (float)(_rng.NextDouble() * 0.3),
            HoldDuration = 0.5f + (float)(_rng.NextDouble() * 1.5),
            Source       = "ML"
        };
    }

    private AvatarAction CreateLookAction(AvatarCognitiveState state)
    {
        // Human-like looking: scan environment, focus on interesting objects
        return new AvatarAction
        {
            InputAction  = "IA_Look",
            AxisX        = (float)((_rng.NextDouble() - 0.5) * 2.0),
            AxisY        = (float)((_rng.NextDouble() - 0.5) * 0.5),
            Magnitude    = 0.3f + (float)(_rng.NextDouble() * 0.4),
            HoldDuration = 0.2f + (float)(_rng.NextDouble() * 0.8),
            Source       = "ML"
        };
    }

    private AvatarAction CreateSprintAction(AvatarCognitiveState state)
    {
        return new AvatarAction
        {
            InputAction  = "IA_Sprint",
            AxisX        = (float)((_rng.NextDouble() - 0.5) * 0.2),
            AxisY        = 1.0f,
            Magnitude    = 1.0f,
            HoldDuration = 1.0f + (float)(_rng.NextDouble() * 3f),
            Source       = "ML"
        };
    }

    private AvatarAction CreateJumpAction()
    {
        return new AvatarAction
        {
            InputAction  = "IA_Jump",
            Magnitude    = 1.0f,
            HoldDuration = 0f,
            Source       = "ML"
        };
    }

    private AvatarAction CreateInteractAction(AvatarCognitiveState state)
    {
        return new AvatarAction
        {
            InputAction  = "IA_Interact",
            Magnitude    = 1.0f,
            HoldDuration = 0.1f,
            Source       = "ML"
        };
    }

    private AvatarAction CreateCameraAction()
    {
        return new AvatarAction
        {
            InputAction  = "IA_Camera",
            Magnitude    = 1.0f,
            HoldDuration = 0f,
            Source       = "ML"
        };
    }
}
