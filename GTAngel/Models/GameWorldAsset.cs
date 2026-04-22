namespace GTA3DE.Wpf.Models;

/// <summary>
/// Represents a single asset entry from the GTA3DE asset archive.
/// </summary>
public class GameWorldAsset
{
    public string FullPath { get; set; } = string.Empty;
    public string FileName { get; set; } = string.Empty;
    public string Extension { get; set; } = string.Empty;
    public long Size { get; set; }
    public AssetCategory Category { get; set; }
    public string SubCategory { get; set; } = string.Empty;
    public string GameTitle { get; set; } = string.Empty; // GTA3, ViceCity, SanAndreas
    public bool IsMap { get; set; }
    public bool IsBlueprint { get; set; }
    public bool IsTexture { get; set; }
    public bool IsAudio { get; set; }

    public string SizeFormatted => Size switch
    {
        < 1024 => $"{Size} B",
        < 1024 * 1024 => $"{Size / 1024.0:F1} KB",
        < 1024 * 1024 * 1024 => $"{Size / (1024.0 * 1024.0):F1} MB",
        _ => $"{Size / (1024.0 * 1024.0 * 1024.0):F2} GB"
    };
}

/// <summary>
/// Asset categories matching the GTA3DE Gameface content structure.
/// </summary>
public enum AssetCategory
{
    Audio,
    Characters,
    Cinematics,
    Cutscene,
    Effects,
    Environment,
    GameData,
    Maps,
    Pickups,
    Radar,
    Textures,
    UI,
    Vehicles,
    Videos,
    Weapons,
    Common,
    Localization,
    OriginalData,
    OBBExtras,
    Engine,
    Config,
    Other
}

/// <summary>
/// Represents a game world map with its sub-levels.
/// </summary>
public class GameWorldMap
{
    public string Name { get; set; } = string.Empty;
    public string MapFilePath { get; set; } = string.Empty;
    public long Size { get; set; }
    public List<string> SubLevels { get; set; } = new();
    public string GameTitle { get; set; } = "GTA3";
    public MapRegion Region { get; set; }
}

public enum MapRegion
{
    Portland,       // Industrial area
    StauntonIsland, // Commercial area
    ShoresideVale,  // Suburban area
    LibertyCity,    // Full city
    Unknown
}

/// <summary>
/// Summary statistics for the asset catalog.
/// </summary>
public class AssetCatalogSummary
{
    public int TotalAssets { get; set; }
    public long TotalSizeBytes { get; set; }
    public Dictionary<AssetCategory, int> CountByCategory { get; set; } = new();
    public Dictionary<AssetCategory, long> SizeByCategory { get; set; } = new();
    public Dictionary<string, int> CountByExtension { get; set; } = new();
    public int MapCount { get; set; }
    public int BlueprintCount { get; set; }
    public int TextureCount { get; set; }
    public int AudioCount { get; set; }
    public string ArchivePath { get; set; } = string.Empty;

    public string TotalSizeFormatted => TotalSizeBytes switch
    {
        < 1024 * 1024 => $"{TotalSizeBytes / 1024.0:F1} KB",
        < 1024L * 1024 * 1024 => $"{TotalSizeBytes / (1024.0 * 1024.0):F1} MB",
        _ => $"{TotalSizeBytes / (1024.0 * 1024.0 * 1024.0):F2} GB"
    };
}
