namespace GTAngel.Models;

/// <summary>
/// A single training episode in the game world environment.
/// Mirrors angelclaw's FTrainingEpisode struct.
/// </summary>
public class TrainingEpisode
{
    public int EpisodeId { get; set; }
    public DateTime StartTime { get; set; }
    public DateTime? EndTime { get; set; }
    public int Steps { get; set; }
    public double TotalReward { get; set; }
    public double AverageReward => Steps > 0 ? TotalReward / Steps : 0;
    public double MaxReward { get; set; }
    public double MinReward { get; set; }
    public bool IsComplete { get; set; }
    public EpisodeTermination TerminationReason { get; set; }

    // Game-specific metrics
    public double ExplorationCoverage { get; set; }     // 0-1
    public int CombatEncounters { get; set; }
    public int SkillsUsed { get; set; }
    public double NavigationEfficiency { get; set; }    // 0-1

    // DTE metrics
    public double StreamCoherence { get; set; }
    public double PropertyCoherence { get; set; }
    public int CognitiveCyclesCompleted { get; set; }
    public AutonomyLevel AutonomyLevelReached { get; set; }

    public TimeSpan Duration => (EndTime ?? DateTime.UtcNow) - StartTime;
}

public enum EpisodeTermination
{
    Running,
    Success,
    Timeout,
    Death,
    CoherenceHalt,
    ManualStop,
    CurriculumAdvance
}

/// <summary>
/// DTE Autonomy Levels (0-5) from the dte-autonomy-evolution framework.
/// Level numbering starts at 0 (root).
/// </summary>
public enum AutonomyLevel
{
    Reactive = 0,       // Level 0: Stimulus-response only
    Adaptive = 1,       // Level 1: Parameter adaptation
    Strategic = 2,      // Level 2: Goal-directed planning
    Cognitive = 3,      // Level 3: Self-model + introspection
    Embodied = 4,       // Level 4: 4E cognition integration
    Autonomous = 5      // Level 5: Self-modification (Autogenesis)
}

/// <summary>
/// Autogenesis experiment record — one row in results.tsv.
/// </summary>
public class AutogenesisExperiment
{
    public int ExperimentId { get; set; }
    public string Description { get; set; } = string.Empty;
    public string Hypothesis { get; set; } = string.Empty;
    public DateTime Timestamp { get; set; } = DateTime.UtcNow;

    // Metrics
    public double PrimaryMetric { get; set; }
    public double BaselineMetric { get; set; }
    public double MetricDelta => PrimaryMetric - BaselineMetric;
    public bool MetricImproved => PrimaryMetric > BaselineMetric;

    // Property Coherence (Alexander's 15 Properties)
    public double PropertyCoherenceScore { get; set; }
    public Dictionary<string, double> PropertyScores { get; set; } = new();

    // Decision
    public ExperimentStatus Status { get; set; }
    public string StatusReason { get; set; } = string.Empty;

    // KSM Step
    public int KsmStep { get; set; }
    public string KsmStepName => KsmStepNames[KsmStep % 12];

    public static readonly string[] KsmStepNames =
    {
        "Observe",          // 0
        "Diagnose",         // 1
        "Hypothesize",      // 2
        "Design",           // 3
        "Implement",        // 4
        "Test",             // 5
        "Measure",          // 6
        "Evaluate",         // 7
        "Integrate",        // 8
        "Consolidate",      // 9
        "Reflect",          // 10
        "Evolve"            // 11
    };
}

public enum ExperimentStatus
{
    Pending,
    Running,
    Keep,
    Discard,
    Crash,
    Baseline
}

/// <summary>
/// Alexander's 15 Properties of Living Structure.
/// Used for property coherence assessment in the KSM evolution cycle.
/// </summary>
public class AlexanderProperty
{
    public int Index { get; set; }
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public double Score { get; set; }       // 0.0-1.0
    public double PreviousScore { get; set; }
    public double Delta => Score - PreviousScore;

    public static List<AlexanderProperty> CreateAll() => new()
    {
        new() { Index = 0, Name = "Levels of Scale", Description = "Hierarchical nesting of centers at different scales" },
        new() { Index = 1, Name = "Strong Centers", Description = "Distinct focal points that organize surrounding space" },
        new() { Index = 2, Name = "Boundaries", Description = "Thick boundaries that unite and separate" },
        new() { Index = 3, Name = "Alternating Repetition", Description = "Rhythmic alternation of contrasting elements" },
        new() { Index = 4, Name = "Positive Space", Description = "Every part of space is positively shaped" },
        new() { Index = 5, Name = "Good Shape", Description = "Each center has a clear, definite shape" },
        new() { Index = 6, Name = "Local Symmetries", Description = "Symmetry at local level, not global" },
        new() { Index = 7, Name = "Deep Interlock & Ambiguity", Description = "Centers hook into each other" },
        new() { Index = 8, Name = "Contrast", Description = "Unity created through contrast" },
        new() { Index = 9, Name = "Gradients", Description = "Gradual transitions between qualities" },
        new() { Index = 10, Name = "Roughness", Description = "Irregularity that gives life" },
        new() { Index = 11, Name = "Echoes", Description = "Deep similarities between elements" },
        new() { Index = 12, Name = "The Void", Description = "Empty calm at the center" },
        new() { Index = 13, Name = "Simplicity & Inner Calm", Description = "Geometric simplicity and peace" },
        new() { Index = 14, Name = "Not-Separateness", Description = "Connected to the world, not isolated" }
    };
}

/// <summary>
/// Training statistics aggregated across episodes.
/// </summary>
public class TrainingStats
{
    public int TotalEpisodes { get; set; }
    public int CompletedEpisodes { get; set; }
    public double BestReward { get; set; }
    public double AverageReward { get; set; }
    public double AverageCoherence { get; set; }
    public AutonomyLevel CurrentAutonomyLevel { get; set; }
    public AutonomyLevel TargetAutonomyLevel { get; set; } = AutonomyLevel.Autonomous;
    public int ExperimentsRun { get; set; }
    public int ExperimentsKept { get; set; }
    public int ExperimentsDiscarded { get; set; }
    public double KeepRatio => ExperimentsRun > 0 ? (double)ExperimentsKept / ExperimentsRun : 0;
    public TimeSpan TotalTrainingTime { get; set; }
    public List<double> RewardHistory { get; set; } = new();
    public List<double> CoherenceHistory { get; set; } = new();
}

/// <summary>
/// Game world training configuration.
/// </summary>
public class TrainingConfig
{
    public string AssetArchivePath { get; set; } = string.Empty;
    public AutonomyLevel TargetLevel { get; set; } = AutonomyLevel.Autonomous;
    public int MaxExperiments { get; set; } = 50;
    public double MinCoherence { get; set; } = 0.15;
    public double MinPropertyCoherence { get; set; } = 0.60;
    public double MaxParameterDelta { get; set; } = 0.20;
    public double ExplorationRate { get; set; } = 0.3;
    public double CurriculumDifficulty { get; set; } = 0.0;
    public EGameGenre Genre { get; set; } = EGameGenre.OpenWorld;
    public ETrainingMode TrainingMode { get; set; } = ETrainingMode.Supervised;
}

public enum EGameGenre
{
    OpenWorld,
    Action,
    Racing,
    Stealth,
    Combat
}

public enum ETrainingMode
{
    Supervised,
    SelfPlay,
    CurriculumLearning,
    ReinforcementLearning,
    Imitation
}
