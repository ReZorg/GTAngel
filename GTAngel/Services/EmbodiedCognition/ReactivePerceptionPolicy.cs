using System;
using System.Collections.Generic;
using System.Linq;
using GTAngel.Models.EmbodiedCognition;

namespace GTAngel.Services.EmbodiedCognition;

/// <summary>
/// Reactive policy demonstrating perception-limited decision making.
/// 
/// Behaviour:
///   • If something <i>visible</i> matches an "interesting" tag, approach it.
///   • Else if a sound is <i>louder than threshold</i>, turn toward it.
///   • Else if memory contains a recently-seen item with high confidence,
///     return to it (limited "explore-the-thing-I-saw" behaviour).
///   • Else wander straight ahead with low magnitude.
///
/// This policy makes no assumption about the avatar's god-view — it cannot
/// see anything outside the supplied <see cref="PerceptualField"/>, and the
/// only persistent state it has is what it has previously stored in
/// <see cref="SpatialMemory"/>.
/// </summary>
public sealed class ReactivePerceptionPolicy : IPerceptionPolicy
{
    public sealed class Config
    {
        /// <summary>Tags that draw the avatar's attention when seen.</summary>
        public string[] InterestingVisualTags { get; set; } = new[]
            { "Pickup", "POI", "Landmark", "Vehicle", "NPC", "Doorway" };

        /// <summary>Visual signal threshold to approach, in [0,1].</summary>
        public float ApproachVisualThreshold { get; set; } = 0.15f;

        /// <summary>Sound loudness threshold to orient toward, in [0,1].</summary>
        public float OrientToSoundThreshold { get; set; } = 0.25f;

        /// <summary>Memory confidence threshold to revisit a remembered location.</summary>
        public float MemoryRevisitThreshold { get; set; } = 0.4f;

        /// <summary>Default walk magnitude when wandering.</summary>
        public float WanderMagnitude { get; set; } = 0.4f;

        /// <summary>Approach magnitude when heading toward a visual target.</summary>
        public float ApproachMagnitude { get; set; } = 0.85f;

        /// <summary>Magnitude scale for orienting toward a sound source.</summary>
        public float OrientToMagnitude { get; set; } = 1.0f;
    }

    public Config Settings { get; set; } = new();

    /// <summary>Exponential moving-average reward baseline used for credit assignment.</summary>
    public float MeanReward { get; private set; }

    public MotorIntent? Decide(PerceptualField field, IReadOnlyList<SpatialMemoryEntry> memory)
    {
        if (field == null) return null;

        // 1) Approach the most salient interesting visible target.
        var visualTarget = field.Visuals
            .Where(v => v.SignalStrength >= Settings.ApproachVisualThreshold)
            .Where(v => Settings.InterestingVisualTags.Length == 0
                        || Settings.InterestingVisualTags.Any(t => string.Equals(t, v.Tag, StringComparison.OrdinalIgnoreCase)))
            .OrderByDescending(v => v.SignalStrength)
            .FirstOrDefault();

        if (visualTarget != null)
        {
            return new MotorIntent
            {
                Type = MotorIntentType.MoveToward,
                TargetWorld = (float[])visualTarget.WorldLocation.Clone(),
                Magnitude = Settings.ApproachMagnitude,
                Source = $"Reactive:Approach:{visualTarget.Tag}"
            };
        }

        // 2) Turn toward the loudest off-axis sound.
        var sound = field.Sounds
            .Where(s => s.Loudness >= Settings.OrientToSoundThreshold)
            .OrderByDescending(s => s.Loudness)
            .FirstOrDefault();

        if (sound != null)
        {
            return new MotorIntent
            {
                Type = MotorIntentType.TurnTo,
                TargetWorld = (float[])sound.WorldLocation.Clone(),
                Magnitude = MathF.Min(1f, sound.Loudness * 1.5f) * Settings.OrientToMagnitude,
                Source = $"Reactive:OrientTo:{sound.Tag}"
            };
        }

        // 3) Re-visit a remembered landmark above confidence threshold.
        if (memory != null)
        {
            var recall = memory
                .Where(e => e.Confidence >= Settings.MemoryRevisitThreshold)
                .OrderByDescending(e => e.Confidence)
                .FirstOrDefault();
            if (recall != null)
            {
                return new MotorIntent
                {
                    Type = MotorIntentType.MoveToward,
                    TargetWorld = (float[])recall.WorldLocation.Clone(),
                    Magnitude = Settings.WanderMagnitude * 1.5f,
                    Source = $"Reactive:Revisit:{recall.Tag}"
                };
            }
        }

        // 4) Wander forward.
        var self = field.Self;
        var yawRad = (self.Rotation.Length > 1 ? self.Rotation[1] : 0f) * (MathF.PI / 180f);
        var ahead = new[]
        {
            self.Position[0] + MathF.Cos(yawRad) * 200f,
            self.Position[1] + MathF.Sin(yawRad) * 200f,
            self.Position.Length > 2 ? self.Position[2] : 0f
        };
        return new MotorIntent
        {
            Type = MotorIntentType.MoveToward,
            TargetWorld = ahead,
            Magnitude = Settings.WanderMagnitude,
            Source = "Reactive:Wander"
        };
    }

    /// <summary>
    /// Adapt thresholds and magnitudes based on the scalar reward from the
    /// training loop. Positive reward relative to the running mean lowers
    /// thresholds / raises magnitudes (encourage the behavior); negative
    /// reward raises thresholds / lowers magnitudes (discourage it).
    /// </summary>
    public void UpdateReward(float reward, PerceptualField? field, MotorIntent? intent)
    {
        const float baselineAlpha = 0.1f;
        const float learningRate = 0.05f;

        MeanReward = baselineAlpha * reward + (1f - baselineAlpha) * MeanReward;
        if (intent == null || intent.Type == MotorIntentType.Idle) return;

        float delta = reward - MeanReward;
        string source = intent.Source ?? string.Empty;

        if (source.StartsWith("Reactive:Approach", StringComparison.Ordinal))
        {
            // Positive reward → lower threshold so we approach more readily,
            // and raise magnitude to move toward targets more decisively.
            Settings.ApproachVisualThreshold = Clamp(
                Settings.ApproachVisualThreshold - learningRate * delta, 0.03f, 0.95f);
            Settings.ApproachMagnitude = Clamp(
                Settings.ApproachMagnitude + learningRate * delta * 0.5f, 0.15f, 1.0f);
        }
        else if (source.StartsWith("Reactive:OrientTo", StringComparison.Ordinal))
        {
            Settings.OrientToSoundThreshold = Clamp(
                Settings.OrientToSoundThreshold - learningRate * delta, 0.03f, 0.95f);
            Settings.OrientToMagnitude = Clamp(
                Settings.OrientToMagnitude + learningRate * delta * 0.5f, 0.15f, 1.0f);
        }
        else if (source.StartsWith("Reactive:Revisit", StringComparison.Ordinal))
        {
            Settings.MemoryRevisitThreshold = Clamp(
                Settings.MemoryRevisitThreshold - learningRate * delta, 0.03f, 0.95f);
            Settings.WanderMagnitude = Clamp(
                Settings.WanderMagnitude + learningRate * delta * 0.5f, 0.1f, 1.0f);
        }
        else // Wander or unrecognized
        {
            Settings.WanderMagnitude = Clamp(
                Settings.WanderMagnitude + learningRate * delta * 0.5f, 0.1f, 1.0f);
        }
    }

    private static float Clamp(float value, float min, float max)
        => value < min ? min : (value > max ? max : value);
}
