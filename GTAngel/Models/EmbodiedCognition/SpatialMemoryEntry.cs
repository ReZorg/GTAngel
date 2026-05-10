namespace GTAngel.Models.EmbodiedCognition;

/// <summary>
/// One remembered observation of a perceived object. Confidence decays over
/// time so the avatar gradually forgets things it has not re-perceived.
///
/// Stored and updated by
/// <see cref="GTAngel.Services.EmbodiedCognition.SpatialMemory"/> and exposed
/// (read-only) to <see cref="GTAngel.Services.EmbodiedCognition.IPerceptionPolicy"/>
/// implementations as part of the embodied decision loop's "limited world
/// knowledge" contract.
/// </summary>
public sealed class SpatialMemoryEntry
{
    /// <summary>Stable identifier (forwarded from the original VisualPercept tag).</summary>
    public string Tag { get; init; } = string.Empty;

    /// <summary>Last known world position.</summary>
    public float[] WorldLocation { get; set; } = new float[3];

    /// <summary>Engine-time when this entry was last seen (seconds).</summary>
    public double LastSeen { get; set; }

    /// <summary>Confidence in [0, 1]. 1 = just-perceived, 0 = forgotten.</summary>
    public float Confidence { get; set; }

    /// <summary>How many times this object has been perceived in total.</summary>
    public int Hits { get; set; }
}
