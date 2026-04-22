using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Services;

/// <summary>
/// Notification service.
/// Translated from: Android NotificationManager + DownloaderActivity notification handling
/// 
/// Original Android notifications:
///   Download complete notification (R.string.notification_download_complete)
///   Download failed notification (R.string.notification_download_failed)
///   Firebase push notifications (Rockstar.pushNotifications())
/// 
/// WPF: Uses Windows Toast Notifications via Windows.UI.Notifications
/// </summary>
public class NotificationService
{
    private readonly ILogger<NotificationService> _logger;

    public NotificationService(ILogger<NotificationService> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Show a toast notification.
    /// Replaces: NotificationManager.notify()
    /// </summary>
    public void ShowNotification(string title, string message)
    {
        _logger.LogInformation("Notification: {Title} - {Message}", title, message);
        // In production, use Microsoft.Toolkit.Uwp.Notifications for toast notifications
    }

    /// <summary>
    /// Show download complete notification.
    /// Replaces: notification_download_complete string resource notification
    /// </summary>
    public void ShowDownloadComplete()
    {
        ShowNotification("GTA III - Definitive Edition", "Download complete");
    }

    /// <summary>
    /// Show download failed notification.
    /// Replaces: notification_download_failed string resource notification
    /// </summary>
    public void ShowDownloadFailed()
    {
        ShowNotification("GTA III - Definitive Edition", "Download unsuccessful");
    }
}
