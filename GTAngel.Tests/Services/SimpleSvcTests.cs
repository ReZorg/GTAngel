using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for NotificationService, TelemetryService, and LicenseService.
/// These services are thin adapters; tests verify they do not throw and
/// behave correctly for their synchronous/async contracts.
/// </summary>
public class NotificationServiceTests
{
    private readonly NotificationService _svc =
        new(NullLogger<NotificationService>.Instance);

    [Fact]
    public void ShowNotification_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.ShowNotification("Title", "Message"));
        Assert.Null(ex);
    }

    [Fact]
    public void ShowDownloadComplete_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.ShowDownloadComplete());
        Assert.Null(ex);
    }

    [Fact]
    public void ShowDownloadFailed_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.ShowDownloadFailed());
        Assert.Null(ex);
    }

    [Fact]
    public void ShowNotification_EmptyTitle_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.ShowNotification(string.Empty, "Message"));
        Assert.Null(ex);
    }

    [Fact]
    public void ShowNotification_EmptyMessage_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.ShowNotification("Title", string.Empty));
        Assert.Null(ex);
    }
}

public class TelemetryServiceTests
{
    private readonly TelemetryService _svc =
        new(NullLogger<TelemetryService>.Instance);

    [Fact]
    public void TrackEvent_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.TrackEvent("test_event"));
        Assert.Null(ex);
    }

    [Fact]
    public void TrackEvent_WithProperties_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            _svc.TrackEvent("test_event", new Dictionary<string, object> { ["key"] = 42 }));
        Assert.Null(ex);
    }

    [Fact]
    public void TrackEvent_NullProperties_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.TrackEvent("test_event", null));
        Assert.Null(ex);
    }

    [Fact]
    public void TrackScreen_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.TrackScreen("MainWindow"));
        Assert.Null(ex);
    }

    [Fact]
    public void TrackException_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            _svc.TrackException(new InvalidOperationException("test"), "test_context"));
        Assert.Null(ex);
    }

    [Fact]
    public void TrackException_NullContext_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            _svc.TrackException(new Exception("oops"), null));
        Assert.Null(ex);
    }

    [Fact]
    public void Flush_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.Flush());
        Assert.Null(ex);
    }
}

public class LicenseServiceTests
{
    private readonly LicenseService _svc =
        new(NullLogger<LicenseService>.Instance);

    [Fact]
    public async Task ValidateLicenseAsync_ReturnsTrue()
    {
        bool result = await _svc.ValidateLicenseAsync();
        Assert.True(result);
    }

    [Fact]
    public async Task ValidateLicenseAsync_CalledMultipleTimes_AlwaysReturnsTrue()
    {
        for (int i = 0; i < 3; i++)
        {
            bool result = await _svc.ValidateLicenseAsync();
            Assert.True(result);
        }
    }
}
