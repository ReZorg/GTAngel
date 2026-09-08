using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO.Pipes;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using GTAngel.Interop;
using GTAngel.Models;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// KSM Cycle 3 — UE5 Avatar Embodiment Service
/// Composition: /echo-wpf-ksm-evolve → target: ue5_avatar_embodiment
///
/// Bridges the WPF cognitive layer to the full UE5 avatar embodiment stack:
///   • FACS Action Unit mapper (AU1..AU46 → Live2D SetParameterValue commands)
///   • EmotionalState synthesizer (ESN reservoir output → FEmotionalState)
///   • IK pose blend sender (ESN action[12..17] → UE5 IK targets via IPC)
///   • NeurochemicalSystem readback (GTAngel_Neuro_IPC pipe → WPF events)
///   • SuperHotGirlPersonality trait applicator (cognitive state → trait weights)
///
/// Alexander's 15 Properties addressed: P5, P8, P11, P12, P14
/// </summary>
public sealed class AvatarEmbodimentService : IDisposable
{
    // ── FACS Action Unit definitions (Ekman AU system) ────────────────────────
    private static readonly Dictionary<int, string> AuToLive2DParam = new()
    {
        // Upper face
        { 1,  "ParamBrowLInnerUp"   },   // Inner Brow Raise
        { 2,  "ParamBrowRInnerUp"   },
        { 4,  "ParamBrowLowerer"    },   // Brow Lowerer
        { 5,  "ParamEyeOpenL"       },   // Upper Lid Raiser
        { 6,  "ParamEyeOpenR"       },
        { 7,  "ParamEyeSquintL"     },   // Lid Tightener
        { 9,  "ParamNoseWrinkle"    },   // Nose Wrinkler
        // Lower face
        { 10, "ParamMouthSmileL"    },   // Upper Lip Raiser
        { 11, "ParamMouthSmileR"    },
        { 12, "ParamMouthSmileL"    },   // Lip Corner Puller (Smile)
        { 13, "ParamMouthSmileR"    },
        { 15, "ParamMouthFrownL"    },   // Lip Corner Depressor
        { 16, "ParamMouthFrownR"    },
        { 17, "ParamChinRaise"      },   // Chin Raiser
        { 20, "ParamMouthStretchL"  },   // Lip Stretcher
        { 23, "ParamMouthPress"     },   // Lip Tightener
        { 24, "ParamMouthPress"     },   // Lip Pressor
        { 25, "ParamMouthOpen"      },   // Lips Part
        { 26, "ParamMouthOpen"      },   // Jaw Drop
        { 27, "ParamMouthOpen"      },   // Mouth Stretch
        { 28, "ParamMouthPucker"    },   // Lip Suck
        { 43, "ParamEyeOpenL"       },   // Eye Closure (inverted)
        { 44, "ParamEyeOpenR"       },
        { 45, "ParamEyeBallX"       },   // Blink
        { 46, "ParamEyeBallY"       },   // Wink
    };

    // ── Emotion → FACS AU weights (Ekman basic emotions) ─────────────────────
    private static readonly Dictionary<string, Dictionary<int, float>> EmotionAuWeights = new()
    {
        ["Happiness"] = new() { { 6, 0.8f }, { 12, 1.0f }, { 13, 1.0f }, { 25, 0.3f } },
        ["Surprise"]  = new() { { 1, 0.9f }, { 2, 0.9f }, { 5, 1.0f }, { 6, 0.5f }, { 26, 0.8f }, { 27, 0.5f } },
        ["Sadness"]   = new() { { 1, 0.6f }, { 4, 0.3f }, { 15, 0.7f }, { 17, 0.5f } },
        ["Anger"]     = new() { { 4, 1.0f }, { 5, 0.4f }, { 7, 0.6f }, { 23, 0.5f }, { 24, 0.4f } },
        ["Fear"]      = new() { { 1, 0.8f }, { 2, 0.7f }, { 4, 0.5f }, { 5, 0.7f }, { 7, 0.4f }, { 20, 0.6f }, { 26, 0.5f } },
        ["Disgust"]   = new() { { 9, 0.8f }, { 15, 0.4f }, { 16, 0.4f }, { 17, 0.3f } },
    };

    // ── SuperHotGirlPersonality default traits ────────────────────────────────
    public record PersonalityTraits(
        float Confidence  = 0.8f,
        float Charm       = 0.9f,
        float Playfulness = 0.7f,
        float Wit         = 0.8f,
        float Sass        = 0.6f
    );

    // ── Neurochemical state (read back from UE5) ──────────────────────────────
    public record NeurochemicalState(
        float Curiosity    = 0.5f,
        float Endorphin    = 0.5f,
        float Chaos        = 0.3f,
        float Homeostasis  = 0.7f
    );

    // ── Emotional state (synthesized from ESN output) ─────────────────────────
    public record EmotionalState(
        float Happiness = 0f,
        float Surprise  = 0f,
        float Sadness   = 0f,
        float Anger     = 0f,
        float Fear      = 0f
    );

    // ── Events ────────────────────────────────────────────────────────────────
    public event EventHandler<EmotionalState>?     OnEmotionalStateUpdated;
    public event EventHandler<NeurochemicalState>? OnNeurochemicalStateUpdated;
    public event EventHandler<PersonalityTraits>?  OnPersonalityTraitsUpdated;
    public event EventHandler<float[]>?            OnFACSAUsUpdated;   // 46-element AU array [0..1]
    public event EventHandler<AvatarRuntimeProfile>? OnAvatarProfileActivated;
    public event EventHandler<string>?             OnEmbodimentLog;

    // ── State ─────────────────────────────────────────────────────────────────
    private EmotionalState     _currentEmotion     = new();
    private NeurochemicalState _currentNeuro       = new();
    private PersonalityTraits  _currentPersonality = new();
    private float[]            _currentAUs         = new float[47]; // AU1..AU46
    private AvatarRuntimeProfile? _activeProfile;

    private readonly ILogger<AvatarEmbodimentService> _logger;
    private readonly ConcurrentQueue<string> _ipcCommandQueue = new();
    private readonly SemaphoreSlim _queueSignal = new(0);
    private CancellationTokenSource? _workerCts;
    private Task? _embodimentWorkerTask;
    private string? _pendingActivationBatch;
    private AvatarRuntimeProfile? _pendingActivationProfile;
    private bool _disposed;

    // Named pipe for sending FACS/IK commands to UE5
    private NamedPipeClientStream? _embodimentPipe;
    private const string EmbodimentPipeName = "GTAngel_Embodiment_IPC";

    public AvatarEmbodimentService(ILogger<AvatarEmbodimentService> logger)
    {
        _logger = logger;
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    public async Task StartAsync(CancellationToken ct = default)
    {
        ObjectDisposedException.ThrowIf(_disposed, this);
        if (_embodimentWorkerTask is { IsCompleted: false }) return;
        Log("AvatarEmbodimentService starting — FACS+IK+Neuro+Personality pipeline");

        _workerCts = CancellationTokenSource.CreateLinkedTokenSource(ct);
        _embodimentWorkerTask = Task.Run(
            () => EmbodimentWorkerAsync(_workerCts.Token), _workerCts.Token);

        Log("AvatarEmbodimentService ready");
        await Task.CompletedTask;
    }

    public async Task StopAsync()
    {
        _workerCts?.Cancel();
        _embodimentPipe?.Dispose();
        if (_embodimentWorkerTask != null)
        {
            try
            {
                await _embodimentWorkerTask.WaitAsync(TimeSpan.FromSeconds(3)).ConfigureAwait(false);
            }
            catch (OperationCanceledException) { }
            catch (TimeoutException) { }
        }
        _embodimentPipe = null;
        Log("AvatarEmbodimentService stopped");
    }

    // ── Avatar asset profile activation ───────────────────────────────────────

    /// <summary>
    /// Build the renderer activation plan for a validated, versioned avatar
    /// package. This is public so the exact UE5 contract can be unit tested
    /// without requiring a running named-pipe endpoint.
    /// </summary>
    public static IReadOnlyList<AvatarModuleCommand> BuildAvatarActivationPlan(
        AvatarRuntimeProfile profile)
    {
        ArgumentNullException.ThrowIfNull(profile);
        var manifest = profile.Manifest;
        var commands = new List<AvatarModuleCommand>
        {
            new("Avatar3DComponent", "LoadAssetProfile", new
            {
                ProfileId = manifest.Id,
                manifest.Name,
                manifest.Version,
                MeshPath = profile.GetAssetPath("Mesh"),
                manifest.HeightCentimeters,
                manifest.CoordinateSystem,
                Rig = manifest.Rig,
                Import = manifest.Ue5Import,
            }),
            new("Avatar3DComponent", "ConfigureLocomotion", new
            {
                WalkAnimationPath = profile.AssetPaths.GetValueOrDefault("Walk"),
                RunAnimationPath = profile.AssetPaths.GetValueOrDefault("Run"),
                WalkFrameRate = manifest.Assets.FirstOrDefault(a => a.Key.Equals("Walk", StringComparison.OrdinalIgnoreCase))?.FrameRate,
                RunFrameRate = manifest.Assets.FirstOrDefault(a => a.Key.Equals("Run", StringComparison.OrdinalIgnoreCase))?.FrameRate,
                IdleMode = profile.AssetPaths.ContainsKey("Idle") ? "Animation" : "ReferencePoseBreathing",
            }),
            new("Avatar3DComponent", "ConfigurePbrMaterial", new
            {
                BaseColorPath = profile.GetAssetPath(manifest.Material.BaseColorAssetKey),
                NormalPath = profile.GetAssetPath(manifest.Material.NormalAssetKey),
                MetallicPath = profile.GetAssetPath(manifest.Material.MetallicAssetKey),
                RoughnessPath = profile.GetAssetPath(manifest.Material.RoughnessAssetKey),
                EmissiveSourcePath = profile.GetAssetPath(manifest.Material.EmissiveSourceAssetKey),
                manifest.Material.EmissiveIntensity,
                manifest.Material.TwoSided,
            }),
            new("Avatar3DComponent", "ConfigureExpressionDriver", new
            {
                manifest.Expression.Mode,
                manifest.Expression.SupportsFacs,
                manifest.Expression.HeadBone,
                manifest.Expression.GazeBone,
                manifest.Expression.JawBone,
                manifest.Expression.DriveEmissiveAura,
            }),
        };

        return commands;
    }

    /// <summary>Activate a validated avatar and transmit its UE5 import/runtime plan.</summary>
    public async Task ActivateAvatarProfileAsync(AvatarRuntimeProfile profile)
    {
        ArgumentNullException.ThrowIfNull(profile);
        var plan = BuildAvatarActivationPlan(profile);
        _pendingActivationProfile = profile;
        Interlocked.Exchange(ref _pendingActivationBatch, SerializeBatch(plan));
        _queueSignal.Release();
        Log($"Avatar profile queued: {profile.Manifest.Name} v{profile.Manifest.Version} " +
            $"({profile.Manifest.Expression.Mode})");
        await Task.CompletedTask;
    }

    // ── Step 1: Synthesize EmotionalState from ESN reservoir output ───────────

    /// <summary>
    /// Maps ESN reservoir output vector (first 6 dims) to a normalised FEmotionalState.
    /// ESN[0]=valence, ESN[1]=arousal, ESN[2]=dominance, ESN[3]=curiosity,
    /// ESN[4]=fear_signal, ESN[5]=social_signal
    /// </summary>
    public EmotionalState SynthesizeEmotionalState(float[] esnOutput, NeurochemicalState neuro)
    {
        if (esnOutput.Length < 6) return _currentEmotion;

        float valence   = Clamp01(esnOutput[0]);
        float arousal   = Clamp01(esnOutput[1]);
        float dominance = Clamp01(esnOutput[2]);
        float curiosity = Clamp01(esnOutput[3]);
        float fearSig   = Clamp01(esnOutput[4]);
        float social    = Clamp01(esnOutput[5]);

        // Map to Ekman basic emotions using Russell's circumplex model
        float happiness = valence * (1f - fearSig) * (0.5f + 0.5f * social)
                          * (1f + 0.2f * neuro.Endorphin);
        float surprise  = arousal * (1f - valence * 0.5f) * curiosity;
        float sadness   = (1f - valence) * (1f - arousal) * (1f - dominance);
        float anger     = (1f - valence) * arousal * dominance;
        float fear      = fearSig * (1f - dominance) * (1f + 0.3f * neuro.Chaos);

        // Normalise so max emotion = 1
        float maxE = Math.Max(0.001f, Math.Max(happiness, Math.Max(surprise,
                     Math.Max(sadness, Math.Max(anger, fear)))));
        float scale = 1f / maxE;

        _currentEmotion = new EmotionalState(
            Happiness: Clamp01(happiness * scale),
            Surprise:  Clamp01(surprise  * scale),
            Sadness:   Clamp01(sadness   * scale),
            Anger:     Clamp01(anger     * scale),
            Fear:      Clamp01(fear      * scale)
        );

        OnEmotionalStateUpdated?.Invoke(this, _currentEmotion);
        return _currentEmotion;
    }

    // ── Step 2: Map EmotionalState → FACS AUs ────────────────────────────────

    public float[] ComputeFACSActionUnits(EmotionalState emotion)
    {
        var aus = new float[47]; // AU1..AU46, index = AU number

        void BlendEmotion(string emotionName, float weight)
        {
            if (!EmotionAuWeights.TryGetValue(emotionName, out var auMap)) return;
            foreach (var (au, auWeight) in auMap)
                aus[au] = Math.Min(1f, aus[au] + auWeight * weight);
        }

        BlendEmotion("Happiness", emotion.Happiness);
        BlendEmotion("Surprise",  emotion.Surprise);
        BlendEmotion("Sadness",   emotion.Sadness);
        BlendEmotion("Anger",     emotion.Anger);
        BlendEmotion("Fear",      emotion.Fear);

        _currentAUs = aus;
        OnFACSAUsUpdated?.Invoke(this, aus);
        return aus;
    }

    // ── Step 3: Send FACS commands to UE5 via IPC ────────────────────────────

    public async Task SendFACSCommandsAsync(float[] aus)
    {
        // Build a native FACS/Live2D batch when the active rig supports it.
        // Arc Angel Echo has no facial deformation channels, so its cognitive
        // expression is projected through head/gaze pose and the neon aura.
        var commands = new List<object>();
        bool useFacialParameters = _activeProfile?.Manifest.Expression.SupportsFacs ?? true;
        if (useFacialParameters)
        {
            foreach (var (au, paramName) in AuToLive2DParam)
            {
                if (au >= aus.Length) continue;
                commands.Add(new
                {
                    Module    = "Live2DCubismAvatarComponent",
                    Command   = "SetParameterValue",
                    Parameter = paramName,
                    Value     = aus[au],
                });
            }
        }
        else
        {
            float valence = Clamp01(_currentEmotion.Happiness -
                0.5f * (_currentEmotion.Sadness + _currentEmotion.Anger) + 0.5f);
            float arousal = Clamp01(Math.Max(_currentEmotion.Surprise,
                Math.Max(_currentEmotion.Anger, _currentEmotion.Fear)));
            commands.Add(new
            {
                Module = "Avatar3DComponent",
                Command = "SetSkeletalAuraExpression",
                Parameters = new
                {
                    HeadBone = _activeProfile?.Manifest.Expression.HeadBone ?? "Head",
                    GazeBone = _activeProfile?.Manifest.Expression.GazeBone ?? "headfront",
                    HeadPitch = (_currentEmotion.Surprise - _currentEmotion.Sadness) * 8f,
                    HeadYaw = (_currentEmotion.Happiness - _currentEmotion.Anger) * 5f,
                    GazeIntensity = Clamp01(0.35f + arousal * 0.65f),
                    AuraIntensity = Clamp01(0.2f + valence * 0.45f + arousal * 0.35f),
                    AuraValence = valence,
                    AuraArousal = arousal,
                },
            });
        }

        if (useFacialParameters)
        {
            commands.Add(new
            {
                Module  = "ExpressionSynthesizer",
                Command = "SynthesizeExpression",
                Emotion = new
                {
                    Happiness = _currentEmotion.Happiness,
                    Surprise  = _currentEmotion.Surprise,
                    Sadness   = _currentEmotion.Sadness,
                    Anger     = _currentEmotion.Anger,
                    Fear      = _currentEmotion.Fear,
                },
            });
        }

        await SendEmbodimentBatchAsync(commands);
    }

    // ── Step 4: Send IK pose blend to UE5 ────────────────────────────────────

    /// <summary>
    /// Maps ESN action output dims [12..17] to UE5 IK targets:
    /// [12]=head_pitch, [13]=head_yaw, [14]=torso_lean, [15]=arm_l_reach, [16]=arm_r_reach, [17]=gaze_target
    /// </summary>
    public async Task SendIKPoseBlendAsync(float[] esnAction)
    {
        if (esnAction.Length < 18) return;

        var ikCommand = new
        {
            Module  = "Avatar3DComponent",
            Command = "SetIKTargets",
            IKTargets = new
            {
                HeadPitch    = esnAction[12] * 45f,   // degrees
                HeadYaw      = esnAction[13] * 90f,
                TorsoLean    = esnAction[14] * 20f,
                ArmLReach    = esnAction[15],          // normalised 0..1
                ArmRReach    = esnAction[16],
                GazeTarget   = esnAction[17],          // normalised 0..1 (look-at blend)
            },
        };

        await SendEmbodimentBatchAsync(new List<object> { ikCommand });
    }

    // ── Step 5: Apply SuperHotGirlPersonality traits ──────────────────────────

    /// <summary>
    /// Maps the current cognitive state (autonomy level, coherence) to personality trait weights
    /// and sends ApplyPersonality command to UE5.
    /// </summary>
    public async Task ApplyPersonalityTraitsAsync(float autonomyLevel, float coherence, NeurochemicalState neuro)
    {
        // Higher autonomy → more confidence and wit
        // Higher coherence → more charm
        // Higher curiosity → more playfulness
        // Higher chaos → more sass
        _currentPersonality = new PersonalityTraits(
            Confidence:  Clamp01(0.6f + autonomyLevel * 0.1f + coherence * 0.1f),
            Charm:       Clamp01(0.7f + coherence * 0.2f),
            Playfulness: Clamp01(0.5f + neuro.Curiosity * 0.4f),
            Wit:         Clamp01(0.6f + autonomyLevel * 0.15f),
            Sass:        Clamp01(0.4f + neuro.Chaos * 0.4f)
        );

        OnPersonalityTraitsUpdated?.Invoke(this, _currentPersonality);

        var personalityCommand = new
        {
            Module  = "Avatar3DComponent",
            Command = "ApplyPersonality",
            Parameters = new
            {
                Traits = new
                {
                    Confidence  = _currentPersonality.Confidence,
                    Charm       = _currentPersonality.Charm,
                    Playfulness = _currentPersonality.Playfulness,
                    Wit         = _currentPersonality.Wit,
                    Sass        = _currentPersonality.Sass,
                },
            },
        };

        await SendEmbodimentBatchAsync(new List<object> { personalityCommand });
    }

    // ── Neurochemical readback from main AvatarObservation transport ──────────

    public void UpdateNeurochemicalState(NeurochemicalSnapshot? snapshot)
    {
        if (snapshot == null) return;
        _currentNeuro = new NeurochemicalState(
            Curiosity: Clamp01(snapshot.Curiosity),
            Endorphin: Clamp01(snapshot.Endorphin),
            Chaos: Clamp01(snapshot.ChaosIntensity),
            Homeostasis: Clamp01(snapshot.Homeostasis));
        OnNeurochemicalStateUpdated?.Invoke(this, _currentNeuro);
    }

    // ── IPC send helpers ──────────────────────────────────────────────────────

    private async Task EmbodimentWorkerAsync(CancellationToken ct)
    {
        while (!ct.IsCancellationRequested)
        {
            try
            {
                var pipe = new NamedPipeClientStream(".", EmbodimentPipeName,
                    PipeDirection.Out, PipeOptions.Asynchronous);
                await pipe.ConnectAsync(3000, ct).ConfigureAwait(false);
                _embodimentPipe = pipe;
                Log($"Connected to {EmbodimentPipeName} embodiment IPC pipe");

                while (!ct.IsCancellationRequested && pipe.IsConnected)
                {
                    await SendPendingActivationAsync(pipe, ct).ConfigureAwait(false);
                    while (_ipcCommandQueue.TryDequeue(out string? batch))
                    {
                        try
                        {
                            await WriteBatchAsync(pipe, batch, ct).ConfigureAwait(false);
                        }
                        catch
                        {
                            _ipcCommandQueue.Enqueue(batch);
                            throw;
                        }
                    }
                    await _queueSignal.WaitAsync(ct).ConfigureAwait(false);
                }
            }
            catch (OperationCanceledException) when (ct.IsCancellationRequested) { return; }
            catch (Exception ex)
            {
                _logger.LogDebug("Embodiment IPC reconnecting: {Msg}", ex.Message);
                _embodimentPipe?.Dispose();
                _embodimentPipe = null;
                await Task.Delay(2000, ct).ConfigureAwait(false);
            }
        }
    }

    private async Task SendPendingActivationAsync(
        NamedPipeClientStream pipe,
        CancellationToken cancellationToken)
    {
        string? activation = Volatile.Read(ref _pendingActivationBatch);
        if (activation == null) return;

        var profile = _pendingActivationProfile;
        await WriteBatchAsync(pipe, activation, cancellationToken).ConfigureAwait(false);
        if (!ReferenceEquals(
            Interlocked.CompareExchange(ref _pendingActivationBatch, null, activation),
            activation))
        {
            return;
        }

        _pendingActivationProfile = null;
        if (profile == null) return;
        _activeProfile = profile;
        OnAvatarProfileActivated?.Invoke(this, profile);
        Log($"Avatar profile activated: {profile.Manifest.Name} v{profile.Manifest.Version} " +
            $"({profile.Manifest.Expression.Mode})");
    }

    private Task SendEmbodimentBatchAsync(IEnumerable<object> commands)
    {
        _ipcCommandQueue.Enqueue(SerializeBatch(commands));
        _queueSignal.Release();
        return Task.CompletedTask;
    }

    private static string SerializeBatch(IEnumerable<object> commands) =>
        JsonSerializer.Serialize(new { Commands = commands }) + "\n";

    private static async Task WriteBatchAsync(
        NamedPipeClientStream pipe,
        string batch,
        CancellationToken cancellationToken)
    {
        byte[] bytes = Encoding.UTF8.GetBytes(batch);
        await pipe.WriteAsync(bytes, cancellationToken).ConfigureAwait(false);
        await pipe.FlushAsync(cancellationToken).ConfigureAwait(false);
    }

    // ── Public accessors ──────────────────────────────────────────────────────

    public EmotionalState     CurrentEmotion     => _currentEmotion;
    public NeurochemicalState CurrentNeuro       => _currentNeuro;
    public PersonalityTraits  CurrentPersonality => _currentPersonality;
    public float[]            CurrentAUs         => _currentAUs;
    public AvatarRuntimeProfile? ActiveProfile   => _activeProfile;

    // ── Helpers ───────────────────────────────────────────────────────────────

    private static float Clamp01(float v) => Math.Max(0f, Math.Min(1f, v));

    private void Log(string msg)
    {
        _logger.LogInformation("[AvatarEmbodiment] {Msg}", msg);
        OnEmbodimentLog?.Invoke(this, msg);
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;
        StopAsync().GetAwaiter().GetResult();
        _workerCts?.Dispose();
        _queueSignal.Dispose();
    }
}
