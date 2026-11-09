using GTAngel.Models;
using Xunit;

namespace GTAngel.Tests.Models;

/// <summary>
/// Tests for DownloadProgressInfo computed properties:
/// ProgressPercent, SpeedString, TimeRemainingString, ProgressFractionString.
/// </summary>
public class DownloadProgressInfoTests
{
    // ── ProgressPercent ────────────────────────────────────────────────────

    [Fact]
    public void ProgressPercent_ZeroTotal_ReturnsZero()
    {
        var info = new DownloadProgressInfo { OverallTotal = 0, OverallProgress = 100 };
        Assert.Equal(0.0, info.ProgressPercent);
    }

    [Fact]
    public void ProgressPercent_ZeroProgress_ReturnsZero()
    {
        var info = new DownloadProgressInfo { OverallTotal = 1000, OverallProgress = 0 };
        Assert.Equal(0.0, info.ProgressPercent);
    }

    [Fact]
    public void ProgressPercent_Halfway_ReturnsFifty()
    {
        var info = new DownloadProgressInfo { OverallTotal = 1000, OverallProgress = 500 };
        Assert.Equal(50.0, info.ProgressPercent, precision: 10);
    }

    [Fact]
    public void ProgressPercent_Complete_ReturnsOneHundred()
    {
        var info = new DownloadProgressInfo { OverallTotal = 1000, OverallProgress = 1000 };
        Assert.Equal(100.0, info.ProgressPercent, precision: 10);
    }

    [Fact]
    public void ProgressPercent_OneByteProgress_IsCorrect()
    {
        var info = new DownloadProgressInfo { OverallTotal = 100, OverallProgress = 1 };
        Assert.Equal(1.0, info.ProgressPercent, precision: 10);
    }

    // ── SpeedString ────────────────────────────────────────────────────────

    [Fact]
    public void SpeedString_BelowOnKb_ReturnsBytesPerSec()
    {
        var info = new DownloadProgressInfo { CurrentSpeed = 512f };
        Assert.Equal("512 B/s", info.SpeedString);
    }

    [Fact]
    public void SpeedString_ZeroSpeed_ReturnsBytesPerSec()
    {
        var info = new DownloadProgressInfo { CurrentSpeed = 0f };
        Assert.Equal("0 B/s", info.SpeedString);
    }

    [Fact]
    public void SpeedString_ExactlyOneKb_ReturnsKBPerSec()
    {
        var info = new DownloadProgressInfo { CurrentSpeed = 1024f };
        Assert.Equal("1.0 KB/s", info.SpeedString);
    }

    [Fact]
    public void SpeedString_KilobyteRange_ReturnsKBPerSec()
    {
        var info = new DownloadProgressInfo { CurrentSpeed = 2048f }; // 2 KB/s
        Assert.Equal("2.0 KB/s", info.SpeedString);
    }

    [Fact]
    public void SpeedString_MegabyteRange_ReturnsMBPerSec()
    {
        var info = new DownloadProgressInfo { CurrentSpeed = 1024 * 1024f }; // 1 MB/s
        Assert.Equal("1.0 MB/s", info.SpeedString);
    }

    [Fact]
    public void SpeedString_LargeSpeed_ReturnsMBPerSec()
    {
        var info = new DownloadProgressInfo { CurrentSpeed = 5 * 1024 * 1024f }; // 5 MB/s
        Assert.Equal("5.0 MB/s", info.SpeedString);
    }

    // ── TimeRemainingString ────────────────────────────────────────────────

    [Fact]
    public void TimeRemainingString_Seconds_ReturnsSecondsOnly()
    {
        var info = new DownloadProgressInfo { TimeRemaining = 30_000 }; // 30 seconds
        Assert.Equal("30s", info.TimeRemainingString);
    }

    [Fact]
    public void TimeRemainingString_ZeroMs_ReturnsZeroSeconds()
    {
        var info = new DownloadProgressInfo { TimeRemaining = 0 };
        Assert.Equal("0s", info.TimeRemainingString);
    }

    [Fact]
    public void TimeRemainingString_OneMinute_ReturnsMinutesSeconds()
    {
        var info = new DownloadProgressInfo { TimeRemaining = 90_000 }; // 1m 30s
        Assert.Equal("1m 30s", info.TimeRemainingString);
    }

    [Fact]
    public void TimeRemainingString_ExactlyOneMinute_ReturnsMinutesSeconds()
    {
        var info = new DownloadProgressInfo { TimeRemaining = 60_000 }; // 1m 0s
        Assert.Equal("1m 0s", info.TimeRemainingString);
    }

    [Fact]
    public void TimeRemainingString_OneHour_ReturnsHoursMinutes()
    {
        var info = new DownloadProgressInfo { TimeRemaining = 3_600_000 }; // 1h 0m
        Assert.Equal("1h 0m", info.TimeRemainingString);
    }

    [Fact]
    public void TimeRemainingString_OneHour30Min_ReturnsHoursMinutes()
    {
        var info = new DownloadProgressInfo { TimeRemaining = 5_400_000 }; // 1h 30m
        Assert.Equal("1h 30m", info.TimeRemainingString);
    }

    // ── ProgressFractionString ─────────────────────────────────────────────

    [Fact]
    public void ProgressFractionString_BothBytes_ReturnsBFormat()
    {
        var info = new DownloadProgressInfo { OverallProgress = 100, OverallTotal = 500 };
        Assert.Equal("100 B / 500 B", info.ProgressFractionString);
    }

    [Fact]
    public void ProgressFractionString_KilobyteRange_ReturnsKBFormat()
    {
        var info = new DownloadProgressInfo { OverallProgress = 512 * 1024, OverallTotal = 1024 * 1024 };
        Assert.Equal("512.0 KB / 1.0 MB", info.ProgressFractionString);
    }

    [Fact]
    public void ProgressFractionString_MegabyteRange_ReturnsMBFormat()
    {
        var info = new DownloadProgressInfo
        {
            OverallProgress = 50L * 1024 * 1024,
            OverallTotal = 100L * 1024 * 1024
        };
        Assert.Equal("50.0 MB / 100.0 MB", info.ProgressFractionString);
    }

    [Fact]
    public void ProgressFractionString_GigabyteRange_ReturnsGBFormat()
    {
        var info = new DownloadProgressInfo
        {
            OverallProgress = 1L * 1024 * 1024 * 1024,
            OverallTotal = 2L * 1024 * 1024 * 1024
        };
        Assert.Contains("GB", info.ProgressFractionString);
    }

    [Fact]
    public void ProgressFractionString_ZeroProgress_ShowsZeroB()
    {
        var info = new DownloadProgressInfo { OverallProgress = 0, OverallTotal = 1000 };
        Assert.StartsWith("0 B", info.ProgressFractionString);
    }
}
