namespace GTAngel.Interop;

/// <summary>
/// An object perceived by the UE5 AI Perception system, as reported via
/// <see cref="AvatarObservation.PerceivedObjects"/>.
///
/// This is the *only* channel through which the embodied cognition pipeline
/// learns about world objects. <see cref="Tag"/> is used as a stable identity
/// key by <see cref="GTAngel.Services.EmbodiedCognition.SpatialMemory"/> and
/// must therefore be stable across ticks for moving objects (recommended:
/// <c>"NPC:&lt;id&gt;"</c>, <c>"Vehicle:&lt;id&gt;"</c>).
/// <see cref="IsVisible"/> must reflect the engine's per-tick line-of-sight
/// check; the embodied perception layer trusts it as the occlusion oracle.
///
/// JSON-serialised inside <see cref="AvatarObservation"/>; the UE5 plugin
/// must mirror this layout (see
/// <c>docs/embodied-cognition/AvatarIPCTypes.h</c>) for the contract reference.
/// </summary>
public class PerceivedObject
{
    /// <summary>Stable identity tag (must be stable across ticks for moving objects).</summary>
    public string Tag      { get; set; } = string.Empty;

    /// <summary>
    /// Engine-reported distance from the avatar, generally 3D Euclidean in
    /// unreal units. The embodied perception layer uses this as the sight /
    /// hearing range gate when &gt; 0; otherwise it computes a planar distance
    /// itself. Elevation math always uses the planar distance internally.
    /// </summary>
    public float Distance  { get; set; }

    /// <summary>World-space location (X, Y, Z) in unreal units.</summary>
    public float[] Location { get; set; } = new float[3];

    /// <summary>
    /// True iff the engine's line-of-sight test passed for this object on
    /// this tick. Must NOT be set true unconditionally — the embodied
    /// perception layer relies on this flag to enforce visual occlusion.
    /// Hearing intentionally ignores this flag (sounds pass through walls).
    /// </summary>
    public bool IsVisible  { get; set; }
}
