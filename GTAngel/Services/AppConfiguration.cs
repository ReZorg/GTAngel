using System.IO;
using System.Text.Json;
using Microsoft.Extensions.Logging;
using GTAngel.Models;

namespace GTAngel.Services;

/// <summary>
/// Application configuration service.
/// Translated from: rockstarmobile/GCConfig
/// Loads and manages SDK configuration from Assets/Config/SDK.config JSON file.
/// 
/// Original Android flow:
///   GCConfig.C = new GCConfig(context) → reads SDK.config from assets
///   GCConfig.general → general app config
///   GCConfig.games → other Rockstar games catalog
///   GCConfig.gates → gate/trial configuration
///   GCConfig.googleplay → Google Play billing config
/// </summary>
public class AppConfiguration
{
    private readonly ILogger<AppConfiguration> _logger;

    public SdkConfig? SdkConfig { get; private set; }
    public GeneralConfig? General => SdkConfig?.General;

    // ── KSM Cycle 2: UE5 Build & Asset Integration ────────────────────────────
    // Configurable engine path (P3 Boundaries, P10 Roughness)
    private const string UserSettingsFile = "gtangel_settings.json";
    private UserSettings _userSettings = new();

    /// <summary>Path to the UE5 engine installation. Configurable by the user.</summary>
    public string Ue5EnginePath
    {
        get => _userSettings.Ue5EnginePath ?? @"E:\u9n\UnrealEngine";
        set
        {
            _userSettings.Ue5EnginePath = value;
            _ = SaveUserSettingsAsync();
        }
    }

    public AppConfiguration(ILogger<AppConfiguration> logger)
    {
        _logger = logger;
    }

    /// <summary>Load persisted user settings (engine path etc.).</summary>
    public async Task LoadUserSettingsAsync()
    {
        try
        {
            var path = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                "GTAngel", UserSettingsFile);
            if (File.Exists(path))
            {
                var json = await File.ReadAllTextAsync(path);
                _userSettings = JsonSerializer.Deserialize<UserSettings>(json) ?? new();
                _logger.LogInformation("User settings loaded: EnginePath={Path}", Ue5EnginePath);
            }
        }
        catch (Exception ex) { _logger.LogWarning(ex, "Could not load user settings"); }
    }

    private async Task SaveUserSettingsAsync()
    {
        try
        {
            var dir  = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "GTAngel");
            Directory.CreateDirectory(dir);
            var path = Path.Combine(dir, UserSettingsFile);
            var json = JsonSerializer.Serialize(_userSettings, new JsonSerializerOptions { WriteIndented = true });
            await File.WriteAllTextAsync(path, json);
        }
        catch (Exception ex) { _logger.LogWarning(ex, "Could not save user settings"); }
    }

    /// <summary>
    /// Load SDK configuration from JSON file.
    /// Replaces: GCConfig constructor reading from Android assets.
    /// </summary>
    public async Task LoadAsync()
    {
        try
        {
            var configPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Assets", "Config", "SDK.config");

            if (File.Exists(configPath))
            {
                var json = await File.ReadAllTextAsync(configPath);
                SdkConfig = JsonSerializer.Deserialize<SdkConfig>(json, new JsonSerializerOptions
                {
                    PropertyNameCaseInsensitive = true
                });
                _logger.LogInformation("SDK config loaded: {Name}", General?.Name ?? "unknown");
            }
            else
            {
                _logger.LogWarning("SDK.config not found at {Path}", configPath);
                SdkConfig = new SdkConfig
                {
                    General = new GeneralConfig
                    {
                        Name = "GTA III - Definitive",
                        ShortName = "GTA III",
                        Slug = "gta3de"
                    }
                };
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to load SDK config");
            SdkConfig = new SdkConfig();
        }
    }
}

/// <summary>Persisted user preferences (separate from SDK config).</summary>
public sealed class UserSettings
{
    public string? Ue5EnginePath { get; set; }
}
