namespace GTAngel.Models;

/// <summary>
/// Echo State Network (ESN) reservoir computing model.
/// Mirrors the angelclaw ReservoirEcho architecture:
///   - 3-layer hierarchical reservoir (128/256/512 neurons)
///   - Spectral radius 0.9, leaking rate 0.3
///   - Cross-reservoir coupling with triadic synchronization
/// </summary>
public class EchoStateNetwork
{
    public int InputSize { get; set; } = 64;
    public int OutputSize { get; set; } = 32;
    public double SpectralRadius { get; set; } = 0.9;
    public double LeakingRate { get; set; } = 0.3;
    public double Sparsity { get; set; } = 0.1;

    public List<ReservoirLayer> Layers { get; set; } = new()
    {
        new ReservoirLayer { Name = "Sensory", NeuronCount = 128, Level = 0 },
        new ReservoirLayer { Name = "Cognitive", NeuronCount = 256, Level = 1 },
        new ReservoirLayer { Name = "Executive", NeuronCount = 512, Level = 2 }
    };

    public double[] CurrentOutput { get; set; } = new double[32];
    public double TotalActivation { get; set; }
    public double StreamCoherence { get; set; }
    public int TotalSteps { get; set; }

    /// <summary>
    /// Simulate one step of the ESN with the given input vector.
    /// </summary>
    public double[] Step(double[] input)
    {
        TotalSteps++;
        var rng = Random.Shared;

        // Process through each layer
        double[] layerOutput = input;
        foreach (var layer in Layers)
        {
            layerOutput = layer.Process(layerOutput, SpectralRadius, LeakingRate, rng);
        }

        // Generate output (simplified readout)
        CurrentOutput = new double[OutputSize];
        var topLayer = Layers[^1];
        for (int i = 0; i < OutputSize; i++)
        {
            double sum = 0;
            for (int j = 0; j < topLayer.NeuronCount; j++)
            {
                sum += topLayer.State[j] * Math.Sin((i + 1.0) * j / topLayer.NeuronCount * Math.PI);
            }
            CurrentOutput[i] = Math.Tanh(sum / topLayer.NeuronCount);
        }

        // Update metrics
        TotalActivation = Layers.Average(l => l.Activation);
        StreamCoherence = ComputeCoherence();

        return CurrentOutput;
    }

    private double ComputeCoherence()
    {
        if (Layers.Count < 2) return 1.0;

        double coherence = 0;
        for (int i = 0; i < Layers.Count - 1; i++)
        {
            var a = Layers[i].State;
            var b = Layers[i + 1].State;
            int minLen = Math.Min(a.Length, b.Length);
            double dot = 0, normA = 0, normB = 0;
            for (int j = 0; j < minLen; j++)
            {
                dot += a[j] * b[j];
                normA += a[j] * a[j];
                normB += b[j] * b[j];
            }
            double denom = Math.Sqrt(normA) * Math.Sqrt(normB);
            coherence += denom > 1e-10 ? Math.Abs(dot / denom) : 0;
        }
        return coherence / (Layers.Count - 1);
    }

    public void Reset()
    {
        foreach (var layer in Layers)
            layer.Reset();
        CurrentOutput = new double[OutputSize];
        TotalActivation = 0;
        StreamCoherence = 0;
        TotalSteps = 0;
    }
}

public class ReservoirLayer
{
    public string Name { get; set; } = string.Empty;
    public int NeuronCount { get; set; }
    public int Level { get; set; }
    public double[] State { get; set; } = Array.Empty<double>();
    public double Activation { get; set; }
    public List<double> ActivationHistory { get; set; } = new();

    private double[,]? _weights;
    private bool _initialized;

    public void Reset()
    {
        State = new double[NeuronCount];
        Activation = 0;
        ActivationHistory.Clear();
        _initialized = false;
    }

    public double[] Process(double[] input, double spectralRadius, double leakingRate, Random rng)
    {
        if (!_initialized)
        {
            Initialize(rng, spectralRadius);
        }

        var newState = new double[NeuronCount];

        // Reservoir update: x(t) = (1-α)x(t-1) + α·tanh(W·x(t-1) + Win·u(t))
        for (int i = 0; i < NeuronCount; i++)
        {
            double recurrent = 0;
            for (int j = 0; j < NeuronCount; j++)
            {
                recurrent += _weights![i, j] * State[j];
            }

            double inputDrive = 0;
            for (int k = 0; k < input.Length; k++)
            {
                inputDrive += input[k] * Math.Sin((i + 1.0) * (k + 1.0) / (NeuronCount * input.Length) * Math.PI * 2);
            }

            newState[i] = (1 - leakingRate) * State[i] + leakingRate * Math.Tanh(recurrent + inputDrive * 0.5);
        }

        State = newState;
        Activation = State.Select(Math.Abs).Average();
        ActivationHistory.Add(Activation);
        if (ActivationHistory.Count > 200) ActivationHistory.RemoveAt(0);

        return State;
    }

    private void Initialize(Random rng, double spectralRadius)
    {
        State = new double[NeuronCount];
        _weights = new double[NeuronCount, NeuronCount];

        // Sparse random initialization
        for (int i = 0; i < NeuronCount; i++)
        {
            for (int j = 0; j < NeuronCount; j++)
            {
                if (rng.NextDouble() < 0.1) // 10% sparsity
                {
                    _weights[i, j] = (rng.NextDouble() * 2 - 1) * spectralRadius;
                }
            }
        }
        _initialized = true;
    }
}
