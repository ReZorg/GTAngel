using System.IO;
using System.IO.Compression;
using GTA3DE.Wpf.Models;

namespace GTA3DE.Wpf.Services;

/// <summary>
/// Service to catalog and browse GTA3DE game world assets from the zip archive.
/// Reads the archive index without extracting files to conserve disk space.
/// </summary>
public class AssetCatalogService
{
    private List<GameWorldAsset> _assets = new();
    private AssetCatalogSummary? _summary;
    private List<GameWorldMap> _maps = new();

    public IReadOnlyList<GameWorldAsset> Assets => _assets;
    public AssetCatalogSummary? Summary => _summary;
    public IReadOnlyList<GameWorldMap> Maps => _maps;
    public bool IsLoaded => _assets.Count > 0;

    /// <summary>
    /// Scan the GTA3DE asset archive and build the catalog index.
    /// Does NOT extract files — reads the zip directory only.
    /// </summary>
    public async Task<AssetCatalogSummary> ScanArchiveAsync(string archivePath, IProgress<string>? progress = null)
    {
        return await Task.Run(() =>
        {
            _assets.Clear();
            _maps.Clear();

            progress?.Report("Opening archive...");

            using var archive = ZipFile.OpenRead(archivePath);
            int total = archive.Entries.Count;
            int processed = 0;

            foreach (var entry in archive.Entries)
            {
                processed++;
                if (processed % 500 == 0)
                    progress?.Report($"Scanning... {processed}/{total} entries");

                if (entry.Length == 0 && string.IsNullOrEmpty(entry.Name))
                    continue; // Skip directories

                var asset = ParseEntry(entry);
                _assets.Add(asset);

                if (asset.IsMap)
                {
                    var map = new GameWorldMap
                    {
                        Name = Path.GetFileNameWithoutExtension(entry.Name),
                        MapFilePath = entry.FullName,
                        Size = entry.Length,
                        GameTitle = asset.GameTitle
                    };
                    map.Region = InferRegion(map.Name);
                    _maps.Add(map);
                }
            }

            // Build sub-level relationships
            foreach (var map in _maps)
            {
                map.SubLevels = _maps
                    .Where(m => m != map && m.MapFilePath.Contains(Path.GetDirectoryName(map.MapFilePath) ?? ""))
                    .Select(m => m.Name)
                    .ToList();
            }

            _summary = BuildSummary(archivePath);
            progress?.Report($"Catalog complete: {_assets.Count} assets indexed");
            return _summary;
        });
    }

    private GameWorldAsset ParseEntry(ZipArchiveEntry entry)
    {
        var fullPath = entry.FullName.Replace("GTA3DE.Assets/", "");
        var ext = Path.GetExtension(entry.Name).ToLowerInvariant();

        var asset = new GameWorldAsset
        {
            FullPath = fullPath,
            FileName = entry.Name,
            Extension = ext,
            Size = entry.Length,
            IsMap = ext == ".umap",
            IsBlueprint = entry.Name.StartsWith("BP_") || entry.Name.StartsWith("BPS_"),
            IsTexture = fullPath.Contains("/Textures/") || ext == ".ubulk",
            IsAudio = fullPath.Contains("/Audio/") || ext is ".wav" or ".mp3" or ".ogg"
        };

        // Determine game title
        if (fullPath.Contains("/GTA3/")) asset.GameTitle = "GTA3";
        else if (fullPath.Contains("/ViceCity/")) asset.GameTitle = "Vice City";
        else if (fullPath.Contains("/SanAndreas/")) asset.GameTitle = "San Andreas";
        else if (fullPath.Contains("/Common/")) asset.GameTitle = "Common";
        else asset.GameTitle = "Engine";

        // Determine category
        asset.Category = InferCategory(fullPath);
        asset.SubCategory = InferSubCategory(fullPath);

        return asset;
    }

    private static AssetCategory InferCategory(string path)
    {
        if (path.Contains("/Audio/")) return AssetCategory.Audio;
        if (path.Contains("/Characters/")) return AssetCategory.Characters;
        if (path.Contains("/Cinematics/")) return AssetCategory.Cinematics;
        if (path.Contains("/Cutscene/")) return AssetCategory.Cutscene;
        if (path.Contains("/Effects/")) return AssetCategory.Effects;
        if (path.Contains("/Environment/")) return AssetCategory.Environment;
        if (path.Contains("/GameData/")) return AssetCategory.GameData;
        if (path.Contains("/Maps/")) return AssetCategory.Maps;
        if (path.Contains("/Pickups/")) return AssetCategory.Pickups;
        if (path.Contains("/Radar/")) return AssetCategory.Radar;
        if (path.Contains("/Textures/")) return AssetCategory.Textures;
        if (path.Contains("/UI/")) return AssetCategory.UI;
        if (path.Contains("/Vehicles/")) return AssetCategory.Vehicles;
        if (path.Contains("/Videos/")) return AssetCategory.Videos;
        if (path.Contains("/Weapons/")) return AssetCategory.Weapons;
        if (path.Contains("/Common/")) return AssetCategory.Common;
        if (path.Contains("/Localization/")) return AssetCategory.Localization;
        if (path.Contains("/OriginalData/")) return AssetCategory.OriginalData;
        if (path.Contains("OBB_Extras/")) return AssetCategory.OBBExtras;
        if (path.Contains("Engine/")) return AssetCategory.Engine;
        if (path.Contains("/Config/")) return AssetCategory.Config;
        return AssetCategory.Other;
    }

    private static string InferSubCategory(string path)
    {
        var parts = path.Split('/');
        // Return the deepest meaningful directory
        for (int i = parts.Length - 2; i >= 0; i--)
        {
            if (!string.IsNullOrEmpty(parts[i]) && parts[i] != "Content" && parts[i] != "Gameface")
                return parts[i];
        }
        return "Root";
    }

    private static MapRegion InferRegion(string mapName)
    {
        var lower = mapName.ToLowerInvariant();
        if (lower.Contains("comn") || lower.Contains("indust")) return MapRegion.Portland;
        if (lower.Contains("comse") || lower.Contains("com")) return MapRegion.StauntonIsland;
        if (lower.Contains("sub")) return MapRegion.ShoresideVale;
        if (lower.Contains("world") || lower.Contains("liberty")) return MapRegion.LibertyCity;
        return MapRegion.Unknown;
    }

    private AssetCatalogSummary BuildSummary(string archivePath)
    {
        var summary = new AssetCatalogSummary
        {
            TotalAssets = _assets.Count,
            TotalSizeBytes = _assets.Sum(a => a.Size),
            MapCount = _assets.Count(a => a.IsMap),
            BlueprintCount = _assets.Count(a => a.IsBlueprint),
            TextureCount = _assets.Count(a => a.IsTexture),
            AudioCount = _assets.Count(a => a.IsAudio),
            ArchivePath = archivePath
        };

        foreach (var group in _assets.GroupBy(a => a.Category))
        {
            summary.CountByCategory[group.Key] = group.Count();
            summary.SizeByCategory[group.Key] = group.Sum(a => a.Size);
        }

        foreach (var group in _assets.GroupBy(a => a.Extension))
        {
            summary.CountByExtension[group.Key] = group.Count();
        }

        return summary;
    }

    public IEnumerable<GameWorldAsset> GetByCategory(AssetCategory category)
        => _assets.Where(a => a.Category == category);

    public IEnumerable<GameWorldAsset> GetByGame(string gameTitle)
        => _assets.Where(a => a.GameTitle == gameTitle);

    public IEnumerable<GameWorldAsset> Search(string query)
        => _assets.Where(a => a.FileName.Contains(query, StringComparison.OrdinalIgnoreCase)
                            || a.FullPath.Contains(query, StringComparison.OrdinalIgnoreCase));
}
