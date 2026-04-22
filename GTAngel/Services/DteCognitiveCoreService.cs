// DteCognitiveCoreService.cs
// KSM Cycle 5 — DTE Cognitive Core
//
// Implements:
//   1. ECAN Attention Economics — STI/LTI importance per neuron cluster
//   2. MOSES Pattern Miner — sliding window + Jaccard similarity
//   3. Ridge Regression Wout Trainer — batch update from replay samples
//   4. Attention-Gated Action Logits — softmax over attention-weighted clusters
//   5. Thompson Sampling Policy — Bayesian action selection with Beta(α,β)
//   6. Cognitive Telemetry Events — for TrainingDashboard live display

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Services;

// ─── Telemetry Records ────────────────────────────────────────────────────────

public sealed record AttentionSnapshot(
    float[] ClusterSTI,          // [16] short-term importance per cluster
    float[] ClusterLTI,          // [16] long-term importance per cluster
    float   AttentionBudget,     // total attention budget consumed
    float   AttentionEntropy,    // Shannon entropy of STI distribution
    string  TopNeuronClusters    // human-readable top-3 cluster indices
);

public sealed record MosesPattern(
    float[] Centroid,            // [32] mean activation pattern (one cluster)
    float   Fitness,             // Jaccard similarity score
    int     MatchCount,          // how many times this pattern was matched
    int     ClusterIndex,        // which neuron cluster this pattern belongs to
    long    LastSeenStep         // training step when last matched
);

public sealed record WoutTrainingSnapshot(
    double  RidgeLoss,           // ridge regression loss (MSE + λ‖W‖²)
    int     SampleCount,         // number of samples used in last batch
    double  WoutNorm,            // Frobenius norm of Wout matrix
    bool    IsConverged          // loss < convergence threshold
);

public sealed record ThompsonSnapshot(
    float[] Alpha,               // [18] Beta distribution α per action
    float[] Beta,                // [18] Beta distribution β per action
    float   PolicyEntropy,       // entropy of Thompson mean policy
    int     SelectedAction       // last Thompson-sampled action
);

public sealed record CognitiveCoherenceSnapshot(
    double  Coherence,           // overall cognitive coherence [0,1]
    double  AttentionCoherence,  // ECAN coherence sub-score
    double  PatternCoherence,    // MOSES coherence sub-score
    double  PolicyCoherence,     // Wout/Thompson coherence sub-score
    int     TotalPatterns,       // total patterns in MOSES library
    double  PatternDiversity     // normalized pattern diversity
);

// ─── Event Args ───────────────────────────────────────────────────────────────

public sealed class AttentionUpdatedEventArgs(AttentionSnapshot snapshot) : EventArgs
{
    public AttentionSnapshot Snapshot { get; } = snapshot;
}

public sealed class PatternMinedEventArgs(MosesPattern pattern) : EventArgs
{
    public MosesPattern Pattern { get; } = pattern;
}

public sealed class WoutTrainedEventArgs(WoutTrainingSnapshot snapshot) : EventArgs
{
    public WoutTrainingSnapshot Snapshot { get; } = snapshot;
}

public sealed class CognitiveCoherenceUpdatedEventArgs(CognitiveCoherenceSnapshot snapshot) : EventArgs
{
    public CognitiveCoherenceSnapshot Snapshot { get; } = snapshot;
}

public sealed class CognitiveLogEventArgs(string message, string level = "INFO") : EventArgs
{
    public string Message { get; } = message;
    public string Level   { get; } = level;
    public DateTime Timestamp { get; } = DateTime.UtcNow;
}

// ─── Main Service ─────────────────────────────────────────────────────────────

/// <summary>
/// DTE Cognitive Core Service — KSM Cycle 5.
///
/// Provides ECAN attention economics, MOSES pattern mining, ridge regression
/// readout training, attention-gated action logits, and Thompson sampling
/// policy for the Deep Tree Echo reservoir pipeline.
/// </summary>
public sealed class DteCognitiveCoreService : IDisposable
{
    // ── Constants ─────────────────────────────────────────────────────────────
    private const int ReservoirSize    = 512;   // total executive neurons
    private const int ClusterCount     = 16;    // neuron clusters (32 neurons each)
    private const int ClusterSize      = 32;    // neurons per cluster
    private const int ActionCount      = 18;    // action dimensions (matches ESN output)
    private const int PatternWindowLen = 50;    // sliding window length for MOSES
    private const int MaxPatterns      = 256;   // max patterns in MOSES library
    private const double RidgeLambda   = 1e-4;  // ridge regression regularization
    private const double LearningRate  = 1e-3;  // Wout update step size
    private const double HebbianDecay  = 0.99;  // LTI hebbian forgetting rate
    private const double StiBudget     = 1.0;   // total STI budget (normalized)
    private const double StiDecay      = 0.95;  // STI decay per step
    private const double JaccardThresh = 0.55;  // MOSES pattern match threshold
    private const double ConvergThresh = 1e-4;  // Wout convergence threshold

    // ── ECAN State ────────────────────────────────────────────────────────────
    private readonly float[] _sti  = new float[ClusterCount];  // short-term importance
    private readonly float[] _lti  = new float[ClusterCount];  // long-term importance
    private long _ecanStep;

    // ── MOSES Pattern Library ─────────────────────────────────────────────────
    private readonly List<MosesPattern> _patterns = new();
    private readonly Queue<float[]> _stateWindow  = new();  // sliding window of reservoir states
    private long _mosesStep;
    private readonly object _patternLock = new();

    // ── Wout (Readout Matrix) ─────────────────────────────────────────────────
    // Wout: [ActionCount × ReservoirSize] — maps reservoir state → action logits
    private readonly float[,] _wout = new float[ActionCount, ReservoirSize];
    private double _woutLoss = 1.0;
    private int _woutSampleCount;
    private bool _woutInitialized;
    private readonly object _woutLock = new();

    // ── Thompson Sampling ─────────────────────────────────────────────────────
    private readonly float[] _thompsonAlpha = new float[ActionCount];
    private readonly float[] _thompsonBeta  = new float[ActionCount];
    private int _lastThompsonAction = -1;

    // ── Telemetry ─────────────────────────────────────────────────────────────
    private long _totalSteps;
    private readonly ILogger<DteCognitiveCoreService>? _logger;
    private readonly Random _rng = new(42);
    private bool _disposed;

    // ── Events ────────────────────────────────────────────────────────────────
    public event EventHandler<AttentionUpdatedEventArgs>?         OnAttentionUpdated;
    public event EventHandler<PatternMinedEventArgs>?             OnPatternMined;
    public event EventHandler<WoutTrainedEventArgs>?              OnWoutTrained;
    public event EventHandler<CognitiveCoherenceUpdatedEventArgs>? OnCognitiveCoherenceUpdated;
    public event EventHandler<CognitiveLogEventArgs>?             OnCognitiveLog;

    // ── Constructor ───────────────────────────────────────────────────────────
    public DteCognitiveCoreService(ILogger<DteCognitiveCoreService>? logger = null)
    {
        _logger = logger;
        InitializeThompson();
        InitializeWout();
        EmitLog("DteCognitiveCoreService initialized — ECAN/MOSES/Wout/Thompson ready.");
    }

    // ─────────────────────────────────────────────────────────────────────────
    // 1. ECAN Attention Economics
    // ─────────────────────────────────────────────────────────────────────────

    /// <summary>
    /// Update ECAN attention scores given the current reservoir state.
    /// Computes cluster activation magnitudes, normalizes to STI budget,
    /// applies hebbian LTI update, and emits OnAttentionUpdated.
    /// </summary>
    public void UpdateAttention(float[] reservoirState)
    {
        if (reservoirState.Length != ReservoirSize) return;

        // Compute cluster activation magnitude (L2 norm per cluster)
        var rawSTI = new float[ClusterCount];
        for (int c = 0; c < ClusterCount; c++)
        {
            float sumSq = 0f;
            int offset = c * ClusterSize;
            for (int n = 0; n < ClusterSize; n++)
            {
                float v = reservoirState[offset + n];
                sumSq += v * v;
            }
            rawSTI[c] = (float)Math.Sqrt(sumSq / ClusterSize);
        }

        // Normalize STI to budget (softmax-style)
        float maxRaw = rawSTI.Max();
        float sumExp = 0f;
        var expSTI = new float[ClusterCount];
        for (int c = 0; c < ClusterCount; c++)
        {
            expSTI[c] = (float)Math.Exp((rawSTI[c] - maxRaw) * 4.0);
            sumExp += expSTI[c];
        }
        for (int c = 0; c < ClusterCount; c++)
        {
            // Decay previous STI, add new activation
            _sti[c] = (float)(_sti[c] * StiDecay + expSTI[c] / sumExp * StiBudget * (1.0 - StiDecay));
        }

        // Hebbian LTI update: LTI slowly tracks STI
        for (int c = 0; c < ClusterCount; c++)
        {
            _lti[c] = (float)(_lti[c] * HebbianDecay + _sti[c] * (1.0 - HebbianDecay));
        }

        _ecanStep++;

        // Compute attention entropy
        float entropy = ComputeEntropy(_sti);
        float budget  = _sti.Sum();

        // Top-3 clusters by STI
        var top3 = _sti
            .Select((v, i) => (v, i))
            .OrderByDescending(x => x.v)
            .Take(3)
            .Select(x => $"C{x.i}({x.v:F2})")
            .ToArray();

        var snapshot = new AttentionSnapshot(
            ClusterSTI:       (float[])_sti.Clone(),
            ClusterLTI:       (float[])_lti.Clone(),
            AttentionBudget:  budget,
            AttentionEntropy: entropy,
            TopNeuronClusters: string.Join(", ", top3)
        );

        OnAttentionUpdated?.Invoke(this, new AttentionUpdatedEventArgs(snapshot));

        if (_ecanStep % 100 == 0)
            EmitLog($"ECAN step {_ecanStep}: top={snapshot.TopNeuronClusters}, entropy={entropy:F3}");
    }

    // ─────────────────────────────────────────────────────────────────────────
    // 2. MOSES Pattern Miner
    // ─────────────────────────────────────────────────────────────────────────

    /// <summary>
    /// Add a reservoir state to the sliding window and attempt pattern mining.
    /// Extracts cluster centroids, computes Jaccard similarity against existing
    /// patterns, and adds new patterns if no match found.
    /// </summary>
    public void MinePatterns(float[] reservoirState)
    {
        if (reservoirState.Length != ReservoirSize) return;

        // Binarize reservoir state per cluster (above-mean activation = 1)
        var binaryState = BinarizeState(reservoirState);

        lock (_patternLock)
        {
            _stateWindow.Enqueue(binaryState);
            if (_stateWindow.Count > PatternWindowLen)
                _stateWindow.Dequeue();

            if (_stateWindow.Count < PatternWindowLen / 2) return;

            _mosesStep++;

            // Compute mean activation pattern per cluster over window
            var windowArr = _stateWindow.ToArray();
            for (int c = 0; c < ClusterCount; c++)
            {
                var centroid = new float[ClusterSize];
                foreach (var state in windowArr)
                {
                    int offset = c * ClusterSize;
                    for (int n = 0; n < ClusterSize; n++)
                        centroid[n] += state[offset + n];
                }
                for (int n = 0; n < ClusterSize; n++)
                    centroid[n] /= windowArr.Length;

                // Binarize centroid
                var binCentroid = centroid.Select(v => v >= 0.5f ? 1f : 0f).ToArray();

                // Find best matching pattern in library
                float bestFitness = 0f;
                int bestIdx = -1;
                for (int p = 0; p < _patterns.Count; p++)
                {
                    if (_patterns[p].ClusterIndex != c) continue;
                    float j = JaccardSimilarity(binCentroid, _patterns[p].Centroid);
                    if (j > bestFitness)
                    {
                        bestFitness = j;
                        bestIdx = p;
                    }
                }

                if (bestFitness >= JaccardThresh && bestIdx >= 0)
                {
                    // Update existing pattern
                    var old = _patterns[bestIdx];
                    _patterns[bestIdx] = old with
                    {
                        Fitness    = (old.Fitness * old.MatchCount + bestFitness) / (old.MatchCount + 1),
                        MatchCount = old.MatchCount + 1,
                        LastSeenStep = _mosesStep
                    };
                }
                else if (_patterns.Count < MaxPatterns)
                {
                    // Add new pattern
                    var newPattern = new MosesPattern(
                        Centroid:     binCentroid,
                        Fitness:      0.5f,
                        MatchCount:   1,
                        ClusterIndex: c,
                        LastSeenStep: _mosesStep
                    );
                    _patterns.Add(newPattern);
                    OnPatternMined?.Invoke(this, new PatternMinedEventArgs(newPattern));
                }
            }

            // Prune stale patterns (not seen in last 10k steps)
            if (_mosesStep % 1000 == 0)
            {
                _patterns.RemoveAll(p => _mosesStep - p.LastSeenStep > 10000);
                EmitLog($"MOSES: {_patterns.Count} patterns, top fitness={GetTopPatternFitness():F3}");
            }
        }
    }

    public int PatternCount
    {
        get { lock (_patternLock) return _patterns.Count; }
    }

    public float GetTopPatternFitness()
    {
        lock (_patternLock)
            return _patterns.Count > 0 ? _patterns.Max(p => p.Fitness) : 0f;
    }

    public double GetPatternDiversity()
    {
        lock (_patternLock)
        {
            if (_patterns.Count == 0) return 0.0;
            // Diversity = mean pairwise Jaccard distance (1 - similarity)
            int n = Math.Min(_patterns.Count, 50); // sample for performance
            double totalDist = 0;
            int pairs = 0;
            for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
            {
                if (_patterns[i].ClusterIndex == _patterns[j].ClusterIndex)
                {
                    totalDist += 1.0 - JaccardSimilarity(_patterns[i].Centroid, _patterns[j].Centroid);
                    pairs++;
                }
            }
            return pairs > 0 ? totalDist / pairs : 0.0;
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // 3. Ridge Regression Wout Trainer
    // ─────────────────────────────────────────────────────────────────────────

    /// <summary>
    /// Batch update Wout using ridge regression on (reservoir_state, target_action) pairs.
    /// Uses online gradient descent with L2 regularization.
    /// </summary>
    public void TrainWout(float[] reservoirState, float[] targetActions)
    {
        if (reservoirState.Length != ReservoirSize) return;
        if (targetActions.Length != ActionCount) return;

        lock (_woutLock)
        {
            // Compute current predictions
            var predicted = new float[ActionCount];
            for (int a = 0; a < ActionCount; a++)
            {
                float dot = 0f;
                for (int n = 0; n < ReservoirSize; n++)
                    dot += _wout[a, n] * reservoirState[n];
                predicted[a] = dot;
            }

            // Compute MSE loss
            double mse = 0;
            for (int a = 0; a < ActionCount; a++)
            {
                double err = predicted[a] - targetActions[a];
                mse += err * err;
            }
            mse /= ActionCount;

            // Ridge penalty
            double ridgePenalty = 0;
            for (int a = 0; a < ActionCount; a++)
            for (int n = 0; n < ReservoirSize; n++)
                ridgePenalty += _wout[a, n] * _wout[a, n];
            ridgePenalty *= RidgeLambda;

            double loss = mse + ridgePenalty;

            // Online gradient descent update
            for (int a = 0; a < ActionCount; a++)
            {
                float err = predicted[a] - targetActions[a];
                for (int n = 0; n < ReservoirSize; n++)
                {
                    double grad = err * reservoirState[n] + RidgeLambda * _wout[a, n];
                    _wout[a, n] -= (float)(LearningRate * grad);
                }
            }

            _woutLoss = _woutLoss * 0.99 + loss * 0.01;  // EMA smoothing
            _woutSampleCount++;

            bool converged = _woutLoss < ConvergThresh;

            // Compute Wout Frobenius norm
            double norm = 0;
            for (int a = 0; a < ActionCount; a++)
            for (int n = 0; n < ReservoirSize; n++)
                norm += _wout[a, n] * _wout[a, n];
            norm = Math.Sqrt(norm);

            var snapshot = new WoutTrainingSnapshot(
                RidgeLoss:    _woutLoss,
                SampleCount:  _woutSampleCount,
                WoutNorm:     norm,
                IsConverged:  converged
            );

            if (_woutSampleCount % 500 == 0)
            {
                OnWoutTrained?.Invoke(this, new WoutTrainedEventArgs(snapshot));
                EmitLog($"Wout: loss={_woutLoss:F6}, samples={_woutSampleCount}, norm={norm:F3}");
            }
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // 4. Attention-Gated Action Logits
    // ─────────────────────────────────────────────────────────────────────────

    /// <summary>
    /// Compute attention-gated action logits from reservoir state.
    /// Each cluster's contribution is weighted by its STI score,
    /// then Wout maps the weighted reservoir to action logits.
    /// </summary>
    public float[] ComputeAttentionGatedLogits(float[] reservoirState)
    {
        if (reservoirState.Length != ReservoirSize)
            return new float[ActionCount];

        // Weight each neuron by its cluster's STI
        var weighted = new float[ReservoirSize];
        for (int c = 0; c < ClusterCount; c++)
        {
            float stiWeight = _sti[c] + 0.1f;  // floor to avoid zero-out
            int offset = c * ClusterSize;
            for (int n = 0; n < ClusterSize; n++)
                weighted[offset + n] = reservoirState[offset + n] * stiWeight;
        }

        // Compute logits via Wout
        var logits = new float[ActionCount];
        lock (_woutLock)
        {
            for (int a = 0; a < ActionCount; a++)
            {
                float dot = 0f;
                for (int n = 0; n < ReservoirSize; n++)
                    dot += _wout[a, n] * weighted[n];
                logits[a] = dot;
            }
        }

        return logits;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // 5. Thompson Sampling Policy
    // ─────────────────────────────────────────────────────────────────────────

    /// <summary>
    /// Select an action using Thompson sampling.
    /// Samples θ_a ~ Beta(α_a, β_a) for each action, returns argmax.
    /// Updates Beta parameters based on reward signal.
    /// </summary>
    public int ThompsonSampleAction(float[] logits)
    {
        if (logits.Length != ActionCount) return 0;

        // Softmax logits to get base probabilities
        float maxLogit = logits.Max();
        float sumExp = 0f;
        var probs = new float[ActionCount];
        for (int a = 0; a < ActionCount; a++)
        {
            probs[a] = (float)Math.Exp(logits[a] - maxLogit);
            sumExp += probs[a];
        }
        for (int a = 0; a < ActionCount; a++)
            probs[a] /= sumExp;

        // Thompson sample: θ_a ~ Beta(α_a, β_a) * prob_a
        float bestScore = float.MinValue;
        int bestAction = 0;
        for (int a = 0; a < ActionCount; a++)
        {
            float theta = SampleBeta(_thompsonAlpha[a], _thompsonBeta[a]);
            float score = theta * probs[a];
            if (score > bestScore)
            {
                bestScore = score;
                bestAction = a;
            }
        }

        _lastThompsonAction = bestAction;
        return bestAction;
    }

    /// <summary>
    /// Update Thompson Beta parameters based on reward for last action.
    /// Positive reward → increment α (success), negative → increment β (failure).
    /// </summary>
    public void UpdateThompson(int action, float reward)
    {
        if (action < 0 || action >= ActionCount) return;
        float normalizedReward = Math.Clamp((reward + 1f) / 2f, 0f, 1f);
        _thompsonAlpha[action] += normalizedReward;
        _thompsonBeta[action]  += (1f - normalizedReward);

        // Clip to prevent overflow
        _thompsonAlpha[action] = Math.Min(_thompsonAlpha[action], 1000f);
        _thompsonBeta[action]  = Math.Min(_thompsonBeta[action],  1000f);
    }

    public ThompsonSnapshot GetThompsonSnapshot()
    {
        float policyEntropy = ComputeEntropy(
            _thompsonAlpha.Zip(_thompsonBeta, (a, b) => a / (a + b)).ToArray()
        );
        return new ThompsonSnapshot(
            Alpha:          (float[])_thompsonAlpha.Clone(),
            Beta:           (float[])_thompsonBeta.Clone(),
            PolicyEntropy:  policyEntropy,
            SelectedAction: _lastThompsonAction
        );
    }

    // ─────────────────────────────────────────────────────────────────────────
    // 6. Cognitive Coherence
    // ─────────────────────────────────────────────────────────────────────────

    public CognitiveCoherenceSnapshot ComputeCoherence()
    {
        // Attention coherence: inverse entropy (focused attention = high coherence)
        float attentionEntropy = ComputeEntropy(_sti);
        double attentionCoherence = 1.0 - attentionEntropy / Math.Log(ClusterCount);

        // Pattern coherence: pattern count / max patterns, weighted by mean fitness
        double patternCoherence;
        lock (_patternLock)
        {
            double meanFitness = _patterns.Count > 0 ? _patterns.Average(p => p.Fitness) : 0.0;
            patternCoherence = Math.Min(1.0, (_patterns.Count / (double)MaxPatterns) * 2.0) * meanFitness;
        }

        // Policy coherence: inverse of Wout loss (normalized)
        double policyCoherence = Math.Max(0.0, 1.0 - Math.Min(1.0, _woutLoss / 0.1));

        double overall = (attentionCoherence * 0.35 + patternCoherence * 0.30 + policyCoherence * 0.35);

        var snapshot = new CognitiveCoherenceSnapshot(
            Coherence:          overall,
            AttentionCoherence: attentionCoherence,
            PatternCoherence:   patternCoherence,
            PolicyCoherence:    policyCoherence,
            TotalPatterns:      PatternCount,
            PatternDiversity:   GetPatternDiversity()
        );

        OnCognitiveCoherenceUpdated?.Invoke(this, new CognitiveCoherenceUpdatedEventArgs(snapshot));
        return snapshot;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // Public Accessors
    // ─────────────────────────────────────────────────────────────────────────

    public float[] GetClusterSTI() => (float[])_sti.Clone();
    public float[] GetClusterLTI() => (float[])_lti.Clone();
    public double  GetWoutLoss()   => _woutLoss;
    public int     GetWoutSampleCount() => _woutSampleCount;
    public float   GetAttentionBudget() => _sti.Sum();
    public float   GetAttentionEntropy() => ComputeEntropy(_sti);

    // ─────────────────────────────────────────────────────────────────────────
    // Private Helpers
    // ─────────────────────────────────────────────────────────────────────────

    private void InitializeThompson()
    {
        for (int a = 0; a < ActionCount; a++)
        {
            _thompsonAlpha[a] = 1f;  // uniform Beta(1,1) prior
            _thompsonBeta[a]  = 1f;
        }
    }

    private void InitializeWout()
    {
        // Xavier initialization for Wout
        double scale = Math.Sqrt(2.0 / (ReservoirSize + ActionCount));
        for (int a = 0; a < ActionCount; a++)
        for (int n = 0; n < ReservoirSize; n++)
            _wout[a, n] = (float)((_rng.NextDouble() * 2 - 1) * scale);
        _woutInitialized = true;
    }

    private static float[] BinarizeState(float[] state)
    {
        float mean = state.Average();
        return state.Select(v => v >= mean ? 1f : 0f).ToArray();
    }

    private static float JaccardSimilarity(float[] a, float[] b)
    {
        if (a.Length != b.Length) return 0f;
        float intersection = 0f, union = 0f;
        for (int i = 0; i < a.Length; i++)
        {
            bool ai = a[i] >= 0.5f, bi = b[i] >= 0.5f;
            if (ai && bi) intersection++;
            if (ai || bi) union++;
        }
        return union > 0 ? intersection / union : 0f;
    }

    private static float ComputeEntropy(float[] distribution)
    {
        float sum = distribution.Sum();
        if (sum <= 0) return 0f;
        float entropy = 0f;
        foreach (float v in distribution)
        {
            float p = v / sum;
            if (p > 1e-9f) entropy -= p * (float)Math.Log(p);
        }
        return entropy;
    }

    /// <summary>
    /// Sample from Beta(α, β) using Johnk's method.
    /// </summary>
    private float SampleBeta(float alpha, float beta)
    {
        // Johnk's method: sample X~Gamma(α,1), Y~Gamma(β,1), return X/(X+Y)
        float x = SampleGamma(alpha);
        float y = SampleGamma(beta);
        float total = x + y;
        return total > 0 ? x / total : 0.5f;
    }

    private float SampleGamma(float shape)
    {
        if (shape < 1f)
            return SampleGamma(shape + 1f) * (float)Math.Pow(_rng.NextDouble(), 1.0 / shape);

        // Marsaglia-Tsang method
        float d = shape - 1f / 3f;
        float c = 1f / (float)Math.Sqrt(9f * d);
        while (true)
        {
            float x, v;
            do
            {
                x = SampleNormal();
                v = 1f + c * x;
            } while (v <= 0);
            v = v * v * v;
            float u = (float)_rng.NextDouble();
            if (u < 1f - 0.0331f * (x * x) * (x * x)) return d * v;
            if (Math.Log(u) < 0.5f * x * x + d * (1f - v + (float)Math.Log(v))) return d * v;
        }
    }

    private float SampleNormal()
    {
        // Box-Muller
        double u1 = 1.0 - _rng.NextDouble();
        double u2 = 1.0 - _rng.NextDouble();
        return (float)(Math.Sqrt(-2.0 * Math.Log(u1)) * Math.Cos(2.0 * Math.PI * u2));
    }

    private void EmitLog(string message, string level = "INFO")
    {
        _logger?.LogInformation("[DteCognitiveCoreService] {Message}", message);
        OnCognitiveLog?.Invoke(this, new CognitiveLogEventArgs(message, level));
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;
        EmitLog("DteCognitiveCoreService disposed.");
    }
}
