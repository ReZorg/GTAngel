using GTAngel.Models;

namespace GTAngel.Services;

/// <summary>
/// Deep Tree Echo Game World Training Engine.
/// Implements the dte-ksm-evo-autogenesis loop:
///   ( /dte-autonomy-evolution ⊗ /ksm-evolve ) ⊕ /autoresearch
/// </summary>
public class TrainingEngine
{
    private readonly EchoStateNetwork _esn = new();
    private readonly Random _rng = new();
    private CognitiveState _cognitiveState = new();
    private TrainingConfig _config = new();
    private TrainingEpisode? _currentEpisode;
    private readonly List<TrainingEpisode> _episodeHistory = new();
    private readonly List<AutogenesisExperiment> _experiments = new();
    private readonly List<AlexanderProperty> _properties;
    private TrainingStats _stats = new();
    private bool _isRunning;
    private CancellationTokenSource? _cts;

    public EchoStateNetwork ESN => _esn;
    public CognitiveState CognitiveState => _cognitiveState;
    public TrainingConfig Config => _config;
    public TrainingEpisode? CurrentEpisode => _currentEpisode;
    public IReadOnlyList<TrainingEpisode> EpisodeHistory => _episodeHistory;
    public IReadOnlyList<AutogenesisExperiment> Experiments => _experiments;
    public IReadOnlyList<AlexanderProperty> Properties => _properties;
    public TrainingStats Stats => _stats;
    public bool IsRunning => _isRunning;

    public event Action<CognitiveState>? OnCognitiveStateChanged;
    public event Action<TrainingEpisode>? OnEpisodeCompleted;
    public event Action<AutogenesisExperiment>? OnExperimentCompleted;
    public event Action<string>? OnLogMessage;

    public TrainingEngine()
    {
        _properties = AlexanderProperty.CreateAll();
        InitializeProperties();
    }

    private void InitializeProperties()
    {
        foreach (var prop in _properties)
        {
            prop.Score = 0.5 + _rng.NextDouble() * 0.2;
            prop.PreviousScore = prop.Score;
        }
    }

    /// <summary>
    /// Configure and start the autogenesis training loop.
    /// </summary>
    public async Task StartAutogenesisAsync(TrainingConfig config, IProgress<string>? progress = null)
    {
        _config = config;
        _cts = new CancellationTokenSource();
        _isRunning = true;

        OnLogMessage?.Invoke($"[Autogenesis] Starting evolution toward {config.TargetLevel}");
        OnLogMessage?.Invoke($"[Autogenesis] Max experiments: {config.MaxExperiments}, Min coherence: {config.MinCoherence}");

        // Record baseline
        var baseline = await RunBaselineExperiment(progress);
        _experiments.Add(baseline);
        OnExperimentCompleted?.Invoke(baseline);

        // Main autogenesis loop
        int experimentId = 1;
        while (experimentId <= config.MaxExperiments && !_cts.Token.IsCancellationRequested)
        {
            try
            {
                var experiment = await RunExperiment(experimentId, progress, _cts.Token);
                _experiments.Add(experiment);
                OnExperimentCompleted?.Invoke(experiment);

                // Update stats
                UpdateStats();

                // Check if target autonomy level reached
                if (_stats.CurrentAutonomyLevel >= config.TargetLevel)
                {
                    OnLogMessage?.Invoke($"[Autogenesis] TARGET REACHED: {config.TargetLevel}!");
                    break;
                }

                experimentId++;
                await Task.Delay(100, _cts.Token); // Small delay for UI responsiveness
            }
            catch (OperationCanceledException)
            {
                OnLogMessage?.Invoke("[Autogenesis] Loop cancelled by user");
                break;
            }
        }

        _isRunning = false;
        OnLogMessage?.Invoke($"[Autogenesis] Complete. {_experiments.Count} experiments, " +
                           $"Keep ratio: {_stats.KeepRatio:P0}");
    }

    public void Stop()
    {
        _cts?.Cancel();
        _isRunning = false;
    }

    private async Task<AutogenesisExperiment> RunBaselineExperiment(IProgress<string>? progress)
    {
        progress?.Report("Running baseline measurement...");
        var episode = await RunEpisode(0, CancellationToken.None);

        return new AutogenesisExperiment
        {
            ExperimentId = 0,
            Description = "Baseline measurement",
            Hypothesis = "Initial state of the system",
            PrimaryMetric = episode.TotalReward,
            BaselineMetric = 0,
            PropertyCoherenceScore = ComputeOverallPropertyCoherence(),
            Status = ExperimentStatus.Baseline,
            KsmStep = 0
        };
    }

    private async Task<AutogenesisExperiment> RunExperiment(int id, IProgress<string>? progress, CancellationToken ct)
    {
        int ksmStep = id % 12;
        var experiment = new AutogenesisExperiment
        {
            ExperimentId = id,
            KsmStep = ksmStep,
            BaselineMetric = _experiments.Count > 0 ? _experiments.Max(e => e.PrimaryMetric) : 0
        };

        // Step 1: Observe & Hypothesize (Levels of Scale)
        experiment.Hypothesis = GenerateHypothesis(ksmStep);
        OnLogMessage?.Invoke($"[Exp {id}] Hypothesis: {experiment.Hypothesis}");
        progress?.Report($"Experiment {id}: {experiment.Hypothesis}");

        // Step 2: Edit & Commit (Good Shape) — modify ESN parameters
        ApplyHypothesis(experiment);

        // Step 3: Run & Measure (Contrast)
        var episode = await RunEpisode(id, ct);
        experiment.PrimaryMetric = episode.TotalReward;

        // Step 4: Assess Property Coherence (Not-Separateness)
        experiment.PropertyCoherenceScore = ComputeOverallPropertyCoherence();
        foreach (var prop in _properties)
        {
            experiment.PropertyScores[prop.Name] = prop.Score;
        }

        // Step 5: Decide & Enact (Gradients)
        bool metricImproved = experiment.MetricImproved;
        bool coherenceOk = experiment.PropertyCoherenceScore >= _config.MinPropertyCoherence;
        bool streamOk = _esn.StreamCoherence >= _config.MinCoherence;

        if (metricImproved && coherenceOk && streamOk)
        {
            experiment.Status = ExperimentStatus.Keep;
            experiment.StatusReason = $"Metric +{experiment.MetricDelta:F3}, Coherence {experiment.PropertyCoherenceScore:F2}";
            OnLogMessage?.Invoke($"[Exp {id}] KEEP: {experiment.StatusReason}");

            // Update property scores positively
            EvolveProperties(0.02);
        }
        else
        {
            experiment.Status = ExperimentStatus.Discard;
            if (!metricImproved) experiment.StatusReason = "Metric regressed";
            else if (!coherenceOk) experiment.StatusReason = $"Property coherence {experiment.PropertyCoherenceScore:F2} < {_config.MinPropertyCoherence}";
            else experiment.StatusReason = $"Stream coherence {_esn.StreamCoherence:F2} < {_config.MinCoherence}";

            OnLogMessage?.Invoke($"[Exp {id}] DISCARD: {experiment.StatusReason}");

            // Revert parameter changes
            RevertHypothesis();
            EvolveProperties(-0.01);
        }

        return experiment;
    }

    private async Task<TrainingEpisode> RunEpisode(int episodeId, CancellationToken ct)
    {
        _currentEpisode = new TrainingEpisode
        {
            EpisodeId = episodeId,
            StartTime = DateTime.UtcNow,
            AutonomyLevelReached = _stats.CurrentAutonomyLevel
        };

        _cognitiveState = new CognitiveState();
        int maxSteps = 100;
        double totalReward = 0;

        for (int step = 0; step < maxSteps && !ct.IsCancellationRequested; step++)
        {
            // Phase 2.4: Semantic CognitiveState cycle advancement (12-step KSM cycle)
            // Steps 0-4: Perception/Attention/Recognition/Comprehension/Evaluation
            // Steps 5-7: Planning/Intention/Execution
            // Steps 8-11: Monitoring/Learning/Consolidation/Reflection
            int cycleStep = step % 12;
            _cognitiveState.CurrentCycleStep = cycleStep;

            // Generate game state observation (simulated from asset catalog)
            var input = GenerateGameObservation(step);

            // ESN forward pass
            var output = _esn.Step(input);

            // Phase 6.3: Drive the wired DteCognitiveCoreService with the executive
            // reservoir state so that JaccardThresh / RidgeLambda mutations applied
            // by ApplyHypothesis actually exercise MOSES pattern mining and the
            // attention economy whose metrics (top pattern fitness, Wout loss)
            // feed back into ComputeStepReward below — otherwise the autogenesis
            // KEEP/DISCARD decision would be uncorrelated with the mutated
            // cognitive-core parameters.
            if (_cognitiveCoreService != null && _esn.Layers.Count >= 3)
            {
                var execLayer = _esn.Layers[^1];
                if (execLayer.State.Length == 512)
                {
                    var execStateF = new float[execLayer.State.Length];
                    for (int i = 0; i < execStateF.Length; i++)
                        execStateF[i] = (float)execLayer.State[i];
                    _cognitiveCoreService.UpdateAttention(execStateF);
                    if (step % 4 == 0)  // pattern mining is heavier; subsample
                        _cognitiveCoreService.MinePatterns(execStateF);
                }
            }

            // Trigger cognitive sub-processes based on semantic cycle step
            UpdateCognitiveState(output, step);

            // Compute reward
            double reward = ComputeStepReward(output, step);
            totalReward += reward;

            _currentEpisode.Steps = step + 1;
            _currentEpisode.TotalReward = totalReward;
            _currentEpisode.MaxReward = Math.Max(_currentEpisode.MaxReward, reward);
            _currentEpisode.MinReward = Math.Min(_currentEpisode.MinReward, reward);
            _currentEpisode.StreamCoherence = _esn.StreamCoherence;
            _currentEpisode.CognitiveCyclesCompleted = step / 12;

            OnCognitiveStateChanged?.Invoke(_cognitiveState);

            // Check coherence halt
            if (_esn.StreamCoherence < _config.MinCoherence && step > 10)
            {
                _currentEpisode.TerminationReason = EpisodeTermination.CoherenceHalt;
                OnLogMessage?.Invoke($"[Episode {episodeId}] Coherence halt at step {step}");
                break;
            }

            await Task.Yield(); // Allow UI updates
        }

        _currentEpisode.EndTime = DateTime.UtcNow;
        _currentEpisode.IsComplete = true;
        _currentEpisode.PropertyCoherence = ComputeOverallPropertyCoherence();
        _currentEpisode.ExplorationCoverage = Math.Min(1.0, _currentEpisode.Steps / 100.0);

        if (_currentEpisode.TerminationReason == EpisodeTermination.Running)
            _currentEpisode.TerminationReason = EpisodeTermination.Success;

        _episodeHistory.Add(_currentEpisode);
        OnEpisodeCompleted?.Invoke(_currentEpisode);

        return _currentEpisode;
    }

    private double[] GenerateGameObservation(int step)
    {
        var input = new double[_esn.InputSize];

        // Simulate player position moving through Liberty City
        double t = step / 100.0;
        input[0] = Math.Sin(t * Math.PI * 2) * 0.5;       // X position
        input[1] = Math.Cos(t * Math.PI * 2) * 0.5;       // Y position
        input[2] = 0.1 * Math.Sin(t * Math.PI * 4);        // Z position

        // Velocity
        input[3] = Math.Cos(t * Math.PI * 2) * 0.3;
        input[4] = -Math.Sin(t * Math.PI * 2) * 0.3;

        // Health/Stamina
        input[5] = 0.8 + 0.2 * Math.Sin(t * Math.PI);
        input[6] = 0.7 + 0.3 * Math.Cos(t * Math.PI * 1.5);

        // Combat flag
        input[7] = step % 20 < 5 ? 1.0 : 0.0;

        // Nearby entities (simulated)
        for (int i = 8; i < Math.Min(28, _esn.InputSize); i++)
        {
            input[i] = _rng.NextDouble() * 0.5 - 0.25;
        }

        // Environmental features (from game world categories)
        for (int i = 28; i < _esn.InputSize; i++)
        {
            input[i] = Math.Sin((i + step) * 0.1) * 0.3 + _rng.NextDouble() * 0.1;
        }

        return input;
    }

    private void UpdateCognitiveState(double[] output, int step)
    {
        var cs = _cognitiveState;

        // Update 4E cognition
        cs.Embodied = Math.Abs(output[0]) * 0.8 + 0.2;
        cs.Embedded = Math.Abs(output[1]) * 0.7 + 0.3;
        cs.Enacted = Math.Abs(output[2]) * 0.6 + 0.4;
        cs.Extended = Math.Abs(output[3]) * 0.5 + 0.5;

        // Update emotional state
        cs.Valence = Math.Tanh(output[4] * 2);
        cs.Arousal = Math.Abs(Math.Tanh(output[5] * 2));
        cs.Stability = 1.0 - Math.Abs(output[6]) * 0.5;

        // Update consciousness streams
        cs.SensoryStream.Activation = Math.Abs(output[7]) * 0.8 + 0.2;
        cs.CognitiveStream.Activation = Math.Abs(output[8]) * 0.7 + 0.3;
        cs.AffectiveStream.Activation = Math.Abs(output[9]) * 0.6 + 0.4;

        cs.SensoryStream.Coherence = _esn.Layers[0].Activation;
        cs.CognitiveStream.Coherence = _esn.Layers[1].Activation;
        cs.AffectiveStream.Coherence = _esn.Layers[2].Activation;

        cs.SensoryStream.CoherenceHistory.Add(cs.SensoryStream.Coherence);
        cs.CognitiveStream.CoherenceHistory.Add(cs.CognitiveStream.Coherence);
        cs.AffectiveStream.CoherenceHistory.Add(cs.AffectiveStream.Coherence);

        // Trim history
        if (cs.SensoryStream.CoherenceHistory.Count > 200)
        {
            cs.SensoryStream.CoherenceHistory.RemoveAt(0);
            cs.CognitiveStream.CoherenceHistory.RemoveAt(0);
            cs.AffectiveStream.CoherenceHistory.RemoveAt(0);
        }

        // Determine cognitive mode
        if (step % 20 < 5) cs.Mode = CognitiveMode.Combat;
        else if (cs.Arousal < 0.3) cs.Mode = CognitiveMode.Introspection;
        else if (cs.Extended > 0.7) cs.Mode = CognitiveMode.Navigation;
        else if (cs.Embodied > 0.7 && cs.Arousal > 0.6) cs.Mode = CognitiveMode.Flow;
        else cs.Mode = CognitiveMode.Exploration;

        // Wisdom grows logarithmically
        cs.WisdomLevel = Math.Log(1 + step * 0.01) * 0.5;
        cs.IntrospectionDepth = Math.Min(1.0, step / 80.0);

        cs.Timestamp = DateTime.UtcNow;
    }

    private double ComputeStepReward(double[] output, int step)
    {
        double reward = 0;

        // Phase 6.3: Scale reward components by the wired RewardShaper weights so
        // ApplyHypothesis mutations of Weights.Exploration / Weights.Navigation
        // actually move PrimaryMetric. Default weights (Exploration=2.0,
        // Navigation=1.0) reproduce the original constants 0.1 and 0.1.
        double wExp = _rewardShaperRef?.Weights.Exploration ?? 2.0;
        double wNav = _rewardShaperRef?.Weights.Navigation  ?? 1.0;

        // Exploration reward (scaled by Exploration weight)
        reward += 0.05 * wExp * Math.Abs(output[0]);

        // Coherence reward
        reward += 0.3 * _esn.StreamCoherence;

        // 4E integration reward (scaled by Navigation weight as a proxy for the
        // spatial/embodiment coupling that navigation pressures).
        double e4 = (_cognitiveState.Embodied + _cognitiveState.Embedded +
                     _cognitiveState.Enacted + _cognitiveState.Extended) / 4.0;
        reward += 0.1 * wNav * e4;

        // Flow state bonus
        if (_cognitiveState.Mode == CognitiveMode.Flow)
            reward += 0.2;

        // Stability reward
        reward += 0.1 * _cognitiveState.Stability;

        // Phase 6.3: Cognitive-core feedback so JaccardThresh / RidgeLambda
        // mutations change PrimaryMetric. Top pattern fitness rises when MOSES
        // generalizes well (sensitive to JaccardThresh); (1 − Wout loss) rises
        // when ridge regression converges well (sensitive to RidgeLambda).
        if (_cognitiveCoreService != null)
        {
            reward += 0.10 * _cognitiveCoreService.GetTopPatternFitness();
            reward += 0.05 * Math.Max(0.0, 1.0 - _cognitiveCoreService.GetWoutLoss());
        }

        // Noise
        reward += (_rng.NextDouble() - 0.5) * 0.05;

        return Math.Max(0, reward);
    }

    private string GenerateHypothesis(int ksmStep)
    {
        string[] hypotheses =
        {
            "Increase sensory layer activation threshold",
            "Reduce leaking rate for better temporal memory",
            "Boost cross-reservoir coupling strength",
            "Adjust spectral radius for edge of chaos",
            "Increase exploration rate for novel strategies",
            "Optimize reward weights for navigation",
            "Enhance 4E embodiment coupling",
            "Strengthen combat pattern recognition",
            "Improve stream synchronization timing",
            "Reduce cognitive load during transitions",
            "Optimize curriculum difficulty progression",
            "Enhance introspection depth for self-model"
        };
        return hypotheses[ksmStep % hypotheses.Length];
    }

    private double _prevSpectralRadius;
    private double _prevLeakingRate;
    private double _prevExplorationRate;
    private double _prevCurriculumDifficulty;
    // Saved values for rollback of service-level parameters
    private float _prevNavWeight;
    private float _prevExplorationWeight;
    private float _prevJaccardThresh;
    private float _prevRidgeLambda;

    // Phase 6.3: References to real services for expanded hypothesis application
    private DteCognitiveCoreService? _cognitiveCoreService;
    private RewardShaper? _rewardShaperRef;

    /// <summary>Phase 6.3: Wire in DteCognitiveCoreService for MOSES/Thompson parameter mutation.</summary>
    public void SetCognitiveCoreService(DteCognitiveCoreService svc) => _cognitiveCoreService = svc;

    /// <summary>Phase 6.3: Wire in RewardShaper for reward weight mutation.</summary>
    public void SetRewardShaperRef(RewardShaper shaper) => _rewardShaperRef = shaper;

    private void ApplyHypothesis(AutogenesisExperiment exp)
    {
        _prevSpectralRadius    = _esn.SpectralRadius;
        _prevLeakingRate       = _esn.LeakingRate;
        _prevExplorationRate   = _config.ExplorationRate;
        _prevCurriculumDifficulty = _config.CurriculumDifficulty;
        _prevNavWeight         = _rewardShaperRef?.Weights.Navigation ?? 1.0f;
        _prevExplorationWeight = _rewardShaperRef?.Weights.Exploration ?? 2.0f;
        _prevJaccardThresh     = _cognitiveCoreService?.JaccardThresh ?? 0.55f;
        _prevRidgeLambda       = _cognitiveCoreService?.RidgeLambda ?? 1e-4f;

        double delta = (_rng.NextDouble() - 0.5) * _config.MaxParameterDelta;

        // Phase 6.3: Expand ApplyHypothesis across 8 parameter spaces (one per KSM cycle)
        switch (exp.KsmStep % 8)
        {
            case 0:
                _esn.SpectralRadius = Math.Clamp(_esn.SpectralRadius + delta * 0.1, 0.5, 1.2);
                break;
            case 1:
                _esn.LeakingRate = Math.Clamp(_esn.LeakingRate + delta * 0.1, 0.05, 0.95);
                break;
            case 2:
                _config.ExplorationRate = Math.Clamp(_config.ExplorationRate + delta * 0.1, 0.05, 0.95);
                break;
            case 3:
                _config.CurriculumDifficulty = Math.Clamp(_config.CurriculumDifficulty + delta * 0.05, 0.0, 1.0);
                break;
            case 4:
                // Phase 6.3: Reward Navigation weight mutation
                if (_rewardShaperRef != null)
                    _rewardShaperRef.Weights.Navigation = Math.Clamp(_rewardShaperRef.Weights.Navigation + (float)(delta * 0.2), 0.1f, 5.0f);
                break;
            case 5:
                // Phase 6.3: MOSES JaccardThresh mutation
                if (_cognitiveCoreService != null)
                    _cognitiveCoreService.JaccardThresh = Math.Clamp(_cognitiveCoreService.JaccardThresh + (float)(delta * 0.05), 0.1f, 0.9f);
                break;
            case 6:
                // Phase 6.3: Thompson RidgeLambda mutation
                if (_cognitiveCoreService != null)
                    _cognitiveCoreService.RidgeLambda = (float)Math.Clamp(_cognitiveCoreService.RidgeLambda + delta * 1e-5, 1e-6, 1e-1);
                break;
            case 7:
                // Phase 6.3: Exploration reward weight mutation
                if (_rewardShaperRef != null)
                    _rewardShaperRef.Weights.Exploration = Math.Clamp(_rewardShaperRef.Weights.Exploration + (float)(delta * 0.3), 0.1f, 8.0f);
                break;
        }
    }

    private void RevertHypothesis()
    {
        _esn.SpectralRadius          = _prevSpectralRadius;
        _esn.LeakingRate             = _prevLeakingRate;
        _config.ExplorationRate      = _prevExplorationRate;
        _config.CurriculumDifficulty = _prevCurriculumDifficulty;
        if (_rewardShaperRef != null)
        {
            _rewardShaperRef.Weights.Navigation  = _prevNavWeight;
            _rewardShaperRef.Weights.Exploration = _prevExplorationWeight;
        }
        if (_cognitiveCoreService != null)
        {
            _cognitiveCoreService.JaccardThresh = _prevJaccardThresh;
            _cognitiveCoreService.RidgeLambda   = _prevRidgeLambda;
        }
    }

    private void EvolveProperties(double delta)
    {
        foreach (var prop in _properties)
        {
            prop.PreviousScore = prop.Score;
            prop.Score = Math.Clamp(prop.Score + delta + (_rng.NextDouble() - 0.5) * 0.02, 0.0, 1.0);
        }
    }

    public double ComputeOverallPropertyCoherence()
    {
        return _properties.Average(p => p.Score);
    }

    private void UpdateStats()
    {
        _stats.TotalEpisodes = _episodeHistory.Count;
        _stats.CompletedEpisodes = _episodeHistory.Count(e => e.IsComplete);
        _stats.BestReward = _episodeHistory.Count > 0 ? _episodeHistory.Max(e => e.TotalReward) : 0;
        _stats.AverageReward = _episodeHistory.Count > 0 ? _episodeHistory.Average(e => e.TotalReward) : 0;
        _stats.AverageCoherence = _episodeHistory.Count > 0 ? _episodeHistory.Average(e => e.StreamCoherence) : 0;
        _stats.ExperimentsRun = _experiments.Count;
        _stats.ExperimentsKept = _experiments.Count(e => e.Status == ExperimentStatus.Keep);
        _stats.ExperimentsDiscarded = _experiments.Count(e => e.Status == ExperimentStatus.Discard);
        _stats.TotalTrainingTime = _episodeHistory.Aggregate(TimeSpan.Zero, (sum, e) => sum + e.Duration);

        _stats.RewardHistory = _episodeHistory.Select(e => e.TotalReward).ToList();
        _stats.CoherenceHistory = _episodeHistory.Select(e => e.StreamCoherence).ToList();

        // Determine autonomy level based on metrics
        double avgCoherence = _stats.AverageCoherence;
        double keepRatio = _stats.KeepRatio;
        double propCoherence = ComputeOverallPropertyCoherence();

        if (propCoherence > 0.85 && keepRatio > 0.6 && avgCoherence > 0.7)
            _stats.CurrentAutonomyLevel = AutonomyLevel.Autonomous;
        else if (propCoherence > 0.75 && keepRatio > 0.5 && avgCoherence > 0.5)
            _stats.CurrentAutonomyLevel = AutonomyLevel.Embodied;
        else if (propCoherence > 0.65 && keepRatio > 0.4)
            _stats.CurrentAutonomyLevel = AutonomyLevel.Cognitive;
        else if (propCoherence > 0.55 && keepRatio > 0.3)
            _stats.CurrentAutonomyLevel = AutonomyLevel.Strategic;
        else if (_experiments.Count > 3)
            _stats.CurrentAutonomyLevel = AutonomyLevel.Adaptive;
        else
            _stats.CurrentAutonomyLevel = AutonomyLevel.Reactive;
    }
}
