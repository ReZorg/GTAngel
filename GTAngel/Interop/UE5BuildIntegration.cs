using System.Diagnostics;
using System.IO;
using System.Text.Json;
using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Interop;

/// <summary>
/// UE5 Build Integration service for discovering, validating, and optionally
/// triggering builds from the UnrealEngineCog source tree.
///
/// Thi/// bridges the gap between the APK-extracted Android assets (which need
///   re-cooking for Windows) and the UE5 engine source at E:\u9n\UnrealEngine.
///   (Upgraded from UnrealEngineCog → UnrealEngine for UE5 full source access)
/// Capabilities:
///   1. Discover the UnrealEngineCog installation and validate its structure
///   2. Enumerate available UE5 projects (Gameface, Archecho, Samples)
///   3. Check if a Windows-cooked build exists for any project
///   4. Provide build commands for cooking assets and compiling projects
///   5. Monitor build progress via output parsing
///
/// Architecture:
///   UnrealEngineCog/
///     Engine/           → UE5 engine source (Build, Source, Content)
///     Archecho/         → Cognitive architecture Electron app
///     Samples/          → Sample UE5 projects
///     Source/           → Custom engine modules
///     Templates/        → Project templates
/// </summary>
public class UE5BuildIntegration
{
    private readonly ILogger<UE5BuildIntegration> _logger;

    /// <summary>Default UnrealEngine installation path (upgraded from UnrealEngineCog)</summary>
    public const string DefaultEnginePath = @"E:\u9n\UnrealEngine";

    /// <summary>Legacy path alias for backwards compatibility</summary>
    public const string LegacyEnginePath = @"E:\u9n\UnrealEngineCog";

    /// <summary>The resolved engine root path</summary>
    public string EnginePath { get; }

    /// <summary>Whether the UE5 engine source is available</summary>
    public bool IsEngineAvailable { get; private set; }

    /// <summary>Detected engine version string</summary>
    public string EngineVersion { get; private set; } = "Unknown";

    /// <summary>Fires when build progress updates</summary>
#pragma warning disable CS0067 // Events are raised by future build orchestration
    public event EventHandler<BuildProgressEventArgs>? BuildProgressChanged;

    /// <summary>Fires when a build completes</summary>
    public event EventHandler<BuildCompletedEventArgs>? BuildCompleted;
#pragma warning restore CS0067

    public UE5BuildIntegration(ILogger<UE5BuildIntegration> logger, string? enginePath = null)
    {
        _logger = logger;
        EnginePath = enginePath ?? DefaultEnginePath;
        ValidateEngineInstallation();
    }

    /// <summary>
    /// Validate the UnrealEngineCog installation structure.
    /// Checks for required directories and build tools.
    /// </summary>
    private void ValidateEngineInstallation()
    {
        if (!Directory.Exists(EnginePath))
        {
            _logger.LogWarning("UnrealEngine not found at {Path} (also checked legacy UnrealEngineCog path)", EnginePath);
            IsEngineAvailable = false;
            return;
        }

        var requiredPaths = new[]
        {
            Path.Combine(EnginePath, "Engine"),
            Path.Combine(EnginePath, "Source"),
        };

        var optionalPaths = new Dictionary<string, string>
        {
            ["BuildBat"] = Path.Combine(EnginePath, "Engine", "Build", "BatchFiles", "Build.bat"),
            ["RunUAT"] = Path.Combine(EnginePath, "Engine", "Build", "BatchFiles", "RunUAT.bat"),
            ["UBT"] = Path.Combine(EnginePath, "Engine", "Binaries", "DotNET", "UnrealBuildTool", "UnrealBuildTool.dll"),
        };

        IsEngineAvailable = requiredPaths.All(Directory.Exists);

        if (IsEngineAvailable)
        {
            EngineVersion = DetectEngineVersion();
            _logger.LogInformation("UnrealEngine (UE5) found at {Path}, version: {Version}",
                EnginePath, EngineVersion);

            foreach (var (name, path) in optionalPaths)
            {
                if (File.Exists(path))
                    _logger.LogDebug("  {Name}: Available", name);
                else
                    _logger.LogDebug("  {Name}: Not found at {Path}", name, path);
            }
        }
        else
        {
            _logger.LogWarning("UnrealEngine structure incomplete at {Path} — Engine/ and Source/ required", EnginePath);
        }
    }

    /// <summary>
    /// Detect the UE5 engine version from the Version.h or Build.version file.
    /// </summary>
    private string DetectEngineVersion()
    {
        // Try Build.version JSON first
        var buildVersionPath = Path.Combine(EnginePath, "Engine", "Build", "Build.version");
        if (File.Exists(buildVersionPath))
        {
            try
            {
                var json = File.ReadAllText(buildVersionPath);
                var doc = JsonDocument.Parse(json);
                var major = doc.RootElement.GetProperty("MajorVersion").GetInt32();
                var minor = doc.RootElement.GetProperty("MinorVersion").GetInt32();
                var patch = doc.RootElement.GetProperty("PatchVersion").GetInt32();
                return $"{major}.{minor}.{patch}";
            }
            catch (Exception ex)
            {
                _logger.LogDebug("Could not parse Build.version: {Error}", ex.Message);
            }
        }

        // Try Version.h header
        var versionHeaderPath = Path.Combine(EnginePath, "Engine", "Source", "Runtime",
            "Launch", "Resources", "Version.h");
        if (File.Exists(versionHeaderPath))
        {
            try
            {
                var lines = File.ReadAllLines(versionHeaderPath);
                var major = ExtractDefine(lines, "ENGINE_MAJOR_VERSION");
                var minor = ExtractDefine(lines, "ENGINE_MINOR_VERSION");
                var patch = ExtractDefine(lines, "ENGINE_PATCH_VERSION");
                if (major != null && minor != null)
                    return $"{major}.{minor}.{patch ?? "0"}";
            }
            catch (Exception ex)
            {
                _logger.LogDebug("Could not parse Version.h: {Error}", ex.Message);
            }
        }

        return "5.x (UnrealEngineCog)";
    }

    private static string? ExtractDefine(string[] lines, string defineName)
    {
        var prefix = $"#define {defineName}";
        var line = lines.FirstOrDefault(l => l.TrimStart().StartsWith(prefix));
        return line?.Substring(line.IndexOf(prefix) + prefix.Length).Trim();
    }

    /// <summary>
    /// Discover all UE5 projects in the UnrealEngineCog tree.
    /// Returns projects from Samples/, Templates/, and the root.
    /// </summary>
    public List<UE5ProjectInfo> DiscoverProjects()
    {
        var projects = new List<UE5ProjectInfo>();
        if (!IsEngineAvailable) return projects;

        var searchDirs = new[]
        {
            EnginePath,
            Path.Combine(EnginePath, "Samples"),
            Path.Combine(EnginePath, "Templates"),
        };

        foreach (var dir in searchDirs)
        {
            if (!Directory.Exists(dir)) continue;

            try
            {
                // Search one level deep for .uproject files
                foreach (var uproject in Directory.EnumerateFiles(dir, "*.uproject", SearchOption.AllDirectories))
                {
                    var projectDir = Path.GetDirectoryName(uproject)!;
                    var projectName = Path.GetFileNameWithoutExtension(uproject);

                    var info = new UE5ProjectInfo
                    {
                        Name = projectName,
                        ProjectFilePath = uproject,
                        RootPath = projectDir,
                        HasContent = Directory.Exists(Path.Combine(projectDir, "Content")),
                        HasSource = Directory.Exists(Path.Combine(projectDir, "Source")),
                        HasBinaries = HasWindowsBinaries(projectDir),
                        HasCookedContent = HasWindowsCookedContent(projectDir),
                        EngineVersion = ReadProjectEngineVersion(uproject),
                        Category = CategorizeProject(uproject, dir)
                    };

                    projects.Add(info);
                    _logger.LogDebug("Discovered UE5 project: {Name} at {Path} (binaries={HasBin}, cooked={HasCooked})",
                        info.Name, info.RootPath, info.HasBinaries, info.HasCookedContent);
                }
            }
            catch (Exception ex)
            {
                _logger.LogDebug("Error scanning {Dir}: {Error}", dir, ex.Message);
            }
        }

        return projects;
    }

    /// <summary>
    /// Check if a project has Windows-compiled binaries.
    /// </summary>
    private static bool HasWindowsBinaries(string projectDir)
    {
        var binDir = Path.Combine(projectDir, "Binaries", "Win64");
        if (!Directory.Exists(binDir)) return false;
        return Directory.EnumerateFiles(binDir, "*.exe", SearchOption.TopDirectoryOnly).Any() ||
               Directory.EnumerateFiles(binDir, "*.dll", SearchOption.TopDirectoryOnly).Any();
    }

    /// <summary>
    /// Check if a project has Windows-cooked content (DXT/BC textures).
    /// </summary>
    private static bool HasWindowsCookedContent(string projectDir)
    {
        var cookedDir = Path.Combine(projectDir, "Saved", "Cooked", "WindowsNoEditor");
        if (Directory.Exists(cookedDir)) return true;

        // Also check Saved/StagedBuilds
        var stagedDir = Path.Combine(projectDir, "Saved", "StagedBuilds", "Windows");
        return Directory.Exists(stagedDir);
    }

    /// <summary>
    /// Read the engine version association from a .uproject file.
    /// </summary>
    private string ReadProjectEngineVersion(string uprojectPath)
    {
        try
        {
            var content = File.ReadAllText(uprojectPath);
            var doc = JsonDocument.Parse(content);
            if (doc.RootElement.TryGetProperty("EngineAssociation", out var version))
                return version.GetString() ?? EngineVersion;
        }
        catch { }
        return EngineVersion;
    }

    /// <summary>
    /// Categorize a project based on its location in the tree.
    /// </summary>
    private static string CategorizeProject(string uprojectPath, string searchDir)
    {
        if (uprojectPath.Contains("Samples", StringComparison.OrdinalIgnoreCase))
            return "Sample";
        if (uprojectPath.Contains("Templates", StringComparison.OrdinalIgnoreCase))
            return "Template";
        if (uprojectPath.Contains("Archecho", StringComparison.OrdinalIgnoreCase))
            return "Cognitive";
        return "Project";
    }

    /// <summary>
    /// Get the build command for compiling a UE5 project.
    /// Returns the command line that would be used to build the project.
    /// </summary>
    public string? GetBuildCommand(string projectName, string configuration = "Shipping", string platform = "Win64")
    {
        if (!IsEngineAvailable) return null;

        var buildBat = Path.Combine(EnginePath, "Engine", "Build", "BatchFiles", "Build.bat");
        if (!File.Exists(buildBat))
        {
            // Try RunUAT for cooking + packaging
            var runUat = Path.Combine(EnginePath, "Engine", "Build", "BatchFiles", "RunUAT.bat");
            if (File.Exists(runUat))
            {
                return $"\"{runUat}\" BuildCookRun " +
                       $"-project=\"{Path.Combine(EnginePath, projectName, $"{projectName}.uproject")}\" " +
                       $"-platform={platform} -configuration={configuration} " +
                       $"-cook -build -stage -pak -archive";
            }
            return null;
        }

        return $"\"{buildBat}\" {projectName} {platform} {configuration} " +
               $"\"{Path.Combine(EnginePath, projectName, $"{projectName}.uproject")}\"";
    }

    /// <summary>
    /// Get the cook command for re-cooking Android assets to Windows format.
    /// This is needed when APK-extracted assets (ASTC/ETC2) need conversion to DXT/BC.
    /// </summary>
    public string? GetCookCommand(string projectName, string platform = "Windows")
    {
        if (!IsEngineAvailable) return null;

        var runUat = Path.Combine(EnginePath, "Engine", "Build", "BatchFiles", "RunUAT.bat");
        if (!File.Exists(runUat)) return null;

        return $"\"{runUat}\" BuildCookRun " +
               $"-project=\"{Path.Combine(EnginePath, projectName, $"{projectName}.uproject")}\" " +
               $"-targetplatform={platform} -cook -skipbuild -allmaps -unversionedcookedcontent";
    }

    /// <summary>
    /// Find the executable for a built UE5 project.
    /// </summary>
    public string? FindProjectExecutable(string projectName)
    {
        if (!IsEngineAvailable) return null;

        var candidates = new[]
        {
            Path.Combine(EnginePath, projectName, "Binaries", "Win64", $"{projectName}-Win64-Shipping.exe"),
            Path.Combine(EnginePath, projectName, "Binaries", "Win64", $"{projectName}.exe"),
            Path.Combine(EnginePath, projectName, "Binaries", "Win64", $"{projectName}-Win64-Development.exe"),
        };

        var found = candidates.FirstOrDefault(File.Exists);
        if (found != null) return found;

        // Wildcard scan
        var binDir = Path.Combine(EnginePath, projectName, "Binaries", "Win64");
        if (Directory.Exists(binDir))
        {
            var exes = Directory.GetFiles(binDir, "*.exe");
            if (exes.Length > 0) return exes[0];
        }

        return null;
    }

    /// <summary>
    /// Get a comprehensive status report of the UE5 integration.
    /// </summary>
    public UE5IntegrationStatus GetStatus()
    {
        var status = new UE5IntegrationStatus
        {
            EnginePath = EnginePath,
            IsEngineAvailable = IsEngineAvailable,
            EngineVersion = EngineVersion,
            HasBuildTools = File.Exists(Path.Combine(EnginePath, "Engine", "Build", "BatchFiles", "Build.bat")),
            HasRunUAT = File.Exists(Path.Combine(EnginePath, "Engine", "Build", "BatchFiles", "RunUAT.bat")),
            Projects = IsEngineAvailable ? DiscoverProjects() : new List<UE5ProjectInfo>()
        };

        return status;
    }
}

/// <summary>Information about a discovered UE5 project</summary>
public class UE5ProjectInfo
{
    public string Name { get; set; } = string.Empty;
    public string ProjectFilePath { get; set; } = string.Empty;
    public string RootPath { get; set; } = string.Empty;
    public bool HasContent { get; set; }
    public bool HasSource { get; set; }
    public bool HasBinaries { get; set; }
    public bool HasCookedContent { get; set; }
    public string EngineVersion { get; set; } = "Unknown";
    public string Category { get; set; } = "Project";

    public override string ToString() =>
        $"{Name} [{Category}] (bin={HasBinaries}, content={HasContent}, cooked={HasCookedContent})";
}

/// <summary>Build progress event arguments</summary>
public class BuildProgressEventArgs : EventArgs
{
    public string Stage { get; set; } = string.Empty;
    public double Progress { get; set; }
    public string Message { get; set; } = string.Empty;
}

/// <summary>Build completed event arguments</summary>
public class BuildCompletedEventArgs : EventArgs
{
    public bool Success { get; set; }
    public string OutputPath { get; set; } = string.Empty;
    public TimeSpan Duration { get; set; }
    public string[] Errors { get; set; } = Array.Empty<string>();
}

/// <summary>Comprehensive UE5 integration status</summary>
public class UE5IntegrationStatus
{
    public string EnginePath { get; set; } = string.Empty;
    public bool IsEngineAvailable { get; set; }
    public string EngineVersion { get; set; } = "Unknown";
    public bool HasBuildTools { get; set; }
    public bool HasRunUAT { get; set; }
    public List<UE5ProjectInfo> Projects { get; set; } = new();
}
