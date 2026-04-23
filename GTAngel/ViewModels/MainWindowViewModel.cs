using CommunityToolkit.Mvvm.ComponentModel;
using Microsoft.Extensions.Logging;
using GTAngel.Services;

namespace GTAngel.ViewModels;

/// <summary>
/// Main window view model.
/// Translated from: GameActivity lifecycle management + Rockstar.setup() initialization.
/// Manages app-wide state, audio pause/resume, and cleanup.
/// </summary>
public partial class MainWindowViewModel : ObservableObject
{
    private readonly ILogger<MainWindowViewModel> _logger;
    private readonly AppConfiguration _config;
    private readonly AppStateService _state;
    private readonly AudioService _audio;
    private readonly TelemetryService _telemetry;

    public MainWindowViewModel(
        ILogger<MainWindowViewModel> logger,
        AppConfiguration config,
        AppStateService state,
        AudioService audio,
        TelemetryService telemetry)
    {
        _logger = logger;
        _config = config;
        _state = state;
        _audio = audio;
        _telemetry = telemetry;
    }

    /// <summary>
    /// Initialize all services.
    /// Replaces: Rockstar.setup() which initializes Firebase, Sentry, audio, subscriptions, etc.
    /// </summary>
    public async Task InitializeAsync()
    {
        _logger.LogInformation("Initializing GTA III services");

        // Track launch event (replaces Rockstar events().track("Game Launched"))
        _telemetry.TrackEvent("GameLaunched", new Dictionary<string, object>
        {
            ["Game"] = _config.General?.Name ?? "GTA III",
            ["Version"] = Models.BuildConfig.VersionName
        });

        // Initialize audio (replaces GCAudioLogic setup)
        _audio.Initialize();

        _logger.LogInformation("GTA III services initialized");
        await Task.CompletedTask;
    }

    /// <summary>
    /// Replaces: GameActivity.onPause() → audio pause
    /// </summary>
    public void OnPause()
    {
        _logger.LogDebug("App paused");
        _audio.Pause();
    }

    /// <summary>
    /// Replaces: GameActivity.onResume() → audio resume
    /// </summary>
    public void OnResume()
    {
        _logger.LogDebug("App resumed");
        _audio.Resume();
    }

    /// <summary>
    /// Replaces: GameActivity.onDestroy() cleanup
    /// </summary>
    public void Cleanup()
    {
        _logger.LogInformation("Cleaning up");
        _audio.Dispose();
        _telemetry.Flush();
    }
}
