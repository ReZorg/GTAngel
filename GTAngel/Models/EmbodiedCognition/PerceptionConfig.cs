namespace GTAngel.Models.EmbodiedCognition;

/// <summary>
/// Tunable parameters controlling how raw engine observations are filtered
/// down to the avatar's allowed perception. Sensible defaults match a
/// human-like first-person GTA-style character.
/// </summary>
public sealed class PerceptionConfig
{
    /// <summary>
    /// Total horizontal field of view in degrees (the cone is symmetric around
    /// the avatar's forward direction). 110° is a reasonable upper bound for
    /// a human's binocular visual field.
    /// </summary>
    public float FieldOfViewDeg { get; set; } = 110f;

    /// <summary>Maximum sight distance in unreal units (≈ centimetres).</summary>
    public float SightRangeUu { get; set; } = 2500f;

    /// <summary>
    /// Distance below which sight signal strength saturates at ~1. Beyond this,
    /// signal strength falls off linearly until it reaches 0 at <see cref="SightRangeUu"/>.
    /// </summary>
    public float SightFullStrengthRangeUu { get; set; } = 300f;

    /// <summary>
    /// If true, only objects with <c>IsVisible == true</c> on the underlying
    /// <see cref="GTAngel.Interop.PerceivedObject"/> are admitted to the visual field.
    /// This honours the engine's line-of-sight check.
    /// </summary>
    public bool RequireVisibility { get; set; } = true;

    /// <summary>Maximum hearing distance in unreal units.</summary>
    public float HearingRangeUu { get; set; } = 4000f;

    /// <summary>
    /// Distance below which sound is at full loudness. Beyond this, loudness
    /// falls off as 1 / (d / FullLoudnessRangeUu)² up to <see cref="HearingRangeUu"/>.
    /// </summary>
    public float FullLoudnessRangeUu { get; set; } = 200f;

    /// <summary>
    /// Tags that should be treated as silent — visual-only objects that do
    /// not contribute to the auditory percept set (e.g. static scenery).
    /// </summary>
    public string[] SilentTags { get; set; } = new[] { "Scenery", "Prop", "Landmark" };

    /// <summary>
    /// Maximum number of visuals/sounds to keep per tick. The strongest
    /// signals win; surplus is dropped. Mirrors human attentional bottlenecks.
    /// </summary>
    public int MaxVisualPercepts { get; set; } = 16;

    /// <summary>Maximum auditory percepts retained per tick.</summary>
    public int MaxAuditoryPercepts { get; set; } = 8;
}
