using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.Logging;
using Microsoft.Win32;
using GTA3DE.Wpf.Services;
using GTA3DE.Wpf.Views;

namespace GTA3DE.Wpf.ViewModels;

/// <summary>
/// Settings page view model.
/// Translated from: FlutterOptionsWithProductBaseScreen + GCAudioLogic + Lang.setLocalePriority
/// Manages graphics, audio, controls, language, and account settings.
/// </summary>
public partial class SettingsViewModel : ObservableObject
{
    private readonly ILogger<SettingsViewModel> _logger;
    private readonly AudioService _audio;
    private readonly LocalizationService _localization;
    private readonly AppStateService _state;
    private readonly NavigationService _navigation;
    private readonly AppConfiguration _config;

    // Graphics (replaces UE4 graphics quality settings)
    [ObservableProperty] private int _selectedResolutionIndex = 0;
    [ObservableProperty] private int _selectedQualityIndex = 2;
    [ObservableProperty] private bool _isVSyncEnabled = true;
    [ObservableProperty] private bool _isFullscreen = true;

    // Audio (replaces GCAudioLogic volume controls)
    [ObservableProperty] private double _masterVolume = 80;
    [ObservableProperty] private double _musicVolume = 70;
    [ObservableProperty] private double _sfxVolume = 80;

    // Controls
    [ObservableProperty] private double _mouseSensitivity = 50;
    [ObservableProperty] private bool _isInvertY;

    // Language (replaces Lang.setLocalePriority)
    [ObservableProperty] private string _selectedLanguage = "English";

    // Account
    [ObservableProperty] private string _accountStatus = "Not signed in";

    // ── KSM Cycle 2: UE5 Build & Asset Integration (P3 Boundaries, P10 Roughness) ──
    // Engine path is now user-configurable, persisted via AppConfiguration
    [ObservableProperty] private string _ue5EnginePath = @"E:\u9n\UnrealEngine";
    [ObservableProperty] private string _ue5EngineStatus = "Not validated";
    [ObservableProperty] private bool _ue5EngineFound;

    public ObservableCollection<string> AvailableLanguages { get; } = new()
    {
        "English", "German", "Spanish", "French", "Italian",
        "Japanese", "Korean", "Portuguese", "Russian", "Chinese (Simplified)",
        "Chinese (Traditional)", "Polish", "Dutch", "Swedish", "Turkish",
        "Arabic", "Hindi", "Finnish", "Estonian", "Hungarian",
        "Indonesian", "Malay", "Thai", "Ukrainian", "Vietnamese", "Hebrew"
    };

    public SettingsViewModel(
        ILogger<SettingsViewModel> logger,
        AudioService audio,
        LocalizationService localization,
        AppStateService state,
        NavigationService navigation,
        AppConfiguration config)
    {
        _logger = logger;
        _audio = audio;
        _localization = localization;
        _state = state;
        _navigation = navigation;
        _config = config;

        // Load current account status
        var user = _state.CurrentUser;
        AccountStatus = user != null
            ? $"Signed in as: {user.DisplayName} ({user.RockstarId})"
            : "Not signed in";

        // Load persisted engine path
        Ue5EnginePath = _config.Ue5EnginePath;
        ValidateEnginePath(Ue5EnginePath);
    }

    partial void OnUe5EnginePathChanged(string value)
    {
        _config.Ue5EnginePath = value;
        ValidateEnginePath(value);
    }

    private void ValidateEnginePath(string path)
    {
        var editorExe = System.IO.Path.Combine(path, @"Engine\Binaries\Win64\UnrealEditor.exe");
        Ue5EngineFound = System.IO.File.Exists(editorExe);
        Ue5EngineStatus = Ue5EngineFound
            ? $"✓ Engine found: {path}"
            : $"✗ UnrealEditor.exe not found at: {editorExe}";
    }

    /// <summary>Browse for UE5 engine installation folder.</summary>
    [RelayCommand]
    private void BrowseEnginePath()
    {
        var dlg = new OpenFolderDialog
        {
            Title = "Select Unreal Engine 5 Installation Folder",
            InitialDirectory = System.IO.Directory.Exists(Ue5EnginePath)
                ? Ue5EnginePath
                : @"E:\u9n",
        };
        if (dlg.ShowDialog() == true)
        {
            Ue5EnginePath = dlg.FolderName;
            _logger.LogInformation("UE5 engine path set to: {Path}", Ue5EnginePath);
        }
    }

    partial void OnMasterVolumeChanged(double value) =>
        _audio.SetMasterVolume((float)(value / 100.0));

    partial void OnMusicVolumeChanged(double value) =>
        _audio.SetMusicVolume((float)(value / 100.0));

    partial void OnSfxVolumeChanged(double value) =>
        _audio.SetSfxVolume((float)(value / 100.0));

    partial void OnSelectedLanguageChanged(string value)
    {
        // Map display name to locale code (replaces Lang.setLocalePriority)
        var localeMap = new Dictionary<string, string>
        {
            ["English"] = "en-US", ["German"] = "de-DE", ["Spanish"] = "es-ES",
            ["French"] = "fr-FR", ["Italian"] = "it-IT", ["Japanese"] = "ja-JP",
            ["Korean"] = "ko-KR", ["Portuguese"] = "pt-BR", ["Russian"] = "ru-RU",
            ["Chinese (Simplified)"] = "zh-CN", ["Chinese (Traditional)"] = "zh-TW",
            ["Polish"] = "pl-PL"
        };

        if (localeMap.TryGetValue(value, out var locale))
        {
            _localization.LoadLanguage(locale);
        }
    }

    /// <summary>
    /// Replaces: Rockstar.login() / Rockstar.logout() toggle
    /// </summary>
    [RelayCommand]
    private void ToggleLogin()
    {
        if (_state.CurrentUser != null)
        {
            // Logout (replaces Rockstar.logout())
            _state.Logout();
            AccountStatus = "Not signed in";
        }
        else
        {
            // Navigate to login
            _navigation.NavigateTo<LoginPage>();
        }
    }

    /// <summary>
    /// Replaces: GTAPlus.deleteAccount() → Rockstar.startAccountDeletion()
    /// </summary>
    [RelayCommand]
    private void DeleteAccount()
    {
        _logger.LogWarning("Account deletion requested");
        // In production, this would show a confirmation dialog and call the API
    }
}
