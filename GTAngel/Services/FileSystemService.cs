using System.IO;
using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Services;

/// <summary>
/// File system service for game asset management and UE project discovery.
/// Translated from: Android OBB file management + APKExpansionSupport
///
/// Original Android file paths:
///   Context.getObbDir() → /storage/emulated/0/Android/obb/{package}/
///   OBB file: main.{versionCode}.{package}.obb
///   Context.getFilesDir() → /data/data/{package}/files/
///   Context.getExternalFilesDir() → /storage/emulated/0/Android/data/{package}/files/
///
/// WPF equivalent paths:
///   Game data: {AppData}/GTA3DE/GameData/
///   Config: {AppData}/GTA3DE/Config/
///   Saves: {Documents}/Rockstar Games/GTA III Definitive Edition/
///   Logs: {AppData}/GTA3DE/Logs/
///   UE Engine: E:\u9n\UnrealEngineCog\Engine\ (if available)
/// </summary>
public class FileSystemService
{
    private readonly ILogger<FileSystemService> _logger;

    /// <summary>Base application data directory</summary>
    public string AppDataPath { get; }

    /// <summary>Game data directory (replaces OBB dir)</summary>
    public string GameDataPath { get; }

    /// <summary>User saves directory</summary>
    public string SavesPath { get; }

    /// <summary>Configuration directory</summary>
    public string ConfigPath { get; }

    /// <summary>Logs directory</summary>
    public string LogsPath { get; }

    /// <summary>UE5 project name from UE4CommandLine.txt</summary>
    public const string UEProjectName = "Gameface";

    /// <summary>Known UnrealEngineCog path on this system</summary>
    public const string UnrealEngineCogPath = @"E:\u9n\UnrealEngineCog";

    public FileSystemService(ILogger<FileSystemService> logger)
    {
        _logger = logger;

        AppDataPath = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "GTA3DE");

        GameDataPath = Path.Combine(AppDataPath, "GameData");
        SavesPath = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
            "Rockstar Games", "GTA III Definitive Edition");
        ConfigPath = Path.Combine(AppDataPath, "Config");
        LogsPath = Path.Combine(AppDataPath, "Logs");

        EnsureDirectories();
    }

    private void EnsureDirectories()
    {
        Directory.CreateDirectory(AppDataPath);
        Directory.CreateDirectory(GameDataPath);
        Directory.CreateDirectory(SavesPath);
        Directory.CreateDirectory(ConfigPath);
        Directory.CreateDirectory(LogsPath);
        _logger.LogInformation("File system directories initialized at {Path}", AppDataPath);
    }

    /// <summary>
    /// Check if game assets are present.
    /// Replaces: DownloaderActivity.expansionFilesDelivered() OBB check
    /// </summary>
    public bool AreGameAssetsPresent()
    {
        var gameExe = GetGameExecutablePath();
        return !string.IsNullOrEmpty(gameExe) && File.Exists(gameExe);
    }

    /// <summary>
    /// Get the game executable path (first found).
    /// Replaces: NativeActivity loading libUE4.so
    /// </summary>
    public string GetGameExecutablePath()
    {
        foreach (var path in GetAllGameExecutablePaths())
        {
            if (File.Exists(path))
                return path;
        }

        // Default fallback path
        return Path.Combine(GameDataPath, "GTA3DE.exe");
    }

    /// <summary>
    /// Get all possible game executable paths to search.
    /// Includes GameData directory, UE project Binaries, and UnrealEngineCog paths.
    /// </summary>
    public string[] GetAllGameExecutablePaths()
    {
        var paths = new List<string>
        {
            // Standard GameData locations
            Path.Combine(GameDataPath, "GTA3DE.exe"),
            Path.Combine(GameDataPath, "Binaries", "Win64", "GTA3DE.exe"),
            Path.Combine(GameDataPath, UEProjectName, "Binaries", "Win64", $"{UEProjectName}-Win64-Shipping.exe"),
            Path.Combine(GameDataPath, UEProjectName, "Binaries", "Win64", $"{UEProjectName}.exe"),
            Path.Combine(GameDataPath, "LibertyCity", "Binaries", "Win64", "LibertyCity-Win64-Shipping.exe"),

            // UnrealEngineCog project locations
            Path.Combine(UnrealEngineCogPath, UEProjectName, "Binaries", "Win64", $"{UEProjectName}-Win64-Shipping.exe"),
            Path.Combine(UnrealEngineCogPath, UEProjectName, "Binaries", "Win64", $"{UEProjectName}.exe"),
        };

        // Also scan for any .exe in the GameData Binaries directory
        var binDir = Path.Combine(GameDataPath, "Binaries", "Win64");
        if (Directory.Exists(binDir))
        {
            try
            {
                paths.AddRange(Directory.GetFiles(binDir, "*.exe"));
            }
            catch (Exception ex)
            {
                _logger.LogDebug("Error scanning {Dir}: {Error}", binDir, ex.Message);
            }
        }

        // Scan UnrealEngineCog for compiled projects
        var uecBinDir = Path.Combine(UnrealEngineCogPath, UEProjectName, "Binaries", "Win64");
        if (Directory.Exists(uecBinDir))
        {
            try
            {
                paths.AddRange(Directory.GetFiles(uecBinDir, "*.exe"));
            }
            catch (Exception ex)
            {
                _logger.LogDebug("Error scanning {Dir}: {Error}", uecBinDir, ex.Message);
            }
        }

        return paths.Distinct().ToArray();
    }

    /// <summary>
    /// Get the UE project file path (.uproject).
    /// Replaces: UE4CommandLine.txt reference to ../../../Gameface/Gameface.uproject
    /// </summary>
    public string? GetUProjectPath()
    {
        var searchPaths = new[]
        {
            Path.Combine(GameDataPath, UEProjectName, $"{UEProjectName}.uproject"),
            Path.Combine(UnrealEngineCogPath, UEProjectName, $"{UEProjectName}.uproject"),
        };

        return searchPaths.FirstOrDefault(File.Exists);
    }

    /// <summary>
    /// Check if the UnrealEngineCog source is available for building.
    /// </summary>
    public bool IsUnrealEngineCogAvailable()
    {
        return Directory.Exists(UnrealEngineCogPath) &&
               File.Exists(Path.Combine(UnrealEngineCogPath, "Engine", "Build", "BatchFiles", "Build.bat"));
    }

    /// <summary>
    /// Get the UE content directory for cooked assets.
    /// Replaces: OBB mount point where UE4 reads .uasset files
    /// </summary>
    public string GetContentPath()
    {
        return Path.Combine(GameDataPath, UEProjectName, "Content");
    }

    /// <summary>
    /// Get total size of game data directory.
    /// Replaces: checking OBB file size
    /// </summary>
    public long GetGameDataSize()
    {
        if (!Directory.Exists(GameDataPath))
            return 0;

        return new DirectoryInfo(GameDataPath)
            .EnumerateFiles("*", SearchOption.AllDirectories)
            .Sum(f => f.Length);
    }
}
