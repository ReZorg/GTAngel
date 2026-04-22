using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Services;

/// <summary>
/// Telemetry and analytics service.
/// Translated from: Rockstar.events() + Firebase Analytics + Sentry crash reporting
/// 
/// Original Android telemetry:
///   Rockstar.events().track(eventName, properties) → Firebase Analytics
///   Rockstar.events().trackScreen(screenName) → screen view tracking
///   Sentry.captureException(exception) → crash reporting
///   Firebase.setUserId(rockstarId) → user identification
/// </summary>
public class TelemetryService
{
    private readonly ILogger<TelemetryService> _logger;

    public TelemetryService(ILogger<TelemetryService> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Track an analytics event.
    /// Replaces: Rockstar.events().track(eventName, properties)
    /// </summary>
    public void TrackEvent(string eventName, Dictionary<string, object>? properties = null)
    {
        _logger.LogInformation("Telemetry event: {Event} {Properties}",
            eventName, properties != null ? System.Text.Json.JsonSerializer.Serialize(properties) : "");
        // In production, send to Application Insights, Sentry, or custom analytics
    }

    /// <summary>
    /// Track a screen view.
    /// Replaces: Rockstar.events().trackScreen(screenName)
    /// </summary>
    public void TrackScreen(string screenName)
    {
        _logger.LogInformation("Screen view: {Screen}", screenName);
    }

    /// <summary>
    /// Track an exception.
    /// Replaces: Sentry.captureException(exception)
    /// </summary>
    public void TrackException(Exception exception, string? context = null)
    {
        _logger.LogError(exception, "Exception tracked: {Context}", context ?? "unknown");
    }

    /// <summary>Flush pending telemetry events.</summary>
    public void Flush()
    {
        _logger.LogDebug("Telemetry flushed");
    }
}
