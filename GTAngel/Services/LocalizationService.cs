using System.IO;
using System.Text.Json;
using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Services;

/// <summary>
/// Localization service.
/// Translated from: rockstarmobile/Lang
/// Loads language JSON files from Assets/Lang/ directory.
/// 
/// Original Android flow:
///   Lang.setLocalePriority(locale) → set active locale
///   Lang.get(key) → get localized string
///   Lang.getWithDefault(key, default) → get with fallback
///   Language files: assets/rockstar/lang/lang_{locale}.json
/// </summary>
public class LocalizationService
{
    private readonly ILogger<LocalizationService> _logger;
    private Dictionary<string, string> _strings = new();
    private string _currentLocale = "en-US";

    public string CurrentLocale => _currentLocale;

    public LocalizationService(ILogger<LocalizationService> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Load language file.
    /// Replaces: Lang.setLocalePriority(locale) + loading from assets
    /// </summary>
    public void LoadLanguage(string locale)
    {
        try
        {
            var langPath = Path.Combine(
                AppDomain.CurrentDomain.BaseDirectory,
                "Assets", "Lang", $"lang_{locale}.json");

            if (File.Exists(langPath))
            {
                var json = File.ReadAllText(langPath);
                _strings = JsonSerializer.Deserialize<Dictionary<string, string>>(json)
                    ?? new Dictionary<string, string>();
                _currentLocale = locale;
                _logger.LogInformation("Language loaded: {Locale} ({Count} strings)", locale, _strings.Count);
            }
            else
            {
                _logger.LogWarning("Language file not found: {Path}", langPath);
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to load language {Locale}", locale);
        }
    }

    /// <summary>
    /// Get localized string by key.
    /// Replaces: Lang.get(key)
    /// </summary>
    public string Get(string key) =>
        _strings.TryGetValue(key, out var value) ? value : key;

    /// <summary>
    /// Get localized string with default fallback.
    /// Replaces: Lang.getWithDefault(key, default)
    /// </summary>
    public string GetWithDefault(string key, string defaultValue) =>
        _strings.TryGetValue(key, out var value) ? value : defaultValue;
}
