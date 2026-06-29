using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using GTAngel.Interop;
using GTAngel.Models.EmbodiedCognition;
using GTAngel.Services.EmbodiedCognition;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// GamerGirl Virtual Game Controller Interface — 4E Embodied Cognition Feature
///
/// Implements the "gamer girl" controller metaphor as a 4E cognitive extension:
///   Embodied  — controller grip posture, haptic feedback, body-schema awareness
///   Embedded  — situated in the game world through controller-as-extension-of-self
///   Enacted   — gaming intent patterns (combo inputs, rhythm timing, flow state)
///   Extended  — controller extends cognitive reach into the virtual world
///
/// Architecture:
///   GamerGirlControllerInterface ←→ VigemControllerService (hardware layer)
///              ↕
///   EmbodiedDecisionLoop (perception→decision→action)
///              ↕
///   DteCognitiveCoreService (ECAN attention / Thompson sampling)
///
/// KSM Cycle 7 — Embodied Interface Cognition:
///   Phase 7.1: Grip Schema (controller posture tracking)
///   Phase 7.2: Haptic Feedback Loop (rumble↔arousal integration)
///   Phase 7.3: Flow State Detection (gamma-band coherence proxy)
///   Phase 7.4: Gaming Intent Patterns (combo/rhythm/reaction)
///   Phase 7.5: Autognosis Integration (self-awareness of play style)
///
/// Alexander's 15 Properties addressed: P1 (Levels of Scale), P3 (Boundaries),
/// P5 (Positive Space), P7 (Local Symmetries), P10 (Not-Separateness)
/// </summary>
public sealed class GamerGirlControllerInterface : IDisposable
{
    private readonly ILogger<GamerGirlControllerInterface> _logger;
    private readonly VigemControllerService _controller;
    private bool _disposed;

    // ── Phase 7.1: Grip Schema ────────────────────────────────────────────────
    private GripPosture _currentGrip = GripPosture.Neutral;
    private float _gripTension;       // [0,1] — relaxed vs white-knuckle
    private float _gripAsymmetry;     // [0,1] — left/right hand balance

    // ── Phase 7.2: Haptic Feedback ────────────────────────────────────────────
    private float _leftRumble;        // [0,1] current left motor
    private float _rightRumble;       // [0,1] current right motor
    private float _hapticArousal;     // integrated arousal from haptic channel
    private readonly Queue<HapticPulse> _hapticQueue = new();
    private const int MaxHapticQueueSize = 32;

    // ── Phase 7.3: Flow State ─────────────────────────────────────────────────
    private FlowState _flowState = FlowState.Idle;
    private float _flowIntensity;     // [0,1] depth of flow immersion
    private float _flowCoherence;     // [0,1] gamma-band proxy (action consistency)
    private readonly RingBuffer<float> _reactionTimes = new(64);
    private readonly RingBuffer<float> _inputRhythm = new(128);
    private double _lastInputTimestamp;

    // ── Phase 7.4: Gaming Intent Patterns ─────────────────────────────────────
    private readonly List<GamingIntent> _intentHistory = new();
    private GamingIntent _currentIntent = GamingIntent.Exploring;
    private readonly ComboDetector _comboDetector = new();

    // ── Phase 7.5: Controller Metrics ─────────────────────────────────────────
    private int _totalInputsDispatched;
    private int _comboCount;
    private float _averageReactionTimeMs;
    private float _inputPrecision;     // [0,1] how close to optimal timing

    // ── Events ────────────────────────────────────────────────────────────────
    public event EventHandler<FlowStateChangedEventArgs>? FlowStateChanged;
    public event EventHandler<GripPostureChangedEventArgs>? GripPostureChanged;
    public event EventHandler<GamingIntentChangedEventArgs>? IntentChanged;
    public event EventHandler<HapticFeedbackEventArgs>? HapticFeedbackTriggered;
    public event EventHandler<ControllerMetricsSnapshot>? MetricsUpdated;

    public GamerGirlControllerInterface(
        ILogger<GamerGirlControllerInterface> logger,
        VigemControllerService controller)
    {
        _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        _controller = controller ?? throw new ArgumentNullException(nameof(controller));
        _logger.LogInformation("GamerGirl 4E Controller Interface initialized");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Public API
    // ═══════════════════════════════════════════════════════════════════════════

    /// <summary>Current grip posture (body schema).</summary>
    public GripPosture CurrentGrip => _currentGrip;

    /// <summary>Grip tension [0=relaxed, 1=white-knuckle]. Affects motor precision.</summary>
    public float GripTension => _gripTension;

    /// <summary>Current flow state (cognitive immersion level).</summary>
    public FlowState CurrentFlowState => _flowState;

    /// <summary>Flow intensity [0,1] — depth of zone immersion.</summary>
    public float FlowIntensity => _flowIntensity;

    /// <summary>Current gaming intent inferred from input patterns.</summary>
    public GamingIntent CurrentIntent => _currentIntent;

    /// <summary>Whether the controller is in a "clutch" high-performance state.</summary>
    public bool IsInClutch => _flowState == FlowState.DeepFlow && _gripTension > 0.7f;

    /// <summary>Total inputs dispatched through this interface.</summary>
    public int TotalInputsDispatched => _totalInputsDispatched;

    /// <summary>Whether ViGEm analog control is available.</summary>
    public bool HasAnalogControl => _controller.IsVigemAvailable;

    // ── Phase 7.1: Body-Schema-Aware Action Dispatch ──────────────────────────

    /// <summary>
    /// Dispatch an action through the gamer-girl embodied interface.
    /// Applies grip schema modulation, tracks flow state, and generates haptic feedback.
    /// </summary>
    public void DispatchEmbodiedAction(AvatarAction action, EmbodiedSelfState selfState)
    {
        if (action == null) return;

        var timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() / 1000.0;

        // Track input rhythm for flow detection
        if (_lastInputTimestamp > 0)
        {
            var delta = (float)(timestamp - _lastInputTimestamp);
            _inputRhythm.Push(delta);
        }
        _lastInputTimestamp = timestamp;

        // Infer grip posture from action type and self-state
        UpdateGripPosture(action, selfState);

        // Modulate action magnitude by grip tension (body schema feedback)
        float modulatedMagnitude = ModulateByGrip(action.Magnitude);

        // Map to controller input
        var controllerAction = MapToControllerAction(action, modulatedMagnitude);
        ExecuteControllerAction(controllerAction);

        // Generate haptic response
        GenerateHapticFeedback(action, selfState);

        // Update flow state
        UpdateFlowState(timestamp);

        // Detect gaming intent
        UpdateGamingIntent(action);

        // Detect combos
        _comboDetector.Feed(action.InputAction, timestamp);
        if (_comboDetector.HasCombo)
        {
            _comboCount++;
            _logger.LogDebug("Combo detected: {Combo} (total: {Count})",
                _comboDetector.LastCombo, _comboCount);
        }

        _totalInputsDispatched++;

        // Emit metrics periodically
        if (_totalInputsDispatched % 20 == 0)
            EmitMetrics();
    }

    /// <summary>
    /// Dispatch a continuous action vector (PPO/SAC style) through the embodied interface.
    /// </summary>
    public void DispatchContinuousAction(float[] actionVector, EmbodiedSelfState selfState)
    {
        if (actionVector == null || actionVector.Length < 2) return;

        // Modulate by grip schema
        var modulated = new float[actionVector.Length];
        for (int i = 0; i < actionVector.Length; i++)
        {
            modulated[i] = ModulateByGrip(actionVector[i]);
        }

        _controller.ExecuteContinuousAction(modulated);
        _totalInputsDispatched++;

        // Update flow from continuous input patterns
        var timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() / 1000.0;
        if (_lastInputTimestamp > 0)
            _inputRhythm.Push((float)(timestamp - _lastInputTimestamp));
        _lastInputTimestamp = timestamp;

        UpdateFlowState(timestamp);
    }

    /// <summary>
    /// Trigger a haptic pulse (rumble feedback) for embodied awareness.
    /// </summary>
    public void TriggerHaptic(float leftMotor, float rightMotor, float durationMs)
    {
        var pulse = new HapticPulse(
            Math.Clamp(leftMotor, 0f, 1f),
            Math.Clamp(rightMotor, 0f, 1f),
            Math.Clamp(durationMs, 0f, 2000f));

        if (_hapticQueue.Count < MaxHapticQueueSize)
            _hapticQueue.Enqueue(pulse);

        _leftRumble = pulse.LeftMotor;
        _rightRumble = pulse.RightMotor;

        // Haptic arousal integrates over time
        _hapticArousal = Math.Clamp(
            _hapticArousal * 0.9f + (leftMotor + rightMotor) * 0.5f * 0.1f,
            0f, 1f);

        HapticFeedbackTriggered?.Invoke(this, new HapticFeedbackEventArgs(
            pulse.LeftMotor,
            pulse.RightMotor,
            pulse.DurationMs));
    }

    /// <summary>
    /// Get the current controller metrics snapshot for telemetry/dashboard.
    /// </summary>
    public ControllerMetricsSnapshot GetMetrics()
    {
        return new ControllerMetricsSnapshot(
            Grip: _currentGrip,
            GripTension: _gripTension,
            GripAsymmetry: _gripAsymmetry,
            FlowState: _flowState,
            FlowIntensity: _flowIntensity,
            FlowCoherence: _flowCoherence,
            Intent: _currentIntent,
            TotalInputs: _totalInputsDispatched,
            ComboCount: _comboCount,
            AverageReactionTimeMs: _averageReactionTimeMs,
            InputPrecision: _inputPrecision,
            HapticArousal: _hapticArousal,
            HasAnalogControl: _controller.IsVigemAvailable,
            IsInClutch: IsInClutch);
    }

    /// <summary>
    /// Reset the interface state (e.g., after respawn or scene change).
    /// </summary>
    public void Reset()
    {
        _currentGrip = GripPosture.Neutral;
        _gripTension = 0f;
        _gripAsymmetry = 0f;
        _flowState = FlowState.Idle;
        _flowIntensity = 0f;
        _flowCoherence = 0f;
        _currentIntent = GamingIntent.Exploring;
        _hapticArousal = 0f;
        _lastInputTimestamp = 0;
        _reactionTimes.Clear();
        _inputRhythm.Clear();
        _comboDetector.Reset();
        _intentHistory.Clear();
        _totalInputsDispatched = 0;
        _comboCount = 0;

        _logger.LogInformation("GamerGirl controller interface reset");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Phase 7.1: Grip Schema (body posture tracking)
    // ═══════════════════════════════════════════════════════════════════════════

    private void UpdateGripPosture(AvatarAction action, EmbodiedSelfState selfState)
    {
        var previousGrip = _currentGrip;

        // Infer grip from action context
        _currentGrip = action.InputAction switch
        {
            "IA_Sprint" or "IA_Jump" => GripPosture.Aggressive,
            "IA_Crouch" => GripPosture.Defensive,
            "IA_Look" => GripPosture.Precision,
            "IA_Interact" => GripPosture.Relaxed,
            "IA_Move" when action.Magnitude > 0.8f => GripPosture.Aggressive,
            "IA_Move" when action.Magnitude < 0.3f => GripPosture.Relaxed,
            _ => _currentGrip
        };

        // Arousal increases grip tension
        _gripTension = Math.Clamp(
            selfState.Arousal * 0.4f + (action.Magnitude * 0.3f) + (_flowIntensity * 0.3f),
            0f, 1f);

        // Asymmetry: left hand (movement) vs right hand (camera/actions)
        _gripAsymmetry = action.InputAction switch
        {
            "IA_Move" => 0.7f,   // Left hand dominant
            "IA_Look" => 0.3f,   // Right hand dominant
            _ => 0.5f            // Balanced
        };

        if (_currentGrip != previousGrip)
        {
            GripPostureChanged?.Invoke(this, new GripPostureChangedEventArgs(
                previousGrip, _currentGrip, _gripTension));
        }
    }

    /// <summary>
    /// Modulate action magnitude based on grip schema.
    /// Aggressive grip = higher magnitude, precision grip = more controlled.
    /// </summary>
    internal float ModulateByGrip(float rawMagnitude)
    {
        float multiplier = _currentGrip switch
        {
            GripPosture.Aggressive => 1.0f + (_gripTension * 0.15f),
            GripPosture.Precision => 0.85f + (_gripTension * 0.05f),
            GripPosture.Defensive => 0.7f + (_gripTension * 0.1f),
            GripPosture.Relaxed => 0.6f + (_gripTension * 0.2f),
            _ => 1.0f
        };

        // Flow state enhances precision
        if (_flowState == FlowState.DeepFlow)
            multiplier *= 1.05f;

        return Math.Clamp(rawMagnitude * multiplier, 0f, 1f);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Phase 7.3: Flow State Detection
    // ═══════════════════════════════════════════════════════════════════════════

    private void UpdateFlowState(double timestamp)
    {
        var previousState = _flowState;

        // Calculate input rhythm consistency (low variance = high flow)
        _flowCoherence = CalculateRhythmCoherence();

        // Flow intensity is a combination of:
        //   - Rhythm coherence (consistent timing)
        //   - Combo frequency
        //   - Low reaction time variance
        //   - Sustained engagement (no idle gaps)
        float comboFactor = Math.Min(1f, _comboCount / 10f);
        float engagementFactor = _totalInputsDispatched > 0 ?
            Math.Min(1f, _totalInputsDispatched / 100f) : 0f;

        _flowIntensity = Math.Clamp(
            _flowCoherence * 0.4f + comboFactor * 0.3f + engagementFactor * 0.3f,
            0f, 1f);

        // State transitions
        _flowState = _flowIntensity switch
        {
            >= 0.8f => FlowState.DeepFlow,
            >= 0.5f => FlowState.InFlow,
            >= 0.2f => FlowState.Warming,
            _ => FlowState.Idle
        };

        if (_flowState != previousState)
        {
            _logger.LogDebug("Flow state: {Previous} → {Current} (intensity: {Intensity:F2})",
                previousState, _flowState, _flowIntensity);
            FlowStateChanged?.Invoke(this, new FlowStateChangedEventArgs(
                previousState, _flowState, _flowIntensity));
        }
    }

    internal float CalculateRhythmCoherence()
    {
        if (_inputRhythm.Count < 4) return 0f;

        var intervals = _inputRhythm.ToArray();
        float mean = intervals.Average();
        if (mean < 0.001f) return 0f;

        float variance = intervals.Select(x => (x - mean) * (x - mean)).Average();
        float cv = MathF.Sqrt(variance) / mean; // coefficient of variation

        // Low CV = high rhythm coherence
        return Math.Clamp(1f - (cv * 2f), 0f, 1f);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Phase 7.4: Gaming Intent Pattern Detection
    // ═══════════════════════════════════════════════════════════════════════════

    private void UpdateGamingIntent(AvatarAction action)
    {
        var previousIntent = _currentIntent;

        // Infer intent from recent action patterns
        _currentIntent = InferIntent(action);

        if (_currentIntent != previousIntent)
        {
            _intentHistory.Add(_currentIntent);
            if (_intentHistory.Count > 100) _intentHistory.RemoveAt(0);

            IntentChanged?.Invoke(this, new GamingIntentChangedEventArgs(
                previousIntent, _currentIntent));
        }
    }

    internal GamingIntent InferIntent(AvatarAction action)
    {
        return action.InputAction switch
        {
            "IA_Sprint" => GamingIntent.Rushing,
            "IA_Crouch" => GamingIntent.Stealth,
            "IA_Interact" => GamingIntent.Interacting,
            "IA_Jump" => GamingIntent.Traversal,
            "IA_Look" when _gripTension > 0.6f => GamingIntent.Scanning,
            "IA_Move" when action.Magnitude > 0.7f => GamingIntent.Rushing,
            "IA_Move" when action.Magnitude < 0.3f => GamingIntent.Exploring,
            "IA_StrafeR" or "IA_StrafeL" => GamingIntent.Combat,
            _ => _currentIntent // maintain current intent
        };
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Phase 7.2: Haptic Feedback Generation
    // ═══════════════════════════════════════════════════════════════════════════

    private void GenerateHapticFeedback(AvatarAction action, EmbodiedSelfState selfState)
    {
        // Generate context-sensitive haptic response
        float intensity = 0f;
        float leftBias = 0.5f;

        switch (action.InputAction)
        {
            case "IA_Sprint":
                intensity = 0.2f + selfState.Speed * 0.001f;
                leftBias = 0.5f; // symmetrical running vibration
                break;
            case "IA_Jump":
                intensity = 0.4f;
                leftBias = 0.5f;
                break;
            case "IA_Interact":
                intensity = 0.15f;
                leftBias = 0.7f; // left hand interaction
                break;
            case "IA_Move":
                intensity = action.Magnitude * 0.1f;
                leftBias = 0.5f + (action.AxisX * 0.3f); // directional feedback
                break;
            case "IA_Crouch":
                intensity = 0.1f;
                break;
        }

        if (intensity > 0.01f)
        {
            float left = Math.Clamp(intensity * leftBias * 2f, 0f, 1f);
            float right = Math.Clamp(intensity * (1f - leftBias) * 2f, 0f, 1f);
            TriggerHaptic(left, right, 50f);
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Private Helpers
    // ═══════════════════════════════════════════════════════════════════════════

    private ControllerAction MapToControllerAction(AvatarAction action, float magnitude)
    {
        return new ControllerAction(
            InputAction: action.InputAction,
            AxisX: action.AxisX,
            AxisY: action.AxisY,
            Magnitude: magnitude,
            Source: $"GamerGirl:{_currentGrip}:{_flowState}");
    }

    private void ExecuteControllerAction(ControllerAction action)
    {
        // Map embodied action to discrete controller input
        int discreteAction = action.InputAction switch
        {
            "IA_Move" when action.AxisY > 0.3f => (int)VigemControllerService.DiscreteAction.MoveForward,
            "IA_Move" when action.AxisY < -0.3f => (int)VigemControllerService.DiscreteAction.MoveBack,
            "IA_Move" when action.AxisX > 0.3f => (int)VigemControllerService.DiscreteAction.MoveRight,
            "IA_Move" when action.AxisX < -0.3f => (int)VigemControllerService.DiscreteAction.MoveLeft,
            "IA_Sprint" => (int)VigemControllerService.DiscreteAction.Sprint,
            "IA_Jump" => (int)VigemControllerService.DiscreteAction.Jump,
            "IA_Interact" => (int)VigemControllerService.DiscreteAction.EnterExitVehicle,
            "IA_Crouch" => (int)VigemControllerService.DiscreteAction.Noop,
            "IA_Look" => (int)VigemControllerService.DiscreteAction.LookLeft,
            "IA_StrafeR" => (int)VigemControllerService.DiscreteAction.MoveRight,
            "IA_StrafeL" => (int)VigemControllerService.DiscreteAction.MoveLeft,
            _ => (int)VigemControllerService.DiscreteAction.Noop
        };

        _controller.ExecuteDiscreteAction(discreteAction);
    }

    private void EmitMetrics()
    {
        _averageReactionTimeMs = _reactionTimes.Count > 0 ?
            _reactionTimes.ToArray().Average() : 0f;
        _inputPrecision = _flowCoherence;

        MetricsUpdated?.Invoke(this, GetMetrics());
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;
        _logger.LogInformation("GamerGirl controller interface disposed");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Nested Types
    // ═══════════════════════════════════════════════════════════════════════════

    internal record ControllerAction(
        string InputAction, float AxisX, float AxisY, float Magnitude, string Source);

    internal record HapticPulse(float LeftMotor, float RightMotor, float DurationMs);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Enums & Event Args
// ═══════════════════════════════════════════════════════════════════════════════

/// <summary>Controller grip posture reflecting body schema.</summary>
public enum GripPosture
{
    /// <summary>Default neutral hold — balanced both hands.</summary>
    Neutral,
    /// <summary>Aggressive forward lean — high intensity, sprint/combat.</summary>
    Aggressive,
    /// <summary>Precision hold — fine aiming, careful camera control.</summary>
    Precision,
    /// <summary>Defensive crouch posture — guarded, reactive.</summary>
    Defensive,
    /// <summary>Relaxed hold — exploration, idle, environmental interaction.</summary>
    Relaxed
}

/// <summary>Cognitive flow state (Csíkszentmihályi model adapted for gaming).</summary>
public enum FlowState
{
    /// <summary>No engagement — idle, menu, paused.</summary>
    Idle,
    /// <summary>Starting to engage — building muscle memory.</summary>
    Warming,
    /// <summary>Active engagement — good rhythm, moderate challenge.</summary>
    InFlow,
    /// <summary>Peak performance zone — perfect rhythm, high combo, auto-pilot precision.</summary>
    DeepFlow
}

/// <summary>Inferred gaming intent from input patterns.</summary>
public enum GamingIntent
{
    /// <summary>Casual exploration / sightseeing.</summary>
    Exploring,
    /// <summary>Fast traversal (sprinting, driving fast).</summary>
    Rushing,
    /// <summary>Stealth approach (crouching, slow movement).</summary>
    Stealth,
    /// <summary>Combat engagement (strafing, aiming).</summary>
    Combat,
    /// <summary>Environment scanning (camera movement, observation).</summary>
    Scanning,
    /// <summary>Object/NPC interaction.</summary>
    Interacting,
    /// <summary>Platforming / vertical traversal.</summary>
    Traversal
}

public sealed class FlowStateChangedEventArgs(FlowState previous, FlowState current, float intensity) : EventArgs
{
    public FlowState Previous { get; } = previous;
    public FlowState Current { get; } = current;
    public float Intensity { get; } = intensity;
}

public sealed class GripPostureChangedEventArgs(GripPosture previous, GripPosture current, float tension) : EventArgs
{
    public GripPosture Previous { get; } = previous;
    public GripPosture Current { get; } = current;
    public float Tension { get; } = tension;
}

public sealed class GamingIntentChangedEventArgs(GamingIntent previous, GamingIntent current) : EventArgs
{
    public GamingIntent Previous { get; } = previous;
    public GamingIntent Current { get; } = current;
}

public sealed class HapticFeedbackEventArgs(float leftMotor, float rightMotor, float durationMs) : EventArgs
{
    public float LeftMotor { get; } = leftMotor;
    public float RightMotor { get; } = rightMotor;
    public float DurationMs { get; } = durationMs;
}

/// <summary>Telemetry snapshot of the gamer-girl controller state.</summary>
public sealed record ControllerMetricsSnapshot(
    GripPosture Grip,
    float GripTension,
    float GripAsymmetry,
    FlowState FlowState,
    float FlowIntensity,
    float FlowCoherence,
    GamingIntent Intent,
    int TotalInputs,
    int ComboCount,
    float AverageReactionTimeMs,
    float InputPrecision,
    float HapticArousal,
    bool HasAnalogControl,
    bool IsInClutch);

// ═══════════════════════════════════════════════════════════════════════════════
// Support Classes
// ═══════════════════════════════════════════════════════════════════════════════

/// <summary>Simple ring buffer for sliding-window metrics.</summary>
internal sealed class RingBuffer<T>
{
    private readonly T[] _buffer;
    private int _head;
    private int _count;

    public RingBuffer(int capacity)
    {
        _buffer = new T[capacity];
    }

    public int Count => _count;
    public int Capacity => _buffer.Length;

    public void Push(T item)
    {
        _buffer[_head] = item;
        _head = (_head + 1) % _buffer.Length;
        if (_count < _buffer.Length) _count++;
    }

    public void Clear()
    {
        _head = 0;
        _count = 0;
    }

    public T[] ToArray()
    {
        if (_count == 0) return Array.Empty<T>();
        var result = new T[_count];
        if (_count < _buffer.Length)
        {
            Array.Copy(_buffer, 0, result, 0, _count);
        }
        else
        {
            int tailLen = _buffer.Length - _head;
            Array.Copy(_buffer, _head, result, 0, tailLen);
            Array.Copy(_buffer, 0, result, tailLen, _head);
        }
        return result;
    }
}

/// <summary>Detects combo input sequences (rapid successive actions).</summary>
internal sealed class ComboDetector
{
    private readonly List<(string Action, double Timestamp)> _buffer = new();
    private const double ComboWindowSec = 1.5;
    private const int MinComboLength = 3;

    public bool HasCombo { get; private set; }
    public string? LastCombo { get; private set; }

    public void Feed(string action, double timestamp)
    {
        // Expire old entries
        _buffer.RemoveAll(e => timestamp - e.Timestamp > ComboWindowSec);
        _buffer.Add((action, timestamp));

        // Check for combo (3+ distinct actions within window)
        HasCombo = false;
        if (_buffer.Count >= MinComboLength)
        {
            var distinct = _buffer.Select(e => e.Action).Distinct().Count();
            if (distinct >= MinComboLength)
            {
                HasCombo = true;
                LastCombo = string.Join("→", _buffer.TakeLast(MinComboLength).Select(e => e.Action));
            }
        }
    }

    public void Reset()
    {
        _buffer.Clear();
        HasCombo = false;
        LastCombo = null;
    }
}
