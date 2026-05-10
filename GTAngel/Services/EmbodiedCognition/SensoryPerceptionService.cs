using System;
using System.Collections.Generic;
using System.Linq;
using GTAngel.Interop;
using GTAngel.Models.EmbodiedCognition;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services.EmbodiedCognition;

/// <summary>
/// Filters raw <see cref="AvatarObservation"/> snapshots from UE5 down to
/// the avatar's allowed perceptual field — only what the body could plausibly
/// sense from its current position and orientation.
///
/// This is the gatekeeper of "limited world knowledge": the cognitive layer
/// is expected to consult only the <see cref="PerceptualField"/> we emit, not
/// the raw observation, so the AI cannot peek at things it has no sensor for.
///
/// Sight: cone (FoV) × distance falloff × engine line-of-sight (occlusion).
/// Hearing: omnidirectional radius × inverse-square loudness, ignoring FoV
///          and (by default) ignoring occlusion.
/// Proprioception: position / orientation / velocity / fatigue / arousal,
///                 derived from the same snapshot.
/// </summary>
public sealed class SensoryPerceptionService
{
    private readonly ILogger<SensoryPerceptionService>? _logger;

    /// <summary>Active configuration. Mutating this field re-tunes the next perception tick.</summary>
    public PerceptionConfig Config { get; set; } = new();

    /// <summary>The most recent perceptual field, or <c>null</c> before the first observation.</summary>
    public PerceptualField? LastField { get; private set; }

    /// <summary>
    /// Raised whenever a new observation has been filtered into a perceptual field.
    /// The provided field is the same instance referenced by <see cref="LastField"/>.
    /// </summary>
    public event EventHandler<PerceptualField>? PerceptionUpdated;

    public SensoryPerceptionService(ILogger<SensoryPerceptionService>? logger = null,
                                    PerceptionConfig? config = null)
    {
        _logger = logger;
        if (config != null) Config = config;
    }

    /// <summary>
    /// Filter a raw engine observation into a perceptual field. Pure, deterministic,
    /// safe to call from any thread.
    /// </summary>
    public PerceptualField Perceive(AvatarObservation obs)
    {
        if (obs == null) throw new ArgumentNullException(nameof(obs));

        var self = BuildProprioception(obs);
        var (visuals, sounds) = FilterExternalPercepts(obs, self);

        var field = new PerceptualField
        {
            Timestamp = obs.Timestamp,
            Self = self,
            Visuals = visuals,
            Sounds = sounds,
            RawCandidateCount = obs.PerceivedObjects?.Length ?? 0
        };

        LastField = field;
        PerceptionUpdated?.Invoke(this, field);
        _logger?.LogTrace("Perception: {Vis} visual, {Aud} audio of {Raw} raw at t={T:F2}",
            visuals.Length, sounds.Length, field.RawCandidateCount, obs.Timestamp);
        return field;
    }

    // ── Helpers ────────────────────────────────────────────────────────────

    private static EmbodiedSelfState BuildProprioception(AvatarObservation obs)
    {
        var pos = obs.Position ?? new float[3];
        var rot = obs.Rotation ?? new float[3];
        var vel = obs.Velocity ?? new float[3];

        // Pad short arrays defensively.
        pos = pos.Length >= 3 ? new[] { pos[0], pos[1], pos[2] } : new float[] { Get(pos, 0), Get(pos, 1), Get(pos, 2) };
        rot = rot.Length >= 3 ? new[] { rot[0], rot[1], rot[2] } : new float[] { Get(rot, 0), Get(rot, 1), Get(rot, 2) };
        vel = vel.Length >= 3 ? new[] { vel[0], vel[1], vel[2] } : new float[] { Get(vel, 0), Get(vel, 1), Get(vel, 2) };

        var planarSpeed = MathF.Sqrt(vel[0] * vel[0] + vel[1] * vel[1]);

        var neuro = obs.NeurochemicalState;
        return new EmbodiedSelfState
        {
            Position = pos,
            Rotation = rot,
            Velocity = vel,
            Speed = planarSpeed,
            Fatigue = neuro != null ? Clamp01(1f - neuro.Homeostasis) : 0f,
            Arousal = neuro != null ? Clamp01(neuro.ChaosIntensity * 0.5f + neuro.Curiosity * 0.5f) : 0f
        };
    }

    private (VisualPercept[] visuals, AuditoryPercept[] sounds) FilterExternalPercepts(
        AvatarObservation obs, EmbodiedSelfState self)
    {
        var raw = obs.PerceivedObjects;
        if (raw == null || raw.Length == 0)
            return (Array.Empty<VisualPercept>(), Array.Empty<AuditoryPercept>());

        var cfg = Config;
        var halfFov = cfg.FieldOfViewDeg * 0.5f;
        var yawDeg = self.Rotation.Length > 1 ? self.Rotation[1] : 0f;
        var silent = new HashSet<string>(cfg.SilentTags ?? Array.Empty<string>(),
                                          StringComparer.OrdinalIgnoreCase);

        var visuals = new List<VisualPercept>(raw.Length);
        var sounds  = new List<AuditoryPercept>(raw.Length);

        foreach (var obj in raw)
        {
            if (obj == null) continue;
            var loc = obj.Location ?? new float[3];
            if (loc.Length < 2) continue;

            // Engine reports distance directly when available; otherwise compute.
            // We track the planar (XY) distance separately because elevation math
            // requires it: ElevationDeg = atan2(dz, planarDist), and the engine's
            // Distance field is generally the full 3D Euclidean magnitude.
            float planarDist = Distance2D(self.Position, loc);
            float dist = obj.Distance > 0f ? obj.Distance : planarDist;
            if (dist <= 0.0001f) dist = 0.0001f;
            if (planarDist <= 0.0001f) planarDist = 0.0001f;

            float bearingDeg = SignedYawDelta(self.Position, loc, yawDeg);

            // ── Visual gate ────────────────────────────────────────────────
            bool inFov  = MathF.Abs(bearingDeg) <= halfFov;
            bool inRange = dist <= cfg.SightRangeUu;
            bool visible = !cfg.RequireVisibility || obj.IsVisible;
            if (inFov && inRange && visible)
            {
                float distScore  = DistanceScore(dist, cfg.SightFullStrengthRangeUu, cfg.SightRangeUu);
                float angleScore = AngleScore(bearingDeg, halfFov);
                float strength   = Clamp01(distScore * angleScore);

                visuals.Add(new VisualPercept
                {
                    Tag = obj.Tag ?? string.Empty,
                    WorldLocation = new[] { Get(loc, 0), Get(loc, 1), Get(loc, 2) },
                    Distance = dist,
                    RelativeBearingDeg = bearingDeg,
                    RelativeElevationDeg = ElevationDeg(self.Position, loc, planarDist),
                    SignalStrength = strength
                });
            }

            // ── Auditory gate ──────────────────────────────────────────────
            if (silent.Contains(obj.Tag ?? string.Empty)) continue;
            if (dist > cfg.HearingRangeUu) continue;

            float loudness = LoudnessScore(dist, cfg.FullLoudnessRangeUu, cfg.HearingRangeUu);
            if (loudness <= 0f) continue;

            sounds.Add(new AuditoryPercept
            {
                Tag = obj.Tag ?? string.Empty,
                WorldLocation = new[] { Get(loc, 0), Get(loc, 1), Get(loc, 2) },
                Distance = dist,
                RelativeBearingDeg = bearingDeg,
                Loudness = loudness
            });
        }

        // Apply attentional bottlenecks: keep the strongest signals.
        var topVisuals = visuals
            .OrderByDescending(v => v.SignalStrength)
            .ThenBy(v => v.Distance)
            .Take(Math.Max(0, cfg.MaxVisualPercepts))
            .ToArray();

        var topSounds = sounds
            .OrderByDescending(s => s.Loudness)
            .ThenBy(s => s.Distance)
            .Take(Math.Max(0, cfg.MaxAuditoryPercepts))
            .ToArray();

        return (topVisuals, topSounds);
    }

    // ── Pure math helpers ─────────────────────────────────────────────────

    private static float Get(float[] a, int i) => (a != null && i < a.Length) ? a[i] : 0f;

    private static float Distance2D(float[] a, float[] b)
    {
        float dx = Get(b, 0) - Get(a, 0);
        float dy = Get(b, 1) - Get(a, 1);
        return MathF.Sqrt(dx * dx + dy * dy);
    }

    /// <summary>
    /// Signed yaw delta from the avatar's forward heading to a world target,
    /// in degrees, normalised to (-180, 180]. Positive = right of forward.
    /// </summary>
    public static float SignedYawDelta(float[] selfPos, float[] target, float yawDeg)
    {
        float dx = Get(target, 0) - Get(selfPos, 0);
        float dy = Get(target, 1) - Get(selfPos, 1);
        if (MathF.Abs(dx) < 1e-5f && MathF.Abs(dy) < 1e-5f) return 0f;

        float bearingWorldDeg = MathF.Atan2(dy, dx) * (180f / MathF.PI);
        float delta = bearingWorldDeg - yawDeg;
        // Wrap to (-180, 180]
        while (delta > 180f)  delta -= 360f;
        while (delta <= -180f) delta += 360f;
        return delta;
    }

    private static float ElevationDeg(float[] selfPos, float[] target, float planarDist)
    {
        if (planarDist <= 0.0001f) return 0f;
        float dz = Get(target, 2) - Get(selfPos, 2);
        return MathF.Atan2(dz, planarDist) * (180f / MathF.PI);
    }

    /// <summary>
    /// 1 inside the full-strength radius, falling linearly to 0 at maxRange.
    /// Out-of-range returns 0.
    /// </summary>
    public static float DistanceScore(float distance, float fullStrengthRange, float maxRange)
    {
        if (distance <= fullStrengthRange) return 1f;
        if (distance >= maxRange) return 0f;
        if (maxRange <= fullStrengthRange) return 0f;
        return 1f - (distance - fullStrengthRange) / (maxRange - fullStrengthRange);
    }

    /// <summary>
    /// 1 directly ahead, falling to 0 at the cone edge. cos-shaped so very
    /// off-axis percepts get a gentle penalty rather than a cliff.
    /// </summary>
    public static float AngleScore(float bearingDeg, float halfFovDeg)
    {
        if (halfFovDeg <= 0f) return 0f;
        float t = Clamp01(MathF.Abs(bearingDeg) / halfFovDeg);
        // cosine falloff: cos(0)=1, cos(π/2)=0
        return MathF.Cos(t * (MathF.PI * 0.5f));
    }

    /// <summary>
    /// 1 inside the full-loudness radius, then 1/(d/full)² out to maxRange,
    /// clamped to [0,1]. Out-of-range returns 0.
    /// </summary>
    public static float LoudnessScore(float distance, float fullRange, float maxRange)
    {
        if (distance <= 0f) return 1f;
        if (distance >= maxRange) return 0f;
        if (distance <= fullRange) return 1f;
        if (fullRange <= 0f) return 0f;
        float r = distance / fullRange;
        float v = 1f / (r * r);
        return Clamp01(v);
    }

    private static float Clamp01(float v) => v < 0f ? 0f : (v > 1f ? 1f : v);
}
