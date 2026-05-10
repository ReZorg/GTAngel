using System;

namespace GTAngel.Models.EmbodiedCognition;

/// <summary>
/// A single object detected through the avatar's visual sense.
///
/// Visual percepts respect the avatar's field-of-view cone, sight range, and
/// the underlying engine's occlusion (line-of-sight) check. Bearings are in
/// the avatar's body frame: <see cref="RelativeBearingDeg"/> is the yaw angle
/// to the object measured from the avatar's forward direction (positive = right,
/// negative = left), in degrees.
/// </summary>
public sealed class VisualPercept
{
    /// <summary>Stable identifier (forwarded from the engine's PerceivedObject.Tag).</summary>
    public string Tag { get; init; } = string.Empty;

    /// <summary>World-space location (X, Y, Z).</summary>
    public float[] WorldLocation { get; init; } = new float[3];

    /// <summary>Distance from the avatar in unreal units.</summary>
    public float Distance { get; init; }

    /// <summary>
    /// Yaw angle to the object in the avatar's body frame, in degrees.
    /// 0 = directly ahead. Positive = right. Negative = left. Range: (-180, 180].
    /// </summary>
    public float RelativeBearingDeg { get; init; }

    /// <summary>
    /// Pitch angle to the object in the avatar's body frame, in degrees.
    /// 0 = at eye level. Positive = above. Negative = below.
    /// </summary>
    public float RelativeElevationDeg { get; init; }

    /// <summary>
    /// Visual signal strength in [0, 1]. 1 = directly ahead, close, fully visible.
    /// 0 = at the edge of perception. Combines distance falloff and angular falloff.
    /// </summary>
    public float SignalStrength { get; init; }
}

/// <summary>
/// A single object detected through the avatar's auditory sense.
///
/// Auditory percepts ignore field-of-view and (by default) ignore visibility:
/// sound passes around the avatar and through (most) obstacles. They are
/// instead gated by hearing range and a 1/r² loudness falloff.
/// </summary>
public sealed class AuditoryPercept
{
    /// <summary>Stable identifier.</summary>
    public string Tag { get; init; } = string.Empty;

    /// <summary>World-space source location.</summary>
    public float[] WorldLocation { get; init; } = new float[3];

    /// <summary>Distance from the avatar in unreal units.</summary>
    public float Distance { get; init; }

    /// <summary>
    /// Bearing to the sound source in the avatar's body frame, in degrees.
    /// Useful for orientation responses ("where did that come from?").
    /// </summary>
    public float RelativeBearingDeg { get; init; }

    /// <summary>
    /// Loudness in [0, 1]. 1 = on top of the avatar. Falls off with 1/r².
    /// </summary>
    public float Loudness { get; init; }
}

/// <summary>
/// The avatar's awareness of its own body — what it knows about itself
/// without looking at the external world (proprioception + interoception
/// blended into a single bundle for the cognitive layer).
///
/// Named <c>EmbodiedSelfState</c> rather than <c>ProprioceptiveState</c> to
/// avoid colliding with the unrelated <c>GTAngel.Services.ProprioceptiveState</c>
/// used by the DTE SensoryInputIntegration on the GameRuntimeService side.
/// </summary>
public sealed class EmbodiedSelfState
{
    /// <summary>World-space position (X, Y, Z) of the avatar.</summary>
    public float[] Position { get; init; } = new float[3];

    /// <summary>Body orientation (Pitch, Yaw, Roll) in degrees.</summary>
    public float[] Rotation { get; init; } = new float[3];

    /// <summary>Linear velocity (X, Y, Z) in unreal units per second.</summary>
    public float[] Velocity { get; init; } = new float[3];

    /// <summary>Cached planar speed magnitude (XY) in unreal units per second.</summary>
    public float Speed { get; init; }

    /// <summary>
    /// Optional self-state hooks. Higher values mean more demand on the avatar's
    /// homeostatic budget (e.g. fatigue from sprinting, pain from collisions).
    /// Default 0 — populated by future systems that hook into the body sim.
    /// </summary>
    public float Fatigue { get; init; }

    /// <summary>Internal arousal proxy in [0, 1].</summary>
    public float Arousal { get; init; }
}

/// <summary>
/// The complete perceptual field the avatar has access to at one decision tick.
///
/// This is the *only* world information the embodied decision loop is allowed
/// to use — it deliberately omits the raw <see cref="GTAngel.Interop.AvatarObservation"/>
/// so the cognitive policy cannot accidentally peek at unperceived state.
/// </summary>
public sealed class PerceptualField
{
    /// <summary>Engine timestamp the perception was sampled at (seconds).</summary>
    public double Timestamp { get; init; }

    /// <summary>Objects the avatar can currently see.</summary>
    public VisualPercept[] Visuals { get; init; } = Array.Empty<VisualPercept>();

    /// <summary>Sources the avatar can currently hear.</summary>
    public AuditoryPercept[] Sounds { get; init; } = Array.Empty<AuditoryPercept>();

    /// <summary>The avatar's awareness of its own body.</summary>
    public EmbodiedSelfState Self { get; init; } = new();

    /// <summary>Total objects considered (pre-filter), exposed for diagnostics only.</summary>
    public int RawCandidateCount { get; init; }
}
