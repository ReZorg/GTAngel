using System;
using System.Collections.Generic;
using System.Linq;
using GTAngel.Interop;
using GTAngel.Models.EmbodiedCognition;
using GTAngel.Services.EmbodiedCognition;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services.EmbodiedCognition;

/// <summary>
/// Integration tests for the full embodied decision loop:
///   AvatarObservation → SensoryPerceptionService → SpatialMemory →
///   IPerceptionPolicy → MotorController → AvatarAction.
///
/// The headline contract these tests enforce is "limited world knowledge":
/// the policy may decide using ONLY the perceptual field + memory it has
/// accumulated through perception. Objects outside the avatar's sensors must
/// not influence its action.
/// </summary>
public sealed class EmbodiedDecisionLoopTests
{
    /// <summary>A stub policy that records exactly what it received and emits a fixed intent.</summary>
    private sealed class RecordingPolicy : IPerceptionPolicy
    {
        public List<PerceptualField> SeenFields { get; } = new();
        public List<IReadOnlyList<SpatialMemoryEntry>> SeenMemories { get; } = new();
        public List<(float Reward, PerceptualField? Field, MotorIntent? Intent)> RewardCalls { get; } = new();
        public Func<PerceptualField, IReadOnlyList<SpatialMemoryEntry>, MotorIntent?> Behaviour { get; set; }
            = (_, _) => MotorIntent.IdleIntent;

        public MotorIntent? Decide(PerceptualField field, IReadOnlyList<SpatialMemoryEntry> memory)
        {
            SeenFields.Add(field);
            SeenMemories.Add(memory);
            return Behaviour(field, memory);
        }

        public void UpdateReward(float reward, PerceptualField? field, MotorIntent? intent)
            => RewardCalls.Add((reward, field, intent));
    }

    private static AvatarObservation MakeObs(double t, params PerceivedObject[] objects)
        => new()
        {
            Timestamp = t,
            Position = new float[] { 0, 0, 0 },
            Rotation = new float[] { 0, 0, 0 },
            Velocity = new float[] { 0, 0, 0 },
            PerceivedObjects = objects ?? Array.Empty<PerceivedObject>()
        };

    private static PerceivedObject Obj(string tag, float x, float y, bool visible = true)
        => new()
        {
            Tag = tag,
            Location = new[] { x, y, 0f },
            IsVisible = visible,
            Distance = MathF.Sqrt(x * x + y * y)
        };

    private static EmbodiedDecisionLoop MakeLoop(IPerceptionPolicy? policy = null,
                                                  PerceptionConfig? cfg = null)
    {
        var perception = new SensoryPerceptionService(NullLogger<SensoryPerceptionService>.Instance, cfg);
        var memory = new SpatialMemory();
        var motor = new MotorController();
        return new EmbodiedDecisionLoop(perception, memory, motor, policy ?? new RecordingPolicy(),
            NullLogger<EmbodiedDecisionLoop>.Instance);
    }

    [Fact]
    public void Step_FiltersOutBehindAvatarObjects_BeforeReachingPolicy()
    {
        var policy = new RecordingPolicy();
        var loop = MakeLoop(policy, new PerceptionConfig
        {
            FieldOfViewDeg = 90f,
            SightRangeUu = 5000f,
            HearingRangeUu = 100f      // tight hearing → behind-object also won't be heard
        });

        loop.Step(MakeObs(1.0,
            Obj("Behind", -500f, 0f),
            Obj("Ahead", 500f, 0f)));

        Assert.Single(policy.SeenFields);
        var seen = policy.SeenFields[0];
        // The "Behind" object is outside the FOV cone and outside hearing range,
        // so the policy must not be told about it.
        Assert.DoesNotContain(seen.Visuals, v => v.Tag == "Behind");
        Assert.DoesNotContain(seen.Sounds, s => s.Tag == "Behind");
        Assert.Contains(seen.Visuals, v => v.Tag == "Ahead");
    }

    [Fact]
    public void Step_RespectsEngineOcclusion()
    {
        var policy = new RecordingPolicy();
        var loop = MakeLoop(policy, new PerceptionConfig { RequireVisibility = true });

        loop.Step(MakeObs(1.0,
            Obj("WallBehind", 500f, 0f, visible: false),
            Obj("Visible",    500f, 0f, visible: true)));

        var seen = policy.SeenFields[0];
        Assert.DoesNotContain(seen.Visuals, v => v.Tag == "WallBehind");
        Assert.Contains(seen.Visuals, v => v.Tag == "Visible");
    }

    [Fact]
    public void Step_PolicyApproachesPerceivedTarget_AndProducesMatchingMotorAction()
    {
        var policy = new RecordingPolicy
        {
            Behaviour = (field, _) =>
            {
                // Pick the highest-strength visual percept and approach it.
                var v = field.Visuals.OrderByDescending(p => p.SignalStrength).First();
                return new MotorIntent
                {
                    Type = MotorIntentType.MoveToward,
                    TargetWorld = (float[])v.WorldLocation.Clone(),
                    Magnitude = 0.9f
                };
            }
        };
        var loop = MakeLoop(policy);

        var act = loop.Step(MakeObs(1.0, Obj("Pickup", 400f, 0f)));

        Assert.NotNull(act);
        Assert.Equal("IA_Move", act!.InputAction);
        Assert.True(act.Magnitude > 0.5f);
    }

    [Fact]
    public void Step_UpdatesMemory_AndMemorySurvivesAcrossTicks()
    {
        var policy = new RecordingPolicy();
        var loop = MakeLoop(policy);

        // Tick 1: the avatar sees a Pickup.
        loop.Step(MakeObs(0.0, Obj("Pickup", 300f, 0f)));
        Assert.Equal(1, loop.Memory.Count);

        // Tick 2: the Pickup is no longer in the observation, but memory should still hold it.
        loop.Step(MakeObs(0.5));   // empty observation
        Assert.Equal(1, loop.Memory.Count);
        Assert.NotNull(loop.Memory.RecallByTag("Pickup"));
    }

    [Fact]
    public void Step_MemoryDecays_AndIsEventuallyForgotten()
    {
        var policy = new RecordingPolicy();
        var loop = MakeLoop(policy);
        loop.Memory.DecayPerSecond = 5f;
        loop.Memory.MinConfidence = 0.05f;

        loop.Step(MakeObs(0.0, Obj("Pickup", 300f, 0f)));
        Assert.Equal(1, loop.Memory.Count);

        // 100 simulated seconds later, with no re-perception → memory should prune.
        loop.Step(MakeObs(100.0));
        Assert.Equal(0, loop.Memory.Count);
    }

    [Fact]
    public void Step_PolicyChoosingIdle_ProducesNoAction()
    {
        var policy = new RecordingPolicy { Behaviour = (_, _) => MotorIntent.IdleIntent };
        var loop = MakeLoop(policy);

        var act = loop.Step(MakeObs(0.0, Obj("Anything", 200f, 0f)));

        Assert.Null(act);
    }

    [Fact]
    public void Step_ReactivePolicy_ApproachesInterestingVisualWhenSeen()
    {
        // Use the production reactive policy to sanity-check the integration.
        var perception = new SensoryPerceptionService(NullLogger<SensoryPerceptionService>.Instance);
        var memory = new SpatialMemory();
        var motor = new MotorController();
        var policy = new ReactivePerceptionPolicy();
        var loop = new EmbodiedDecisionLoop(perception, memory, motor, policy,
            NullLogger<EmbodiedDecisionLoop>.Instance);

        var act = loop.Step(MakeObs(0.0, Obj("Pickup", 400f, 0f)));

        Assert.NotNull(act);
        Assert.Equal("IA_Move", act!.InputAction);
        Assert.Equal("Reactive:Approach:Pickup", act.Source);
    }

    [Fact]
    public void Step_ReactivePolicy_OrientsTowardSoundWhenNothingVisible()
    {
        // Pick perception parameters so the source is unambiguously above
        // ReactivePerceptionPolicy's OrientToSoundThreshold (0.25):
        // loudness = 1/(distance/fullLoudness)² = 1/(250/200)² = 0.64
        var perception = new SensoryPerceptionService(NullLogger<SensoryPerceptionService>.Instance,
            new PerceptionConfig
            {
                FieldOfViewDeg = 60f,           // narrow visual cone
                SightRangeUu = 200f,            // short sight (object at 250 is invisible)
                HearingRangeUu = 5000f,
                FullLoudnessRangeUu = 200f
            });
        var loop = new EmbodiedDecisionLoop(perception, new SpatialMemory(), new MotorController(),
            new ReactivePerceptionPolicy(), NullLogger<EmbodiedDecisionLoop>.Instance);

        // Object behind & beyond sight range → not visible, but plenty audible.
        var act = loop.Step(MakeObs(0.0, Obj("NPC", -250f, 0f)));

        Assert.NotNull(act);
        Assert.Equal("IA_Look", act!.InputAction);
        Assert.StartsWith("Reactive:OrientTo:", act.Source);
    }

    [Fact]
    public void Step_ReactivePolicy_WandersWhenNothingPerceived()
    {
        var loop = MakeLoop(new ReactivePerceptionPolicy());

        var act = loop.Step(MakeObs(0.0));   // empty world

        Assert.NotNull(act);
        Assert.Equal("IA_Move", act!.InputAction);
        Assert.Equal("Reactive:Wander", act.Source);
    }

    [Fact]
    public void Step_AccessorsExposeLastPerceptionAndAction()
    {
        var loop = MakeLoop(new ReactivePerceptionPolicy());
        loop.Step(MakeObs(0.0, Obj("Pickup", 400f, 0f)));

        Assert.NotNull(loop.LastField);
        Assert.NotNull(loop.LastIntent);
        Assert.NotNull(loop.LastAction);
    }

    [Fact]
    public void ActionProducedEvent_FiresOnSuccessfulStep()
    {
        var loop = MakeLoop(new ReactivePerceptionPolicy());
        AvatarAction? captured = null;
        loop.ActionProduced += (_, a) => captured = a;

        loop.Step(MakeObs(0.0, Obj("Pickup", 400f, 0f)));

        Assert.NotNull(captured);
    }

    [Fact]
    public void UpdateReward_ForwardsRewardAndLastContextToPolicy()
    {
        var policy = new RecordingPolicy();
        var loop = MakeLoop(policy);

        loop.Step(MakeObs(0.0, Obj("Pickup", 400f, 0f)));
        loop.UpdateReward(0.75f);

        Assert.Single(policy.RewardCalls);
        var call = policy.RewardCalls[0];
        Assert.Equal(0.75f, call.Reward, 3);
        Assert.NotNull(call.Field);
        Assert.NotNull(call.Intent);
    }
}
