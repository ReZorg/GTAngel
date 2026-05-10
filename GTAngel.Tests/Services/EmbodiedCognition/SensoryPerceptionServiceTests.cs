using System;
using System.Linq;
using GTAngel.Interop;
using GTAngel.Models.EmbodiedCognition;
using GTAngel.Services.EmbodiedCognition;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services.EmbodiedCognition;

/// <summary>
/// Tests for <see cref="SensoryPerceptionService"/>.
///
/// Verifies that:
///   1. Visual percepts are gated by FOV cone, sight range, and engine visibility.
///   2. Auditory percepts are gated by hearing range and not by FOV / visibility.
///   3. Signal strength / loudness fall off with distance and angle as designed.
///   4. Proprioception is derived correctly (planar speed, rotation, neuro-derived state).
///   5. Bearing math handles wraparound at ±180°.
///   6. Attentional bottleneck (MaxVisualPercepts) keeps the strongest percepts.
/// </summary>
public sealed class SensoryPerceptionServiceTests
{
    private static AvatarObservation MakeObs(
        float[]? selfPos = null, float[]? selfRot = null, float[]? selfVel = null,
        params PerceivedObject[] objects)
    {
        return new AvatarObservation
        {
            Timestamp = 1.0,
            Position = selfPos ?? new float[] { 0, 0, 0 },
            Rotation = selfRot ?? new float[] { 0, 0, 0 },     // facing +X by default
            Velocity = selfVel ?? new float[] { 0, 0, 0 },
            PerceivedObjects = objects ?? Array.Empty<PerceivedObject>()
        };
    }

    private static PerceivedObject Obj(string tag, float x, float y, float z = 0,
                                       bool visible = true, float? distance = null)
    {
        return new PerceivedObject
        {
            Tag = tag,
            Location = new[] { x, y, z },
            IsVisible = visible,
            Distance = distance ?? MathF.Sqrt(x * x + y * y)
        };
    }

    private static SensoryPerceptionService MakeSvc(PerceptionConfig? cfg = null)
        => new(NullLogger<SensoryPerceptionService>.Instance, cfg);

    // ── 1. Visual gate: FOV cone ──────────────────────────────────────────

    [Fact]
    public void Sight_ExcludesObjects_OutsideFovCone()
    {
        var svc = MakeSvc(new PerceptionConfig { FieldOfViewDeg = 90f, SightRangeUu = 5000f });
        // Avatar at origin, yaw=0 → forward = +X.
        // Object directly behind (-X) should be invisible.
        var obs = MakeObs(
            objects: new[]
            {
                Obj("Behind", -500f, 0f),
                Obj("Ahead", 500f, 0f)
            });

        var field = svc.Perceive(obs);

        Assert.DoesNotContain(field.Visuals, v => v.Tag == "Behind");
        Assert.Contains(field.Visuals, v => v.Tag == "Ahead");
    }

    [Fact]
    public void Sight_IncludesObjects_AtConeEdgeButNotBeyond()
    {
        var svc = MakeSvc(new PerceptionConfig { FieldOfViewDeg = 90f, SightRangeUu = 5000f });
        // half-FOV = 45°. Object at 30° to the right of forward → inside.
        // Object at 60° to the right of forward → outside.
        var inside = Obj("InsideCone", x: MathF.Cos(MathF.PI / 6f) * 500f,
                                       y: MathF.Sin(MathF.PI / 6f) * 500f);
        var outside = Obj("OutsideCone", x: MathF.Cos(MathF.PI / 3f) * 500f,
                                         y: MathF.Sin(MathF.PI / 3f) * 500f);
        var obs = MakeObs(objects: new[] { inside, outside });

        var field = svc.Perceive(obs);

        Assert.Contains(field.Visuals, v => v.Tag == "InsideCone");
        Assert.DoesNotContain(field.Visuals, v => v.Tag == "OutsideCone");
    }

    // ── 2. Visual gate: sight range ───────────────────────────────────────

    [Fact]
    public void Sight_ExcludesObjects_BeyondSightRange()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            FieldOfViewDeg = 180f,           // full half-circle so FOV is not the gate
            SightRangeUu = 1000f,
            SightFullStrengthRangeUu = 200f,
            RequireVisibility = false
        });
        var obs = MakeObs(
            objects: new[]
            {
                Obj("Near", 500f, 0f),
                Obj("Far",  1500f, 0f)
            });

        var field = svc.Perceive(obs);

        Assert.Contains(field.Visuals, v => v.Tag == "Near");
        Assert.DoesNotContain(field.Visuals, v => v.Tag == "Far");
    }

    // ── 3. Visual gate: engine visibility (occlusion) ─────────────────────

    [Fact]
    public void Sight_RespectsEngineOcclusion_WhenRequireVisibilityIsTrue()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            FieldOfViewDeg = 180f,
            RequireVisibility = true
        });
        var obs = MakeObs(
            objects: new[]
            {
                Obj("Visible",   500f, 0f, visible: true),
                Obj("Occluded",  500f, 0f, visible: false)
            });

        var field = svc.Perceive(obs);

        Assert.Contains(field.Visuals, v => v.Tag == "Visible");
        Assert.DoesNotContain(field.Visuals, v => v.Tag == "Occluded");
    }

    [Fact]
    public void Sight_AllowsOccludedObjects_WhenRequireVisibilityIsFalse()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            FieldOfViewDeg = 180f,
            RequireVisibility = false
        });
        var obs = MakeObs(
            objects: new[] { Obj("Occluded", 500f, 0f, visible: false) });

        var field = svc.Perceive(obs);

        Assert.Contains(field.Visuals, v => v.Tag == "Occluded");
    }

    // ── 4. Signal strength falloff ────────────────────────────────────────

    [Fact]
    public void Sight_SignalStrength_FallsOffWithDistance()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            FieldOfViewDeg = 180f,
            SightFullStrengthRangeUu = 100f,
            SightRangeUu = 1000f,
            RequireVisibility = false
        });
        var obs = MakeObs(
            objects: new[]
            {
                Obj("Close", 50f, 0f),
                Obj("Mid",   500f, 0f),
                Obj("Far",   900f, 0f)
            });

        var field = svc.Perceive(obs);
        var close = field.Visuals.Single(v => v.Tag == "Close");
        var mid   = field.Visuals.Single(v => v.Tag == "Mid");
        var far   = field.Visuals.Single(v => v.Tag == "Far");

        Assert.Equal(1f, close.SignalStrength, 3);
        Assert.True(mid.SignalStrength > far.SignalStrength);
        Assert.True(close.SignalStrength > mid.SignalStrength);
    }

    /// <summary>
    /// Regression: <see cref="VisualPercept.RelativeElevationDeg"/> must be derived
    /// from the planar (XY) distance, even when the engine's <c>PerceivedObject.Distance</c>
    /// field is the full 3D Euclidean distance. Previously the 3D distance was used
    /// as the denominator of <c>atan2(dz, dist)</c>, underestimating elevation angles
    /// for objects with significant vertical offset.
    /// </summary>
    [Fact]
    public void Sight_Elevation_UsesPlanarDistance_NotEngineReported3DDistance()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            FieldOfViewDeg = 180f,
            RequireVisibility = false,
            SightRangeUu = 5000f
        });
        // Object at (100, 0, 100): planar distance = 100, height = 100 → 45° elevation.
        // Engine reports 3D distance = sqrt(100²+100²) ≈ 141.42 (which is wrong for elevation).
        var obj = new PerceivedObject
        {
            Tag = "HighSign",
            Location = new[] { 100f, 0f, 100f },
            IsVisible = true,
            Distance = MathF.Sqrt(100f * 100f + 100f * 100f)
        };
        var obs = MakeObs(objects: new[] { obj });

        var field = svc.Perceive(obs);
        var p = field.Visuals.Single(v => v.Tag == "HighSign");

        // Correct elevation atan2(100, 100) = 45°. Tolerate a tiny floating-point fuzz.
        Assert.InRange(p.RelativeElevationDeg, 44.5f, 45.5f);
    }

    [Fact]
    public void Sight_SignalStrength_FallsOffWithAngularOffset()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            FieldOfViewDeg = 120f,           // half-FOV = 60°
            SightFullStrengthRangeUu = 1000f,
            SightRangeUu = 2000f,
            RequireVisibility = false
        });
        // Same distance, different angles.
        var center = Obj("Center", 500f, 0f);
        var off    = Obj("Off",    MathF.Cos(MathF.PI / 4f) * 500f,
                                   MathF.Sin(MathF.PI / 4f) * 500f);  // 45° off
        var obs = MakeObs(objects: new[] { center, off });

        var field = svc.Perceive(obs);
        var c = field.Visuals.Single(v => v.Tag == "Center");
        var o = field.Visuals.Single(v => v.Tag == "Off");

        Assert.True(c.SignalStrength > o.SignalStrength);
    }

    // ── 5. Auditory gate ──────────────────────────────────────────────────

    [Fact]
    public void Hearing_IgnoresFov_AndAdmitsSoundsBehindAvatar()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            FieldOfViewDeg = 90f,           // narrow visual cone
            HearingRangeUu = 4000f,
            FullLoudnessRangeUu = 100f
        });
        var obs = MakeObs(
            objects: new[] { Obj("Behind", -1000f, 0f) });

        var field = svc.Perceive(obs);

        Assert.Empty(field.Visuals);                // not seen
        Assert.Contains(field.Sounds, s => s.Tag == "Behind"); // but heard
    }

    [Fact]
    public void Hearing_ExcludesSilentTaggedObjects()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            HearingRangeUu = 4000f,
            FullLoudnessRangeUu = 100f,
            SilentTags = new[] { "Scenery", "Prop" }
        });
        var obs = MakeObs(
            objects: new[]
            {
                Obj("Scenery", 200f, 0f),
                Obj("NPC",     200f, 0f)
            });

        var field = svc.Perceive(obs);

        Assert.DoesNotContain(field.Sounds, s => s.Tag == "Scenery");
        Assert.Contains(field.Sounds, s => s.Tag == "NPC");
    }

    [Fact]
    public void Hearing_LoudnessFallsOffWithDistance()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            HearingRangeUu = 4000f,
            FullLoudnessRangeUu = 100f
        });
        var obs = MakeObs(
            objects: new[]
            {
                Obj("Near", 100f, 0f),
                Obj("Far",  2000f, 0f)
            });

        var field = svc.Perceive(obs);
        var near = field.Sounds.Single(s => s.Tag == "Near");
        var far  = field.Sounds.Single(s => s.Tag == "Far");

        Assert.True(near.Loudness >= far.Loudness);
        Assert.Equal(1f, near.Loudness, 2);
    }

    // ── 6. Proprioception ─────────────────────────────────────────────────

    [Fact]
    public void Proprioception_DerivesPlanarSpeed_AndPassesThroughBodyState()
    {
        var svc = MakeSvc();
        var obs = MakeObs(selfVel: new float[] { 30f, 40f, 5f });

        var field = svc.Perceive(obs);

        // Planar speed should be sqrt(30² + 40²) = 50.
        Assert.Equal(50f, field.Self.Speed, 1);
        Assert.Equal(30f, field.Self.Velocity[0]);
        Assert.Equal(40f, field.Self.Velocity[1]);
    }

    [Fact]
    public void Proprioception_DerivesFatigueAndArousal_FromNeuroState()
    {
        var svc = MakeSvc();
        var obs = MakeObs();
        obs.NeurochemicalState = new NeurochemicalSnapshot
        {
            Curiosity = 0.8f, Endorphin = 0.5f, ChaosIntensity = 0.6f, Homeostasis = 0.4f
        };

        var field = svc.Perceive(obs);

        // Fatigue = 1 - homeostasis = 0.6
        Assert.InRange(field.Self.Fatigue, 0.59f, 0.61f);
        // Arousal = 0.5*curiosity + 0.5*chaos = 0.7
        Assert.InRange(field.Self.Arousal, 0.69f, 0.71f);
    }

    // ── 7. Bearing math ───────────────────────────────────────────────────

    [Theory]
    [InlineData(0f,    1f, 0f,   0f)]    // ahead
    [InlineData(0f,    0f, 1f,   90f)]   // right
    [InlineData(0f,    0f, -1f, -90f)]   // left
    [InlineData(90f,   1f, 0f,  -90f)]   // facing +Y, target +X is on the left
    public void SignedYawDelta_ProducesExpectedBearings(
        float yawDeg, float tx, float ty, float expected)
    {
        var got = SensoryPerceptionService.SignedYawDelta(
            new[] { 0f, 0f, 0f }, new[] { tx, ty, 0f }, yawDeg);
        Assert.InRange(got, expected - 1f, expected + 1f);
    }

    [Fact]
    public void SignedYawDelta_WrapsAtPlusMinus180()
    {
        // Avatar facing +X, target directly behind. Should wrap to ±180.
        var d = SensoryPerceptionService.SignedYawDelta(
            new[] { 0f, 0f, 0f }, new[] { -500f, 0f, 0f }, 0f);
        Assert.True(MathF.Abs(d) >= 179f && MathF.Abs(d) <= 180.0001f);
    }

    // ── 8. Attentional bottleneck ─────────────────────────────────────────

    [Fact]
    public void AttentionalBottleneck_KeepsStrongestVisualPercepts()
    {
        var svc = MakeSvc(new PerceptionConfig
        {
            FieldOfViewDeg = 180f,
            SightRangeUu = 5000f,
            SightFullStrengthRangeUu = 100f,
            MaxVisualPercepts = 2,
            RequireVisibility = false
        });
        // Three percepts at increasing distance — only the two nearest should survive.
        var obs = MakeObs(
            objects: new[]
            {
                Obj("A", 100f, 0f),
                Obj("B", 500f, 0f),
                Obj("C", 1000f, 0f)
            });

        var field = svc.Perceive(obs);

        Assert.Equal(2, field.Visuals.Length);
        Assert.Contains(field.Visuals, v => v.Tag == "A");
        Assert.Contains(field.Visuals, v => v.Tag == "B");
        Assert.DoesNotContain(field.Visuals, v => v.Tag == "C");
        // RawCandidateCount should still report the full pre-filter total.
        Assert.Equal(3, field.RawCandidateCount);
    }

    // ── 9. Event contract ─────────────────────────────────────────────────

    [Fact]
    public void Perceive_FiresPerceptionUpdatedEvent()
    {
        var svc = MakeSvc();
        PerceptualField? captured = null;
        svc.PerceptionUpdated += (_, f) => captured = f;

        var field = svc.Perceive(MakeObs());

        Assert.NotNull(captured);
        Assert.Same(field, captured);
        Assert.Same(field, svc.LastField);
    }

    // ── 10. Defensive null/empty handling ─────────────────────────────────

    [Fact]
    public void Perceive_ThrowsOnNullObservation()
    {
        var svc = MakeSvc();
        Assert.Throws<ArgumentNullException>(() => svc.Perceive(null!));
    }

    [Fact]
    public void Perceive_HandlesEmptyPerceivedObjectsGracefully()
    {
        var svc = MakeSvc();
        var field = svc.Perceive(new AvatarObservation
        {
            Position = new float[] { 0, 0, 0 },
            Rotation = new float[] { 0, 0, 0 },
            Velocity = new float[] { 0, 0, 0 },
            PerceivedObjects = Array.Empty<PerceivedObject>()
        });

        Assert.Empty(field.Visuals);
        Assert.Empty(field.Sounds);
        Assert.Equal(0, field.RawCandidateCount);
    }
}
