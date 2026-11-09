using System.Diagnostics;
using System.Security.Cryptography;
using GTAngel.Organizer.Core.Models;

namespace GTAngel.Organizer.Core.Services;

public sealed class RepositoryScanner
{
    private static readonly HashSet<string> ProtectedWorkspaceFileNames = new(StringComparer.OrdinalIgnoreCase)
    {
        ".gitignore", ".gitattributes", ".editorconfig", "AGENTS.md", "README.md",
        "CHANGELOG.md", "LICENSE", "LICENSE.md", "Directory.Build.props",
        "Directory.Build.targets", "global.json", "NuGet.config"
    };

    private readonly FileClassifier _classifier;

    public RepositoryScanner(FileClassifier? classifier = null)
    {
        _classifier = classifier ?? new FileClassifier();
    }

    public async Task<ScanResult> ScanAsync(
        string rootPath,
        ScanOptions? options = null,
        IProgress<ScanProgress>? progress = null,
        CancellationToken cancellationToken = default)
    {
        options ??= new ScanOptions();
        var started = DateTime.UtcNow;
        var root = PathSafety.NormalizeRoot(rootPath);
        if (!Directory.Exists(root))
        {
            throw new DirectoryNotFoundException($"Workspace '{root}' does not exist.");
        }

        var warnings = new List<string>();
        var discoveredFiles = new List<FileInfo>();
        var protectedBoundaries = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        var componentRoots = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        var manifestsByDirectory = new Dictionary<string, List<ManifestInfo>>(StringComparer.OrdinalIgnoreCase);
        var repositoryRoots = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        var wasTruncated = false;

        var rootInfo = new DirectoryInfo(root);
        foreach (var directory in SafeEnumerateDirectories(rootInfo, warnings))
        {
            if (!PathSafety.IsExcludedDirectory(directory.FullName))
            {
                componentRoots.Add(PathSafety.NormalizeRoot(directory.FullName));
            }
        }

        var queue = new Queue<(DirectoryInfo Directory, int Depth)>();
        queue.Enqueue((rootInfo, 0));
        progress?.Report(new ScanProgress("Discovering", 0, 0, root, 0));

        while (queue.Count > 0)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var (directory, depth) = queue.Dequeue();
            if (depth > options.MaximumDepth)
            {
                wasTruncated = true;
                continue;
            }

            if ((directory.Attributes & FileAttributes.ReparsePoint) != 0 && depth > 0)
            {
                warnings.Add($"Skipped reparse-point directory: {directory.FullName}");
                continue;
            }

            var normalizedDirectory = PathSafety.NormalizeRoot(directory.FullName);
            var hasGitBoundary = Directory.Exists(Path.Combine(directory.FullName, ".git"))
                || File.Exists(Path.Combine(directory.FullName, ".git"));
            if (hasGitBoundary && !string.Equals(normalizedDirectory, root, StringComparison.OrdinalIgnoreCase))
            {
                protectedBoundaries.Add(normalizedDirectory);
                componentRoots.Add(normalizedDirectory);
                repositoryRoots.Add(normalizedDirectory);
            }

            foreach (var file in SafeEnumerateFiles(directory, warnings))
            {
                cancellationToken.ThrowIfCancellationRequested();
                if (!options.IncludeHiddenFiles && IsHiddenOrSystem(file))
                {
                    continue;
                }

                discoveredFiles.Add(file);
                if (PathSafety.TryGetManifest(file.FullName, out var kind, out var technology))
                {
                    if (!manifestsByDirectory.TryGetValue(normalizedDirectory, out var manifests))
                    {
                        manifests = [];
                        manifestsByDirectory[normalizedDirectory] = manifests;
                    }

                    manifests.Add(new ManifestInfo(file.FullName, file.Name, kind, technology));
                    if (!string.Equals(normalizedDirectory, root, StringComparison.OrdinalIgnoreCase))
                    {
                        protectedBoundaries.Add(normalizedDirectory);
                        var isInsideRepository = repositoryRoots.Any(repositoryRoot =>
                            !string.Equals(repositoryRoot, normalizedDirectory, StringComparison.OrdinalIgnoreCase)
                            && PathSafety.IsContainedBy(normalizedDirectory, repositoryRoot));
                        if (!isInsideRepository && ShouldRegisterNestedComponent(kind, file, depth))
                        {
                            componentRoots.Add(normalizedDirectory);
                        }
                    }
                }

                if (discoveredFiles.Count >= options.MaximumFiles)
                {
                    wasTruncated = true;
                    warnings.Add($"Scan stopped at the configured maximum of {options.MaximumFiles:N0} files.");
                    queue.Clear();
                    break;
                }
            }

            if (options.IncludeSubdirectories && discoveredFiles.Count < options.MaximumFiles)
            {
                foreach (var child in SafeEnumerateDirectories(directory, warnings))
                {
                    if (PathSafety.IsExcludedDirectory(child.FullName))
                    {
                        continue;
                    }

                    queue.Enqueue((child, depth + 1));
                }
            }

            if (discoveredFiles.Count % 250 == 0)
            {
                progress?.Report(new ScanProgress("Discovering", discoveredFiles.Count, discoveredFiles.Count, directory.FullName, 0));
                await Task.Yield();
            }
        }

        var orderedBoundaries = protectedBoundaries
            .OrderByDescending(path => path.Length)
            .ToArray();
        var orderedComponentRoots = componentRoots
            .OrderByDescending(path => path.Length)
            .ToArray();
        var classifications = new List<FileClassification>(discoveredFiles.Count);

        for (var index = 0; index < discoveredFiles.Count; index++)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var file = discoveredFiles[index];
            var componentRoot = FindNearestRoot(file.FullName, orderedComponentRoots);
            var protectedBoundary = FindNearestRoot(file.FullName, orderedBoundaries);
            var isManifest = PathSafety.TryGetManifest(file.FullName, out _, out _);
            var isRootStructuralFile = string.Equals(file.DirectoryName, root, StringComparison.OrdinalIgnoreCase)
                && ProtectedWorkspaceFileNames.Contains(file.Name);
            var isProtected = protectedBoundary is not null || isManifest || isRootStructuralFile;
            var protectedBy = protectedBoundary is not null
                ? $"component boundary '{Path.GetFileName(protectedBoundary)}'"
                : isManifest
                    ? "project manifest"
                    : isRootStructuralFile
                        ? "workspace structural file"
                        : null;

            string? hash = null;
            var isLooseRootFile = string.Equals(file.DirectoryName, root, StringComparison.OrdinalIgnoreCase) && !isProtected;
            if (options.HashLooseFiles && isLooseRootFile)
            {
                hash = await ComputeSha256Async(file.FullName, cancellationToken).ConfigureAwait(false);
            }

            classifications.Add(_classifier.Classify(file, root, componentRoot, isProtected, protectedBy, hash));
            if (index % 50 == 0 || index == discoveredFiles.Count - 1)
            {
                var percent = discoveredFiles.Count == 0 ? 100 : (index + 1) * 100.0 / discoveredFiles.Count;
                progress?.Report(new ScanProgress("Classifying", index + 1, discoveredFiles.Count, file.FullName, percent));
            }
        }

        var components = BuildComponents(
            root,
            classifications,
            componentRoots,
            manifestsByDirectory,
            repositoryRoots,
            warnings);

        progress?.Report(new ScanProgress("Complete", classifications.Count, classifications.Count, root, 100));
        return new ScanResult
        {
            RootPath = root,
            StartedAtUtc = started,
            CompletedAtUtc = DateTime.UtcNow,
            Components = components,
            Files = classifications,
            Warnings = warnings.Distinct(StringComparer.OrdinalIgnoreCase).ToList(),
            WasTruncated = wasTruncated
        };
    }

    private static List<RepositoryComponent> BuildComponents(
        string root,
        IReadOnlyCollection<FileClassification> classifications,
        IEnumerable<string> componentRoots,
        IReadOnlyDictionary<string, List<ManifestInfo>> manifestsByDirectory,
        IReadOnlySet<string> repositoryRoots,
        IReadOnlyCollection<string> warnings)
    {
        var components = new List<RepositoryComponent>();
        foreach (var componentRoot in componentRoots.Distinct(StringComparer.OrdinalIgnoreCase).OrderBy(path => path))
        {
            var files = classifications
                .Where(file => string.Equals(file.ComponentRoot, componentRoot, StringComparison.OrdinalIgnoreCase))
                .ToArray();
            manifestsByDirectory.TryGetValue(componentRoot, out var manifests);
            manifests ??= [];
            var kind = InferKind(componentRoot, manifests, repositoryRoots.Contains(componentRoot), files);
            var technologies = InferTechnologies(manifests, files);
            var warningCount = warnings.Count(warning => warning.Contains(componentRoot, StringComparison.OrdinalIgnoreCase));
            var git = repositoryRoots.Contains(componentRoot)
                ? ReadGitMetadata(componentRoot)
                : (Branch: (string?)null, Remote: (string?)null);

            components.Add(new RepositoryComponent
            {
                Name = Path.GetFileName(componentRoot),
                RootPath = componentRoot,
                Kind = kind,
                IsRepository = repositoryRoots.Contains(componentRoot),
                GitBranch = git.Branch,
                GitRemote = git.Remote,
                Manifests = manifests,
                Technologies = technologies,
                FileCount = files.Length,
                TotalBytes = files.Sum(file => file.Length),
                ProtectedFileCount = files.Count(file => file.IsProtected),
                WarningCount = warningCount
            });
        }

        return components;
    }

    private static bool ShouldRegisterNestedComponent(ComponentKind kind, FileInfo manifest, int depth)
    {
        if (kind is ComponentKind.UnrealProject or ComponentKind.UnrealPlugin) return true;
        if (depth <= 2) return true;
        if (depth <= 4 && kind is ComponentKind.DotNetProject or ComponentKind.NodeProject or ComponentKind.PythonProject or ComponentKind.Installer)
        {
            return true;
        }

        return false;
    }

    private static ComponentKind InferKind(
        string componentRoot,
        IReadOnlyCollection<ManifestInfo> manifests,
        bool isRepository,
        IReadOnlyCollection<FileClassification> files)
    {
        if (manifests.Any(manifest => manifest.Kind == ComponentKind.UnrealProject)) return ComponentKind.UnrealProject;
        if (manifests.Any(manifest => manifest.Kind == ComponentKind.UnrealPlugin)) return ComponentKind.UnrealPlugin;
        if (manifests.Any(manifest => manifest.Kind == ComponentKind.DotNetProject)) return ComponentKind.DotNetProject;
        if (manifests.Any(manifest => manifest.Kind == ComponentKind.NodeProject)) return ComponentKind.NodeProject;
        if (manifests.Any(manifest => manifest.Kind == ComponentKind.PythonProject)) return ComponentKind.PythonProject;
        if (manifests.Any(manifest => manifest.Kind == ComponentKind.CppProject)) return ComponentKind.CppProject;
        if (manifests.Any(manifest => manifest.Kind == ComponentKind.Installer)) return ComponentKind.Installer;
        if (isRepository) return ComponentKind.GitRepository;
        if (files.Count > 0 && files.All(file => file.FileType is FileTypeCategory.Image or FileTypeCategory.Media or FileTypeCategory.ThreeDimensionalAsset)) return ComponentKind.Assets;
        if (files.Count > 0 && files.All(file => file.FileType is FileTypeCategory.Documentation or FileTypeCategory.ReferenceMaterial)) return ComponentKind.Documentation;
        return ComponentKind.Mixed;
    }

    private static List<string> InferTechnologies(
        IEnumerable<ManifestInfo> manifests,
        IReadOnlyCollection<FileClassification> files)
    {
        var technologies = new HashSet<string>(manifests.Select(manifest => manifest.Technology), StringComparer.OrdinalIgnoreCase);
        foreach (var extension in files.Select(file => Path.GetExtension(file.SourcePath)).Distinct(StringComparer.OrdinalIgnoreCase))
        {
            switch (extension.ToLowerInvariant())
            {
                case ".cs":
                case ".xaml": technologies.Add(".NET/WPF"); break;
                case ".cpp":
                case ".c":
                case ".h":
                case ".hpp": technologies.Add("C/C++"); break;
                case ".ts":
                case ".tsx":
                case ".js":
                case ".jsx": technologies.Add("Node/TypeScript"); break;
                case ".py": technologies.Add("Python"); break;
                case ".rs": technologies.Add("Rust"); break;
                case ".uasset":
                case ".umap": technologies.Add("Unreal Engine"); break;
            }
        }

        return technologies.Where(value => !string.IsNullOrWhiteSpace(value)).Order(StringComparer.OrdinalIgnoreCase).ToList();
    }

    private static IEnumerable<FileInfo> SafeEnumerateFiles(DirectoryInfo directory, ICollection<string> warnings)
    {
        try
        {
            return directory.EnumerateFiles().ToArray();
        }
        catch (Exception exception) when (exception is UnauthorizedAccessException or IOException)
        {
            warnings.Add($"Could not enumerate files in '{directory.FullName}': {exception.Message}");
            return [];
        }
    }

    private static IEnumerable<DirectoryInfo> SafeEnumerateDirectories(DirectoryInfo directory, ICollection<string> warnings)
    {
        try
        {
            return directory.EnumerateDirectories().ToArray();
        }
        catch (Exception exception) when (exception is UnauthorizedAccessException or IOException)
        {
            warnings.Add($"Could not enumerate directories in '{directory.FullName}': {exception.Message}");
            return [];
        }
    }

    private static bool IsHiddenOrSystem(FileInfo file) =>
        (file.Attributes & (FileAttributes.Hidden | FileAttributes.System)) != 0;

    private static string? FindNearestRoot(string filePath, IEnumerable<string> roots)
    {
        var fullPath = Path.GetFullPath(filePath);
        foreach (var root in roots)
        {
            if (PathSafety.IsContainedBy(fullPath, root))
            {
                return root;
            }
        }

        return null;
    }

    private static async Task<string> ComputeSha256Async(string path, CancellationToken cancellationToken)
    {
        await using var stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read, 131_072, FileOptions.Asynchronous | FileOptions.SequentialScan);
        var hash = await SHA256.HashDataAsync(stream, cancellationToken).ConfigureAwait(false);
        return Convert.ToHexString(hash);
    }

    private static (string? Branch, string? Remote) ReadGitMetadata(string repositoryRoot)
    {
        try
        {
            var branch = RunGit(repositoryRoot, "branch --show-current");
            var remote = RunGit(repositoryRoot, "remote get-url origin");
            return (string.IsNullOrWhiteSpace(branch) ? null : branch, string.IsNullOrWhiteSpace(remote) ? null : remote);
        }
        catch
        {
            return (null, null);
        }
    }

    private static string? RunGit(string repositoryRoot, string arguments)
    {
        using var process = Process.Start(new ProcessStartInfo
        {
            FileName = "git",
            Arguments = $"-C \"{repositoryRoot}\" {arguments}",
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true
        });
        if (process is null) return null;
        var output = process.StandardOutput.ReadToEnd();
        if (!process.WaitForExit(3000) || process.ExitCode != 0) return null;
        return output.Trim();
    }
}
