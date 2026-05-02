using GTAngel.Services;
using GTAngel.Models;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for AssetDownloadService — events, state transitions, cancel, pause/resume.
/// </summary>
public class AssetDownloadServiceTests
{
    private static AssetDownloadService CreateService()
    {
        var fs = new FileSystemService(NullLogger<FileSystemService>.Instance);
        var ns = new NotificationService(NullLogger<NotificationService>.Instance);
        return new AssetDownloadService(
            NullLogger<AssetDownloadService>.Instance, fs, ns);
    }

    [Fact]
    public void PauseDownload_DoesNotThrow()
    {
        var svc = CreateService();
        var ex = Record.Exception(() => svc.PauseDownload());
        Assert.Null(ex);
    }

    [Fact]
    public void ResumeDownload_DoesNotThrow()
    {
        var svc = CreateService();
        var ex = Record.Exception(() => svc.ResumeDownload());
        Assert.Null(ex);
    }

    [Fact]
    public void CancelDownload_BeforeStart_DoesNotThrow()
    {
        var svc = CreateService();
        var ex = Record.Exception(() => svc.CancelDownload());
        Assert.Null(ex);
    }

    [Fact]
    public void ResumeOverCellular_DoesNotThrow()
    {
        var svc = CreateService();
        var ex = Record.Exception(() => svc.ResumeOverCellular());
        Assert.Null(ex);
    }

    [Fact]
    public async Task StartDownloadAsync_CanBeCancelledImmediately()
    {
        var svc = CreateService();

        int stateChanges = 0;
        svc.StateChanged += _ => stateChanges++;

        var downloadTask = svc.StartDownloadAsync();

        // Cancel shortly after start
        await Task.Delay(50);
        svc.CancelDownload();

        var ex = await Record.ExceptionAsync(() => downloadTask);
        Assert.Null(ex);
    }

    [Fact]
    public async Task StartDownloadAsync_EmitsProgressEvents()
    {
        var svc = CreateService();
        var progressEvents = new List<DownloadProgressInfo>();
        svc.ProgressChanged += p => progressEvents.Add(p);

        var downloadTask = svc.StartDownloadAsync();
        await Task.Delay(200);
        svc.CancelDownload();

        await Record.ExceptionAsync(() => downloadTask);

        // Should have received at least one progress update before cancellation
        Assert.True(progressEvents.Count >= 1);
    }

    [Fact]
    public async Task StartDownloadAsync_ProgressOverallProgressIsPositive()
    {
        var svc = CreateService();
        DownloadProgressInfo? lastProgress = null;
        svc.ProgressChanged += p => lastProgress = p;

        var downloadTask = svc.StartDownloadAsync();
        await Task.Delay(200);
        svc.CancelDownload();

        await Record.ExceptionAsync(() => downloadTask);

        if (lastProgress != null)
            Assert.True(lastProgress.OverallProgress > 0);
    }

    [Fact]
    public async Task StartDownloadAsync_EmitsDownloadingState()
    {
        var svc = CreateService();
        var states = new List<int>();
        svc.StateChanged += s => states.Add(s);

        var downloadTask = svc.StartDownloadAsync();
        await Task.Delay(100);
        svc.CancelDownload();

        await Record.ExceptionAsync(() => downloadTask);

        Assert.Contains(1, states); // 1 = DOWNLOADING
    }

    [Fact]
    public void PauseThenResume_DoesNotThrow()
    {
        var svc = CreateService();
        svc.PauseDownload();
        svc.ResumeDownload();
        // No exception expected
    }
}
