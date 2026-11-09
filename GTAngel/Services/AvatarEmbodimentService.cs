using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO.Pipes;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
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
    public event EventHandler<string>?             OnEmbodimentLog;

    // ── State ─────────────────────────────────────────────────────────────────
    private EmotionalState     _currentEmotion     = new();
    private NeurochemicalState _currentNeuro       = new();
    private PersonalityTraits  _currentPersonality = new();
    private float[]            _currentAUs         = new float[47]; // AU1..AU46

    private readonly ILogger<AvatarEmbodimentService> _logger;
    private readonly ConcurrentQueue<string>          _ipcCommandQueue = new();

    // Named pipe for neurochemical readback
    private NamedPipeClientStream? _neuroPipe;
    private CancellationTokenSource? _neuroCts;
    private Task? _neuroReadbackTask;

    // Named pipe for sending FACS/IK commands to UE5
    private NamedPipeClientStream? _embodimentPipe;
    private readonly SemaphoreSlim _pipeLock = new(1, 1);

    private const string NeuroPipeName      = "GTAngel_Neuro_IPC";
    private const string EmbodimentPipeName = "GTAngel_Embodiment_IPC";

    public AvatarEmbodimentService(ILogger<AvatarEmbodimentService> logger)
    {
        _logger = logger;
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    public async Task StartAsync(CancellationToken ct = default)
    {
        Log("AvatarEmbodimentService starting — FACS+IK+Neuro+Personality pipeline");

        // Start neurochemical readback loop
        _neuroCts = CancellationTokenSource.CreateLinkedTokenSource(ct);
        _neuroReadbackTask = Task.Run(() => NeuroReadbackLoopAsync(_neuroCts.Token), ct);

        // Try to connect embodiment pipe (non-blocking — UE5 may not be running yet)
        _ = Task.Run(() => ConnectEmbodimentPipeAsync(ct), ct);

        Log("AvatarEmbodimentService ready");
        await Task.CompletedTask;
    }

    public async Task StopAsync()
    {
        _neuroCts?.Cancel();
        if (_neuroReadbackTask != null)
            await _neuroReadbackTask.WaitAsync(TimeSpan.FromSeconds(2)).ConfigureAwait(false);

        _neuroPipe?.Dispose();
        _embodimentPipe?.Dispose();
        Log("AvatarEmbodimentService stopped");
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
        // Build batch of Live2D SetParameterValue commands
        var commands = new List<object>();
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

        // Also send ExpressionSynthesizer.SynthesizeExpression
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

        // Trigger PhysicsDeformer for hair/cloth simulation
        commands.Add(new
        {
            Module  = "PhysicsDeformer",
            Command = "UpdatePhysicsSimulation",
            DeltaTime = 0.033f,
        });

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
            Module  = "SuperHotGirlPersonality",
            Command = "ApplyPersonality",
            Traits  = new
            {
                Confidence  = _currentPersonality.Confidence,
                Charm       = _currentPersonality.Charm,
                Playfulness = _currentPersonality.Playfulness,
                Wit         = _currentPersonality.Wit,
                Sass        = _currentPersonality.Sass,
            },
        };

        await SendEmbodimentBatchAsync(new List<object> { personalityCommand });
    }

    // ── Neurochemical readback loop ───────────────────────────────────────────

    private async Task NeuroReadbackLoopAsync(CancellationToken ct)
    {
        Log("Neurochemical readback loop starting — connecting to GTAngel_Neuro_IPC");

        while (!ct.IsCancellationRequested)
        {
            try
            {
                using var pipe = new NamedPipeClientStream(".", NeuroPipeName,
                    PipeDirection.In, PipeOptions.Asynchronous);

                await pipe.ConnectAsync(3000, ct);
                Log("Connected to GTAngel_Neuro_IPC neurochemical readback pipe");

                var buffer = new byte[4096];
                var sb = new StringBuilder();

                while (!ct.IsCancellationRequested && pipe.IsConnected)
                {
                    int bytesRead = await pipe.ReadAsync(buffer, ct);
                    if (bytesRead == 0) break;

                    sb.Append(Encoding.UTF8.GetString(buffer, 0, bytesRead));
                    string data = sb.ToString();
                    int newline;
                    while ((newline = data.IndexOf('\n')) >= 0)
                    {
                        string line = data[..newline].Trim();
                        data = data[(newline + 1)..];
                        if (!string.IsNullOrEmpty(line))
                            ProcessNeuroReadback(line);
                    }
                    sb.Clear();
                    sb.Append(data);
                }
            }
            catch (OperationCanceledException) { break; }
            catch (Exception ex)
            {
                _logger.LogDebug("Neuro pipe reconnecting: {Msg}", ex.Message);
                await Task.Delay(2000, ct).ConfigureAwait(false);
            }
        }
    }

    private void ProcessNeuroReadback(string json)
    {
        try
        {
            using var doc = JsonDocument.Parse(json);
            var root = doc.RootElement;

            _currentNeuro = new NeurochemicalState(
                Curiosity:   GetFloat(root, "CuriosityModule",       _currentNeuro.Curiosity),
                Endorphin:   GetFloat(root, "EndorphinJelly",        _currentNeuro.Endorphin),
                Chaos:       GetFloat(root, "ChaosController",       _currentNeuro.Chaos),
                Homeostasis: GetFloat(root, "HomeostasisRegulator",  _currentNeuro.Homeostasis)
            );

            OnNeurochemicalStateUpdated?.Invoke(this, _currentNeuro);
        }
        catch (JsonException) { /* malformed packet — skip */ }
    }

    private static float GetFloat(JsonElement root, string key, float fallback)
    {
        if (root.TryGetProperty(key, out var el) && el.TryGetSingle(out float v))
            return Math.Max(0f, Math.Min(1f, v));
        return fallback;
    }

    // ── IPC send helpers ──────────────────────────────────────────────────────

    private async Task ConnectEmbodimentPipeAsync(CancellationToken ct)
    {
        while (!ct.IsCancellationRequested)
        {
            try
            {
                var pipe = new NamedPipeClientStream(".", EmbodimentPipeName,
                    PipeDirection.Out, PipeOptions.Asynchronous);
                await pipe.ConnectAsync(3000, ct);
                _embodimentPipe = pipe;
                Log($"Connected to {EmbodimentPipeName} embodiment IPC pipe");
                return;
            }
            catch (OperationCanceledException) { return; }
            catch
            {
                await Task.Delay(2000, ct).ConfigureAwait(false);
            }
        }
    }

    private async Task SendEmbodimentBatchAsync(IEnumerable<object> commands)
    {
        if (_embodimentPipe == null || !_embodimentPipe.IsConnected) return;

        await _pipeLock.WaitAsync();
        try
        {
            string json = JsonSerializer.Serialize(new { Commands = commands }) + "\n";
            byte[] bytes = Encoding.UTF8.GetBytes(json);
            await _embodimentPipe.WriteAsync(bytes);
            await _embodimentPipe.FlushAsync();
        }
        catch (Exception ex)
        {
            _logger.LogDebug("Embodiment IPC send failed: {Msg}", ex.Message);
            _embodimentPipe?.Dispose();
            _embodimentPipe = null;
        }
        finally
        {
            _pipeLock.Release();
        }
    }

    // ── Public accessors ──────────────────────────────────────────────────────

    public EmotionalState     CurrentEmotion     => _currentEmotion;
    public NeurochemicalState CurrentNeuro       => _currentNeuro;
    public PersonalityTraits  CurrentPersonality => _currentPersonality;
    public float[]            CurrentAUs         => _currentAUs;

    // ── Helpers ───────────────────────────────────────────────────────────────

    private static float Clamp01(float v) => Math.Max(0f, Math.Min(1f, v));

    private void Log(string msg)
    {
        _logger.LogInformation("[AvatarEmbodiment] {Msg}", msg);
        OnEmbodimentLog?.Invoke(this, msg);
    }

    public void Dispose()
    {
        _neuroCts?.Cancel();
        _neuroCts?.Dispose();
        _neuroPipe?.Dispose();
        _embodimentPipe?.Dispose();
        _pipeLock.Dispose();
    }
}
