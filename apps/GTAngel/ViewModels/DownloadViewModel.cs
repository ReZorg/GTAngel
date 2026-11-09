using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.Logging;
using GTAngel.Models;
using GTAngel.Services;
using GTAngel.Views;

namespace GTAngel.ViewModels;

/// <summary>
/// Download page view model.
/// Translated from: DownloaderActivity (com.rockstargames.gta3.p011de.DownloaderActivity)
/// Manages OBB/asset download with progress, pause/resume, and cellular approval.
/// 
/// Original Android flow:
///   onCreate → check if files present → start download service
///   onDownloadStateChanged → update UI state (dashboard/cellular panels)
///   onDownloadProgress → update progress bar, speed, time remaining
///   validateXAPKZipFiles → verify downloaded files
/// </summary>
public partial class DownloadViewModel : ObservableObject
{
    private readonly ILogger<DownloadViewModel> _logger;
    private readonly AssetDownloadService _downloadService;
    private readonly NavigationService _navigation;
    private readonly FileSystemService _fileSystem;
    private bool _isPaused;

    [ObservableProperty]
    private string _statusText = "Preparing download...";

    [ObservableProperty]
    private string _progressFraction = "0 B / 0 B";

    [ObservableProperty]
    private string _progressPercent = "0%";

    [ObservableProperty]
    private double _progressValue;

    [ObservableProperty]
    private string _averageSpeed = "0 KB/s";

    [ObservableProperty]
    private string _timeRemaining = "Calculating...";

    [ObservableProperty]
    private string _pauseButtonText = "Pause Download";

    [ObservableProperty]
    private bool _isDashboardVisible = true;

    [ObservableProperty]
    private bool _isCancelVisible;

    [ObservableProperty]
    private bool _isCellularApprovalVisible;

    public DownloadViewModel(
        ILogger<DownloadViewModel> logger,
        AssetDownloadService downloadService,
        NavigationService navigation,
        FileSystemService fileSystem)
    {
        _logger = logger;
        _downloadService = downloadService;
        _navigation = navigation;
        _fileSystem = fileSystem;

        // Subscribe to download progress (replaces IDownloaderClient callbacks)
        _downloadService.ProgressChanged += OnDownloadProgress;
        _downloadService.StateChanged += OnDownloadStateChanged;

        StartDownload();
    }

    private async void StartDownload()
    {
        _logger.LogInformation("Starting asset download");
        StatusText = "Downloading game data...";

        try
        {
            await _downloadService.StartDownloadAsync();
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Download failed");
            StatusText = "Download failed. Please try again.";
            IsCancelVisible = true;
        }
    }

    /// <summary>
    /// Replaces: DownloaderActivity.onDownloadProgress(DownloadProgressInfo)
    /// </summary>
    private void OnDownloadProgress(DownloadProgressInfo info)
    {
        ProgressValue = info.ProgressPercent;
        ProgressPercent = $"{info.ProgressPercent:F0}%";
        ProgressFraction = info.ProgressFractionString;
        AverageSpeed = info.SpeedString;
        TimeRemaining = $"Time remaining: {info.TimeRemainingString}";
    }

    /// <summary>
    /// Replaces: DownloaderActivity.onDownloadStateChanged(int)
    /// Maps Android download states to WPF UI states.
    /// </summary>
    private void OnDownloadStateChanged(int state)
    {
        // State mapping from Android DownloaderClientMarshaller:
        // 1-3: downloading states → show dashboard
        // 4: completed → validate and navigate
        // 5: validation → validateXAPKZipFiles
        // 6-19: various error/pause states
        switch (state)
        {
            case 4: // DOWNLOAD_COMPLETED
                StatusText = "Download complete! Verifying...";
                ValidateAndNavigate();
                break;
            case 5: // DOWNLOAD_VALIDATING
                StatusText = "Validating game files...";
                break;
            case 15: // DOWNLOAD_PAUSED_NEED_CELLULAR
            case 16:
                IsDashboardVisible = false;
                IsCellularApprovalVisible = true;
                break;
            default:
                IsDashboardVisible = true;
                IsCellularApprovalVisible = false;
                break;
        }
    }

    private async void ValidateAndNavigate()
    {
        // Replaces: DownloaderActivity.validateXAPKZipFiles()
        await Task.Delay(1000);
        _navigation.NavigateTo<GamePage>();
    }

    /// <summary>
    /// Replaces: pauseButton click handler
    /// </summary>
    [RelayCommand]
    private void Pause()
    {
        _isPaused = !_isPaused;
        PauseButtonText = _isPaused ? "Resume Download" : "Pause Download";

        if (_isPaused)
            _downloadService.PauseDownload();
        else
            _downloadService.ResumeDownload();
    }

    /// <summary>
    /// Replaces: cancelButton click handler
    /// </summary>
    [RelayCommand]
    private void Cancel()
    {
        _downloadService.CancelDownload();
        _navigation.GoBack();
    }

    /// <summary>
    /// Replaces: resumeOverCellular button click
    /// </summary>
    [RelayCommand]
    private void ResumeCellular()
    {
        IsCellularApprovalVisible = false;
        IsDashboardVisible = true;
        _downloadService.ResumeOverCellular();
    }

    /// <summary>
    /// Replaces: wifiSettingsButton click → open network settings
    /// </summary>
    [RelayCommand]
    private void WifiSettings()
    {
        // On Windows, open network settings
        System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo
        {
            FileName = "ms-settings:network",
            UseShellExecute = true
        });
    }
}
