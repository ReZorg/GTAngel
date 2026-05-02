using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for DxgiFrameCaptureService — default configuration, Initialize (DXGI falls back
/// to GDI on CI), properties, and Dispose.
/// </summary>
public class DxgiFrameCaptureServiceTests : IDisposable
{
    private readonly DxgiFrameCaptureService _svc =
        new(NullLogger<DxgiFrameCaptureService>.Instance);

    // ── Default configuration ─────────────────────────────────────────────────

    [Fact]
    public void TargetWidth_DefaultIs768()
    {
        Assert.Equal(768, _svc.TargetWidth);
    }

    [Fact]
    public void TargetHeight_DefaultIs768()
    {
        Assert.Equal(768, _svc.TargetHeight);
    }

    [Fact]
    public void MaxFps_DefaultIs30()
    {
        Assert.Equal(30, _svc.MaxFps);
    }

    // ── Initial state ─────────────────────────────────────────────────────────

    [Fact]
    public void IsCapturing_Initially_IsFalse()
    {
        Assert.False(_svc.IsCapturing);
    }

    [Fact]
    public void TotalFramesCaptured_Initially_IsZero()
    {
        Assert.Equal(0L, _svc.TotalFramesCaptured);
    }

    [Fact]
    public void AverageCaptureTimeMs_Initially_IsZero()
    {
        Assert.Equal(0.0, _svc.AverageCaptureTimeMs);
    }

    [Fact]
    public void CurrentFps_Initially_IsZero()
    {
        Assert.Equal(0.0, _svc.CurrentFps);
    }

    // ── Configuration setters ─────────────────────────────────────────────────

    [Fact]
    public void TargetWidth_CanBeChanged()
    {
        _svc.TargetWidth = 1920;
        Assert.Equal(1920, _svc.TargetWidth);
    }

    [Fact]
    public void TargetHeight_CanBeChanged()
    {
        _svc.TargetHeight = 1080;
        Assert.Equal(1080, _svc.TargetHeight);
    }

    [Fact]
    public void MaxFps_CanBeChanged()
    {
        _svc.MaxFps = 60;
        Assert.Equal(60, _svc.MaxFps);
    }

    // ── Initialize ────────────────────────────────────────────────────────────

    [Fact]
    public void Initialize_ReturnsTrueAlways()
    {
        // DXGI may not be available on CI; the service falls back to GDI
        bool result = _svc.Initialize();
        Assert.True(result);
    }

    [Fact]
    public void Initialize_CalledTwice_ReturnsTrueBothTimes()
    {
        Assert.True(_svc.Initialize());
        Assert.True(_svc.Initialize());
    }

    // ── AttachToWindow ────────────────────────────────────────────────────────

    [Fact]
    public void AttachToWindow_NonExistentTitle_ReturnsFalse()
    {
        bool result = _svc.AttachToWindow("__NonExistentGameWindow__12345");
        Assert.False(result);
    }

    [Fact]
    public void AttachToWindow_EmptyTitle_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.AttachToWindow(string.Empty));
        Assert.Null(ex);
    }

    // ── IsDxgiMode ────────────────────────────────────────────────────────────

    [Fact]
    public void IsDxgiMode_AfterInit_IsBoolean()
    {
        _svc.Initialize();
        // Just verify the property is accessible and doesn't throw
        _ = _svc.IsDxgiMode;
    }

    // ── Dispose ───────────────────────────────────────────────────────────────

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var svc2 = new DxgiFrameCaptureService(NullLogger<DxgiFrameCaptureService>.Instance);
        var ex = Record.Exception(() => svc2.Dispose());
        Assert.Null(ex);
    }

    [Fact]
    public void Dispose_CalledTwice_DoesNotThrow()
    {
        var svc2 = new DxgiFrameCaptureService(NullLogger<DxgiFrameCaptureService>.Instance);
        svc2.Dispose();
        var ex = Record.Exception(() => svc2.Dispose());
        Assert.Null(ex);
    }

    public void Dispose() => _svc.Dispose();
}
