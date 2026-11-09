using System;
using GTAngel.Models.EmbodiedCognition;
using GTAngel.Services.EmbodiedCognition;
using Xunit;

namespace GTAngel.Tests.Services.EmbodiedCognition;

public sealed class ReactivePerceptionPolicyTests
{
    private static PerceptualField EmptyField()
        => new()
        {
            Timestamp = 0.0,
            Self = new EmbodiedSelfState
            {
                Position = new float[] { 0, 0, 0 },
                Rotation = new float[] { 0, 0, 0 }
            }
        };

    private static MotorIntent ApproachIntent()
        => new()
        {
            Type = MotorIntentType.MoveToward,
            Source = "Reactive:Approach:Pickup",
            Magnitude = 0.85f,
            TargetWorld = new float[] { 100f, 0, 0 }
        };

    private static MotorIntent OrientIntent()
        => new()
        {
            Type = MotorIntentType.TurnTo,
            Source = "Reactive:OrientTo:NPC",
            Magnitude = 0.5f,
            TargetWorld = new float[] { -100f, 0, 0 }
        };

    private static MotorIntent WanderIntent()
        => new()
        {
            Type = MotorIntentType.MoveToward,
            Source = "Reactive:Wander",
            Magnitude = 0.4f,
            TargetWorld = new float[] { 0, 100f, 0 }
        };

    [Fact]
    public void UpdateReward_UpdatesExponentialMovingAverageBaseline()
    {
        var policy = new ReactivePerceptionPolicy();
        policy.UpdateReward(1.0f, EmptyField(), ApproachIntent());

        Assert.Equal(0.1f, policy.MeanReward, 3);
    }

    [Fact]
    public void UpdateReward_PositiveApproach_LowersThresholdAndRaisesMagnitude()
    {
        var policy = new ReactivePerceptionPolicy();
        var originalThreshold = policy.Settings.ApproachVisualThreshold;
        var originalMagnitude = policy.Settings.ApproachMagnitude;

        policy.UpdateReward(1.0f, EmptyField(), ApproachIntent());

        Assert.True(policy.Settings.ApproachVisualThreshold < originalThreshold);
        Assert.True(policy.Settings.ApproachMagnitude > originalMagnitude);
    }

    [Fact]
    public void UpdateReward_NegativeApproach_RaisesThresholdAndLowersMagnitude()
    {
        var policy = new ReactivePerceptionPolicy();
        var originalThreshold = policy.Settings.ApproachVisualThreshold;
        var originalMagnitude = policy.Settings.ApproachMagnitude;

        policy.UpdateReward(-1.0f, EmptyField(), ApproachIntent());

        Assert.True(policy.Settings.ApproachVisualThreshold > originalThreshold);
        Assert.True(policy.Settings.ApproachMagnitude < originalMagnitude);
    }

    [Fact]
    public void UpdateReward_PositiveOrientTo_LowersSoundThreshold()
    {
        var policy = new ReactivePerceptionPolicy();
        var originalThreshold = policy.Settings.OrientToSoundThreshold;

        policy.UpdateReward(1.0f, EmptyField(), OrientIntent());

        Assert.True(policy.Settings.OrientToSoundThreshold < originalThreshold);
    }

    [Fact]
    public void UpdateReward_Wander_AdjustsWanderMagnitude()
    {
        var policy = new ReactivePerceptionPolicy();
        var originalMagnitude = policy.Settings.WanderMagnitude;

        policy.UpdateReward(1.0f, EmptyField(), WanderIntent());

        Assert.True(policy.Settings.WanderMagnitude > originalMagnitude);
    }

    [Fact]
    public void UpdateReward_KeepsValuesWithinClampedBounds()
    {
        var policy = new ReactivePerceptionPolicy();

        // Multiple extreme positive updates should not exceed upper clamp.
        for (int i = 0; i < 20; i++)
            policy.UpdateReward(10.0f, EmptyField(), ApproachIntent());

        Assert.True(policy.Settings.ApproachMagnitude <= 1.0f);
        Assert.True(policy.Settings.ApproachVisualThreshold >= 0.03f);
        Assert.True(policy.Settings.OrientToMagnitude <= 1.0f);

        // Multiple extreme negative updates should not break lower clamp.
        for (int i = 0; i < 20; i++)
            policy.UpdateReward(-10.0f, EmptyField(), WanderIntent());

        Assert.True(policy.Settings.WanderMagnitude >= 0.1f);
    }

    [Fact]
    public void UpdateReward_IdleIntent_IsIgnored()
    {
        var policy = new ReactivePerceptionPolicy();
        var original = policy.Settings.WanderMagnitude;

        policy.UpdateReward(1.0f, EmptyField(), MotorIntent.IdleIntent);

        Assert.Equal(original, policy.Settings.WanderMagnitude, 3);
    }
}
