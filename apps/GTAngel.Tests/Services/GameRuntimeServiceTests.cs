using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for GameRuntimeService — default configuration, static constants,
/// ProprioceptiveState.ToFeatureVector, RLStepResult, and Dispose.
/// </summary>
public class GameRuntimeServiceTests : IDisposable
{
    private readonly GameRuntimeService _svc = new();

    // ── Default configuration ─────────────────────────────────────────────────

    [Fact]
    public void CaptureWidth_DefaultIs768()
    {
        Assert.Equal(768, _svc.CaptureWidth);
    }

    [Fact]
    public void CaptureHeight_DefaultIs768()
    {
        Assert.Equal(768, _svc.CaptureHeight);
    }

    [Fact]
    public void TargetFPS_DefaultIs15()
    {
        Assert.Equal(15, _svc.TargetFPS);
    }

    [Fact]
    public void GameWindowTitle_DefaultIsGTA3()
    {
        Assert.Equal("GTA3", _svc.GameWindowTitle);
    }

    [Fact]
    public void AlternateWindowTitles_ContainsKnownTitles()
    {
        Assert.Contains("re3", _svc.AlternateWindowTitles);
    }

    // ── Initial state ─────────────────────────────────────────────────────────

    [Fact]
    public void IsConnected_Initially_IsFalse()
    {
        Assert.False(_svc.IsConnected);
    }

    [Fact]
    public void GetLatestFrame_Initially_ReturnsNull()
    {
        Assert.Null(_svc.GetLatestFrame());
    }

    // ── GTA3Keys static constants ─────────────────────────────────────────────

    [Fact]
    public void GTA3Keys_Forward_Is_W()
    {
        Assert.Equal((ushort)0x57, GameRuntimeService.GTA3Keys.Forward);
    }

    [Fact]
    public void GTA3Keys_Backward_Is_S()
    {
        Assert.Equal((ushort)0x53, GameRuntimeService.GTA3Keys.Backward);
    }

    [Fact]
    public void GTA3Keys_Left_Is_A()
    {
        Assert.Equal((ushort)0x41, GameRuntimeService.GTA3Keys.Left);
    }

    [Fact]
    public void GTA3Keys_Right_Is_D()
    {
        Assert.Equal((ushort)0x44, GameRuntimeService.GTA3Keys.Right);
    }

    [Fact]
    public void GTA3Keys_Jump_Is_Space()
    {
        Assert.Equal((ushort)0x20, GameRuntimeService.GTA3Keys.Jump);
    }

    [Fact]
    public void GTA3Keys_EnterVehicle_Is_F()
    {
        Assert.Equal((ushort)0x46, GameRuntimeService.GTA3Keys.EnterVehicle);
    }

    // ── ProprioceptiveState.ToFeatureVector ───────────────────────────────────

    [Fact]
    public void ProprioceptiveState_ToFeatureVector_Returns14Elements()
    {
        var state = new ProprioceptiveState();
        var vector = state.ToFeatureVector();
        Assert.Equal(14, vector.Length);
    }

    [Fact]
    public void ProprioceptiveState_ToFeatureVector_NoNaN()
    {
        var state = new ProprioceptiveState
        {
            PositionX = 1500f,
            PositionY = -1000f,
            PositionZ = 30f,
            Health = 80f,
            Armor = 50f,
            WantedLevel = 3,
            IsInVehicle = true,
            VehicleSpeed = 60f,
            Money = 250_000,
        };

        var vector = state.ToFeatureVector();

        foreach (float v in vector)
            Assert.False(float.IsNaN(v), $"NaN in feature vector");
    }

    [Fact]
    public void ProprioceptiveState_IsInVehicle_True_SetsOneInVector()
    {
        var state = new ProprioceptiveState { IsInVehicle = true };
        var vector = state.ToFeatureVector();
        Assert.Equal(1f, vector[11]); // IsInVehicle at index 11
    }

    [Fact]
    public void ProprioceptiveState_IsInVehicle_False_SetsZeroInVector()
    {
        var state = new ProprioceptiveState { IsInVehicle = false };
        var vector = state.ToFeatureVector();
        Assert.Equal(0f, vector[11]);
    }

    [Fact]
    public void ProprioceptiveState_DefaultHealth_Is100()
    {
        Assert.Equal(100f, new ProprioceptiveState().Health);
    }

    // ── StopFrameCapture / StopStateExtraction when not started ──────────────

    [Fact]
    public void StopFrameCapture_WhenNotStarted_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.StopFrameCapture());
        Assert.Null(ex);
    }

    [Fact]
    public void StopStateExtraction_WhenNotStarted_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.StopStateExtraction());
        Assert.Null(ex);
    }

    // ── ConnectOrLaunchAsync on CI (no GTA3) ─────────────────────────────────

    [Fact]
    public async Task ConnectOrLaunchAsync_WhenGameAbsent_ReturnsFalse()
    {
        bool result = await _svc.ConnectOrLaunchAsync(null);
        Assert.False(result);
    }

    [Fact]
    public async Task ConnectOrLaunchAsync_WithNonExistentPath_ReturnsFalse()
    {
        bool result = await _svc.ConnectOrLaunchAsync(@"C:\NonExistent\gta3.exe");
        Assert.False(result);
    }

    // ── RLStepResult defaults ─────────────────────────────────────────────────

    [Fact]
    public void RLStepResult_DefaultDone_IsFalse()
    {
        var result = new RLStepResult();
        Assert.False(result.Done);
    }

    [Fact]
    public void RLStepResult_DefaultReward_IsZero()
    {
        var result = new RLStepResult();
        Assert.Equal(0f, result.Reward);
    }

    [Fact]
    public void RLStepResult_DefaultState_IsInitialized()
    {
        var result = new RLStepResult();
        Assert.NotNull(result.State);
    }

    [Fact]
    public void RLStepResult_CanStoreFrameStateAndAction()
    {
        var frame = new FrameData { Width = 32, Height = 32, FrameNumber = 7 };
        var state = new ProprioceptiveState { Health = 80f };
        var result = new RLStepResult
        {
            Frame = frame,
            State = state,
            Reward = 1.25f,
            Done = true,
            Action = 3,
        };

        Assert.Same(frame, result.Frame);
        Assert.Same(state, result.State);
        Assert.Equal(1.25f, result.Reward);
        Assert.True(result.Done);
        Assert.Equal(3, result.Action);
    }

    [Fact]
    public void FrameData_Defaults_AreEmptyAndNull()
    {
        var frame = new FrameData();
        Assert.Empty(frame.NormalizedPixels);
        Assert.Null(frame.Preview);
    }

    [Fact]
    public void FrameData_Properties_CanBeAssigned()
    {
        var pixels = new[] { 0.1f, 0.2f, 0.3f };
        var frame = new FrameData
        {
            Timestamp = 123,
            FrameNumber = 4,
            Width = 1,
            Height = 1,
            NormalizedPixels = pixels,
        };

        Assert.Equal(123, frame.Timestamp);
        Assert.Equal(4, frame.FrameNumber);
        Assert.Equal(1, frame.Width);
        Assert.Equal(1, frame.Height);
        Assert.Same(pixels, frame.NormalizedPixels);
    }

    // ── Dispose ───────────────────────────────────────────────────────────────

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var svc2 = new GameRuntimeService();
        var ex = Record.Exception(() => svc2.Dispose());
        Assert.Null(ex);
    }

    [Fact]
    public void Dispose_CalledTwice_DoesNotThrow()
    {
        var svc2 = new GameRuntimeService();
        svc2.Dispose();
        var ex = Record.Exception(() => svc2.Dispose());
        Assert.Null(ex);
    }

    public void Dispose() => _svc.Dispose();
}
