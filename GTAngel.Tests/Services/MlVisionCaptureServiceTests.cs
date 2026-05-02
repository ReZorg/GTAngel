using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for MlVisionCaptureService — constants, init, capture lifecycle, feature extraction.
/// </summary>
public class MlVisionCaptureServiceTests : IDisposable
{
    private readonly MlVisionCaptureService _svc =
        new(NullLogger<MlVisionCaptureService>.Instance);

    // ── Constants ─────────────────────────────────────────────────────────────

    [Fact]
    public void ML_WIDTH_Is768()
    {
        Assert.Equal(768, MlVisionCaptureService.ML_WIDTH);
    }

    [Fact]
    public void ML_HEIGHT_Is768()
    {
        Assert.Equal(768, MlVisionCaptureService.ML_HEIGHT);
    }

    [Fact]
    public void ML_CHANNELS_Is3()
    {
        Assert.Equal(3, MlVisionCaptureService.ML_CHANNELS);
    }

    [Fact]
    public void FRAME_FLOATS_Equals_768x768x3()
    {
        Assert.Equal(768 * 768 * 3, MlVisionCaptureService.FRAME_FLOATS);
    }

    [Fact]
    public void Resolution_ReturnsCorrectString()
    {
        Assert.Equal("768×768", _svc.Resolution);
    }

    // ── Initial state ─────────────────────────────────────────────────────────

    [Fact]
    public void IsCapturing_Initially_IsFalse()
    {
        Assert.False(_svc.IsCapturing);
    }

    [Fact]
    public void IsInitialised_Initially_IsFalse()
    {
        Assert.False(_svc.IsInitialised);
    }

    [Fact]
    public void FrameCount_Initially_IsZero()
    {
        Assert.Equal(0, _svc.FrameCount);
    }

    [Fact]
    public void FrameRate_Initially_IsZero()
    {
        Assert.Equal(0.0, _svc.FrameRate);
    }

    [Fact]
    public void LastFeatureNorm_Initially_IsZero()
    {
        Assert.Equal(0.0f, _svc.LastFeatureNorm);
    }

    // ── Initialisation ────────────────────────────────────────────────────────

    [Fact]
    public async Task InitialiseAsync_ReturnsTrue()
    {
        bool result = await _svc.InitialiseAsync();
        Assert.True(result);
    }

    [Fact]
    public async Task InitialiseAsync_SetsIsInitialisedTrue()
    {
        await _svc.InitialiseAsync();
        Assert.True(_svc.IsInitialised);
    }

    [Fact]
    public async Task InitialiseAsync_CalledTwice_ReturnsTrue()
    {
        await _svc.InitialiseAsync();
        bool result = await _svc.InitialiseAsync(); // idempotent
        Assert.True(result);
    }

    [Fact]
    public async Task InitialiseAsync_FiresStatusChangedEvent()
    {
        var statuses = new List<string>();
        _svc.OnStatusChanged += s => statuses.Add(s);

        await _svc.InitialiseAsync();

        Assert.NotEmpty(statuses);
    }

    // ── GetLatestFrame ────────────────────────────────────────────────────────

    [Fact]
    public void GetLatestFrame_ReturnsCorrectLength()
    {
        var frame = _svc.GetLatestFrame();
        Assert.Equal(MlVisionCaptureService.FRAME_FLOATS, frame.Length);
    }

    [Fact]
    public void GetLatestFrame_ReturnsCopy_NotSameReference()
    {
        var frame1 = _svc.GetLatestFrame();
        var frame2 = _svc.GetLatestFrame();
        Assert.NotSame(frame1, frame2);
    }

    // ── GetLatestFeatures ─────────────────────────────────────────────────────

    [Fact]
    public void GetLatestFeatures_Returns512Elements()
    {
        var features = _svc.GetLatestFeatures();
        Assert.Equal(512, features.Length);
    }

    [Fact]
    public void GetLatestFeatures_ReturnsCopy_NotSameReference()
    {
        var f1 = _svc.GetLatestFeatures();
        var f2 = _svc.GetLatestFeatures();
        Assert.NotSame(f1, f2);
    }

    // ── GetMetrics ────────────────────────────────────────────────────────────

    [Fact]
    public void GetMetrics_Returns_Correct_InitialValues()
    {
        var (frameCount, frameRate, featureNorm, isCapturing) = _svc.GetMetrics();
        Assert.Equal(0, frameCount);
        Assert.Equal(0.0, frameRate);
        Assert.Equal(0.0f, featureNorm);
        Assert.False(isCapturing);
    }

    // ── Capture lifecycle ─────────────────────────────────────────────────────

    [Fact]
    public async Task StartCaptureAsync_SetsIsCapturingTrue()
    {
        await _svc.InitialiseAsync();
        await _svc.StartCaptureAsync();

        Assert.True(_svc.IsCapturing);

        await _svc.StopCaptureAsync();
    }

    [Fact]
    public async Task StopCaptureAsync_SetsIsCapturingFalse()
    {
        await _svc.InitialiseAsync();
        await _svc.StartCaptureAsync();
        await _svc.StopCaptureAsync();

        Assert.False(_svc.IsCapturing);
    }

    [Fact]
    public async Task StartCaptureAsync_WhenAlreadyCapturing_DoesNotThrow()
    {
        await _svc.InitialiseAsync();
        await _svc.StartCaptureAsync();

        var ex = await Record.ExceptionAsync(() => _svc.StartCaptureAsync());
        Assert.Null(ex);

        await _svc.StopCaptureAsync();
    }

    [Fact]
    public async Task StopCaptureAsync_WhenNotCapturing_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() => _svc.StopCaptureAsync());
        Assert.Null(ex);
    }

    [Fact]
    public async Task StartCapture_IncreasesFrameCount()
    {
        await _svc.InitialiseAsync();
        await _svc.StartCaptureAsync();

        await Task.Delay(200); // Let a few frames accumulate

        await _svc.StopCaptureAsync();

        Assert.True(_svc.FrameCount > 0);
    }

    [Fact]
    public async Task OnFrameCaptured_Event_FiresDuringCapture()
    {
        int fired = 0;
        _svc.OnFrameCaptured += (_, _) => fired++;

        await _svc.InitialiseAsync();
        await _svc.StartCaptureAsync();
        await Task.Delay(200);
        await _svc.StopCaptureAsync();

        Assert.True(fired > 0);
    }

    // ── Dispose ───────────────────────────────────────────────────────────────

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var svc2 = new MlVisionCaptureService(NullLogger<MlVisionCaptureService>.Instance);
        var ex = Record.Exception(() => svc2.Dispose());
        Assert.Null(ex);
    }

    [Fact]
    public void Dispose_SetsIsInitialisedFalse()
    {
        var svc2 = new MlVisionCaptureService(NullLogger<MlVisionCaptureService>.Instance);
        svc2.Dispose();
        Assert.False(svc2.IsInitialised);
    }

    [Fact]
    public void Dispose_SetsIsCapturingFalse()
    {
        var svc2 = new MlVisionCaptureService(NullLogger<MlVisionCaptureService>.Instance);
        svc2.Dispose();
        Assert.False(svc2.IsCapturing);
    }

    public void Dispose() => _svc.Dispose();
}
