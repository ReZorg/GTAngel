namespace GTAngel.Models;

/// <summary>
/// Download progress information.
/// Translated from: com.google.android.vending.expansion.downloader.DownloadProgressInfo
/// Used by DownloaderActivity.onDownloadProgress()
/// </summary>
public class DownloadProgressInfo
{
    /// <summary>Total bytes to download (replaces mOverallTotal)</summary>
    public long OverallTotal { get; set; }

    /// <summary>Bytes downloaded so far (replaces mOverallProgress)</summary>
    public long OverallProgress { get; set; }

    /// <summary>Current download speed in bytes/sec (replaces mCurrentSpeed)</summary>
    public float CurrentSpeed { get; set; }

    /// <summary>Estimated time remaining in ms (replaces mTimeRemaining)</summary>
    public long TimeRemaining { get; set; }

    /// <summary>Progress as percentage (0-100)</summary>
    public double ProgressPercent =>
        OverallTotal > 0 ? (double)OverallProgress / OverallTotal * 100.0 : 0;

    /// <summary>Format speed as human-readable string (replaces Helpers.getSpeedString)</summary>
    public string SpeedString
    {
        get
        {
            if (CurrentSpeed < 1024)
                return $"{CurrentSpeed:F0} B/s";
            if (CurrentSpeed < 1024 * 1024)
                return $"{CurrentSpeed / 1024:F1} KB/s";
            return $"{CurrentSpeed / (1024 * 1024):F1} MB/s";
        }
    }

    /// <summary>Format time remaining as human-readable string (replaces Helpers.getTimeRemaining)</summary>
    public string TimeRemainingString
    {
        get
        {
            var ts = TimeSpan.FromMilliseconds(TimeRemaining);
            if (ts.TotalHours >= 1)
                return $"{ts.Hours}h {ts.Minutes}m";
            if (ts.TotalMinutes >= 1)
                return $"{ts.Minutes}m {ts.Seconds}s";
            return $"{ts.Seconds}s";
        }
    }

    /// <summary>Format progress as fraction string (replaces Helpers.getDownloadProgressString)</summary>
    public string ProgressFractionString
    {
        get
        {
            static string FormatSize(long bytes)
            {
                if (bytes < 1024) return $"{bytes} B";
                if (bytes < 1024 * 1024) return $"{bytes / 1024.0:F1} KB";
                if (bytes < 1024L * 1024 * 1024) return $"{bytes / (1024.0 * 1024):F1} MB";
                return $"{bytes / (1024.0 * 1024 * 1024):F2} GB";
            }
            return $"{FormatSize(OverallProgress)} / {FormatSize(OverallTotal)}";
        }
    }
}
