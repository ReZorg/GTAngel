using System.Text.Json.Serialization;

namespace GTAngel.Organizer.Core.Models;

public sealed record ManifestInfo(
    string Path,
    string Name,
    ComponentKind Kind,
    string Technology);

public sealed record RepositoryComponent
{
    public required string Name { get; init; }
    public required string RootPath { get; init; }
    public required ComponentKind Kind { get; init; }
    public bool IsRepository { get; init; }
    public string? GitBranch { get; init; }
    public string? GitRemote { get; init; }
    public List<ManifestInfo> Manifests { get; init; } = [];
    public List<string> Technologies { get; init; } = [];
    public int FileCount { get; init; }
    public long TotalBytes { get; init; }
    public int ProtectedFileCount { get; init; }
    public int WarningCount { get; init; }
}

public sealed record FileClassification
{
    public required string SourcePath { get; init; }
    public required string RelativePath { get; init; }
    public required FileTypeCategory FileType { get; init; }
    public required FunctionalRole Function { get; init; }
    public required double Confidence { get; init; }
    public required string Reason { get; init; }
    public required RiskLevel Risk { get; init; }
    public bool IsProtected { get; init; }
    public string? ProtectedBy { get; init; }
    public string? ComponentRoot { get; init; }
    public long Length { get; init; }
    public DateTime LastWriteTimeUtc { get; init; }
    public string? Sha256 { get; init; }
}

public sealed record ScanOptions
{
    public bool IncludeSubdirectories { get; init; } = true;
    public bool IncludeHiddenFiles { get; init; }
    public bool HashLooseFiles { get; init; } = true;
    public bool CatalogProtectedFiles { get; init; } = true;
    public bool GenerateMoveProposalsForLooseRootFilesOnly { get; init; } = true;
    public int MaximumFiles { get; init; } = 100_000;
    public int MaximumDepth { get; init; } = 32;
}

public sealed record ScanProgress(
    string Stage,
    int ProcessedFiles,
    int DiscoveredFiles,
    string? CurrentPath,
    double Percent);

public sealed record ScanResult
{
    public required string RootPath { get; init; }
    public required DateTime StartedAtUtc { get; init; }
    public required DateTime CompletedAtUtc { get; init; }
    public List<RepositoryComponent> Components { get; init; } = [];
    public List<FileClassification> Files { get; init; } = [];
    public List<string> Warnings { get; init; } = [];
    public bool WasCancelled { get; init; }
    public bool WasTruncated { get; init; }

    [JsonIgnore]
    public int ProtectedFileCount => Files.Count(file => file.IsProtected);

    [JsonIgnore]
    public int LooseFileCount => Files.Count(file => !file.IsProtected);
}

public sealed record OrganizationConfig
{
    public string StateDirectoryName { get; init; } = ".gtangel-organizer";
    public string DocumentationDirectory { get; init; } = "docs/general";
    public string ReferenceDirectory { get; init; } = "docs/reference";
    public string ImageDirectory { get; init; } = "assets/images";
    public string MediaDirectory { get; init; } = "assets/media";
    public string ThreeDimensionalAssetDirectory { get; init; } = "assets/3d";
    public string AutomationDirectory { get; init; } = "tools/scripts";
    public string ConfigurationDirectory { get; init; } = "config";
    public string DataDirectory { get; init; } = "data";
    public string ArchiveDirectory { get; init; } = "archives";
    public string BinaryDirectory { get; init; } = "artifacts/binaries";
    public string UnknownDirectory { get; init; } = "other/review";
    public bool ProposeUnknownFiles { get; init; }
    public bool ProposeSourceFiles { get; init; }
    public bool ProposeLooseRootFilesOnly { get; init; } = true;
    public ConflictResolution ConflictResolution { get; init; } = ConflictResolution.Review;
    public int WatchDebounceMilliseconds { get; init; } = 1500;
    public bool WatchLooseRootOnly { get; init; } = true;
}

public sealed record MovePlanEntry
{
    public required Guid Id { get; init; }
    public required string SourcePath { get; init; }
    public required string DestinationPath { get; set; }
    public required string RelativeSourcePath { get; init; }
    public required string RelativeDestinationPath { get; set; }
    public required FileTypeCategory FileType { get; init; }
    public required FunctionalRole Function { get; init; }
    public required double Confidence { get; init; }
    public required string Reason { get; init; }
    public required RiskLevel Risk { get; init; }
    public bool IsSelected { get; set; }
    public bool HasConflict { get; set; }
    public bool IsDuplicate { get; set; }
    public string? ConflictReason { get; set; }
    public required long ExpectedLength { get; init; }
    public required DateTime ExpectedLastWriteTimeUtc { get; init; }
    public string? ExpectedSha256 { get; init; }
    public PlanEntryStatus Status { get; set; } = PlanEntryStatus.Proposed;
    public string? Error { get; set; }
}

public sealed record OrganizationPlan
{
    public required Guid Id { get; init; }
    public required string RootPath { get; init; }
    public required DateTime CreatedAtUtc { get; init; }
    public required OrganizationConfig Config { get; init; }
    public List<MovePlanEntry> Entries { get; init; } = [];
    public List<string> Warnings { get; init; } = [];

    [JsonIgnore]
    public int SelectedCount => Entries.Count(entry => entry.IsSelected);

    [JsonIgnore]
    public int ConflictCount => Entries.Count(entry => entry.HasConflict);

    [JsonIgnore]
    public int DuplicateCount => Entries.Count(entry => entry.IsDuplicate);
}

public sealed record OperationRecord
{
    public required Guid PlanEntryId { get; init; }
    public required string SourcePath { get; init; }
    public required string DestinationPath { get; init; }
    public required long ExpectedLength { get; init; }
    public required DateTime ExpectedLastWriteTimeUtc { get; init; }
    public string? ExpectedSha256 { get; init; }
    public PlanEntryStatus Status { get; set; } = PlanEntryStatus.Prepared;
    public DateTime? CompletedAtUtc { get; set; }
    public DateTime? UndoneAtUtc { get; set; }
    public string? Error { get; set; }
}

public sealed record TransactionJournal
{
    public required Guid Id { get; init; }
    public required Guid PlanId { get; init; }
    public required string RootPath { get; init; }
    public required DateTime CreatedAtUtc { get; init; }
    public DateTime? CompletedAtUtc { get; set; }
    public DateTime? UndoneAtUtc { get; set; }
    public TransactionStatus Status { get; set; } = TransactionStatus.Prepared;
    public List<OperationRecord> Operations { get; init; } = [];
    public List<string> Errors { get; init; } = [];
    public string? JournalPath { get; set; }
}

public sealed record WatchRecommendation(
    DateTime DetectedAtUtc,
    string Path,
    string ChangeType,
    FileClassification? Classification,
    string Status,
    string? Error = null);
