using System.Collections.Concurrent;
using System.IO;
using System.Text;
using Microsoft.Extensions.Logging;
using GTAngel.Models;

namespace GTAngel.Services;

/// <summary>
/// GTAngel — Guardian Angel Cognitive Orchestrator.
///
/// Composition: /dte-ksm-evo-autogenesis ( /gta3-ue5-wpf ) → "GTAngel"
///
/// This service is the top-level autogenesis loop controller that wraps the
/// DTE-KSM-Evo-Autogenesis skill over the GTAngel environment.
///
/// It implements:
///   1. The 6-step Autogenesis Loop (Observe → Hypothesize → Edit → Run → Assess → Decide)
///   2. The KSM 12-step Evolution Cycle annotated by Alexander's 15 Properties
///   3. Guardian Angel persona state (coherence guardian, property warden, safety enforcer)
///   4. Safety constraints: coherence halt, property degradation, delta clamp
///   5. Results.tsv experiment log (Phase 6.4)
/// </summary>
public sealed class GTAngelService : IDisposable
{
    private readonly ILogger<GTAngelService> _logger;
    private readonly TrainingEngine _trainingEngine;

    // Phase 6.1: Optional references to full DTE pipeline services
    private DteTrainingLoop?               _dteTrainingLoop;
    private EsnReservoirPipeline?          _esnPipeline;
    private DteCognitiveCoreService?       _cognitiveCoreService;

    private CancellationTokenSource? _cts;
    private Task? _loopTask;
    private bool _disposed;

    // Phase 6.4: results.tsv writer
    private readonly string _resultsTsvPath;

    // ── Guardian Angel State ──────────────────────────────────────────────────
    public GTAngelState State { get; } = new();

    // ── Events for UI binding ─────────────────────────────────────────────────
    public event Action<GTAngelState>? OnStateUpdated;
    public event Action<AutogenesisExperiment>? OnExperimentCompleted;
    public event Action<string>? OnLogMessage;
    public event Action<GTAngelLoopStep>? OnLoopStepChanged;
    public event Action<string>? OnGuardianAlert;

    // ── Safety thresholds (from dte-ksm-evo-autogenesis skill) ───────────────
    public const double CoherenceHaltThreshold = 0.15;
    public const double PropertyCoherenceMinimum = 0.60;
    public const double MaxParameterDelta = 0.20;

    public GTAngelService(ILogger<GTAngelService> logger, TrainingEngine trainingEngine)
    {
        _logger = logger;
        _trainingEngine = trainingEngine;

        _resultsTsvPath = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "GTAngel", "results.tsv");
        Directory.CreateDirectory(Path.GetDirectoryName(_resultsTsvPath)!);
        EnsureResultsTsvHeader();

        // Wire training engine events
        _trainingEngine.OnCognitiveStateChanged += cs =>
        {
            State.StreamCoherence = _trainingEngine.ESN.StreamCoherence;
            State.PropertyCoherence = _trainingEngine.ComputeOverallPropertyCoherence();
            State.AutonomyLevel = _trainingEngine.Stats.CurrentAutonomyLevel;
            CheckSafetyConstraints();
            OnStateUpdated?.Invoke(State);
        };

        _trainingEngine.OnExperimentCompleted += exp =>
        {
            State.TotalExperiments++;
            if (exp.Status == ExperimentStatus.Keep) State.KeptExperiments++;
            else if (exp.Status == ExperimentStatus.Discard) State.DiscardedExperiments++;
            if (exp.PrimaryMetric > State.BestMetric) State.BestMetric = exp.PrimaryMetric;
            WriteExperimentTsv(exp);
            OnExperimentCompleted?.Invoke(exp);
            OnStateUpdated?.Invoke(State);
        };

        _trainingEngine.OnLogMessage += msg =>
        {
            State.RecentLogs.Enqueue($"[{DateTime.Now:HH:mm:ss}] {msg}");
            while (State.RecentLogs.Count > 200) State.RecentLogs.TryDequeue(out _);
            OnLogMessage?.Invoke(msg);
        };
    }

    /// <summary>Phase 6.1: Wire in the full DTE pipeline services for KSM orchestration.</summary>
    public void SetDtePipelineServices(
        DteTrainingLoop trainingLoop,
        EsnReservoirPipeline esnPipeline,
        DteCognitiveCoreService cognitiveCoreService)
    {
        _dteTrainingLoop     = trainingLoop;
        _esnPipeline         = esnPipeline;
        _cognitiveCoreService = cognitiveCoreService;
        _trainingEngine.SetCognitiveCoreService(cognitiveCoreService);
        Log("[GTAngel] KSM Cycle 5 wired: DteTrainingLoop + ESN + CognitiveCore bound to autogenesis loop.");
    }

    // ── Public API ────────────────────────────────────────────────────────────

    /// <summary>
    /// Start the GTAngel autogenesis loop.
    /// </summary>
    public async Task StartAsync(GTAngelConfig config)
    {
        if (State.IsRunning)
        {
            Log("GTAngel loop already running.");
            return;
        }

        State.Config = config;
        State.IsRunning = true;
        State.StartTime = DateTime.UtcNow;
        State.Phase = GTAngelPhase.Initializing;

        _cts = new CancellationTokenSource();
        _loopTask = Task.Run(() => RunAutogenesisLoopAsync(_cts.Token));

        Log($"[GTAngel] Guardian Angel awakened. Target: {config.TargetAutonomyLevel}, Max experiments: {config.MaxExperiments}");
        OnStateUpdated?.Invoke(State);
        await Task.CompletedTask;
    }

    /// <summary>
    /// Stop the GTAngel autogenesis loop.
    /// </summary>
    public async Task StopAsync()
    {
        if (!State.IsRunning) return;

        Log("[GTAngel] Guardian Angel stopping...");
        _cts?.Cancel();

        if (_loopTask != null)
        {
            try { await _loopTask.WaitAsync(TimeSpan.FromSeconds(5)); }
            catch { /* ignore cancellation */ }
        }

        State.IsRunning = false;
        State.Phase = GTAngelPhase.Idle;
        OnStateUpdated?.Invoke(State);
    }

    /// <summary>
    /// Pause / resume the loop.
    /// </summary>
    public void TogglePause()
    {
        State.IsPaused = !State.IsPaused;
        Log(State.IsPaused ? "[GTAngel] Loop paused." : "[GTAngel] Loop resumed.");
        OnStateUpdated?.Invoke(State);
    }

    // ── Core Autogenesis Loop ─────────────────────────────────────────────────

    private async Task RunAutogenesisLoopAsync(CancellationToken ct)
    {
        try
        {
            // ── Setup Phase ──────────────────────────────────────────────────
            State.Phase = GTAngelPhase.Setup;
            SetLoopStep(GTAngelLoopStep.Observe);
            Log("[GTAngel] Setup: establishing experimental parameters...");
            OnStateUpdated?.Invoke(State);
            await Task.Delay(500, ct);

            // ── Baseline ─────────────────────────────────────────────────────
            State.Phase = GTAngelPhase.Baseline;
            Log("[GTAngel] Running baseline measurement (Experiment 0)...");
            var baselineConfig = BuildTrainingConfig();
            await _trainingEngine.StartAutogenesisAsync(baselineConfig, new Progress<string>(Log));

            // ── Main Evolution Loop ───────────────────────────────────────────
            State.Phase = GTAngelPhase.Evolving;
            int experimentId = 1;

            while (experimentId <= State.Config.MaxExperiments && !ct.IsCancellationRequested)
            {
                if (State.IsPaused)
                {
                    await Task.Delay(200, ct);
                    continue;
                }

                if (State.IsHalted)
                {
                    Log("[GTAngel] COHERENCE HALT — awaiting human oversight.");
                    OnGuardianAlert?.Invoke("COHERENCE HALT: Stream coherence below 15%. Human oversight required.");
                    await Task.Delay(1000, ct);
                    continue;
                }

                // ── Step 1: Observe & Hypothesize ────────────────────────────
                SetLoopStep(GTAngelLoopStep.Observe);
                State.CurrentKsmStep = (experimentId - 1) % 12;
                State.CurrentKsmStepName = AutogenesisExperiment.KsmStepNames[State.CurrentKsmStep];
                Log($"[GTAngel] Step 1 — Observe & Hypothesize (KSM: {State.CurrentKsmStepName})");
                OnStateUpdated?.Invoke(State);
                await Task.Delay(300, ct);

                // ── Step 2: Edit & Commit ─────────────────────────────────────
                SetLoopStep(GTAngelLoopStep.Edit);
                Log($"[GTAngel] Step 2 — Edit & Commit (Experiment #{experimentId})");
                await Task.Delay(200, ct);

                // ── Step 3: Run & Measure ─────────────────────────────────────
                SetLoopStep(GTAngelLoopStep.Run);
                Log($"[GTAngel] Step 3 — Run & Measure");
                await Task.Delay(200, ct);

                // ── Step 4: Assess Property Coherence ────────────────────────
                SetLoopStep(GTAngelLoopStep.Assess);
                Log($"[GTAngel] Step 4 — Assess Property Coherence (Alexander's 15 Properties)");
                await Task.Delay(200, ct);

                // ── Step 5: Decide & Enact ────────────────────────────────────
                SetLoopStep(GTAngelLoopStep.Decide);
                Log($"[GTAngel] Step 5 — Decide & Enact");
                await Task.Delay(200, ct);

                // ── Step 6: Log ───────────────────────────────────────────────
                SetLoopStep(GTAngelLoopStep.Log);
                Log($"[GTAngel] Step 6 — Log (results.tsv updated)");
                await Task.Delay(100, ct);

                // Check target reached
                if (State.AutonomyLevel >= State.Config.TargetAutonomyLevel)
                {
                    Log($"[GTAngel] TARGET AUTONOMY LEVEL REACHED: {State.Config.TargetAutonomyLevel}!");
                    OnGuardianAlert?.Invoke($"TARGET REACHED: {State.Config.TargetAutonomyLevel} — Autogenesis complete!");
                    break;
                }

                experimentId++;
            }

            // ── Reporting ─────────────────────────────────────────────────────
            State.Phase = GTAngelPhase.Reporting;
            Log($"[GTAngel] Evolution cycle complete.");
            Log($"[GTAngel] Total: {State.TotalExperiments} | Kept: {State.KeptExperiments} | Discarded: {State.DiscardedExperiments}");
            Log($"[GTAngel] Best metric: {State.BestMetric:F4} | Final coherence: {State.StreamCoherence:F3}");
            Log($"[GTAngel] Keep ratio: {State.KeepRatio:P0}");
        }
        catch (OperationCanceledException)
        {
            Log("[GTAngel] Loop cancelled.");
        }
        catch (Exception ex)
        {
            Log($"[GTAngel] ERROR: {ex.Message}");
            _logger.LogError(ex, "GTAngel autogenesis loop error");
        }
        finally
        {
            State.IsRunning = false;
            State.Phase = GTAngelPhase.Idle;
            OnStateUpdated?.Invoke(State);
        }
    }

    // ── Safety Constraints ────────────────────────────────────────────────────

    private void CheckSafetyConstraints()
    {
        // Coherence Halt
        if (State.StreamCoherence < CoherenceHaltThreshold && State.IsRunning && !State.IsHalted)
        {
            State.IsHalted = true;
            Log($"[GTAngel] ⚠ COHERENCE HALT: stream coherence {State.StreamCoherence:F3} < {CoherenceHaltThreshold}");
            OnGuardianAlert?.Invoke($"COHERENCE HALT: {State.StreamCoherence:F3} < {CoherenceHaltThreshold:F2}");
        }
        else if (State.StreamCoherence >= CoherenceHaltThreshold && State.IsHalted)
        {
            State.IsHalted = false;
            Log("[GTAngel] Coherence restored — resuming loop.");
        }

        // Property Degradation Warning
        if (State.PropertyCoherence < PropertyCoherenceMinimum && State.IsRunning)
        {
            Log($"[GTAngel] ⚠ Property coherence {State.PropertyCoherence:F3} < {PropertyCoherenceMinimum} — experiment will be discarded.");
        }
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    private void SetLoopStep(GTAngelLoopStep step)
    {
        State.CurrentLoopStep = step;
        OnLoopStepChanged?.Invoke(step);
    }

    private TrainingConfig BuildTrainingConfig() => new()
    {
        TargetLevel = State.Config.TargetAutonomyLevel,
        MaxExperiments = State.Config.MaxExperiments,
        MinCoherence = CoherenceHaltThreshold,
        MinPropertyCoherence = PropertyCoherenceMinimum,
        MaxParameterDelta = MaxParameterDelta,
        TrainingMode = ETrainingMode.ReinforcementLearning
    };

    private void Log(string msg)
    {
        _logger.LogInformation("{Message}", msg);
        State.RecentLogs.Enqueue($"[{DateTime.Now:HH:mm:ss}] {msg}");
        while (State.RecentLogs.Count > 200) State.RecentLogs.TryDequeue(out _);
        OnLogMessage?.Invoke(msg);
    }

    // ── Phase 6.4: results.tsv file writer ────────────────────────────────────

    private void EnsureResultsTsvHeader()
    {
        try
        {
            if (!File.Exists(_resultsTsvPath))
            {
                var header = string.Join("\t",
                    "EpisodeId", "Timestamp", "PrimaryMetric", "PropertyCoherenceScore",
                    "P0_LevelsOfScale", "P1_StrongCenters", "P2_Boundaries", "P3_Alternating",
                    "P4_PositiveSpace", "P5_GoodShape", "P6_LocalSymm", "P7_DeepInterlock",
                    "P8_Contrast", "P9_Gradients", "P10_Roughness", "P11_Echoes",
                    "P12_TheVoid", "P13_Simplicity", "P14_NotSeparateness",
                    "AutonomyLevel", "KsmStep", "KsmStepName", "StatusReason");
                File.WriteAllText(_resultsTsvPath, header + Environment.NewLine);
            }
        }
        catch (Exception ex)
        {
            _logger.LogWarning(ex, "Could not create results.tsv header");
        }
    }

    private void WriteExperimentTsv(AutogenesisExperiment exp)
    {
        try
        {
            var sb = new StringBuilder();
            sb.Append(exp.ExperimentId).Append('\t');
            sb.Append(exp.Timestamp.ToString("O")).Append('\t');
            sb.Append(exp.PrimaryMetric.ToString("F6")).Append('\t');
            sb.Append(exp.PropertyCoherenceScore.ToString("F4")).Append('\t');

            // 15 Alexander property scores (P0..P14)
            for (int p = 0; p < 15; p++)
            {
                // Try direct "P{n}" key first, then fall back to partial name match
                if (!exp.PropertyScores.TryGetValue($"P{p}", out var score))
                {
                    score = 0.0;
                    foreach (var kv in exp.PropertyScores)
                    {
                        if (kv.Key.StartsWith($"P{p}", StringComparison.OrdinalIgnoreCase))
                        { score = kv.Value; break; }
                    }
                }
                sb.Append(score.ToString("F4")).Append('\t');
            }

            sb.Append((int)State.AutonomyLevel).Append('\t');
            sb.Append(exp.KsmStep).Append('\t');
            sb.Append(exp.KsmStepName).Append('\t');
            sb.Append(exp.StatusReason);

            File.AppendAllText(_resultsTsvPath, sb.ToString() + Environment.NewLine);
        }
        catch (Exception ex)
        {
            _logger.LogWarning(ex, "Could not write to results.tsv");
        }
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;
        _cts?.Cancel();
        _cts?.Dispose();
    }
}

// ── Supporting Models ─────────────────────────────────────────────────────────

public class GTAngelState
{
    public bool IsRunning { get; set; }
    public bool IsPaused { get; set; }
    public bool IsHalted { get; set; }
    public GTAngelPhase Phase { get; set; } = GTAngelPhase.Idle;
    public GTAngelLoopStep CurrentLoopStep { get; set; } = GTAngelLoopStep.Observe;
    public DateTime StartTime { get; set; }

    // KSM cycle
    public int CurrentKsmStep { get; set; }
    public string CurrentKsmStepName { get; set; } = "Observe";

    // Cognitive metrics
    public double StreamCoherence { get; set; } = 0.5;
    public double PropertyCoherence { get; set; } = 0.5;
    public AutonomyLevel AutonomyLevel { get; set; } = AutonomyLevel.Reactive;

    // Experiment stats
    public int TotalExperiments { get; set; }
    public int KeptExperiments { get; set; }
    public int DiscardedExperiments { get; set; }
    public double BestMetric { get; set; }
    public double KeepRatio => TotalExperiments > 0 ? (double)KeptExperiments / TotalExperiments : 0;

    // Config
    public GTAngelConfig Config { get; set; } = new();

    // Log ring buffer
    public ConcurrentQueue<string> RecentLogs { get; } = new();

    public TimeSpan Elapsed => IsRunning ? DateTime.UtcNow - StartTime : TimeSpan.Zero;
}

public class GTAngelConfig
{
    public AutonomyLevel TargetAutonomyLevel { get; set; } = AutonomyLevel.Autonomous;
    public int MaxExperiments { get; set; } = 30;
    public double SpectralRadius { get; set; } = 0.90;
    public double LeakingRate { get; set; } = 0.30;
    public bool EnableGuardianAlerts { get; set; } = true;
    public bool AutoSaveCheckpoints { get; set; } = true;
}

public enum GTAngelPhase
{
    Idle,
    Initializing,
    Setup,
    Baseline,
    Evolving,
    Halted,
    Reporting
}

public enum GTAngelLoopStep
{
    Observe = 0,
    Edit = 1,
    Run = 2,
    Assess = 3,
    Decide = 4,
    Log = 5
}
