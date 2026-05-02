using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for VigemControllerService — constants, action enums, GamepadState.FromContinuousAction,
/// Initialize (keyboard fallback on CI), discrete action dispatch, and Dispose.
/// </summary>
public class VigemControllerServiceTests : IDisposable
{
    private readonly VigemControllerService _svc =
        new(NullLogger<VigemControllerService>.Instance);

    // ── Constants ─────────────────────────────────────────────────────────────

    [Fact]
    public void DiscreteActionCount_Is18()
    {
        Assert.Equal(18, VigemControllerService.DiscreteActionCount);
    }

    [Fact]
    public void ContinuousActionDimension_Is6()
    {
        Assert.Equal(6, VigemControllerService.ContinuousActionDimension);
    }

    // ── DiscreteAction enum ───────────────────────────────────────────────────

    [Fact]
    public void DiscreteAction_Noop_IsZero()
    {
        Assert.Equal(0, (int)VigemControllerService.DiscreteAction.Noop);
    }

    [Fact]
    public void DiscreteAction_MaxIndex_Is17()
    {
        var values = Enum.GetValues<VigemControllerService.DiscreteAction>();
        Assert.Equal(17, (int)values.Max());
    }

    [Fact]
    public void DiscreteAction_Count_Matches_DiscreteActionCount()
    {
        var count = Enum.GetValues<VigemControllerService.DiscreteAction>().Length;
        Assert.Equal(VigemControllerService.DiscreteActionCount, count);
    }

    // ── GamepadButtons flags ──────────────────────────────────────────────────

    [Fact]
    public void GamepadButtons_None_IsZero()
    {
        Assert.Equal(0, (int)VigemControllerService.GamepadButtons.None);
    }

    [Fact]
    public void GamepadButtons_A_HasExpectedValue()
    {
        Assert.Equal(0x1000, (int)VigemControllerService.GamepadButtons.A);
    }

    // ── GamepadState.FromContinuousAction ─────────────────────────────────────

    [Fact]
    public void FromContinuousAction_FullVector_SetsAllFields()
    {
        var action = new float[] { 0.5f, -0.5f, 0.25f, -0.25f, 0.8f, 0.9f };
        var state = VigemControllerService.GamepadState.FromContinuousAction(action);

        Assert.Equal(0.5f, state.LeftStickX, 5);
        Assert.Equal(-0.5f, state.LeftStickY, 5);
        Assert.Equal(0.25f, state.RightStickX, 5);
        Assert.Equal(-0.25f, state.RightStickY, 5);
        Assert.Equal(0.8f, state.LeftTrigger, 5);
        Assert.Equal(0.9f, state.RightTrigger, 5);
    }

    [Fact]
    public void FromContinuousAction_EmptyVector_DefaultsToZero()
    {
        var state = VigemControllerService.GamepadState.FromContinuousAction(Array.Empty<float>());

        Assert.Equal(0f, state.LeftStickX);
        Assert.Equal(0f, state.LeftStickY);
        Assert.Equal(0f, state.RightStickX);
        Assert.Equal(0f, state.RightStickY);
        Assert.Equal(0f, state.LeftTrigger);
        Assert.Equal(0f, state.RightTrigger);
    }

    [Fact]
    public void FromContinuousAction_ClampsStickAboveOne()
    {
        var action = new float[] { 5f, 5f, 5f, 5f, 5f, 5f };
        var state = VigemControllerService.GamepadState.FromContinuousAction(action);

        Assert.Equal(1f, state.LeftStickX);
        Assert.Equal(1f, state.LeftStickY);
    }

    [Fact]
    public void FromContinuousAction_ClampsStickBelowMinusOne()
    {
        var action = new float[] { -5f, -5f, 0f, 0f, 0f, 0f };
        var state = VigemControllerService.GamepadState.FromContinuousAction(action);

        Assert.Equal(-1f, state.LeftStickX);
        Assert.Equal(-1f, state.LeftStickY);
    }

    [Fact]
    public void FromContinuousAction_ClampsTriggerBelowZero()
    {
        var action = new float[] { 0f, 0f, 0f, 0f, -1f, -1f };
        var state = VigemControllerService.GamepadState.FromContinuousAction(action);

        Assert.Equal(0f, state.LeftTrigger);
        Assert.Equal(0f, state.RightTrigger);
    }

    [Fact]
    public void FromContinuousAction_ClampsTriggerAboveOne()
    {
        var action = new float[] { 0f, 0f, 0f, 0f, 10f, 10f };
        var state = VigemControllerService.GamepadState.FromContinuousAction(action);

        Assert.Equal(1f, state.LeftTrigger);
        Assert.Equal(1f, state.RightTrigger);
    }

    [Fact]
    public void FromContinuousAction_PartialVector_DefaultsMissingToZero()
    {
        var action = new float[] { 0.3f }; // Only LX provided
        var state = VigemControllerService.GamepadState.FromContinuousAction(action);

        Assert.Equal(0.3f, state.LeftStickX, 5);
        Assert.Equal(0f, state.LeftStickY);
        Assert.Equal(0f, state.RightStickX);
        Assert.Equal(0f, state.RightTrigger);
    }

    // ── Initialize ────────────────────────────────────────────────────────────

    [Fact]
    public void Initialize_ReturnsTrueAlways()
    {
        // On CI there is no ViGEm bus, so keyboard fallback activates
        bool result = _svc.Initialize();
        Assert.True(result);
    }

    // ── SetTargetWindow ───────────────────────────────────────────────────────

    [Fact]
    public void SetTargetWindow_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.SetTargetWindow(nint.Zero));
        Assert.Null(ex);
    }

    // ── ExecuteDiscreteAction ─────────────────────────────────────────────────

    [Fact]
    public void ExecuteDiscreteAction_Noop_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            _svc.ExecuteDiscreteAction(VigemControllerService.DiscreteAction.Noop));
        Assert.Null(ex);
    }

    [Fact]
    public void ExecuteDiscreteAction_AllActions_DoNotThrow()
    {
        foreach (var action in Enum.GetValues<VigemControllerService.DiscreteAction>())
        {
            var ex = Record.Exception(() => _svc.ExecuteDiscreteAction(action));
            Assert.Null(ex);
        }
    }

    [Fact]
    public void ExecuteDiscreteAction_ByIndex_DoesNotThrow()
    {
        for (int i = 0; i < VigemControllerService.DiscreteActionCount; i++)
        {
            var ex = Record.Exception(() => _svc.ExecuteDiscreteAction(i));
            Assert.Null(ex);
        }
    }

    [Fact]
    public void ExecuteDiscreteAction_OutOfRangeIndex_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.ExecuteDiscreteAction(999));
        Assert.Null(ex);
    }

    // ── ExecuteContinuousAction ───────────────────────────────────────────────

    [Fact]
    public void ExecuteContinuousAction_ZeroVector_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            _svc.ExecuteContinuousAction(new float[6]));
        Assert.Null(ex);
    }

    [Fact]
    public void ExecuteContinuousAction_FullActionVector_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            _svc.ExecuteContinuousAction(new float[] { 1f, -1f, 0.5f, -0.5f, 0.8f, 0.2f }));
        Assert.Null(ex);
    }

    // ── Dispose ───────────────────────────────────────────────────────────────

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var svc2 = new VigemControllerService(NullLogger<VigemControllerService>.Instance);
        var ex = Record.Exception(() => svc2.Dispose());
        Assert.Null(ex);
    }

    public void Dispose() => _svc.Dispose();
}
