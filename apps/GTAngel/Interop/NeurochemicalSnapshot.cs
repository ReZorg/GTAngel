namespace GTAngel.Interop;

/// <summary>
/// Snapshot of the DTE NeurochemicalSystem state, transmitted from UE5 each
/// observation tick. The embodied cognition pipeline reads
/// <see cref="Homeostasis"/>, <see cref="ChaosIntensity"/>, and
/// <see cref="Curiosity"/> to populate
/// <see cref="GTAngel.Models.EmbodiedCognition.EmbodiedSelfState"/>; the
/// remaining fields are consumed by adjacent subsystems.
///
/// JSON-serialised as a nested object inside <see cref="AvatarObservation"/>;
/// the UE5 plugin must mirror this layout (see
/// <c>docs/embodied-cognition/AvatarIPCTypes.h</c>) for the contract reference.
/// </summary>
public class NeurochemicalSnapshot
{
    public float Curiosity      { get; set; }
    public float Endorphin      { get; set; }
    public float ChaosIntensity { get; set; }
    public float Homeostasis    { get; set; }
    public float Abundance      { get; set; }
    public float Scarcity       { get; set; }
}
