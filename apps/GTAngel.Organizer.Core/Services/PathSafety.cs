using GTAngel.Organizer.Core.Models;

namespace GTAngel.Organizer.Core.Services;

public static class PathSafety
{
    public static readonly HashSet<string> ExcludedDirectoryNames = new(StringComparer.OrdinalIgnoreCase)
    {
        ".git", ".vs", ".idea", ".vscode", ".cache", ".next", ".nuxt",
        "node_modules", "packages", "bin", "obj", "build", "dist", "out",
        "coverage", "Binaries", "DerivedDataCache", "Intermediate", "Saved",
        "__pycache__", ".pytest_cache", ".mypy_cache", ".venv", "venv",
        ".gtangel-organizer", "KSM-ORGANIZED"
    };

    public static readonly Dictionary<string, (ComponentKind Kind, string Technology)> ManifestNames =
        new(StringComparer.OrdinalIgnoreCase)
        {
            ["package.json"] = (ComponentKind.NodeProject, "Node/TypeScript"),
            ["pyproject.toml"] = (ComponentKind.PythonProject, "Python"),
            ["requirements.txt"] = (ComponentKind.PythonProject, "Python"),
            ["Pipfile"] = (ComponentKind.PythonProject, "Python"),
            ["Cargo.toml"] = (ComponentKind.CppProject, "Rust"),
            ["CMakeLists.txt"] = (ComponentKind.CppProject, "C/C++"),
            ["Makefile"] = (ComponentKind.CppProject, "C/C++")
        };

    public static readonly Dictionary<string, (ComponentKind Kind, string Technology)> ManifestExtensions =
        new(StringComparer.OrdinalIgnoreCase)
        {
            [".sln"] = (ComponentKind.DotNetProject, ".NET"),
            [".csproj"] = (ComponentKind.DotNetProject, ".NET/C#"),
            [".fsproj"] = (ComponentKind.DotNetProject, ".NET/F#"),
            [".vbproj"] = (ComponentKind.DotNetProject, ".NET/VB"),
            [".vcxproj"] = (ComponentKind.CppProject, "Visual C++"),
            [".wixproj"] = (ComponentKind.Installer, "WiX"),
            [".uproject"] = (ComponentKind.UnrealProject, "Unreal Engine"),
            [".uplugin"] = (ComponentKind.UnrealPlugin, "Unreal Engine")
        };

    public static string NormalizeRoot(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        return Path.GetFullPath(path).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
    }

    public static bool IsContainedBy(string candidatePath, string rootPath)
    {
        var root = NormalizeRoot(rootPath) + Path.DirectorySeparatorChar;
        var candidate = Path.GetFullPath(candidatePath);
        return candidate.StartsWith(root, StringComparison.OrdinalIgnoreCase)
            || string.Equals(candidate.TrimEnd(Path.DirectorySeparatorChar), root.TrimEnd(Path.DirectorySeparatorChar), StringComparison.OrdinalIgnoreCase);
    }

    public static bool IsExcludedDirectory(string directoryPath) =>
        ExcludedDirectoryNames.Contains(Path.GetFileName(directoryPath.TrimEnd(Path.DirectorySeparatorChar)));

    public static bool TryGetManifest(string filePath, out ComponentKind kind, out string technology)
    {
        var fileName = Path.GetFileName(filePath);
        if (ManifestNames.TryGetValue(fileName, out var byName))
        {
            kind = byName.Kind;
            technology = byName.Technology;
            return true;
        }

        if (ManifestExtensions.TryGetValue(Path.GetExtension(filePath), out var byExtension))
        {
            kind = byExtension.Kind;
            technology = byExtension.Technology;
            return true;
        }

        kind = ComponentKind.Mixed;
        technology = string.Empty;
        return false;
    }

    public static string? FindNearestBoundary(string filePath, string workspaceRoot, IReadOnlySet<string> boundaries)
    {
        var root = NormalizeRoot(workspaceRoot);
        var current = Directory.Exists(filePath)
            ? new DirectoryInfo(filePath)
            : new FileInfo(filePath).Directory;

        while (current is not null && IsContainedBy(current.FullName, root))
        {
            if (boundaries.Contains(NormalizeRoot(current.FullName)))
            {
                return NormalizeRoot(current.FullName);
            }

            if (string.Equals(NormalizeRoot(current.FullName), root, StringComparison.OrdinalIgnoreCase))
            {
                break;
            }

            current = current.Parent;
        }

        return null;
    }

    public static string GetSafeRelativePath(string rootPath, string fullPath)
    {
        var relative = Path.GetRelativePath(NormalizeRoot(rootPath), Path.GetFullPath(fullPath));
        if (relative == ".." || relative.StartsWith($"..{Path.DirectorySeparatorChar}", StringComparison.Ordinal))
        {
            throw new InvalidOperationException($"Path '{fullPath}' is outside workspace '{rootPath}'.");
        }

        return relative;
    }
}
