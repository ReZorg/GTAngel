using GTAngel.Organizer.Core.Models;

namespace GTAngel.Organizer.Core.Services;

public sealed class OrganizationPlanner
{
    public OrganizationPlan CreatePlan(ScanResult scan, OrganizationConfig? config = null)
    {
        ArgumentNullException.ThrowIfNull(scan);
        config ??= new OrganizationConfig();
        var root = PathSafety.NormalizeRoot(scan.RootPath);
        var warnings = new List<string>(scan.Warnings);
        var entries = new List<MovePlanEntry>();
        var duplicateHashes = scan.Files
            .Where(file => !string.IsNullOrWhiteSpace(file.Sha256))
            .GroupBy(file => file.Sha256!, StringComparer.OrdinalIgnoreCase)
            .Where(group => group.Count() > 1)
            .ToDictionary(group => group.Key, group => group.ToArray(), StringComparer.OrdinalIgnoreCase);

        foreach (var file in scan.Files.OrderBy(file => file.RelativePath, StringComparer.OrdinalIgnoreCase))
        {
            if (file.IsProtected)
            {
                continue;
            }

            if (config.ProposeLooseRootFilesOnly
                && !string.Equals(Path.GetDirectoryName(file.SourcePath), root, StringComparison.OrdinalIgnoreCase))
            {
                continue;
            }

            if (file.FileType == FileTypeCategory.Source && !config.ProposeSourceFiles)
            {
                continue;
            }

            if (file.FileType == FileTypeCategory.Unknown && !config.ProposeUnknownFiles)
            {
                continue;
            }

            var baseDirectory = GetBaseDirectory(config, file.FileType);
            if (string.IsNullOrWhiteSpace(baseDirectory))
            {
                continue;
            }

            var functionalDirectory = GetFunctionalSubdirectory(file.FileType, file.Function);
            var relativeDestination = string.IsNullOrWhiteSpace(functionalDirectory)
                ? Path.Combine(baseDirectory, Path.GetFileName(file.SourcePath))
                : Path.Combine(baseDirectory, functionalDirectory, Path.GetFileName(file.SourcePath));
            var destination = Path.GetFullPath(Path.Combine(root, relativeDestination));

            if (!PathSafety.IsContainedBy(destination, root))
            {
                warnings.Add($"Rejected destination outside workspace: {destination}");
                continue;
            }

            if (string.Equals(file.SourcePath, destination, StringComparison.OrdinalIgnoreCase))
            {
                continue;
            }

            var isDuplicate = !string.IsNullOrWhiteSpace(file.Sha256)
                && duplicateHashes.ContainsKey(file.Sha256!);
            var hasConflict = File.Exists(destination) || Directory.Exists(destination);
            var conflictReason = hasConflict ? "Destination already exists" : null;

            if (hasConflict && File.Exists(destination) && !string.IsNullOrWhiteSpace(file.Sha256))
            {
                var destinationInfo = new FileInfo(destination);
                if (destinationInfo.Length == file.Length)
                {
                    var destinationHash = ComputeSha256(destination);
                    if (string.Equals(destinationHash, file.Sha256, StringComparison.OrdinalIgnoreCase))
                    {
                        isDuplicate = true;
                        conflictReason = "Destination contains identical content";
                    }
                }
            }

            if (hasConflict && config.ConflictResolution == ConflictResolution.RenameWithSuffix)
            {
                destination = FindUniqueDestination(destination);
                relativeDestination = PathSafety.GetSafeRelativePath(root, destination);
                hasConflict = false;
                conflictReason = "Destination renamed with a deterministic numeric suffix";
            }

            var status = isDuplicate
                ? PlanEntryStatus.Duplicate
                : hasConflict
                    ? PlanEntryStatus.Conflict
                    : PlanEntryStatus.Proposed;
            var selected = file.Risk == RiskLevel.Low
                && file.Confidence >= 0.75
                && !hasConflict
                && !isDuplicate;

            entries.Add(new MovePlanEntry
            {
                Id = Guid.NewGuid(),
                SourcePath = file.SourcePath,
                DestinationPath = destination,
                RelativeSourcePath = file.RelativePath,
                RelativeDestinationPath = relativeDestination,
                FileType = file.FileType,
                Function = file.Function,
                Confidence = file.Confidence,
                Reason = file.Reason,
                Risk = file.Risk,
                IsSelected = selected,
                HasConflict = hasConflict,
                IsDuplicate = isDuplicate,
                ConflictReason = conflictReason,
                ExpectedLength = file.Length,
                ExpectedLastWriteTimeUtc = file.LastWriteTimeUtc,
                ExpectedSha256 = file.Sha256,
                Status = status
            });
        }

        return new OrganizationPlan
        {
            Id = Guid.NewGuid(),
            RootPath = root,
            CreatedAtUtc = DateTime.UtcNow,
            Config = config,
            Entries = entries,
            Warnings = warnings.Distinct(StringComparer.OrdinalIgnoreCase).ToList()
        };
    }

    private static string GetBaseDirectory(OrganizationConfig config, FileTypeCategory fileType) => fileType switch
    {
        FileTypeCategory.Source => "src/intake",
        FileTypeCategory.Test => "tests/intake",
        FileTypeCategory.Documentation => config.DocumentationDirectory,
        FileTypeCategory.ReferenceMaterial => config.ReferenceDirectory,
        FileTypeCategory.Image => config.ImageDirectory,
        FileTypeCategory.Media => config.MediaDirectory,
        FileTypeCategory.ThreeDimensionalAsset => config.ThreeDimensionalAssetDirectory,
        FileTypeCategory.Automation => config.AutomationDirectory,
        FileTypeCategory.Configuration => config.ConfigurationDirectory,
        FileTypeCategory.Data => config.DataDirectory,
        FileTypeCategory.Archive => config.ArchiveDirectory,
        FileTypeCategory.Binary => config.BinaryDirectory,
        FileTypeCategory.Unknown => config.UnknownDirectory,
        _ => config.UnknownDirectory
    };

    private static string GetFunctionalSubdirectory(FileTypeCategory fileType, FunctionalRole function)
    {
        if (function is FunctionalRole.Unknown or FunctionalRole.Asset or FunctionalRole.Documentation
            or FunctionalRole.Automation or FunctionalRole.Configuration or FunctionalRole.Data or FunctionalRole.Source)
        {
            return string.Empty;
        }

        return (fileType, function) switch
        {
            (FileTypeCategory.ReferenceMaterial, FunctionalRole.Research) => "research",
            (FileTypeCategory.Documentation, FunctionalRole.Research) => "research",
            (_, FunctionalRole.UserInterface) => "ui",
            (_, FunctionalRole.Service) => "services",
            (_, FunctionalRole.DomainModel) => "models",
            (_, FunctionalRole.Integration) => "integration",
            (_, FunctionalRole.Plugin) => "plugins",
            (_, FunctionalRole.Build) => "build",
            (_, FunctionalRole.Installer) => "installer",
            (_, FunctionalRole.Test) => "tests",
            _ => function.ToString().ToLowerInvariant()
        };
    }

    private static string FindUniqueDestination(string destination)
    {
        var directory = Path.GetDirectoryName(destination) ?? throw new InvalidOperationException("Destination has no directory.");
        var name = Path.GetFileNameWithoutExtension(destination);
        var extension = Path.GetExtension(destination);
        for (var index = 2; index < 10_000; index++)
        {
            var candidate = Path.Combine(directory, $"{name} ({index}){extension}");
            if (!File.Exists(candidate) && !Directory.Exists(candidate))
            {
                return candidate;
            }
        }

        throw new IOException($"Could not find a unique destination for '{destination}'.");
    }

    private static string ComputeSha256(string path)
    {
        using var stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read, 131_072, FileOptions.SequentialScan);
        return Convert.ToHexString(System.Security.Cryptography.SHA256.HashData(stream));
    }
}
