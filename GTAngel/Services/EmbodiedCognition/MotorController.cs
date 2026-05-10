using System;
using GTAngel.Interop;
using GTAngel.Models.EmbodiedCognition;

namespace GTAngel.Services.EmbodiedCognition;

/// <summary>
/// Translates <see cref="MotorIntent"/> into <see cref="AvatarAction"/>s
/// honoring the avatar's physical constraints.
///
/// The controller maintains a small internal body-state (crouched / sprinting)
/// so consecutive intents compose sensibly: e.g. <c>Crouch</c> then
/// <c>Sprint</c> will correctly suppress the sprint, even though each intent
/// arrives independently.
/// </summary>
public sealed class MotorController
{
    /// <summary>Live config — caller may tweak between ticks.</summary>
    public MotorConfig Config { get; set; } = new();

    /// <summary>Whether the avatar is currently crouched.</summary>
    public bool IsCrouched { get; private set; }

    /// <summary>Whether the avatar is currently sprinting.</summary>
    public bool IsSprinting { get; private set; }

    public MotorController(MotorConfig? config = null)
    {
        if (config != null) Config = config;
    }

    /// <summary>
    /// Convert an intent into a concrete UE5 action, given the avatar's current
    /// proprioceptive state. Returns <c>null</c> for <see cref="MotorIntentType.Idle"/>
    /// or for intents that resolve to a no-op (e.g. zero-magnitude move).
    /// </summary>
    public AvatarAction? Translate(MotorIntent intent, EmbodiedSelfState self)
    {
        if (intent == null) throw new ArgumentNullException(nameof(intent));
        if (self == null) throw new ArgumentNullException(nameof(self));

        switch (intent.Type)
        {
            case MotorIntentType.Idle:
                return null;

            case MotorIntentType.MoveToward:
                return BuildMoveToward(intent, self);

            case MotorIntentType.Strafe:
                return BuildStrafe(intent);

            case MotorIntentType.TurnTo:
                return BuildTurnTo(intent, self, "IA_Look");

            case MotorIntentType.LookAt:
                return BuildTurnTo(intent, self, "IA_Look");

            case MotorIntentType.Jump:
                if (IsCrouched) return null;
                return new AvatarAction { InputAction = "IA_Jump", Magnitude = 1f, Source = intent.Source };

            case MotorIntentType.Crouch:
                IsCrouched = !IsCrouched;
                if (IsCrouched) IsSprinting = false;
                return new AvatarAction
                {
                    InputAction = "IA_Crouch",
                    Magnitude = IsCrouched ? 1f : 0f,
                    Source = intent.Source
                };

            case MotorIntentType.Sprint:
                if (Config.SuppressSprintWhileCrouched && IsCrouched)
                {
                    // Down-grade to ordinary move if a target was provided.
                    if (HasTarget(intent.TargetWorld))
                        return BuildMoveToward(intent, self);
                    return null;
                }
                // Validate magnitude against deadzone BEFORE committing IsSprinting,
                // otherwise a sub-deadzone request would corrupt the body-state flag
                // while producing a zero-magnitude no-op action.
                var sprintMag = ApplyDeadzone(intent.Magnitude);
                if (sprintMag <= 0f) return null;
                IsSprinting = true;
                return new AvatarAction
                {
                    InputAction = "IA_Sprint",
                    Magnitude = sprintMag,
                    Source = intent.Source
                };

            case MotorIntentType.Interact:
                return new AvatarAction { InputAction = "IA_Interact", Magnitude = 1f, Source = intent.Source };

            default:
                return null;
        }
    }

    /// <summary>Reset all transient body-state (crouch / sprint flags).</summary>
    public void Reset()
    {
        IsCrouched = false;
        IsSprinting = false;
    }

    // ── Builders ──────────────────────────────────────────────────────────

    private AvatarAction? BuildMoveToward(MotorIntent intent, EmbodiedSelfState self)
    {
        if (!HasTarget(intent.TargetWorld)) return null;

        float yawDeg = self.Rotation.Length > 1 ? self.Rotation[1] : 0f;
        float bearingDeg = SensoryPerceptionService.SignedYawDelta(
            self.Position, intent.TargetWorld, yawDeg);

        float bearingRad = bearingDeg * (MathF.PI / 180f);
        // Body-frame: forward = +Y in our convention, right = +X.
        // Convert "want to go in this bearing relative to forward" into an
        // (axisX, axisY) joystick pair.
        float axisX = MathF.Sin(bearingRad);
        float axisY = MathF.Cos(bearingRad);

        float mag = ApplyDeadzone(Math.Clamp(intent.Magnitude, -Config.MaxAxis, Config.MaxAxis));
        if (mag <= 0f) return null;

        return new AvatarAction
        {
            InputAction = "IA_Move",
            AxisX = axisX * mag,
            AxisY = axisY * mag,
            Magnitude = mag,
            HoldDuration = intent.HoldDuration > 0f ? intent.HoldDuration : Config.DefaultHoldSeconds,
            Source = intent.Source
        };
    }

    private AvatarAction? BuildStrafe(MotorIntent intent)
    {
        float mag = ApplyDeadzone(Math.Clamp(intent.Magnitude, -Config.MaxAxis, Config.MaxAxis));
        if (MathF.Abs(mag) < 1e-5f) return null;

        return new AvatarAction
        {
            InputAction = mag >= 0f ? "IA_StrafeR" : "IA_StrafeL",
            AxisX = MathF.Abs(mag),
            AxisY = 0f,
            Magnitude = MathF.Abs(mag),
            HoldDuration = intent.HoldDuration > 0f ? intent.HoldDuration : Config.DefaultHoldSeconds,
            Source = intent.Source
        };
    }

    private AvatarAction? BuildTurnTo(MotorIntent intent, EmbodiedSelfState self, string actionName)
    {
        if (!HasTarget(intent.TargetWorld)) return null;

        float yawDeg = self.Rotation.Length > 1 ? self.Rotation[1] : 0f;
        float bearingDeg = SensoryPerceptionService.SignedYawDelta(
            self.Position, intent.TargetWorld, yawDeg);

        float saturate = MathF.Max(1f, Config.TurnSaturationDeg);
        float axisX = Math.Clamp(bearingDeg / saturate, -Config.MaxAxis, Config.MaxAxis);

        if (MathF.Abs(axisX) < Config.DeadzoneMagnitude) return null;

        return new AvatarAction
        {
            InputAction = actionName,
            AxisX = axisX,
            AxisY = 0f,
            Magnitude = MathF.Abs(axisX),
            HoldDuration = intent.HoldDuration,
            Source = intent.Source
        };
    }

    private float ApplyDeadzone(float v)
    {
        if (MathF.Abs(v) < Config.DeadzoneMagnitude) return 0f;
        return Math.Clamp(v, -Config.MaxAxis, Config.MaxAxis);
    }

    private static bool HasTarget(float[]? target)
    {
        if (target == null || target.Length < 2) return false;
        return MathF.Abs(target[0]) + MathF.Abs(target[1]) > 1e-5f
            || (target.Length >= 3 && MathF.Abs(target[2]) > 1e-5f);
    }
}
