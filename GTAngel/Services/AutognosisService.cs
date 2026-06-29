using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// DTE Autognosis Service — ESN Self-Monitoring &amp; DAO-Style Governance
///
/// Implements Deep Tree Echo's "self-awareness" layer — the capacity of the ESN
/// reservoir system to introspect on its own cognitive health, detect degradation,
/// and initiate self-repair through KSM-inspired improvement cycles.
///
/// This mirrors the DAO (Decentralized Autonomous Organization) pattern:
///   - Each neuron cluster is a "stakeholder" with voting weight (STI)
///   - Health metrics are "proposals" evaluated by the governance layer
///   - Repair actions are "executed" only when quorum is reached
///   - The system self-regulates without external intervention
///
/// KSM Improvement Methodology:
///   Cycle 1: Observe  — continuous metric collection from ESN reservoir layers
///   Cycle 2: Diagnose — anomaly detection on spectral radius, coherence, entropy
///   Cycle 3: Prescribe — select repair strategy from KSM pattern library
///   Cycle 4: Apply    — execute structure-preserving transformation
///   Cycle 5: Verify   — confirm improvement via pre/post metric comparison
///   Cycle 6: Evolve   — update governance weights based on repair outcomes
///
/// Alexander's 15 Properties: P4 (Alternating Repetition), P6 (Good Shape),
/// P9 (Contrast), P13 (The Void), P15 (Inner Calm)
/// </summary>
public sealed class AutognosisService : IDisposable
{
    private readonly ILogger<AutognosisService> _logger;
    private bool _disposed;
    private CancellationTokenSource? _cts;
    private Task? _monitoringTask;

    // ── Reservoir Health Metrics ──────────────────────────────────────────────
    private ReservoirHealthSnapshot _lastHealth = ReservoirHealthSnapshot.Default;
    private readonly List<ReservoirHealthSnapshot> _healthHistory = new();
    private const int MaxHealthHistory = 1000;

    // ── Spectral Radius Monitoring ───────────────────────────────────────────
    private float _spectralRadius = 0.9f;
    private float _spectralRadiusTarget = 0.95f;
    private float _spectralRadiusTolerance = 0.1f;

    // ── Cognitive Coherence (DAO Governance) ──────────────────────────────────
    private float[] _clusterVotingWeights = new float[16]; // one per neuron cluster
    private float _governanceQuorum = 0.6f;   // 60% agreement needed for repair
    private int _repairProposalsExecuted;
    private int _repairProposalsRejected;

    // ── KSM Self-Repair State ────────────────────────────────────────────────
    private RepairState _repairState = RepairState.Observing;
    private RepairStrategy? _currentStrategy;
    private readonly Queue<RepairOutcome> _repairLog = new();
    private const int MaxRepairLogSize = 100;

    // ── Autognosis Metrics ───────────────────────────────────────────────────
    private float _selfAwareness;      // [0,1] quality of self-model
    private float _cognitiveStability;  // [0,1] absence of oscillation/chaos
    private float _adaptiveCapacity;   // [0,1] ability to recover from perturbation
    private float _wisdomAccumulation; // [0,1] long-term knowledge consolidation

    // ── Events ───────────────────────────────────────────────────────────────
    public event EventHandler<ReservoirHealthSnapshot>? HealthUpdated;
    public event EventHandler<RepairProposalEventArgs>? RepairProposed;
    public event EventHandler<RepairOutcome>? RepairCompleted;
    public event EventHandler<AutognosisSnapshot>? AutognosisUpdated;

    public AutognosisService(ILogger<AutognosisService> logger)
    {
        _logger = logger ?? throw new ArgumentNullException(nameof(logger));

        // Initialize governance weights equally
        for (int i = 0; i < _clusterVotingWeights.Length; i++)
            _clusterVotingWeights[i] = 1f / _clusterVotingWeights.Length;

        _logger.LogInformation("DTE Autognosis Service initialized — DAO governance active");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Public API
    // ═══════════════════════════════════════════════════════════════════════════

    /// <summary>Current reservoir health assessment.</summary>
    public ReservoirHealthSnapshot LastHealth => _lastHealth;

    /// <summary>Current spectral radius (edge-of-chaos indicator).</summary>
    public float SpectralRadius => _spectralRadius;

    /// <summary>Self-awareness quality [0,1].</summary>
    public float SelfAwareness => _selfAwareness;

    /// <summary>Cognitive stability [0,1].</summary>
    public float CognitiveStability => _cognitiveStability;

    /// <summary>Adaptive capacity [0,1].</summary>
    public float AdaptiveCapacity => _adaptiveCapacity;

    /// <summary>Wisdom accumulation [0,1].</summary>
    public float WisdomAccumulation => _wisdomAccumulation;

    /// <summary>Current repair state in the KSM cycle.</summary>
    public RepairState CurrentRepairState => _repairState;

    /// <summary>Total repair proposals executed.</summary>
    public int RepairProposalsExecuted => _repairProposalsExecuted;

    /// <summary>Whether the monitoring loop is active.</summary>
    public bool IsMonitoring => _monitoringTask != null && !_monitoringTask.IsCompleted;

    // ── KSM Cycle 1: Observe ──────────────────────────────────────────────────

    /// <summary>
    /// Feed reservoir state for health assessment. Called each ESN step.
    /// </summary>
    public ReservoirHealthSnapshot Observe(
        float[] sensoryActivation,
        float[] cognitiveActivation,
        float[] executiveActivation,
        float[] clusterSTI,
        float currentSpectralRadius,
        double woutLoss)
    {
        _spectralRadius = currentSpectralRadius;

        // Compute health metrics
        var health = new ReservoirHealthSnapshot
        {
            Timestamp = DateTimeOffset.UtcNow,
            SpectralRadius = currentSpectralRadius,
            SpectralRadiusDeviation = Math.Abs(currentSpectralRadius - _spectralRadiusTarget),
            SensoryLayerEntropy = ComputeEntropy(sensoryActivation),
            CognitiveLayerEntropy = ComputeEntropy(cognitiveActivation),
            ExecutiveLayerEntropy = ComputeEntropy(executiveActivation),
            AttentionConcentration = ComputeConcentration(clusterSTI),
            WoutLoss = (float)woutLoss,
            OverallHealth = 0f // computed below
        };

        // Compute overall health score
        health = health with
        {
            OverallHealth = ComputeOverallHealth(health)
        };

        _lastHealth = health;
        _healthHistory.Add(health);
        if (_healthHistory.Count > MaxHealthHistory)
            _healthHistory.RemoveAt(0);

        // Update autognosis metrics
        UpdateSelfModel(health);

        HealthUpdated?.Invoke(this, health);

        // KSM Cycle 2: Diagnose
        if (_repairState == RepairState.Observing)
            DiagnoseAndProposeRepair(health);

        return health;
    }

    // ── KSM Cycle 2: Diagnose ─────────────────────────────────────────────────

    /// <summary>
    /// Diagnose reservoir health and propose repair if needed.
    /// Uses DAO-style governance: each cluster votes on whether repair is needed.
    /// </summary>
    internal DiagnosisResult Diagnose(ReservoirHealthSnapshot health)
    {
        var anomalies = new List<string>();

        // Check spectral radius (edge-of-chaos)
        if (health.SpectralRadiusDeviation > _spectralRadiusTolerance)
            anomalies.Add($"SpectralRadius deviation {health.SpectralRadiusDeviation:F3} > tolerance {_spectralRadiusTolerance:F3}");

        // Check entropy balance (too low = dead neurons, too high = noise)
        if (health.SensoryLayerEntropy < 0.1f)
            anomalies.Add("Sensory layer entropy too low (dead neurons)");
        if (health.ExecutiveLayerEntropy > 0.95f)
            anomalies.Add("Executive layer entropy too high (noise dominance)");

        // Check attention concentration (too concentrated = tunnel vision)
        if (health.AttentionConcentration > 0.9f)
            anomalies.Add("Attention over-concentrated (tunnel vision)");
        if (health.AttentionConcentration < 0.1f)
            anomalies.Add("Attention too dispersed (no focus)");

        // Check Wout convergence
        if (health.WoutLoss > 10f)
            anomalies.Add($"Wout loss {health.WoutLoss:F2} exceeds acceptable range");

        // DAO governance: vote on repair
        float governanceScore = ComputeGovernanceVote(health, anomalies.Count);

        return new DiagnosisResult(
            NeedsRepair: anomalies.Count > 0 && governanceScore >= _governanceQuorum,
            GovernanceScore: governanceScore,
            Anomalies: anomalies.ToArray(),
            Severity: anomalies.Count switch
            {
                0 => DiagnosisSeverity.Healthy,
                1 => DiagnosisSeverity.Mild,
                2 => DiagnosisSeverity.Moderate,
                _ => DiagnosisSeverity.Severe
            });
    }

    // ── KSM Cycle 3: Prescribe ────────────────────────────────────────────────

    /// <summary>
    /// Select appropriate repair strategy based on diagnosis.
    /// </summary>
    internal RepairStrategy PrescribeRepair(DiagnosisResult diagnosis)
    {
        if (diagnosis.Anomalies.Length == 0)
            return RepairStrategy.None;

        // Check for spectral radius issues first (most critical)
        if (diagnosis.Anomalies.Any(a => a.Contains("SpectralRadius")))
        {
            return _spectralRadius > _spectralRadiusTarget
                ? RepairStrategy.SpectralRadiusDamping
                : RepairStrategy.SpectralRadiusBoost;
        }

        // Check entropy issues
        if (diagnosis.Anomalies.Any(a => a.Contains("entropy too low")))
            return RepairStrategy.NeuronReactivation;

        if (diagnosis.Anomalies.Any(a => a.Contains("entropy too high")))
            return RepairStrategy.NoiseReduction;

        // Check attention issues
        if (diagnosis.Anomalies.Any(a => a.Contains("over-concentrated")))
            return RepairStrategy.AttentionRedistribution;

        if (diagnosis.Anomalies.Any(a => a.Contains("too dispersed")))
            return RepairStrategy.AttentionFocusing;

        // Check Wout
        if (diagnosis.Anomalies.Any(a => a.Contains("Wout loss")))
            return RepairStrategy.RidgeRegularizationIncrease;

        return RepairStrategy.GeneralRebalance;
    }

    // ── KSM Cycle 4: Apply ────────────────────────────────────────────────────

    /// <summary>
    /// Apply prescribed repair. Returns parameters for the ESN to use.
    /// </summary>
    public RepairParameters ApplyRepair(RepairStrategy strategy)
    {
        _repairState = RepairState.Applying;
        _currentStrategy = strategy;

        var parameters = strategy switch
        {
            RepairStrategy.SpectralRadiusDamping => new RepairParameters
            {
                SpectralRadiusMultiplier = 0.95f,
                LeakRateAdjustment = -0.02f,
                Description = "Damping spectral radius toward target"
            },
            RepairStrategy.SpectralRadiusBoost => new RepairParameters
            {
                SpectralRadiusMultiplier = 1.05f,
                LeakRateAdjustment = 0.01f,
                Description = "Boosting spectral radius toward edge-of-chaos"
            },
            RepairStrategy.NeuronReactivation => new RepairParameters
            {
                NoiseInjectionScale = 0.05f,
                LeakRateAdjustment = 0.03f,
                Description = "Reactivating dormant neurons via noise injection"
            },
            RepairStrategy.NoiseReduction => new RepairParameters
            {
                NoiseInjectionScale = -0.02f,
                LeakRateAdjustment = -0.01f,
                Description = "Reducing noise in executive layer"
            },
            RepairStrategy.AttentionRedistribution => new RepairParameters
            {
                AttentionEntropyTarget = 0.7f,
                Description = "Redistributing attention across clusters"
            },
            RepairStrategy.AttentionFocusing => new RepairParameters
            {
                AttentionEntropyTarget = 0.4f,
                Description = "Focusing attention on high-STI clusters"
            },
            RepairStrategy.RidgeRegularizationIncrease => new RepairParameters
            {
                RidgeLambdaMultiplier = 1.5f,
                Description = "Increasing ridge regularization to reduce Wout overfitting"
            },
            RepairStrategy.GeneralRebalance => new RepairParameters
            {
                SpectralRadiusMultiplier = 1.0f,
                LeakRateAdjustment = 0f,
                NoiseInjectionScale = 0.01f,
                Description = "General rebalance — gentle perturbation for self-organization"
            },
            _ => RepairParameters.NoOp
        };

        _repairProposalsExecuted++;
        _logger.LogInformation("KSM Repair applied: {Strategy} — {Description}",
            strategy, parameters.Description);

        RepairProposed?.Invoke(this, new RepairProposalEventArgs(strategy, parameters));

        return parameters;
    }

    // ── KSM Cycle 5: Verify ──────────────────────────────────────────────────

    /// <summary>
    /// Verify repair outcome by comparing health before and after.
    /// </summary>
    public RepairOutcome VerifyRepair(ReservoirHealthSnapshot afterHealth)
    {
        var beforeHealth = _healthHistory.Count > 1
            ? _healthHistory[^2]
            : ReservoirHealthSnapshot.Default;

        float improvement = afterHealth.OverallHealth - beforeHealth.OverallHealth;
        bool successful = improvement > -0.05f; // allow slight regression

        var outcome = new RepairOutcome(
            Strategy: _currentStrategy ?? RepairStrategy.None,
            HealthBefore: beforeHealth.OverallHealth,
            HealthAfter: afterHealth.OverallHealth,
            Improvement: improvement,
            Successful: successful,
            Timestamp: DateTimeOffset.UtcNow);

        _repairLog.Enqueue(outcome);
        if (_repairLog.Count > MaxRepairLogSize)
            _repairLog.Dequeue();

        // KSM Cycle 6: Evolve governance
        EvolveGovernance(outcome);

        _repairState = RepairState.Observing;
        _currentStrategy = null;

        RepairCompleted?.Invoke(this, outcome);

        _logger.LogInformation("KSM Repair verified: {Success} (Δ={Improvement:+F3})",
            successful ? "SUCCESS" : "FAILED", improvement);

        return outcome;
    }

    // ── KSM Cycle 6: Evolve ──────────────────────────────────────────────────

    private void EvolveGovernance(RepairOutcome outcome)
    {
        // Successful repairs increase governance confidence (lower quorum)
        if (outcome.Successful)
        {
            _governanceQuorum = Math.Max(0.4f, _governanceQuorum - 0.01f);
            _adaptiveCapacity = Math.Min(1f, _adaptiveCapacity + 0.02f);
        }
        else
        {
            _governanceQuorum = Math.Min(0.9f, _governanceQuorum + 0.02f);
            _repairProposalsRejected++;
        }

        // Accumulate wisdom
        _wisdomAccumulation = Math.Min(1f, _wisdomAccumulation + 0.005f);
    }

    // ── Autognosis Self-Model ─────────────────────────────────────────────────

    private void UpdateSelfModel(ReservoirHealthSnapshot health)
    {
        // Self-awareness: how well our predictions match actual state
        _selfAwareness = Math.Clamp(
            health.OverallHealth * 0.5f + _wisdomAccumulation * 0.3f + _adaptiveCapacity * 0.2f,
            0f, 1f);

        // Cognitive stability: low variance in recent health scores
        if (_healthHistory.Count >= 5)
        {
            var recentHealthValues = _healthHistory.TakeLast(10)
                .Select(h => h.OverallHealth).ToArray();
            float mean = recentHealthValues.Average();
            float variance = recentHealthValues.Select(v => (v - mean) * (v - mean)).Average();
            _cognitiveStability = Math.Clamp(1f - (MathF.Sqrt(variance) * 5f), 0f, 1f);
        }

        // Emit autognosis update
        AutognosisUpdated?.Invoke(this, GetSnapshot());
    }

    /// <summary>Get complete autognosis snapshot for telemetry.</summary>
    public AutognosisSnapshot GetSnapshot()
    {
        return new AutognosisSnapshot(
            SelfAwareness: _selfAwareness,
            CognitiveStability: _cognitiveStability,
            AdaptiveCapacity: _adaptiveCapacity,
            WisdomAccumulation: _wisdomAccumulation,
            SpectralRadius: _spectralRadius,
            SpectralRadiusTarget: _spectralRadiusTarget,
            GovernanceQuorum: _governanceQuorum,
            RepairState: _repairState,
            TotalRepairs: _repairProposalsExecuted,
            SuccessfulRepairs: _repairProposalsExecuted - _repairProposalsRejected,
            CurrentHealth: _lastHealth.OverallHealth);
    }

    // ── Start / Stop Monitoring ───────────────────────────────────────────────

    /// <summary>Start the continuous autognosis monitoring loop.</summary>
    public void StartMonitoring(CancellationToken ct = default)
    {
        if (IsMonitoring) return;
        _cts = CancellationTokenSource.CreateLinkedTokenSource(ct);
        _monitoringTask = Task.Run(() => MonitoringLoopAsync(_cts.Token), _cts.Token);
        _logger.LogInformation("Autognosis monitoring started");
    }

    /// <summary>Stop the monitoring loop.</summary>
    public void StopMonitoring()
    {
        _cts?.Cancel();
        _logger.LogInformation("Autognosis monitoring stopped");
    }

    private async Task MonitoringLoopAsync(CancellationToken ct)
    {
        while (!ct.IsCancellationRequested)
        {
            try
            {
                await Task.Delay(1000, ct);
                // Monitoring loop: periodic self-check even without new observations
                if (_healthHistory.Count > 0)
                {
                    var latestHealth = _healthHistory[^1];
                    if (_repairState == RepairState.Observing)
                        DiagnoseAndProposeRepair(latestHealth);
                }
            }
            catch (OperationCanceledException) { break; }
        }
    }

    // ── Private Helpers ───────────────────────────────────────────────────────

    private void DiagnoseAndProposeRepair(ReservoirHealthSnapshot health)
    {
        var diagnosis = Diagnose(health);
        if (diagnosis.NeedsRepair)
        {
            _repairState = RepairState.Diagnosing;
            var strategy = PrescribeRepair(diagnosis);
            if (strategy != RepairStrategy.None)
            {
                _repairState = RepairState.Prescribing;
                _currentStrategy = strategy;
                ApplyRepair(strategy);
            }
        }
    }

    internal static float ComputeEntropy(float[] activations)
    {
        if (activations == null || activations.Length == 0) return 0f;

        // Normalize to probability distribution
        float sum = activations.Select(Math.Abs).Sum();
        if (sum < 1e-8f) return 0f;

        float entropy = 0f;
        float logN = MathF.Log(activations.Length);
        if (logN < 1e-8f) return 0f;

        foreach (var a in activations)
        {
            float p = Math.Abs(a) / sum;
            if (p > 1e-8f)
                entropy -= p * MathF.Log(p);
        }

        return entropy / logN; // normalized [0, 1]
    }

    internal static float ComputeConcentration(float[] values)
    {
        if (values == null || values.Length == 0) return 0f;
        float max = values.Max();
        float sum = values.Sum();
        return sum > 1e-8f ? max / sum : 0f;
    }

    private float ComputeOverallHealth(ReservoirHealthSnapshot h)
    {
        // Weighted combination of health indicators
        float spectralScore = 1f - Math.Min(1f, h.SpectralRadiusDeviation / _spectralRadiusTolerance);
        float entropyScore = (h.SensoryLayerEntropy + h.CognitiveLayerEntropy + h.ExecutiveLayerEntropy) / 3f;
        float attentionScore = 1f - Math.Abs(h.AttentionConcentration - 0.5f) * 2f;
        float lossScore = Math.Max(0f, 1f - h.WoutLoss / 10f);

        return Math.Clamp(
            spectralScore * 0.3f + entropyScore * 0.25f + attentionScore * 0.2f + lossScore * 0.25f,
            0f, 1f);
    }

    private float ComputeGovernanceVote(ReservoirHealthSnapshot health, int anomalyCount)
    {
        // Each cluster votes based on its weight and the severity
        float totalVote = 0f;
        float totalWeight = 0f;

        for (int i = 0; i < _clusterVotingWeights.Length; i++)
        {
            float weight = _clusterVotingWeights[i];
            // Clusters vote for repair proportional to anomaly severity
            float vote = anomalyCount > 0 ? Math.Min(1f, anomalyCount * 0.3f) : 0f;

            // Weight by health deviation
            vote *= (1f - health.OverallHealth);

            totalVote += vote * weight;
            totalWeight += weight;
        }

        return totalWeight > 0 ? totalVote / totalWeight : 0f;
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;
        _cts?.Cancel();
        _cts?.Dispose();
        _logger.LogInformation("AutognosisService disposed");
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Data Types
// ═══════════════════════════════════════════════════════════════════════════════

/// <summary>Reservoir health snapshot at a single point in time.</summary>
public record ReservoirHealthSnapshot
{
    public DateTimeOffset Timestamp { get; init; }
    public float SpectralRadius { get; init; }
    public float SpectralRadiusDeviation { get; init; }
    public float SensoryLayerEntropy { get; init; }
    public float CognitiveLayerEntropy { get; init; }
    public float ExecutiveLayerEntropy { get; init; }
    public float AttentionConcentration { get; init; }
    public float WoutLoss { get; init; }
    public float OverallHealth { get; init; }

    public static ReservoirHealthSnapshot Default => new()
    {
        Timestamp = DateTimeOffset.UtcNow,
        SpectralRadius = 0.9f,
        OverallHealth = 0.5f,
        SensoryLayerEntropy = 0.5f,
        CognitiveLayerEntropy = 0.5f,
        ExecutiveLayerEntropy = 0.5f,
        AttentionConcentration = 0.5f
    };
}

/// <summary>KSM repair cycle state.</summary>
public enum RepairState
{
    Observing, Diagnosing, Prescribing, Applying, Verifying
}

/// <summary>Available repair strategies.</summary>
public enum RepairStrategy
{
    None,
    SpectralRadiusDamping,
    SpectralRadiusBoost,
    NeuronReactivation,
    NoiseReduction,
    AttentionRedistribution,
    AttentionFocusing,
    RidgeRegularizationIncrease,
    GeneralRebalance
}

/// <summary>Diagnosis severity level.</summary>
public enum DiagnosisSeverity { Healthy, Mild, Moderate, Severe }

/// <summary>Diagnosis result from the autognosis system.</summary>
public record DiagnosisResult(
    bool NeedsRepair,
    float GovernanceScore,
    string[] Anomalies,
    DiagnosisSeverity Severity);

/// <summary>Parameters for ESN repair (returned to the reservoir pipeline).</summary>
public sealed class RepairParameters
{
    public float SpectralRadiusMultiplier { get; init; } = 1.0f;
    public float LeakRateAdjustment { get; init; } = 0f;
    public float NoiseInjectionScale { get; init; } = 0f;
    public float AttentionEntropyTarget { get; init; } = -1f;
    public float RidgeLambdaMultiplier { get; init; } = 1.0f;
    public string Description { get; init; } = "";

    public static RepairParameters NoOp => new() { Description = "No operation" };
}

/// <summary>Outcome of a repair cycle.</summary>
public record RepairOutcome(
    RepairStrategy Strategy,
    float HealthBefore,
    float HealthAfter,
    float Improvement,
    bool Successful,
    DateTimeOffset Timestamp);

/// <summary>Complete autognosis snapshot for telemetry.</summary>
public record AutognosisSnapshot(
    float SelfAwareness,
    float CognitiveStability,
    float AdaptiveCapacity,
    float WisdomAccumulation,
    float SpectralRadius,
    float SpectralRadiusTarget,
    float GovernanceQuorum,
    RepairState RepairState,
    int TotalRepairs,
    int SuccessfulRepairs,
    float CurrentHealth);

/// <summary>Event args for repair proposals.</summary>
public sealed class RepairProposalEventArgs(RepairStrategy strategy, RepairParameters parameters) : EventArgs
{
    public RepairStrategy Strategy { get; } = strategy;
    public RepairParameters Parameters { get; } = parameters;
}
