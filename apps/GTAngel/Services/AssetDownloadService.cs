using System.Net.Http;
using Microsoft.Extensions.Logging;
using GTAngel.Models;

namespace GTAngel.Services;

/// <summary>
/// Asset download service.
/// Translated from: Google APK Expansion Downloader Library + DownloaderActivity
/// Manages downloading game data files (replaces OBB download from Google Play).
/// 
/// Original Android flow:
///   DownloaderClientMarshaller.CreateStub() → create download client
///   DownloaderService.startDownloadServiceIfRequired() → check and start download
///   IDownloaderClient.onDownloadStateChanged(state) → state callback
///   IDownloaderClient.onDownloadProgress(progress) → progress callback
///   IStub.connect(context) → connect to download service
///   IStub.disconnect(context) → disconnect from download service
/// </summary>
public class AssetDownloadService
{
    private readonly ILogger<AssetDownloadService> _logger;
    private readonly FileSystemService _fileSystem;
    private readonly NotificationService _notifications;
    private readonly HttpClient _httpClient;
    private CancellationTokenSource? _cancellationSource;
    private bool _isPaused;

    /// <summary>
    /// Progress callback (replaces IDownloaderClient.onDownloadProgress)
    /// </summary>
    public event Action<DownloadProgressInfo>? ProgressChanged;

    /// <summary>
    /// State change callback (replaces IDownloaderClient.onDownloadStateChanged)
    /// </summary>
    public event Action<int>? StateChanged;

    public AssetDownloadService(
        ILogger<AssetDownloadService> logger,
        FileSystemService fileSystem,
        NotificationService notifications)
    {
        _logger = logger;
        _fileSystem = fileSystem;
        _notifications = notifications;
        _httpClient = new HttpClient();
    }

    /// <summary>
    /// Start downloading game assets.
    /// Replaces: DownloaderService.startDownloadServiceIfRequired()
    /// </summary>
    public async Task StartDownloadAsync()
    {
        _cancellationSource = new CancellationTokenSource();
        _logger.LogInformation("Starting asset download to {Path}", _fileSystem.GameDataPath);

        StateChanged?.Invoke(1); // DOWNLOADING

        // In production, this would download from Rockstar CDN
        // Simulating download progress for demonstration
        var totalSize = 507_000_000L; // ~507MB OBB equivalent
        var downloaded = 0L;
        var startTime = DateTime.UtcNow;

        try
        {
        while (downloaded < totalSize && !_cancellationSource.Token.IsCancellationRequested)
        {
            if (_isPaused)
            {
                await Task.Delay(100, _cancellationSource.Token);
                continue;
            }

            // Simulate download chunk
            var chunkSize = Math.Min(1_000_000, totalSize - downloaded); // 1MB chunks
            downloaded += chunkSize;

            var elapsed = (DateTime.UtcNow - startTime).TotalSeconds;
            var speed = elapsed > 0 ? downloaded / elapsed : 0;
            var remaining = speed > 0 ? (totalSize - downloaded) / speed * 1000 : 0;

            ProgressChanged?.Invoke(new DownloadProgressInfo
            {
                OverallTotal = totalSize,
                OverallProgress = downloaded,
                CurrentSpeed = (float)speed,
                TimeRemaining = (long)remaining
            });

            await Task.Delay(50, _cancellationSource.Token);
        }
        }
        catch (OperationCanceledException) { }

        if (!_cancellationSource.Token.IsCancellationRequested)
        {
            StateChanged?.Invoke(4); // DOWNLOAD_COMPLETED
            _notifications.ShowDownloadComplete();
            _logger.LogInformation("Asset download completed");
        }
    }

    /// <summary>Replaces: IDownloaderService.requestPauseDownload()</summary>
    public void PauseDownload()
    {
        _isPaused = true;
        _logger.LogInformation("Download paused");
    }

    /// <summary>Replaces: IDownloaderService.requestContinueDownload()</summary>
    public void ResumeDownload()
    {
        _isPaused = false;
        _logger.LogInformation("Download resumed");
    }

    /// <summary>Replaces: IDownloaderService.requestAbortDownload()</summary>
    public void CancelDownload()
    {
        _cancellationSource?.Cancel();
        _logger.LogInformation("Download cancelled");
    }

    /// <summary>Replaces: cellular approval → resume over metered connection</summary>
    public void ResumeOverCellular()
    {
        _isPaused = false;
        _logger.LogInformation("Download resumed over cellular/metered connection");
    }
}
