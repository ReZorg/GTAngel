using GTAngel.Models;
using Xunit;

namespace GTAngel.Tests.Models;

public class GameWorldAssetTests
{
    // ── SizeFormatted ──────────────────────────────────────────────────────

    [Fact]
    public void SizeFormatted_Bytes_ReturnsBytes()
    {
        var asset = new GameWorldAsset { Size = 512 };
        Assert.Equal("512 B", asset.SizeFormatted);
    }

    [Fact]
    public void SizeFormatted_ExactlyOneKb_ReturnsKb()
    {
        var asset = new GameWorldAsset { Size = 1024 };
        Assert.Equal("1.0 KB", asset.SizeFormatted);
    }

    [Fact]
    public void SizeFormatted_KilobyteRange_ReturnsKb()
    {
        var asset = new GameWorldAsset { Size = 2048 };
        Assert.Equal("2.0 KB", asset.SizeFormatted);
    }

    [Fact]
    public void SizeFormatted_MegabyteRange_ReturnsMb()
    {
        var asset = new GameWorldAsset { Size = 1024 * 1024 };
        Assert.Equal("1.0 MB", asset.SizeFormatted);
    }

    [Fact]
    public void SizeFormatted_GigabyteRange_ReturnsGb()
    {
        var asset = new GameWorldAsset { Size = 1024L * 1024 * 1024 };
        Assert.Equal("1.00 GB", asset.SizeFormatted);
    }

    [Fact]
    public void SizeFormatted_ZeroBytes_ReturnsZeroB()
    {
        var asset = new GameWorldAsset { Size = 0 };
        Assert.Equal("0 B", asset.SizeFormatted);
    }

    // ── AssetCatalogSummary.TotalSizeFormatted ─────────────────────────────

    [Fact]
    public void TotalSizeFormatted_BelowOneMb_ReturnsKb()
    {
        var summary = new AssetCatalogSummary { TotalSizeBytes = 512 * 1024 }; // 512 KB
        Assert.Contains("KB", summary.TotalSizeFormatted);
    }

    [Fact]
    public void TotalSizeFormatted_MegabyteRange_ReturnsMb()
    {
        var summary = new AssetCatalogSummary { TotalSizeBytes = 5L * 1024 * 1024 }; // 5 MB
        Assert.Equal("5.0 MB", summary.TotalSizeFormatted);
    }

    [Fact]
    public void TotalSizeFormatted_GigabyteRange_ReturnsGb()
    {
        var summary = new AssetCatalogSummary { TotalSizeBytes = 2L * 1024 * 1024 * 1024 };
        Assert.Equal("2.00 GB", summary.TotalSizeFormatted);
    }

    // ── AssetCatalogSummary defaults ────────────────────────────────────────

    [Fact]
    public void AssetCatalogSummary_DefaultDictionaries_AreNotNull()
    {
        var summary = new AssetCatalogSummary();
        Assert.NotNull(summary.CountByCategory);
        Assert.NotNull(summary.SizeByCategory);
        Assert.NotNull(summary.CountByExtension);
    }

    // ── GameWorldAsset defaults ─────────────────────────────────────────────

    [Fact]
    public void GameWorldAsset_DefaultFlags_AreFalse()
    {
        var asset = new GameWorldAsset();
        Assert.False(asset.IsMap);
        Assert.False(asset.IsBlueprint);
        Assert.False(asset.IsTexture);
        Assert.False(asset.IsAudio);
    }

    [Fact]
    public void GameWorldAsset_DefaultStrings_AreEmpty()
    {
        var asset = new GameWorldAsset();
        Assert.Equal(string.Empty, asset.FullPath);
        Assert.Equal(string.Empty, asset.FileName);
        Assert.Equal(string.Empty, asset.Extension);
        Assert.Equal(string.Empty, asset.SubCategory);
        Assert.Equal(string.Empty, asset.GameTitle);
    }

    // ── GameWorldMap ────────────────────────────────────────────────────────

    [Fact]
    public void GameWorldMap_DefaultGameTitle_IsGTA3()
    {
        var map = new GameWorldMap();
        Assert.Equal("GTA3", map.GameTitle);
    }

    [Fact]
    public void GameWorldMap_DefaultSubLevels_IsEmptyList()
    {
        var map = new GameWorldMap();
        Assert.NotNull(map.SubLevels);
        Assert.Empty(map.SubLevels);
    }
}
