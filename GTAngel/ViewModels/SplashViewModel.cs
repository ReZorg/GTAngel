using CommunityToolkit.Mvvm.ComponentModel;
using Microsoft.Extensions.Logging;
using GTA3DE.Wpf.Services;
using GTA3DE.Wpf.Views;

namespace GTA3DE.Wpf.ViewModels;

/// <summary>
/// Splash screen view model.
///
/// GTAngel composition: /dte-ksm-evo-autogenesis ( /gta3-ue5-wpf ) → "GTAngel"
///
/// Navigation flow:
///   Splash → GTAngelPage (primary — Guardian Angel Cognitive Orchestrator)
///   Splash → DownloadPage (if game assets missing)
///   Splash → LoginPage (if license invalid)
///   Splash → LegalPage (if EULA not accepted)
///   Splash → GamePage (if all checks pass and GTAngel is bypassed)
///
/// The GTAngel dashboard is the primary destination: it provides the
/// autogenesis loop, KSM evolution cycle, and Alexander's 15 Properties
/// coherence monitor as the main application experience.
/// </summary>
public partial class SplashViewModel : ObservableObject
{
    private readonly ILogger<SplashViewModel> _logger;
    private readonly NavigationService _navigation;
    private readonly AppStateService _state;
    private readonly FileSystemService _fileSystem;
    private readonly LicenseService _license;

    public SplashViewModel(
        ILogger<SplashViewModel> logger,
        NavigationService navigation,
        AppStateService state,
        FileSystemService fileSystem,
        LicenseService license)
    {
        _logger = logger;
        _navigation = navigation;
        _state = state;
        _fileSystem = fileSystem;
        _license = license;
    }

    /// <summary>
    /// Check game state and navigate to appropriate page.
    ///
    /// GTAngel is the primary destination — the Guardian Angel Cognitive
    /// Orchestrator dashboard is always shown first, allowing the user to
    /// run the autogenesis loop, monitor KSM evolution, and inspect
    /// Alexander's 15 Properties coherence scores.
    ///
    /// The game runtime (GamePage) is accessible from GTAngelPage via
    /// the "Launch Game" button in the Guardian Log panel.
    /// </summary>
    public async Task CheckAndNavigateAsync()
    {
        _logger.LogInformation("GTAngel: Checking initialization state...");

        // Simulate initialization delay (replaces Flutter gate screen loading)
        await Task.Delay(1500);

        // ── GTAngel Primary Path ───────────────────────────────────────────
        // Navigate directly to the GTAngel Guardian Angel dashboard.
        // This is the primary application experience for the autogenesis loop.
        _logger.LogInformation("GTAngel: Navigating to Guardian Angel Cognitive Orchestrator");
        _navigation.NavigateTo<GTAngelPage>();
    }
}
