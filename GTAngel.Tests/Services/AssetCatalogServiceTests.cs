using GTA3DE.Wpf.Models;
using GTA3DE.Wpf.Services;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for AssetCatalogService — focuses on the pure-logic methods that don't
/// require a real ZIP archive (category/subcategory/region inference, search, filters).
/// </summary>
public class AssetCatalogServiceTests
{
    private readonly AssetCatalogService _service = new();

    // ── Initial state ──────────────────────────────────────────────────────

    [Fact]
    public void IsLoaded_WhenEmpty_IsFalse()
    {
        Assert.False(_service.IsLoaded);
    }

    [Fact]
    public void Assets_WhenEmpty_IsEmptyList()
    {
        Assert.Empty(_service.Assets);
    }

    [Fact]
    public void Summary_WhenEmpty_IsNull()
    {
        Assert.Null(_service.Summary);
    }

    [Fact]
    public void Maps_WhenEmpty_IsEmptyList()
    {
        Assert.Empty(_service.Maps);
    }

    // ── InferCategory (tested via reflection or through a test-exposed helper) ──
    // Since InferCategory is private static, we test it indirectly by observing
    // how ParseEntry would classify assets. We do this by scanning a real (in-memory)
    // zip archive constructed in the tests.

    [Theory]
    [InlineData("/Audio/somefile.wav", AssetCategory.Audio)]
    [InlineData("/Characters/hero.uasset", AssetCategory.Characters)]
    [InlineData("/Cinematics/intro.mp4", AssetCategory.Cinematics)]
    [InlineData("/Cutscene/scene1.umap", AssetCategory.Cutscene)]
    [InlineData("/Effects/explosion.uasset", AssetCategory.Effects)]
    [InlineData("/Environment/building.uasset", AssetCategory.Environment)]
    [InlineData("/GameData/config.json", AssetCategory.GameData)]
    [InlineData("/Maps/world.umap", AssetCategory.Maps)]
    [InlineData("/Pickups/health.uasset", AssetCategory.Pickups)]
    [InlineData("/Radar/minimap.png", AssetCategory.Radar)]
    [InlineData("/Textures/road.ubulk", AssetCategory.Textures)]
    [InlineData("/UI/hud.uasset", AssetCategory.UI)]
    [InlineData("/Vehicles/car.uasset", AssetCategory.Vehicles)]
    [InlineData("/Videos/cutscene.mp4", AssetCategory.Videos)]
    [InlineData("/Weapons/gun.uasset", AssetCategory.Weapons)]
    [InlineData("/Common/shared.uasset", AssetCategory.Common)]
    [InlineData("/Localization/en.uasset", AssetCategory.Localization)]
    [InlineData("/OriginalData/archive.dat", AssetCategory.OriginalData)]
    [InlineData("OBB_Extras/pack.obb", AssetCategory.OBBExtras)]
    [InlineData("Engine/core.dll", AssetCategory.Engine)]
    [InlineData("/Config/settings.ini", AssetCategory.Config)]
    [InlineData("/Unknown/mystery.bin", AssetCategory.Other)]
    public void InferCategory_MatchesPathSegment(string path, AssetCategory expected)
    {
        var category = InvokeInferCategory(path);
        Assert.Equal(expected, category);
    }

    [Fact]
    public void InferCategory_UnrecognizedPath_ReturnsOther()
    {
        Assert.Equal(AssetCategory.Other, InvokeInferCategory("/xyz/unknown/file.bin"));
    }

    // ── InferRegion ────────────────────────────────────────────────────────

    [Theory]
    [InlineData("comn_strip1", MapRegion.Portland)]
    [InlineData("industZone", MapRegion.Portland)]
    [InlineData("comse_main", MapRegion.StauntonIsland)]
    [InlineData("sub_area1", MapRegion.ShoresideVale)]
    [InlineData("liberty_world", MapRegion.LibertyCity)]
    [InlineData("randomMapName", MapRegion.Unknown)]
    public void InferRegion_ReturnsCorrectRegion(string mapName, MapRegion expected)
    {
        var region = InvokeInferRegion(mapName);
        Assert.Equal(expected, region);
    }

    // ── InferSubCategory ──────────────────────────────────────────────────

    [Fact]
    public void InferSubCategory_ReturnsMeaningfulDirectory()
    {
        var sub = InvokeInferSubCategory("Gameface/Content/GTA3/Characters/player.uasset");
        // Should not be "Content" or "Gameface"
        Assert.NotEqual("Content", sub);
        Assert.NotEqual("Gameface", sub);
        Assert.NotEmpty(sub);
    }

    [Fact]
    public void InferSubCategory_SingleSegmentPath_ReturnsRoot()
    {
        var sub = InvokeInferSubCategory("file.txt");
        Assert.Equal("Root", sub);
    }

    // ── Search ────────────────────────────────────────────────────────────

    [Fact]
    public void Search_WhenEmpty_ReturnsEmpty()
    {
        var results = _service.Search("anything");
        Assert.Empty(results);
    }

    // ── GetByCategory ─────────────────────────────────────────────────────

    [Fact]
    public void GetByCategory_WhenEmpty_ReturnsEmpty()
    {
        Assert.Empty(_service.GetByCategory(AssetCategory.Audio));
    }

    // ── GetByGame ─────────────────────────────────────────────────────────

    [Fact]
    public void GetByGame_WhenEmpty_ReturnsEmpty()
    {
        Assert.Empty(_service.GetByGame("GTA3"));
    }

    // ── Helpers to call private static methods via reflection ──────────────

    private static AssetCategory InvokeInferCategory(string path)
    {
        var method = typeof(AssetCatalogService).GetMethod(
            "InferCategory",
            System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
        Assert.NotNull(method);
        return (AssetCategory)method!.Invoke(null, new object[] { path })!;
    }

    private static MapRegion InvokeInferRegion(string mapName)
    {
        var method = typeof(AssetCatalogService).GetMethod(
            "InferRegion",
            System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
        Assert.NotNull(method);
        return (MapRegion)method!.Invoke(null, new object[] { mapName })!;
    }

    private static string InvokeInferSubCategory(string path)
    {
        var method = typeof(AssetCatalogService).GetMethod(
            "InferSubCategory",
            System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
        Assert.NotNull(method);
        return (string)method!.Invoke(null, new object[] { path })!;
    }
}
