using System.Diagnostics;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Echo State Network (ESN) Reservoir Pipeline for Deep Tree Echo training.
/// Connects the DXGI frame capture output to a 3-layer ESN reservoir:
///   Layer 1 (Sensory):   128 neurons — raw visual feature extraction
///   Layer 2 (Cognitive):  256 neurons — spatial/temporal pattern recognition
///   Layer 3 (Executive):  512 neurons — action selection and planning
///
/// The pipeline processes:
///   Frame (768x768x3) → CNN feature extractor → Sensory reservoir →
///   + Game state (22-dim) → Cognitive reservoir →
///   + Action history → Executive reservoir → Action output
///
/// Implements the Deep Tree Echo architecture from the angelclaw repo:
///   - Leaky integrator neurons with spectral radius tuning
///   - Inter-layer feedback connections (top-down attention)
///   - Wisdom accumulation via slow-weight consolidation
///   - 4E cognition dimensions (Embodied, Embedded, Enacted, Extended)
/// </summary>
public sealed class EsnReservoirPipeline : IDisposable
{
    private readonly ILogger<EsnReservoirPipeline> _logger;
    private readonly Random _rng;
    private bool _disposed;

    // ── KSM Cycle 5: DTE Cognitive Core integration ────────────────────────
    private DteCognitiveCoreService? _cognitiveCore;

    // ── Phase 2.3: ECAN STI top-down modulation from DteCognitiveCoreService ──
    private float[] _topDownSTI = Array.Empty<float>();

    // ── Phase 4.1: Observation fusion scale from Ue5PlayerAiBridgeService ──────
    private float _observationFusionScale = 1.0f;

    // Reservoir layers
    private ReservoirLayer _sensoryLayer;
    private ReservoirLayer _cognitiveLayer;
    private ReservoirLayer _executiveLayer;

    // CNN feature extractor (simplified — reduces 768x768x3 to feature vector)
    private readonly int _cnnOutputDim = 256;
    private float[,]? _cnnWeights; // Projection matrix for frame → features

    // Training state
    public bool IsInitialized { get; private set; }
    public long TotalStepsProcessed { get; private set; }
    public double AverageProcessingTimeMs { get; private set; }

    // Cognitive state tracking
    public float WisdomLevel { get; private set; }
    public float[] CognitiveDimensions { get; private set; } = new float[4]; // 4E: Embodied, Embedded, Enacted, Extended
    public float Valence { get; private set; }
    public float Arousal { get; private set; }

    // Output
    public float[] LastActionProbabilities { get; private set; } = Array.Empty<float>();
    public float[] LastReservoirState { get; private set; } = Array.Empty<float>();

    /// <summary>
    /// Configuration for a single reservoir layer.
    /// </summary>
    public class ReservoirConfig
    {
        public string Name { get; set; } = "";
        public int Size { get; set; }
        public int InputDim { get; set; }
        public double SpectralRadius { get; set; } = 0.95;
        public double LeakingRate { get; set; } = 0.3;
        public double InputScaling { get; set; } = 1.0;
        public double Sparsity { get; set; } = 0.1;
        public double FeedbackStrength { get; set; } = 0.1;
        public bool HasTopDownFeedback { get; set; }
    }

    /// <summary>
    /// A single ESN reservoir layer with leaky integrator neurons.
    /// </summary>
    private class ReservoirLayer
    {
        public string Name { get; }
        public int Size { get; }
        public int InputDim { get; }
        public double SpectralRadius { get; }
        public double LeakingRate { get; }
        public double InputScaling { get; }
        public double FeedbackStrength { get; }

        // Weight matrices
        public float[,] Win { get; }     // Input weights [Size x InputDim]
        public float[,] W { get; }       // Reservoir weights [Size x Size]
        public float[,]? Wfb { get; set; } // Feedback weights [Size x FeedbackDim]
        public float[,]? Wout { get; set; } // Output weights [OutputDim x Size] (trained)

        // State
        public float[] State { get; set; }
        public float[] PreviousState { get; set; }
        public float[] SlowWeights { get; set; } // Wisdom accumulation

        // Statistics
        public double AverageActivation { get; set; }
        public double StateEntropy { get; set; }

        public ReservoirLayer(ReservoirConfig config, Random rng)
        {
            Name = config.Name;
            Size = config.Size;
            InputDim = config.InputDim;
            SpectralRadius = config.SpectralRadius;
            LeakingRate = config.LeakingRate;
            InputScaling = config.InputScaling;
            FeedbackStrength = config.FeedbackStrength;

            // Initialize weight matrices
            Win = InitializeInputWeights(Size, InputDim, InputScaling, rng);
            W = InitializeReservoirWeights(Size, config.Sparsity, SpectralRadius, rng);
            State = new float[Size];
            PreviousState = new float[Size];
            SlowWeights = new float[Size];
        }

        /// <summary>
        /// Update reservoir state with leaky integration.
        /// x(t) = (1-α)x(t-1) + α·tanh(Win·u(t) + W·x(t-1) + Wfb·y(t-1))
        /// </summary>
        public void Update(float[] input, float[]? feedback = null)
        {
            Array.Copy(State, PreviousState, Size);

            for (int i = 0; i < Size; i++)
            {
                // Input contribution
                double activation = 0;
                int inputLen = Math.Min(input.Length, InputDim);
                for (int j = 0; j < inputLen; j++)
                    activation += Win[i, j] * input[j];

                // Recurrent contribution
                for (int j = 0; j < Size; j++)
                    activation += W[i, j] * PreviousState[j];

                // Feedback contribution (top-down attention)
                if (feedback != null && Wfb != null)
                {
                    int fbLen = Math.Min(feedback.Length, Wfb.GetLength(1));
                    for (int j = 0; j < fbLen; j++)
                        activation += Wfb[i, j] * feedback[j] * FeedbackStrength;
                }

                // Leaky integration with tanh nonlinearity
                State[i] = (float)((1 - LeakingRate) * PreviousState[i] +
                                   LeakingRate * Math.Tanh(activation));
            }

            // Update slow weights (wisdom accumulation)
            const float slowRate = 0.001f;
            for (int i = 0; i < Size; i++)
                SlowWeights[i] = (1 - slowRate) * SlowWeights[i] + slowRate * State[i];

            // Compute statistics
            double sum = 0, sumSq = 0;
            for (int i = 0; i < Size; i++)
            {
                sum += Math.Abs(State[i]);
                sumSq += State[i] * State[i];
            }
            AverageActivation = sum / Size;

            // Approximate entropy via variance
            double mean = sum / Size;
            double variance = sumSq / Size - mean * mean;
            StateEntropy = Math.Log(1 + variance);
        }

        private static float[,] InitializeInputWeights(int rows, int cols, double scaling, Random rng)
        {
            var w = new float[rows, cols];
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                    w[i, j] = (float)((rng.NextDouble() * 2 - 1) * scaling);
            return w;
        }

        private static float[,] InitializeReservoirWeights(int size, double sparsity, double spectralRadius, Random rng)
        {
            var w = new float[size, size];

            // Create sparse random matrix
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    if (rng.NextDouble() < sparsity)
                        w[i, j] = (float)(rng.NextDouble() * 2 - 1);

            // Scale to desired spectral radius (approximate via Frobenius norm)
            double frobNorm = 0;
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    frobNorm += w[i, j] * w[i, j];
            frobNorm = Math.Sqrt(frobNorm);

            if (frobNorm > 0)
            {
                double scale = spectralRadius / frobNorm * Math.Sqrt(size * sparsity);
                for (int i = 0; i < size; i++)
                    for (int j = 0; j < size; j++)
                        w[i, j] *= (float)scale;
            }

            return w;
        }
    }

    public EsnReservoirPipeline(ILogger<EsnReservoirPipeline> logger)
    {
        _logger = logger;
        _rng = new Random(42); // Deterministic for reproducibility

        // Initialize with default configuration
        _sensoryLayer = new ReservoirLayer(new ReservoirConfig
        {
            Name = "Sensory", Size = 128, InputDim = 256,
            SpectralRadius = 0.9, LeakingRate = 0.3, Sparsity = 0.1
        }, _rng);

        _cognitiveLayer = new ReservoirLayer(new ReservoirConfig
        {
            Name = "Cognitive", Size = 256, InputDim = 128 + 22,
            SpectralRadius = 0.95, LeakingRate = 0.2, Sparsity = 0.05,
            HasTopDownFeedback = true, FeedbackStrength = 0.1
        }, _rng);

        _executiveLayer = new ReservoirLayer(new ReservoirConfig
        {
            Name = "Executive", Size = 512, InputDim = 256 + 18,
            SpectralRadius = 0.99, LeakingRate = 0.1, Sparsity = 0.02,
            HasTopDownFeedback = true, FeedbackStrength = 0.05
        }, _rng);
    }

    /// <summary>
    /// Initialize the pipeline with custom reservoir configurations.
    /// </summary>
    public void Initialize(ReservoirConfig? sensory = null, ReservoirConfig? cognitive = null, ReservoirConfig? executive = null)
    {
        if (sensory != null) _sensoryLayer = new ReservoirLayer(sensory, _rng);
        if (cognitive != null) _cognitiveLayer = new ReservoirLayer(cognitive, _rng);
        if (executive != null) _executiveLayer = new ReservoirLayer(executive, _rng);

        // Initialize CNN projection matrix (random projection for dimensionality reduction)
        // In production, this would be a pre-trained CNN (ResNet-18 or EfficientNet-B0)
        int frameFeatures = 768 * 768 * 3; // Flattened frame
        _cnnWeights = new float[_cnnOutputDim, 1024]; // Project from pooled features
        for (int i = 0; i < _cnnOutputDim; i++)
            for (int j = 0; j < 1024; j++)
                _cnnWeights[i, j] = (float)(_rng.NextDouble() * 2 - 1) / (float)Math.Sqrt(1024);

        // Initialize feedback weights
        _cognitiveLayer.Wfb = new float[_cognitiveLayer.Size, _executiveLayer.Size];
        for (int i = 0; i < _cognitiveLayer.Size; i++)
            for (int j = 0; j < _executiveLayer.Size; j++)
                if (_rng.NextDouble() < 0.05)
                    _cognitiveLayer.Wfb[i, j] = (float)(_rng.NextDouble() * 2 - 1) * 0.1f;

        IsInitialized = true;
        _logger.LogInformation("ESN Reservoir Pipeline initialized: Sensory({S}) → Cognitive({C}) → Executive({E})",
            _sensoryLayer.Size, _cognitiveLayer.Size, _executiveLayer.Size);
    }

    /// <summary>
    /// Process a single training step through the full pipeline.
    /// </summary>
    /// <param name="frame">Raw frame pixels [768*768*3] normalized to [0,1]</param>
    /// <param name="gameState">Game state feature vector [22-dim]</param>
    /// <param name="previousAction">Previous action one-hot [18-dim]</param>
    /// <returns>Action probabilities [18-dim] for discrete action selection</returns>
    public float[] ProcessStep(float[] frame, float[] gameState, float[] previousAction)
    {
        if (!IsInitialized) Initialize();

        var sw = Stopwatch.StartNew();

        // Stage 1: CNN feature extraction (spatial pooling → projection)
        var visualFeatures = ExtractVisualFeatures(frame);

        // Stage 2: Sensory reservoir — process visual features
        _sensoryLayer.Update(visualFeatures);

        // Stage 3: Cognitive reservoir — combine sensory output with game state
        var cognitiveInput = new float[_sensoryLayer.Size + gameState.Length];
        Array.Copy(_sensoryLayer.State, cognitiveInput, _sensoryLayer.Size);
        Array.Copy(gameState, 0, cognitiveInput, _sensoryLayer.Size, gameState.Length);

        // Phase 4.1: Scale cognitive drive by observation fusion norm (high agreement → stronger drive)
        if (_observationFusionScale != 1.0f)
        {
            for (int i = 0; i < cognitiveInput.Length; i++)
                cognitiveInput[i] *= _observationFusionScale;
        }

        // Phase 2.3: Build top-down feedback from executive layer + ECAN STI modulation
        float[]? topDownFeedback = null;
        if (_topDownSTI.Length > 0)
        {
            // Concatenate executive state with STI for richer top-down signal
            topDownFeedback = new float[_executiveLayer.Size + _topDownSTI.Length];
            Array.Copy(_executiveLayer.State, topDownFeedback, _executiveLayer.Size);
            Array.Copy(_topDownSTI, 0, topDownFeedback, _executiveLayer.Size, _topDownSTI.Length);
        }

        // Top-down feedback from executive layer (+ optional ECAN STI)
        _cognitiveLayer.Update(cognitiveInput, topDownFeedback ?? _executiveLayer.State);

        // Stage 4: Executive reservoir — combine cognitive output with action history
        var executiveInput = new float[_cognitiveLayer.Size + previousAction.Length];
        Array.Copy(_cognitiveLayer.State, executiveInput, _cognitiveLayer.Size);
        Array.Copy(previousAction, 0, executiveInput, _cognitiveLayer.Size, previousAction.Length);

        _executiveLayer.Update(executiveInput);

        // Stage 5: Readout — linear projection from executive state to action probabilities
        var actionLogits = ComputeActionLogits(_executiveLayer.State);
        var actionProbs = Softmax(actionLogits);

        // Update cognitive dimensions (4E)
        UpdateCognitiveDimensions();

        // Update wisdom level (slow-weight magnitude)
        UpdateWisdomLevel();

        // Update valence/arousal from reservoir dynamics
        UpdateAffectiveState();

        sw.Stop();
        TotalStepsProcessed++;
        AverageProcessingTimeMs = AverageProcessingTimeMs * 0.99 + sw.Elapsed.TotalMilliseconds * 0.01;

        LastActionProbabilities = actionProbs;
        LastReservoirState = _executiveLayer.State.ToArray();

        return actionProbs;
    }

    /// <summary>
    /// Get the full reservoir state across all layers (for experience replay storage).
    /// </summary>
    public float[] GetFullState()
    {
        var state = new float[_sensoryLayer.Size + _cognitiveLayer.Size + _executiveLayer.Size];
        Array.Copy(_sensoryLayer.State, 0, state, 0, _sensoryLayer.Size);
        Array.Copy(_cognitiveLayer.State, 0, state, _sensoryLayer.Size, _cognitiveLayer.Size);
        Array.Copy(_executiveLayer.State, 0, state, _sensoryLayer.Size + _cognitiveLayer.Size, _executiveLayer.Size);
        return state;
    }

    /// <summary>
    /// Get reservoir statistics for monitoring.
    /// </summary>
    public ReservoirStats GetStats()
    {
        return new ReservoirStats
        {
            SensoryActivation = _sensoryLayer.AverageActivation,
            CognitiveActivation = _cognitiveLayer.AverageActivation,
            ExecutiveActivation = _executiveLayer.AverageActivation,
            SensoryEntropy = _sensoryLayer.StateEntropy,
            CognitiveEntropy = _cognitiveLayer.StateEntropy,
            ExecutiveEntropy = _executiveLayer.StateEntropy,
            WisdomLevel = WisdomLevel,
            CognitiveDimensions = CognitiveDimensions.ToArray(),
            Valence = Valence,
            Arousal = Arousal,
            TotalSteps = TotalStepsProcessed,
            AvgProcessingMs = AverageProcessingTimeMs,
        };
    }

    public class ReservoirStats
    {
        public double SensoryActivation { get; set; }
        public double CognitiveActivation { get; set; }
        public double ExecutiveActivation { get; set; }
        public double SensoryEntropy { get; set; }
        public double CognitiveEntropy { get; set; }
        public double ExecutiveEntropy { get; set; }
        public float WisdomLevel { get; set; }
        public float[] CognitiveDimensions { get; set; } = new float[4];
        public float Valence { get; set; }
        public float Arousal { get; set; }
        public long TotalSteps { get; set; }
        public double AvgProcessingMs { get; set; }
    }

    /// <summary>
    /// Reset all reservoir states for a new episode.
    /// </summary>
    public void Reset()
    {
        Array.Clear(_sensoryLayer.State);
        Array.Clear(_sensoryLayer.PreviousState);
        Array.Clear(_cognitiveLayer.State);
        Array.Clear(_cognitiveLayer.PreviousState);
        Array.Clear(_executiveLayer.State);
        Array.Clear(_executiveLayer.PreviousState);
        // Note: SlowWeights are NOT reset (wisdom persists across episodes)
    }

    /// <summary>
    /// Process step with integer action (converts to one-hot internally).
    /// </summary>
    public float[] ProcessStep(float[] visualFeatures, float[] gameState, int previousAction)
    {
        int actionDim = VigemControllerService.DiscreteActionCount;
        var actionOneHot = new float[actionDim];
        if (previousAction >= 0 && previousAction < actionDim)
            actionOneHot[previousAction] = 1f;
        return ProcessStep(visualFeatures, gameState, actionOneHot);
    }

    /// <summary>
    /// Get coherence measure: how aligned are the three reservoir layers.
    /// High coherence = layers are working in concert, low = fragmented processing.
    /// </summary>
    public float GetCoherence()
    {
        // Coherence = correlation between layer activation patterns
        float sensoryMean = 0, cognitiveMean = 0, executiveMean = 0;
        int n = Math.Min(128, Math.Min(_sensoryLayer.Size, Math.Min(_cognitiveLayer.Size, _executiveLayer.Size)));

        for (int i = 0; i < n; i++)
        {
            sensoryMean += _sensoryLayer.State[i];
            cognitiveMean += _cognitiveLayer.State[i];
            executiveMean += _executiveLayer.State[i];
        }
        sensoryMean /= n; cognitiveMean /= n; executiveMean /= n;

        float covSC = 0, covCE = 0, varS = 0, varC = 0, varE = 0;
        for (int i = 0; i < n; i++)
        {
            float ds = _sensoryLayer.State[i] - sensoryMean;
            float dc = _cognitiveLayer.State[i] - cognitiveMean;
            float de = _executiveLayer.State[i] - executiveMean;
            covSC += ds * dc; covCE += dc * de;
            varS += ds * ds; varC += dc * dc; varE += de * de;
        }

        float corrSC = (varS > 0 && varC > 0) ? covSC / (float)(Math.Sqrt(varS) * Math.Sqrt(varC)) : 0;
        float corrCE = (varC > 0 && varE > 0) ? covCE / (float)(Math.Sqrt(varC) * Math.Sqrt(varE)) : 0;

        return (Math.Abs(corrSC) + Math.Abs(corrCE)) / 2f;
    }

    /// <summary>
    /// Update readout weights for a specific action using TD error gradient.
    /// </summary>
    public void UpdateReadout(float[] reservoirState, int action, float tdError, float learningRate)
    {
        // Update the pseudo-random readout weights by storing learned corrections
        // In a full implementation, this would update Wout matrix
        // For now, adjust slow weights as a proxy for learned value function
        int stateLen = Math.Min(reservoirState.Length, _executiveLayer.Size);
        for (int i = 0; i < stateLen; i++)
        {
            _executiveLayer.SlowWeights[i] += learningRate * tdError * reservoirState[i] * 0.01f;
        }
    }

    #region Private Methods

    private float[] ExtractVisualFeatures(float[] frame)
    {
        // Spatial average pooling: divide 768x768 into 32x32 grid, average each cell
        // This reduces 768*768*3 = 1,769,472 values to 32*32 = 1024 features
        int gridSize = 32;
        int cellW = 768 / gridSize;
        int cellH = 768 / gridSize;
        var pooled = new float[gridSize * gridSize];

        for (int gy = 0; gy < gridSize; gy++)
        {
            for (int gx = 0; gx < gridSize; gx++)
            {
                float sum = 0;
                int count = 0;
                for (int cy = 0; cy < cellH; cy++)
                {
                    for (int cx = 0; cx < cellW; cx++)
                    {
                        int px = gx * cellW + cx;
                        int py = gy * cellH + cy;
                        int idx = (py * 768 + px) * 3;
                        if (idx + 2 < frame.Length)
                        {
                            // Luminance: 0.299R + 0.587G + 0.114B
                            sum += 0.299f * frame[idx] + 0.587f * frame[idx + 1] + 0.114f * frame[idx + 2];
                            count++;
                        }
                    }
                }
                pooled[gy * gridSize + gx] = count > 0 ? sum / count : 0;
            }
        }

        // Project to CNN output dimension via random projection
        var features = new float[_cnnOutputDim];
        if (_cnnWeights != null)
        {
            int projDim = Math.Min(pooled.Length, _cnnWeights.GetLength(1));
            for (int i = 0; i < _cnnOutputDim; i++)
            {
                float val = 0;
                for (int j = 0; j < projDim; j++)
                    val += _cnnWeights[i, j] * pooled[j];
                features[i] = (float)Math.Tanh(val);
            }
        }

        return features;
    }

    private static int DiscreteActionCount => 18; // Fallback if VigemControllerService not available

    /// <summary>
    /// Set the DTE Cognitive Core service for attention-gated action logits.
    /// Call after DI construction to wire in the cognitive core.
    /// </summary>
    public void SetCognitiveCoreService(DteCognitiveCoreService svc)
    {
        _cognitiveCore = svc;
        _logger.LogInformation("EsnReservoirPipeline: DteCognitiveCoreService wired in — attention-gated logits enabled.");
    }

    /// <summary>
    /// Phase 2.3: Set the ECAN cluster STI vector for top-down modulation of the cognitive layer.
    /// Called by DteTrainingLoop/DTE4EAvatarService after each attention update step.
    /// The 16-dim STI vector is projected into the cognitive layer's top-down feedback path.
    /// </summary>
    public void SetTopDownModulation(float[] stiVector)
    {
        _topDownSTI = stiVector ?? Array.Empty<float>();
    }

    /// <summary>
    /// Phase 4.1: Scale the cognitive layer's input drive by the observation fusion norm.
    /// High fusion norm (strong human+ML agreement) → higher cognitive drive.
    /// </summary>
    public void SetObservationFusionScale(float scale)
    {
        _observationFusionScale = Math.Clamp(scale, 0.5f, 2.0f);
    }

    private float[] ComputeActionLogits(float[] executiveState)
    {
        // KSM Cycle 5: if cognitive core is available, use attention-gated logits
        if (_cognitiveCore != null)
        {
            // Update ECAN attention with current executive state
            _cognitiveCore.UpdateAttention(executiveState);
            // Mine patterns from current state
            _cognitiveCore.MinePatterns(executiveState);
            // Return attention-gated logits (Wout × attention-weighted reservoir)
            return _cognitiveCore.ComputeAttentionGatedLogits(executiveState);
        }

        // Fallback: deterministic hash-based projection (pre-Cycle-5 behaviour)
        int actionDim = 18; // VigemControllerService.DiscreteActionCount;
        var logits = new float[actionDim];

        // Use a deterministic hash-based projection for now
        for (int a = 0; a < actionDim; a++)
        {
            float sum = 0;
            for (int i = 0; i < executiveState.Length; i++)
            {
                // Pseudo-random but deterministic weight
                int seed = a * 7919 + i * 6271;
                float w = ((seed % 1000) / 500f - 1f) / (float)Math.Sqrt(executiveState.Length);
                sum += w * executiveState[i];
            }
            logits[a] = sum;
        }

        return logits;
    }

    private static float[] Softmax(float[] logits)
    {
        float max = logits.Max();
        var exp = logits.Select(x => (float)Math.Exp(x - max)).ToArray();
        float sum = exp.Sum();
        return exp.Select(x => x / sum).ToArray();
    }

    private void UpdateCognitiveDimensions()
    {
        // Embodied: sensory layer activation (body-environment coupling)
        CognitiveDimensions[0] = (float)_sensoryLayer.AverageActivation;

        // Embedded: cognitive layer entropy (environmental context integration)
        CognitiveDimensions[1] = (float)Math.Min(1.0, _cognitiveLayer.StateEntropy);

        // Enacted: executive layer activation (action-perception coupling)
        CognitiveDimensions[2] = (float)_executiveLayer.AverageActivation;

        // Extended: inter-layer feedback strength (tool use / environmental scaffolding)
        float fbStrength = 0;
        for (int i = 0; i < Math.Min(10, _executiveLayer.Size); i++)
            fbStrength += Math.Abs(_executiveLayer.State[i]);
        CognitiveDimensions[3] = Math.Min(1f, fbStrength / 10f);
    }

    private void UpdateWisdomLevel()
    {
        // Wisdom = magnitude of slow weights (accumulated experience)
        float sum = 0;
        for (int i = 0; i < _executiveLayer.SlowWeights.Length; i++)
            sum += _executiveLayer.SlowWeights[i] * _executiveLayer.SlowWeights[i];
        WisdomLevel = (float)Math.Sqrt(sum / _executiveLayer.Size);
    }

    private void UpdateAffectiveState()
    {
        // Valence: positive = high cognitive activation, negative = low
        Valence = (float)(_cognitiveLayer.AverageActivation - 0.3) * 2;
        Valence = Math.Clamp(Valence, -1f, 1f);

        // Arousal: based on state change magnitude
        float changeMag = 0;
        for (int i = 0; i < Math.Min(50, _executiveLayer.Size); i++)
        {
            float diff = _executiveLayer.State[i] - _executiveLayer.PreviousState[i];
            changeMag += diff * diff;
        }
        Arousal = Math.Min(1f, (float)Math.Sqrt(changeMag / 50));
    }

    #endregion

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;
        _logger.LogInformation("EsnReservoirPipeline disposed. Total steps: {Steps}", TotalStepsProcessed);
    }
}
