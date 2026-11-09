namespace GTAngel.Organizer.Core.Models;

public enum ComponentKind
{
    Workspace,
    GitRepository,
    DotNetProject,
    NodeProject,
    PythonProject,
    CppProject,
    UnrealProject,
    UnrealPlugin,
    Installer,
    Documentation,
    Assets,
    Mixed
}

public enum FileTypeCategory
{
    Source,
    Test,
    Documentation,
    ReferenceMaterial,
    Image,
    Media,
    ThreeDimensionalAsset,
    Automation,
    Configuration,
    Data,
    Archive,
    Binary,
    Unknown
}

public enum FunctionalRole
{
    UserInterface,
    Service,
    DomainModel,
    Integration,
    Plugin,
    Build,
    Installer,
    Automation,
    Test,
    Documentation,
    Research,
    Asset,
    Configuration,
    Data,
    Source,
    Unknown
}

public enum RiskLevel
{
    Low,
    Medium,
    High,
    Protected
}

public enum PlanEntryStatus
{
    Proposed,
    Skipped,
    Conflict,
    Duplicate,
    Prepared,
    Completed,
    Failed,
    Undone
}

public enum ConflictResolution
{
    Review,
    Skip,
    RenameWithSuffix
}

public enum TransactionStatus
{
    Prepared,
    Running,
    Completed,
    PartiallyCompleted,
    Failed,
    Undoing,
    Undone,
    PartiallyUndone
}

public enum WatcherLifecycle
{
    Stopped,
    Starting,
    Watching,
    Paused,
    Stopping,
    Faulted
}
