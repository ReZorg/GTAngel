namespace GTAngel.Models.EmbodiedCognition;

/// <summary>
/// High-level, body-independent action the cognitive layer wants the avatar
/// to perform. The <see cref="GTAngel.Services.EmbodiedCognition.MotorController"/>
/// is responsible for translating these into concrete UE5 Enhanced Input
/// commands (<see cref="GTAngel.Interop.AvatarAction"/>) while honouring the
/// avatar's physical constraints.
/// </summary>
public enum MotorIntentType
{
    /// <summary>Stand still, no input dispatched.</summary>
    Idle = 0,

    /// <summary>Move toward a world location (planar XY).</summary>
    MoveToward,

    /// <summary>Strafe along the avatar's right-hand axis (positive) or left (negative).</summary>
    Strafe,

    /// <summary>Rotate the avatar to face a world location.</summary>
    TurnTo,

    /// <summary>Hop. Suppressed by the motor controller while crouched.</summary>
    Jump,

    /// <summary>Crouch toggle.</summary>
    Crouch,

    /// <summary>Run faster. Suppressed by the motor controller while crouched.</summary>
    Sprint,

    /// <summary>Interact with whatever is in the avatar's central focus.</summary>
    Interact,

    /// <summary>Look (camera) toward a world location.</summary>
    LookAt
}

/// <summary>
/// A perception-grounded motor command produced by the embodied decision loop.
///
/// All target coordinates are world-space; the <see cref="MotorController"/>
/// converts them into body-frame analog axes ([-1, 1]) when filling the
/// downstream <see cref="GTAngel.Interop.AvatarAction"/>.
/// </summary>
public sealed class MotorIntent
{
    /// <summary>What kind of motor action this is.</summary>
    public MotorIntentType Type { get; init; }

    /// <summary>Optional world-space target (used by MoveToward / TurnTo / LookAt).</summary>
    public float[] TargetWorld { get; init; } = new float[3];

    /// <summary>
    /// Strength in [0, 1] of the analog axis. The motor controller clamps
    /// this and applies a deadzone before dispatch.
    /// </summary>
    public float Magnitude { get; init; } = 1.0f;

    /// <summary>Hold duration in seconds (0 = single-frame impulse).</summary>
    public float HoldDuration { get; init; }

    /// <summary>Free-form provenance label for telemetry.</summary>
    public string Source { get; init; } = "EmbodiedCognition";

    /// <summary>Conventional no-op intent.</summary>
    public static MotorIntent IdleIntent { get; } = new() { Type = MotorIntentType.Idle, Source = "Idle" };
}
