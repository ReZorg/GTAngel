namespace GTAngel.Interop;

/// <summary>
/// A virtual player control action sent to the UE5 Enhanced Input System.
/// Replaces UE4's legacy HandleCustomTouchEvent / keyboard injection.
///
/// JSON-serialised over the <see cref="UE5ProcessManager.PipeName"/> named
/// pipe; the UE5 plugin must mirror this layout (see
/// <c>docs/embodied-cognition/AvatarIPCTypes.h</c>) for the contract reference.
/// </summary>
public class AvatarAction
{
    /// <summary>UE5 Enhanced Input Action name (e.g. "IA_Move", "IA_Look", "IA_Jump")</summary>
    public string InputAction { get; set; } = string.Empty;

    /// <summary>Axis magnitude [-1.0 .. 1.0] for analog inputs, 1.0 for digital</summary>
    public float Magnitude { get; set; } = 1.0f;

    /// <summary>X axis for 2D inputs (movement, look)</summary>
    public float AxisX { get; set; }

    /// <summary>Y axis for 2D inputs</summary>
    public float AxisY { get; set; }

    /// <summary>Duration to hold the action (0 = single frame)</summary>
    public float HoldDuration { get; set; }

    /// <summary>Source: "ML" (from ESN/RL policy), "Human" (passthrough), "Script" (waypoint)</summary>
    public string Source { get; set; } = "ML";
}
