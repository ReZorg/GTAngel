using GTAngel.Organizer.Core.Models;

namespace GTAngel.Organizer.Core.Services;

public sealed class FileClassifier
{
    private static readonly HashSet<string> SourceExtensions = new(StringComparer.OrdinalIgnoreCase)
    {
        ".cs", ".fs", ".vb", ".cpp", ".c", ".cc", ".cxx", ".h", ".hpp",
        ".py", ".ts", ".tsx", ".js", ".jsx", ".rs", ".go", ".java", ".kt",
        ".swift", ".scala", ".rb", ".php", ".lua", ".scm"
    };

    private static readonly HashSet<string> DocumentationExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".md", ".rst", ".adoc", ".txt" };

    private static readonly HashSet<string> ReferenceExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".pdf", ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx", ".odt", ".ods" };

    private static readonly HashSet<string> ImageExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".png", ".jpg", ".jpeg", ".gif", ".svg", ".webp", ".bmp", ".tif", ".tiff", ".ico" };

    private static readonly HashSet<string> MediaExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".wav", ".mp3", ".flac", ".ogg", ".mp4", ".webm", ".mov", ".avi", ".mkv" };

    private static readonly HashSet<string> ThreeDimensionalExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".fbx", ".obj", ".glb", ".gltf", ".blend", ".uasset", ".umap" };

    private static readonly HashSet<string> AutomationExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".ps1", ".sh", ".bat", ".cmd", ".zsh", ".fish" };

    private static readonly HashSet<string> ConfigurationExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".json", ".yaml", ".yml", ".toml", ".ini", ".xml", ".config", ".props", ".targets", ".env" };

    private static readonly HashSet<string> DataExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".csv", ".tsv", ".parquet", ".sqlite", ".sqlite3", ".db", ".sql", ".jsonl", ".ndjson" };

    private static readonly HashSet<string> ArchiveExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".zip", ".7z", ".rar", ".tar", ".gz", ".bz2", ".xz" };

    private static readonly HashSet<string> BinaryExtensions = new(StringComparer.OrdinalIgnoreCase)
    { ".exe", ".dll", ".pdb", ".lib", ".a", ".so", ".dylib", ".msi", ".nupkg", ".jar", ".wasm" };

    public FileClassification Classify(
        FileInfo file,
        string workspaceRoot,
        string? componentRoot,
        bool isProtected,
        string? protectedBy,
        string? sha256 = null)
    {
        ArgumentNullException.ThrowIfNull(file);
        var relative = PathSafety.GetSafeRelativePath(workspaceRoot, file.FullName);
        var normalized = relative.Replace(Path.DirectorySeparatorChar, '/').ToLowerInvariant();
        var extension = file.Extension;
        var reasons = new List<string>();

        var fileType = DetectType(file, normalized, extension, reasons);
        var function = DetectFunction(file, normalized, fileType, reasons);
        var confidence = CalculateConfidence(fileType, function, reasons.Count, isProtected);
        var risk = CalculateRisk(fileType, function, isProtected, componentRoot, workspaceRoot);

        if (isProtected && !string.IsNullOrWhiteSpace(protectedBy))
        {
            reasons.Add($"Protected by {protectedBy}");
        }

        return new FileClassification
        {
            SourcePath = file.FullName,
            RelativePath = relative,
            FileType = fileType,
            Function = function,
            Confidence = confidence,
            Reason = string.Join("; ", reasons.Distinct(StringComparer.OrdinalIgnoreCase)),
            Risk = risk,
            IsProtected = isProtected,
            ProtectedBy = protectedBy,
            ComponentRoot = componentRoot,
            Length = file.Length,
            LastWriteTimeUtc = file.LastWriteTimeUtc,
            Sha256 = sha256
        };
    }

    private static FileTypeCategory DetectType(FileInfo file, string normalized, string extension, List<string> reasons)
    {
        if (IsTestPath(file, normalized))
        {
            reasons.Add("Test naming or directory convention");
            return FileTypeCategory.Test;
        }

        if (SourceExtensions.Contains(extension))
        {
            reasons.Add($"Source extension {extension}");
            return FileTypeCategory.Source;
        }

        if (DocumentationExtensions.Contains(extension))
        {
            reasons.Add($"Documentation extension {extension}");
            return FileTypeCategory.Documentation;
        }

        if (ReferenceExtensions.Contains(extension))
        {
            reasons.Add($"Reference-document extension {extension}");
            return FileTypeCategory.ReferenceMaterial;
        }

        if (ImageExtensions.Contains(extension))
        {
            reasons.Add($"Image extension {extension}");
            return FileTypeCategory.Image;
        }

        if (MediaExtensions.Contains(extension))
        {
            reasons.Add($"Media extension {extension}");
            return FileTypeCategory.Media;
        }

        if (ThreeDimensionalExtensions.Contains(extension))
        {
            reasons.Add($"3D/game-asset extension {extension}");
            return FileTypeCategory.ThreeDimensionalAsset;
        }

        if (AutomationExtensions.Contains(extension))
        {
            reasons.Add($"Automation extension {extension}");
            return FileTypeCategory.Automation;
        }

        if (DataExtensions.Contains(extension))
        {
            reasons.Add($"Data extension {extension}");
            return FileTypeCategory.Data;
        }

        if (ConfigurationExtensions.Contains(extension) || IsConfigurationName(file.Name))
        {
            reasons.Add("Configuration extension or filename");
            return FileTypeCategory.Configuration;
        }

        if (ArchiveExtensions.Contains(extension))
        {
            reasons.Add($"Archive extension {extension}");
            return FileTypeCategory.Archive;
        }

        if (BinaryExtensions.Contains(extension))
        {
            reasons.Add($"Binary/artifact extension {extension}");
            return FileTypeCategory.Binary;
        }

        reasons.Add(string.IsNullOrWhiteSpace(extension) ? "No extension rule" : $"Unrecognized extension {extension}");
        return FileTypeCategory.Unknown;
    }

    private static FunctionalRole DetectFunction(FileInfo file, string normalized, FileTypeCategory fileType, List<string> reasons)
    {
        if (MatchesPath(normalized, "test", "tests", "spec", "specs", "__tests__") || IsTestPath(file, normalized))
        {
            reasons.Add("Functional path indicates tests");
            return FunctionalRole.Test;
        }

        if (MatchesPath(normalized, "view", "views", "viewmodel", "viewmodels", "ui", "frontend", "renderer", "components")
            || file.Extension.Equals(".xaml", StringComparison.OrdinalIgnoreCase)
            || file.Extension.Equals(".tsx", StringComparison.OrdinalIgnoreCase))
        {
            reasons.Add("Functional path indicates user interface");
            return FunctionalRole.UserInterface;
        }

        if (MatchesPath(normalized, "service", "services", "server", "api", "backend"))
        {
            reasons.Add("Functional path indicates service/backend");
            return FunctionalRole.Service;
        }

        if (MatchesPath(normalized, "model", "models", "domain", "entities", "contracts", "types"))
        {
            reasons.Add("Functional path indicates domain model");
            return FunctionalRole.DomainModel;
        }

        if (MatchesPath(normalized, "interop", "bridge", "ipc", "integration", "integrations", "adapters", "connectors"))
        {
            reasons.Add("Functional path indicates integration/bridge");
            return FunctionalRole.Integration;
        }

        if (MatchesPath(normalized, "plugin", "plugins", "module", "modules")
            || file.Extension.Equals(".uplugin", StringComparison.OrdinalIgnoreCase))
        {
            reasons.Add("Functional path indicates plugin/module");
            return FunctionalRole.Plugin;
        }

        if (MatchesPath(normalized, "build", "cmake", "ci", "workflows") || IsBuildName(file.Name))
        {
            reasons.Add("Functional path indicates build infrastructure");
            return FunctionalRole.Build;
        }

        if (MatchesPath(normalized, "installer", "installers", "setup") || file.Extension.Equals(".wixproj", StringComparison.OrdinalIgnoreCase))
        {
            reasons.Add("Functional path indicates installer");
            return FunctionalRole.Installer;
        }

        if (fileType == FileTypeCategory.Automation || MatchesPath(normalized, "tool", "tools", "script", "scripts", "automation"))
        {
            reasons.Add("Functional path indicates automation/tooling");
            return FunctionalRole.Automation;
        }

        if (fileType == FileTypeCategory.Documentation)
        {
            if (normalized.Contains("analysis", StringComparison.Ordinal) || normalized.Contains("research", StringComparison.Ordinal))
            {
                reasons.Add("Documentation path indicates research/analysis");
                return FunctionalRole.Research;
            }

            return FunctionalRole.Documentation;
        }

        return fileType switch
        {
            FileTypeCategory.ReferenceMaterial => FunctionalRole.Research,
            FileTypeCategory.Image or FileTypeCategory.Media or FileTypeCategory.ThreeDimensionalAsset => FunctionalRole.Asset,
            FileTypeCategory.Configuration => FunctionalRole.Configuration,
            FileTypeCategory.Data => FunctionalRole.Data,
            FileTypeCategory.Source => FunctionalRole.Source,
            _ => FunctionalRole.Unknown
        };
    }

    private static double CalculateConfidence(FileTypeCategory fileType, FunctionalRole function, int evidenceCount, bool isProtected)
    {
        var score = fileType == FileTypeCategory.Unknown ? 0.35 : 0.72;
        if (function != FunctionalRole.Unknown) score += 0.12;
        if (evidenceCount > 1) score += 0.08;
        if (isProtected) score += 0.04;
        return Math.Round(Math.Clamp(score, 0.0, 0.99), 2);
    }

    private static RiskLevel CalculateRisk(
        FileTypeCategory fileType,
        FunctionalRole function,
        bool isProtected,
        string? componentRoot,
        string workspaceRoot)
    {
        if (isProtected) return RiskLevel.Protected;
        if (!string.IsNullOrWhiteSpace(componentRoot)
            && !string.Equals(PathSafety.NormalizeRoot(componentRoot), PathSafety.NormalizeRoot(workspaceRoot), StringComparison.OrdinalIgnoreCase))
        {
            return RiskLevel.High;
        }

        if (fileType is FileTypeCategory.Source or FileTypeCategory.Test or FileTypeCategory.Configuration or FileTypeCategory.Binary
            || function is FunctionalRole.Build or FunctionalRole.Installer or FunctionalRole.Plugin or FunctionalRole.Integration)
        {
            return RiskLevel.High;
        }

        return fileType == FileTypeCategory.Unknown ? RiskLevel.Medium : RiskLevel.Low;
    }

    private static bool IsTestPath(FileInfo file, string normalized) =>
        MatchesPath(normalized, "test", "tests", "spec", "specs", "__tests__")
        || file.Name.Contains(".test.", StringComparison.OrdinalIgnoreCase)
        || file.Name.Contains(".spec.", StringComparison.OrdinalIgnoreCase)
        || file.Name.EndsWith("Tests.cs", StringComparison.OrdinalIgnoreCase)
        || file.Name.EndsWith("Test.cs", StringComparison.OrdinalIgnoreCase);

    private static bool IsConfigurationName(string name) =>
        name.StartsWith("appsettings", StringComparison.OrdinalIgnoreCase)
        || name.StartsWith("settings", StringComparison.OrdinalIgnoreCase)
        || name.StartsWith("config", StringComparison.OrdinalIgnoreCase)
        || name.StartsWith(".env", StringComparison.OrdinalIgnoreCase)
        || name.Equals("package.json", StringComparison.OrdinalIgnoreCase)
        || name.Equals("pyproject.toml", StringComparison.OrdinalIgnoreCase)
        || name.Equals("Cargo.toml", StringComparison.OrdinalIgnoreCase);

    private static bool IsBuildName(string name) =>
        name.Equals("CMakeLists.txt", StringComparison.OrdinalIgnoreCase)
        || name.Equals("Makefile", StringComparison.OrdinalIgnoreCase)
        || name.EndsWith(".sln", StringComparison.OrdinalIgnoreCase)
        || name.EndsWith(".csproj", StringComparison.OrdinalIgnoreCase)
        || name.EndsWith(".vcxproj", StringComparison.OrdinalIgnoreCase)
        || name.EndsWith(".props", StringComparison.OrdinalIgnoreCase)
        || name.EndsWith(".targets", StringComparison.OrdinalIgnoreCase);

    private static bool MatchesPath(string normalized, params string[] segments)
    {
        var parts = normalized.Split('/', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
        return parts.Any(part => segments.Contains(part, StringComparer.OrdinalIgnoreCase));
    }
}
