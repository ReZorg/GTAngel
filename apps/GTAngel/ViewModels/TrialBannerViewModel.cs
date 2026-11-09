using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.Logging;
using GTAngel.Services;

namespace GTAngel.ViewModels;

/// <summary>
/// Trial banner view model.
/// Translated from: rockstarmobile/p018ui/TrialBanner.java
/// Manages trial countdown timer, progress display, and unlock button.
/// 
/// Original Android logic:
///   TrialBanner.setHostActivity(activity) → register with activity
///   Timer updates caption text with remaining time
///   Progress bar width_percent updated via ConstraintLayout params
///   Unlock button triggers purchase flow
///   Close button (X) hides banner
/// </summary>
public partial class TrialBannerViewModel : ObservableObject
{
    private readonly ILogger<TrialBannerViewModel> _logger;
    private readonly SubscriptionService _subscriptions;
    private readonly AppStateService _state;
    private System.Timers.Timer? _countdownTimer;

    [ObservableProperty]
    private string _captionText = "Play for free for 30:00 minutes";

    [ObservableProperty]
    private double _progressPercent = 100;

    [ObservableProperty]
    private bool _isVisible;

    private TimeSpan _remainingTime = TimeSpan.FromMinutes(30);
    private readonly TimeSpan _totalTime = TimeSpan.FromMinutes(30);

    public TrialBannerViewModel(
        ILogger<TrialBannerViewModel> logger,
        SubscriptionService subscriptions,
        AppStateService state)
    {
        _logger = logger;
        _subscriptions = subscriptions;
        _state = state;
    }

    public void StartCountdown()
    {
        IsVisible = true;
        _countdownTimer = new System.Timers.Timer(1000);
        _countdownTimer.Elapsed += (_, _) => UpdateCountdown();
        _countdownTimer.Start();
    }

    private void UpdateCountdown()
    {
        _remainingTime -= TimeSpan.FromSeconds(1);

        if (_remainingTime <= TimeSpan.Zero)
        {
            _countdownTimer?.Stop();
            CaptionText = "Free trial has ended";
            ProgressPercent = 0;
            return;
        }

        CaptionText = $"Play for free for {_remainingTime.Minutes}:{_remainingTime.Seconds:D2} minutes";
        ProgressPercent = _remainingTime.TotalSeconds / _totalTime.TotalSeconds * 100;
    }

    /// <summary>
    /// Replaces: buttonUnlock click → purchase flow
    /// </summary>
    [RelayCommand]
    private async Task UnlockAsync()
    {
        _logger.LogInformation("Unlock button pressed - starting purchase flow");
        await _subscriptions.StartPurchaseFlowAsync();
    }

    /// <summary>
    /// Replaces: buttonX click → hide banner
    /// </summary>
    [RelayCommand]
    private void Close()
    {
        IsVisible = false;
        _countdownTimer?.Stop();
    }
}
