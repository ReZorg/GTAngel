using System;
using System.Linq;
using GTAngel.Interop;
using GTAngel.Models.EmbodiedCognition;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services.EmbodiedCognition;

/// <summary>
/// Closed-loop embodied cognition for an AI player avatar.
///
///   AvatarObservation
///         │           (raw, ground-truth — only the perception service may see this)
///         ▼
///   SensoryPerceptionService.Perceive() ─► PerceptualField  (sight + hearing + body)
///         │
///         ▼
///   SpatialMemory.Update()                 (decay + landmark accumulation)
///         │
///         ▼
///   IPerceptionPolicy.Decide()             (cognitive decision — sees ONLY field + memory)
///         │
///         ▼
///   MotorController.Translate() ───────────► AvatarAction (UE5 Enhanced Input)
///
/// The cognitive policy receives the perceptual field and the memory snapshot
/// — it does not receive the raw <see cref="AvatarObservation"/>. This enforces
/// "limited world knowledge": the AI only knows what it has perceived.
/// </summary>
public sealed class EmbodiedDecisionLoop
{
    private readonly SensoryPerceptionService _perception;
    private readonly SpatialMemory _memory;
    private readonly MotorController _motor;
    private readonly IPerceptionPolicy _policy;
    private readonly ILogger<EmbodiedDecisionLoop>? _logger;

    /// <summary>The most recent perception, intent, and dispatched action — for telemetry.</summary>
    public PerceptualField? LastField { get; private set; }
    public MotorIntent?     LastIntent { get; private set; }
    public AvatarAction?    LastAction { get; private set; }

    /// <summary>Raised after each successful Step() with the dispatched action.</summary>
    public event EventHandler<AvatarAction>? ActionProduced;

    public EmbodiedDecisionLoop(
        SensoryPerceptionService perception,
        SpatialMemory memory,
        MotorController motor,
        IPerceptionPolicy policy,
        ILogger<EmbodiedDecisionLoop>? logger = null)
    {
        _perception = perception ?? throw new ArgumentNullException(nameof(perception));
        _memory     = memory     ?? throw new ArgumentNullException(nameof(memory));
        _motor      = motor      ?? throw new ArgumentNullException(nameof(motor));
        _policy     = policy     ?? throw new ArgumentNullException(nameof(policy));
        _logger     = logger;
    }

    /// <summary>Convenience accessors for callers that want to inspect substate.</summary>
    public SensoryPerceptionService Perception => _perception;
    public SpatialMemory            Memory     => _memory;
    public MotorController          Motor      => _motor;
    public IPerceptionPolicy        Policy     => _policy;

    /// <summary>
    /// Run a single Perceive → Remember → Decide → Act cycle and return the
    /// action to be dispatched (or <c>null</c> if the policy chose Idle).
    ///
    /// The caller is responsible for actually sending the returned action to UE5.
    /// </summary>
    public AvatarAction? Step(AvatarObservation observation)
    {
        if (observation == null) throw new ArgumentNullException(nameof(observation));

        // ── Perceive ──────────────────────────────────────────────────────
        var field = _perception.Perceive(observation);
        LastField = field;

        // ── Remember ──────────────────────────────────────────────────────
        _memory.Decay(field.Timestamp);
        _memory.Update(field);

        // ── Decide (perception-limited) ───────────────────────────────────
        var memorySnapshot = _memory.Snapshot();
        var intent = _policy.Decide(field, memorySnapshot) ?? MotorIntent.IdleIntent;
        LastIntent = intent;

        // ── Act ───────────────────────────────────────────────────────────
        var action = _motor.Translate(intent, field.Self);
        LastAction = action;

        if (action != null)
        {
            _logger?.LogTrace(
                "EmbodiedDecisionLoop: intent={IntentType} → action={Action} mag={Mag:F2}",
                intent.Type, action.InputAction, action.Magnitude);
            ActionProduced?.Invoke(this, action);
        }

        return action;
    }
}
