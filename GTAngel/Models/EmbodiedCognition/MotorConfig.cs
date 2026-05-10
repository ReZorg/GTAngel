namespace GTAngel.Models.EmbodiedCognition;

/// <summary>
/// Tunables for translating motor intents into UE5 Enhanced Input commands.
/// Models the avatar's body limitations (deadzones, max axis, crouch lockouts).
/// </summary>
public sealed class MotorConfig
{
    /// <summary>Analog axes below this magnitude are zeroed out.</summary>
    public float DeadzoneMagnitude { get; set; } = 0.05f;

    /// <summary>Magnitude is hard-clamped into [-MaxAxis, +MaxAxis].</summary>
    public float MaxAxis { get; set; } = 1.0f;

    /// <summary>Default hold duration for analog moves (seconds).</summary>
    public float DefaultHoldSeconds { get; set; } = 0.25f;

    /// <summary>Yaw error (degrees) at which TurnTo / LookAt outputs full magnitude.</summary>
    public float TurnSaturationDeg { get; set; } = 90f;

    /// <summary>
    /// If sprinting is requested while crouched, the controller will down-grade
    /// the sprint to a normal move. Set to false to allow the engine to filter.
    /// </summary>
    public bool SuppressSprintWhileCrouched { get; set; } = true;
}
