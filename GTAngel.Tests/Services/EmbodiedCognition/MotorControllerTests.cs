using System;
using GTAngel.Models.EmbodiedCognition;
using GTAngel.Services.EmbodiedCognition;
using Xunit;

namespace GTAngel.Tests.Services.EmbodiedCognition;

/// <summary>
/// Tests for <see cref="MotorController"/>: physical-constraint enforcement,
/// crouch/sprint interaction, deadzone application, and intent → AvatarAction
/// translation.
/// </summary>
public sealed class MotorControllerTests
{
    private static EmbodiedSelfState Self(float[] pos, float[] rot)
        => new() { Position = pos, Rotation = rot, Velocity = new float[3] };

    // ── 1. Idle ───────────────────────────────────────────────────────────

    [Fact]
    public void Idle_ProducesNoAction()
    {
        var mc = new MotorController();
        var act = mc.Translate(MotorIntent.IdleIntent, Self(new float[3], new float[3]));
        Assert.Null(act);
    }

    // ── 2. MoveToward ─────────────────────────────────────────────────────

    [Fact]
    public void MoveToward_ProducesIA_Move_WithExpectedAxes()
    {
        var mc = new MotorController();
        // Avatar at origin, facing forward (yaw=0). Target straight ahead at (500, 0).
        var intent = new MotorIntent
        {
            Type = MotorIntentType.MoveToward,
            TargetWorld = new float[] { 500f, 0f, 0f },
            Magnitude = 0.7f
        };

        var act = mc.Translate(intent, Self(new float[3], new float[3]));

        Assert.NotNull(act);
        Assert.Equal("IA_Move", act!.InputAction);
        // Bearing 0 → axisX = sin(0)=0, axisY = cos(0)=1 (scaled by magnitude).
        Assert.InRange(act.AxisX, -0.01f, 0.01f);
        Assert.InRange(act.AxisY, 0.69f, 0.71f);
        Assert.InRange(act.Magnitude, 0.69f, 0.71f);
    }

    [Fact]
    public void MoveToward_TargetToTheRight_ProducesPositiveAxisX()
    {
        var mc = new MotorController();
        var intent = new MotorIntent
        {
            Type = MotorIntentType.MoveToward,
            TargetWorld = new float[] { 0f, 500f, 0f },  // 90° right of forward
            Magnitude = 1f
        };
        var act = mc.Translate(intent, Self(new float[3], new float[3]));

        Assert.NotNull(act);
        // Bearing +90° → axisX = sin(90°) = 1, axisY = cos(90°) ≈ 0.
        Assert.InRange(act!.AxisX, 0.99f, 1.01f);
        Assert.InRange(act.AxisY, -0.01f, 0.01f);
    }

    [Fact]
    public void MoveToward_AppliesDeadzone()
    {
        var mc = new MotorController(new MotorConfig { DeadzoneMagnitude = 0.2f });
        var intent = new MotorIntent
        {
            Type = MotorIntentType.MoveToward,
            TargetWorld = new float[] { 100f, 0f, 0f },
            Magnitude = 0.05f   // below the 0.2 deadzone
        };
        var act = mc.Translate(intent, Self(new float[3], new float[3]));
        Assert.Null(act);
    }

    [Fact]
    public void MoveToward_NoTarget_IsRejected()
    {
        var mc = new MotorController();
        var intent = new MotorIntent
        {
            Type = MotorIntentType.MoveToward,
            TargetWorld = new float[] { 0f, 0f, 0f },
            Magnitude = 1f
        };
        var act = mc.Translate(intent, Self(new float[3], new float[3]));
        Assert.Null(act);
    }

    // ── 3. Strafe ─────────────────────────────────────────────────────────

    [Fact]
    public void Strafe_PositiveMagnitude_ProducesIA_StrafeR()
    {
        var mc = new MotorController();
        var act = mc.Translate(
            new MotorIntent { Type = MotorIntentType.Strafe, Magnitude = 0.6f },
            Self(new float[3], new float[3]));
        Assert.NotNull(act);
        Assert.Equal("IA_StrafeR", act!.InputAction);
    }

    [Fact]
    public void Strafe_NegativeMagnitude_ProducesIA_StrafeL()
    {
        var mc = new MotorController();
        var act = mc.Translate(
            new MotorIntent { Type = MotorIntentType.Strafe, Magnitude = -0.6f },
            Self(new float[3], new float[3]));
        Assert.NotNull(act);
        Assert.Equal("IA_StrafeL", act!.InputAction);
    }

    // ── 4. Jump / Crouch interaction ──────────────────────────────────────

    [Fact]
    public void Jump_BlockedWhileCrouched()
    {
        var mc = new MotorController();
        // Crouch first.
        var crouch = mc.Translate(
            new MotorIntent { Type = MotorIntentType.Crouch },
            Self(new float[3], new float[3]));
        Assert.NotNull(crouch);
        Assert.True(mc.IsCrouched);

        // Now try to jump.
        var jump = mc.Translate(
            new MotorIntent { Type = MotorIntentType.Jump },
            Self(new float[3], new float[3]));
        Assert.Null(jump);
    }

    [Fact]
    public void Jump_AllowedWhenStanding()
    {
        var mc = new MotorController();
        var jump = mc.Translate(
            new MotorIntent { Type = MotorIntentType.Jump },
            Self(new float[3], new float[3]));
        Assert.NotNull(jump);
        Assert.Equal("IA_Jump", jump!.InputAction);
    }

    [Fact]
    public void Crouch_TogglesState()
    {
        var mc = new MotorController();
        Assert.False(mc.IsCrouched);

        mc.Translate(new MotorIntent { Type = MotorIntentType.Crouch },
                     Self(new float[3], new float[3]));
        Assert.True(mc.IsCrouched);

        mc.Translate(new MotorIntent { Type = MotorIntentType.Crouch },
                     Self(new float[3], new float[3]));
        Assert.False(mc.IsCrouched);
    }

    [Fact]
    public void Sprint_DowngradesToMove_WhileCrouched_WithTarget()
    {
        var mc = new MotorController();
        mc.Translate(new MotorIntent { Type = MotorIntentType.Crouch },
                     Self(new float[3], new float[3]));

        var act = mc.Translate(new MotorIntent
        {
            Type = MotorIntentType.Sprint,
            TargetWorld = new float[] { 500f, 0f, 0f },
            Magnitude = 1f
        }, Self(new float[3], new float[3]));

        Assert.NotNull(act);
        Assert.Equal("IA_Move", act!.InputAction);   // not IA_Sprint
        Assert.False(mc.IsSprinting);                // sprint flag never set
    }

    [Fact]
    public void Sprint_StandingProducesIA_Sprint()
    {
        var mc = new MotorController();
        var act = mc.Translate(new MotorIntent { Type = MotorIntentType.Sprint, Magnitude = 0.8f },
                               Self(new float[3], new float[3]));
        Assert.NotNull(act);
        Assert.Equal("IA_Sprint", act!.InputAction);
        Assert.True(mc.IsSprinting);
    }

    /// <summary>
    /// Regression: a sprint intent whose magnitude is below the deadzone must NOT
    /// flip <see cref="MotorController.IsSprinting"/> to true. Previously the
    /// state flag was committed before the deadzone check, leaving the controller
    /// reporting "sprinting" while emitting a zero-magnitude no-op action.
    /// </summary>
    [Fact]
    public void Sprint_BelowDeadzone_DoesNotCorruptIsSprintingState()
    {
        var mc = new MotorController(new MotorConfig { DeadzoneMagnitude = 0.2f });
        var act = mc.Translate(
            new MotorIntent { Type = MotorIntentType.Sprint, Magnitude = 0.05f },
            Self(new float[3], new float[3]));

        Assert.Null(act);                  // sub-deadzone → no action
        Assert.False(mc.IsSprinting);      // state must stay clean
    }

    // ── 5. TurnTo ─────────────────────────────────────────────────────────

    [Fact]
    public void TurnTo_TargetToTheRight_ProducesPositiveAxisX()
    {
        var mc = new MotorController();
        var act = mc.Translate(new MotorIntent
        {
            Type = MotorIntentType.TurnTo,
            TargetWorld = new float[] { 0f, 500f, 0f },  // right
            Magnitude = 1f
        }, Self(new float[3], new float[3]));

        Assert.NotNull(act);
        Assert.Equal("IA_Look", act!.InputAction);
        Assert.True(act.AxisX > 0f);
    }

    [Fact]
    public void TurnTo_AlreadyOnTarget_IsRejected()
    {
        var mc = new MotorController(new MotorConfig
        {
            TurnSaturationDeg = 90f, DeadzoneMagnitude = 0.05f
        });
        // Target directly ahead (yaw=0, target on +X) → bearing 0 → below deadzone.
        var act = mc.Translate(new MotorIntent
        {
            Type = MotorIntentType.TurnTo,
            TargetWorld = new float[] { 500f, 0f, 0f },
            Magnitude = 1f
        }, Self(new float[3], new float[3]));

        Assert.Null(act);
    }

    // ── 6. Interact ───────────────────────────────────────────────────────

    [Fact]
    public void Interact_ProducesIA_Interact()
    {
        var mc = new MotorController();
        var act = mc.Translate(new MotorIntent { Type = MotorIntentType.Interact },
                               Self(new float[3], new float[3]));
        Assert.NotNull(act);
        Assert.Equal("IA_Interact", act!.InputAction);
    }

    // ── 7. Reset ──────────────────────────────────────────────────────────

    [Fact]
    public void Reset_ClearsCrouchAndSprintState()
    {
        var mc = new MotorController();
        mc.Translate(new MotorIntent { Type = MotorIntentType.Crouch },
                     Self(new float[3], new float[3]));
        mc.Translate(new MotorIntent { Type = MotorIntentType.Sprint, Magnitude = 1f },
                     Self(new float[3], new float[3]));

        mc.Reset();
        Assert.False(mc.IsCrouched);
        Assert.False(mc.IsSprinting);
    }

    // ── 8. Argument validation ────────────────────────────────────────────

    [Fact]
    public void Translate_ThrowsOnNullArgs()
    {
        var mc = new MotorController();
        Assert.Throws<ArgumentNullException>(() => mc.Translate(null!, Self(new float[3], new float[3])));
        Assert.Throws<ArgumentNullException>(() => mc.Translate(MotorIntent.IdleIntent, null!));
    }
}
