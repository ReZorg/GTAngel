using System;

namespace GTAngel.Interop;

/// <summary>
/// An ML vision observation frame from the UE5 768×768 secondary viewport.
/// Used by the ESN reservoir, RL policy, and the embodied cognition pipeline.
///
/// JSON-serialised over the <see cref="UE5ProcessManager.PipeName"/> named
/// pipe; the UE5 plugin must mirror this layout (see
/// <c>docs/embodied-cognition/AvatarIPCTypes.h</c>) for the contract reference.
/// </summary>
public class AvatarObservation
{
    /// <summary>Frame timestamp (engine time)</summary>
    public double Timestamp { get; set; }

    /// <summary>768×768 RGB frame encoded as base64 PNG</summary>
    public string? FrameBase64 { get; set; }

    /// <summary>Avatar world position (X, Y, Z)</summary>
    public float[] Position { get; set; } = new float[3];

    /// <summary>Avatar rotation (Pitch, Yaw, Roll)</summary>
    public float[] Rotation { get; set; } = new float[3];

    /// <summary>Avatar velocity vector</summary>
    public float[] Velocity { get; set; } = new float[3];

    /// <summary>Current UE5 Enhanced Input state (active actions)</summary>
    public string[] ActiveInputActions { get; set; } = Array.Empty<string>();

    /// <summary>Neurochemical state from the DTE NeurochemicalSystem</summary>
    public NeurochemicalSnapshot? NeurochemicalState { get; set; }

    /// <summary>Nearby objects detected by UE5 perception system</summary>
    public PerceivedObject[] PerceivedObjects { get; set; } = Array.Empty<PerceivedObject>();

    /// <summary>KSM Cycle 6: The current arbitration mode (Human, AI, Arbitrated)</summary>
    public string PlayerMode { get; set; } = "Human";

    /// <summary>KSM Cycle 6: Arbitration score (0.0 = AI, 1.0 = Human)</summary>
    public float ArbitrationScore { get; set; } = 1.0f;
}
