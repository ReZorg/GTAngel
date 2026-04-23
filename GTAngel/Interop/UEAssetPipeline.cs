using System.IO;
using System.IO.Compression;
using Microsoft.Extensions.Logging;

namespace GTAngel.Interop;

/// <summary>
/// Asset pipeline for extracting and managing UE4/UE5 game assets.
/// Translated from: Android OBB (Opaque Binary Blob) expansion file system.
///
/// On Android:
///   Game assets are stored in OBB files: main.{version}.{package}.obb
///   OBB files are ZIP archives mounted at /storage/emulated/0/Android/obb/{package}/
///   The APKExpansionSupport library handles downloading and verification
///   UE4 reads assets via FAndroidPlatformFile which transparently reads from OBB
///
/// On Windows:
///   Game assets are extracted to {AppData}/GTA3DE/GameData/
///   The OBB ZIP is extracted to produce the standard UE4 content directory:
///     GameData/
///       Engine/         (engine content)
///       Gameface/       (game content - the UE4 project name from UE4CommandLine.txt)
///         Content/      (cooked assets: .uasset, .umap, .ubulk)
///         Config/       (game configuration)
///
/// Asset Conversion Notes:
///   Android-cooked assets (.uasset) use different texture formats (ASTC/ETC2)
///   than Windows (DXT/BC). A full conversion would require re-cooking from source.
///   For the integration layer, we provide the extraction pipeline and the
///   UE5 project scaffold that can be compiled from UnrealEngineCog source.
/// </summary>
public class UEAssetPipeline
{
    private readonly ILogger<UEAssetPipeline> _logger;
    private readonly string _gameDataPath;

    /// <summary>Expected UE4 project name from UE4CommandLine.txt</summary>
    public const string ProjectName = "Gameface";

    /// <summary>The UE4 content directory structure</summary>
    public string ContentPath => Path.Combine(_gameDataPath, ProjectName, "Content");

    /// <summary>The UE4 config directory</summary>
    public string ConfigPath => Path.Combine(_gameDataPath, ProjectName, "Config");

    /// <summary>The UE4 engine content directory</summary>
    public string EngineContentPath => Path.Combine(_gameDataPath, "Engine", "Content");

    /// <summary>The UE4 binaries directory (where the game exe lives)</summary>
    public string BinariesPath => Path.Combine(_gameDataPath, ProjectName, "Binaries", "Win64");

    public UEAssetPipeline(ILogger<UEAssetPipeline> logger, string gameDataPath)
    {
        _logger = logger;
        _gameDataPath = gameDataPath;
    }

    /// <summary>
    /// Extract an OBB file (ZIP archive) to the game data directory.
    /// Replaces: APKExpansionSupport mounting the OBB at runtime.
    ///
    /// The Android OBB is a ZIP file containing the cooked UE4 content.
    /// We extract it to produce the standard Windows directory layout.
    /// </summary>
    public async Task<bool> ExtractObbAsync(string obbPath, IProgress<double>? progress = null,
        CancellationToken ct = default)
    {
        if (!File.Exists(obbPath))
        {
            _logger.LogError("OBB file not found: {Path}", obbPath);
            return false;
        }

        _logger.LogInformation("Extracting OBB: {Path} → {Dest}", obbPath, _gameDataPath);

        try
        {
            using var archive = ZipFile.OpenRead(obbPath);
            var totalEntries = archive.Entries.Count;
            var processed = 0;

            foreach (var entry in archive.Entries)
            {
                ct.ThrowIfCancellationRequested();

                var destPath = Path.Combine(_gameDataPath, entry.FullName);

                if (string.IsNullOrEmpty(entry.Name))
                {
                    // Directory entry
                    Directory.CreateDirectory(destPath);
                }
                else
                {
                    // File entry
                    Directory.CreateDirectory(Path.GetDirectoryName(destPath)!);

                    using var entryStream = entry.Open();
                    using var fileStream = File.Create(destPath);
                    await entryStream.CopyToAsync(fileStream, ct);
                }

                processed++;
                progress?.Report((double)processed / totalEntries);
            }

            _logger.LogInformation("OBB extraction complete: {Count} entries", totalEntries);
            return true;
        }
        catch (OperationCanceledException)
        {
            _logger.LogWarning("OBB extraction cancelled");
            return false;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "OBB extraction failed");
            return false;
        }
    }

    /// <summary>
    /// Extract the split_obbassets.apk from the APKM bundle.
    /// The split_obbassets.apk is itself a ZIP containing the OBB data
    /// nested under assets/ directory.
    /// </summary>
    public async Task<bool> ExtractApkmAssetsAsync(string apkmPath, IProgress<double>? progress = null,
        CancellationToken ct = default)
    {
        if (!File.Exists(apkmPath))
        {
            _logger.LogError("APKM file not found: {Path}", apkmPath);
            return false;
        }

        _logger.LogInformation("Extracting APKM assets: {Path}", apkmPath);

        try
        {
            // First, extract the split_obbassets.apk from the APKM
            using var apkm = ZipFile.OpenRead(apkmPath);
            var obbEntry = apkm.Entries.FirstOrDefault(e =>
                e.Name.Equals("split_obbassets.apk", StringComparison.OrdinalIgnoreCase));

            if (obbEntry == null)
            {
                _logger.LogError("split_obbassets.apk not found in APKM");
                return false;
            }

            var tempObbPath = Path.Combine(Path.GetTempPath(), "split_obbassets.apk");
            obbEntry.ExtractToFile(tempObbPath, overwrite: true);

            // Now extract the assets from the split_obbassets.apk
            using var obbApk = ZipFile.OpenRead(tempObbPath);
            var assetEntries = obbApk.Entries
                .Where(e => e.FullName.StartsWith("assets/", StringComparison.OrdinalIgnoreCase))
                .ToList();

            var totalEntries = assetEntries.Count;
            var processed = 0;

            foreach (var entry in assetEntries)
            {
                ct.ThrowIfCancellationRequested();

                // Strip the "assets/" prefix
                var relativePath = entry.FullName.Substring("assets/".Length);
                var destPath = Path.Combine(_gameDataPath, relativePath);

                if (string.IsNullOrEmpty(entry.Name))
                {
                    Directory.CreateDirectory(destPath);
                }
                else
                {
                    Directory.CreateDirectory(Path.GetDirectoryName(destPath)!);
                    using var entryStream = entry.Open();
                    using var fileStream = File.Create(destPath);
                    await entryStream.CopyToAsync(fileStream, ct);
                }

                processed++;
                progress?.Report((double)processed / totalEntries);
            }

            // Cleanup temp file
            try { File.Delete(tempObbPath); } catch { }

            _logger.LogInformation("APKM asset extraction complete: {Count} entries", totalEntries);
            return true;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "APKM asset extraction failed");
            return false;
        }
    }

    /// <summary>
    /// Verify the game data directory has the expected UE4 content structure.
    /// Replaces: DownloaderActivity.expansionFilesDelivered() OBB verification.
    /// </summary>
    public AssetVerificationResult VerifyAssets()
    {
        var result = new AssetVerificationResult();

        result.HasGameContent = Directory.Exists(ContentPath) &&
            Directory.EnumerateFiles(ContentPath, "*.uasset", SearchOption.AllDirectories).Any();

        result.HasGameConfig = Directory.Exists(ConfigPath) &&
            Directory.EnumerateFiles(ConfigPath, "*.ini", SearchOption.AllDirectories).Any();

        result.HasEngineContent = Directory.Exists(EngineContentPath);

        result.HasBinaries = Directory.Exists(BinariesPath) &&
            Directory.EnumerateFiles(BinariesPath, "*.exe", SearchOption.TopDirectoryOnly).Any();

        result.HasUProject = File.Exists(Path.Combine(_gameDataPath, ProjectName, $"{ProjectName}.uproject"));

        // Check for Windows-cooked assets (DXT/BC textures vs Android ASTC/ETC2)
        if (result.HasGameContent)
        {
            var sampleAsset = Directory.EnumerateFiles(ContentPath, "*.uasset", SearchOption.AllDirectories).First();
            result.AssetPlatform = DetectAssetPlatform(sampleAsset);
        }

        result.IsComplete = result.HasGameContent && result.HasBinaries;
        result.NeedsRecooking = result.AssetPlatform == "Android";

        return result;
    }

    /// <summary>
    /// Discover UE5 project paths on the local system.
    /// Searches for .uproject files in common locations and the UnrealEngineCog directory.
    /// </summary>
    public List<DiscoveredUEProject> DiscoverLocalProjects()
    {
        var projects = new List<DiscoveredUEProject>();
        var searchPaths = new[]
        {
            // UnrealEngineCog location
            @"E:\u9n\UnrealEngineCog",
            // Common UE project locations
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Unreal Projects"),
            @"C:\Users\Public\Documents\Unreal Projects",
            _gameDataPath
        };

        foreach (var searchPath in searchPaths)
        {
            if (!Directory.Exists(searchPath)) continue;

            try
            {
                foreach (var uproject in Directory.EnumerateFiles(searchPath, "*.uproject", SearchOption.AllDirectories))
                {
                    projects.Add(new DiscoveredUEProject
                    {
                        Name = Path.GetFileNameWithoutExtension(uproject),
                        ProjectFilePath = uproject,
                        RootPath = Path.GetDirectoryName(uproject)!,
                        HasBinaries = Directory.Exists(Path.Combine(Path.GetDirectoryName(uproject)!, "Binaries", "Win64")),
                        HasContent = Directory.Exists(Path.Combine(Path.GetDirectoryName(uproject)!, "Content")),
                        EngineVersion = DetectEngineVersion(uproject)
                    });
                }
            }
            catch (Exception ex)
            {
                _logger.LogDebug("Error scanning {Path}: {Error}", searchPath, ex.Message);
            }
        }

        return projects;
    }

    private string DetectAssetPlatform(string assetPath)
    {
        try
        {
            using var fs = File.OpenRead(assetPath);
            using var reader = new BinaryReader(fs);

            // UE4 .uasset magic: 0xC1832A9E
            var magic = reader.ReadUInt32();
            if (magic != 0x9E2A83C1) return "Unknown";

            // Skip to the platform tag (simplified detection)
            // In practice, the cooked platform is embedded in the package header
            return "Android"; // Default assumption for APK-extracted assets
        }
        catch
        {
            return "Unknown";
        }
    }

    private string DetectEngineVersion(string uprojectPath)
    {
        try
        {
            var content = File.ReadAllText(uprojectPath);
            var doc = System.Text.Json.JsonDocument.Parse(content);
            if (doc.RootElement.TryGetProperty("EngineAssociation", out var version))
                return version.GetString() ?? "Unknown";
        }
        catch { }
        return "Unknown";
    }
}

/// <summary>Result of asset verification</summary>
public class AssetVerificationResult
{
    public bool HasGameContent { get; set; }
    public bool HasGameConfig { get; set; }
    public bool HasEngineContent { get; set; }
    public bool HasBinaries { get; set; }
    public bool HasUProject { get; set; }
    public string AssetPlatform { get; set; } = "Unknown";
    public bool IsComplete { get; set; }
    public bool NeedsRecooking { get; set; }
}

/// <summary>A discovered UE project on the local filesystem</summary>
public class DiscoveredUEProject
{
    public string Name { get; set; } = string.Empty;
    public string ProjectFilePath { get; set; } = string.Empty;
    public string RootPath { get; set; } = string.Empty;
    public bool HasBinaries { get; set; }
    public bool HasContent { get; set; }
    public string EngineVersion { get; set; } = "Unknown";
}
