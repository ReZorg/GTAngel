using GTA3DE.Wpf.Models;
using Xunit;

namespace GTAngel.Tests.Models;

public class EchoStateNetworkTests
{
    // ── Defaults ───────────────────────────────────────────────────────────

    [Fact]
    public void ESN_DefaultHasThreeLayers()
    {
        var esn = new EchoStateNetwork();
        Assert.Equal(3, esn.Layers.Count);
    }

    [Fact]
    public void ESN_DefaultLayerSizes_Are128_256_512()
    {
        var esn = new EchoStateNetwork();
        Assert.Equal(128, esn.Layers[0].NeuronCount);
        Assert.Equal(256, esn.Layers[1].NeuronCount);
        Assert.Equal(512, esn.Layers[2].NeuronCount);
    }

    [Fact]
    public void ESN_DefaultLayerNames_AreCorrect()
    {
        var esn = new EchoStateNetwork();
        Assert.Equal("Sensory", esn.Layers[0].Name);
        Assert.Equal("Cognitive", esn.Layers[1].Name);
        Assert.Equal("Executive", esn.Layers[2].Name);
    }

    [Fact]
    public void ESN_DefaultParameters_AreCorrect()
    {
        var esn = new EchoStateNetwork();
        Assert.Equal(64, esn.InputSize);
        Assert.Equal(32, esn.OutputSize);
        Assert.Equal(0.9, esn.SpectralRadius);
        Assert.Equal(0.3, esn.LeakingRate);
        Assert.Equal(0.1, esn.Sparsity);
    }

    // ── Step ───────────────────────────────────────────────────────────────

    [Fact]
    public void Step_ReturnsOutputOfCorrectSize()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        var output = esn.Step(input);
        Assert.Equal(esn.OutputSize, output.Length);
    }

    [Fact]
    public void Step_IncrementsTotalSteps()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        esn.Step(input);
        Assert.Equal(1, esn.TotalSteps);
        esn.Step(input);
        Assert.Equal(2, esn.TotalSteps);
    }

    [Fact]
    public void Step_OutputValuesAreBoundedByTanh()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        // Fill with large values to stress-test bounds
        for (int i = 0; i < input.Length; i++) input[i] = 100.0;
        var output = esn.Step(input);
        foreach (var v in output)
        {
            Assert.InRange(v, -1.0, 1.0);
        }
    }

    [Fact]
    public void Step_WithZeroInput_DoesNotThrow()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        var exception = Record.Exception(() => esn.Step(input));
        Assert.Null(exception);
    }

    [Fact]
    public void Step_UpdatesTotalActivation()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        for (int i = 0; i < input.Length; i++) input[i] = 1.0;
        esn.Step(input);
        Assert.True(esn.TotalActivation >= 0);
    }

    [Fact]
    public void Step_MultipleCalls_UpdatesStreamCoherence()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        for (int i = 0; i < 5; i++) esn.Step(input);
        // After initialization and several steps, coherence should be in [0,1]
        Assert.InRange(esn.StreamCoherence, 0.0, 1.0);
    }

    // ── Reset ──────────────────────────────────────────────────────────────

    [Fact]
    public void Reset_ClearsTotalSteps()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        esn.Step(input);
        esn.Step(input);
        Assert.Equal(2, esn.TotalSteps);
        esn.Reset();
        Assert.Equal(0, esn.TotalSteps);
    }

    [Fact]
    public void Reset_ClearsTotalActivation()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        for (int i = 0; i < input.Length; i++) input[i] = 1.0;
        esn.Step(input);
        esn.Reset();
        Assert.Equal(0.0, esn.TotalActivation);
    }

    [Fact]
    public void Reset_ClearsStreamCoherence()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        esn.Step(input);
        esn.Reset();
        Assert.Equal(0.0, esn.StreamCoherence);
    }

    [Fact]
    public void Reset_ResetsCurrentOutput()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        for (int i = 0; i < input.Length; i++) input[i] = 1.0;
        esn.Step(input);
        esn.Reset();
        Assert.All(esn.CurrentOutput, v => Assert.Equal(0.0, v));
    }

    [Fact]
    public void Reset_AllowsReuseAfterReset()
    {
        var esn = new EchoStateNetwork();
        var input = new double[esn.InputSize];
        esn.Step(input);
        esn.Reset();
        var output = esn.Step(input);
        Assert.Equal(esn.OutputSize, output.Length);
        Assert.Equal(1, esn.TotalSteps);
    }

    // ── ReservoirLayer ─────────────────────────────────────────────────────

    [Fact]
    public void ReservoirLayer_Reset_ClearsActivationHistory()
    {
        var layer = new ReservoirLayer { Name = "Test", NeuronCount = 10, Level = 0 };
        var rng = new Random(42);
        layer.Process(new double[5], 0.9, 0.3, rng);
        layer.Process(new double[5], 0.9, 0.3, rng);
        Assert.NotEmpty(layer.ActivationHistory);
        layer.Reset();
        Assert.Empty(layer.ActivationHistory);
    }

    [Fact]
    public void ReservoirLayer_Reset_SetsActivationToZero()
    {
        var layer = new ReservoirLayer { Name = "Test", NeuronCount = 10, Level = 0 };
        var rng = new Random(42);
        layer.Process(new double[5], 0.9, 0.3, rng);
        layer.Reset();
        Assert.Equal(0.0, layer.Activation);
    }

    [Fact]
    public void ReservoirLayer_Process_ReturnsArrayOfCorrectSize()
    {
        var layer = new ReservoirLayer { Name = "Test", NeuronCount = 16, Level = 0 };
        var rng = new Random(42);
        var output = layer.Process(new double[8], 0.9, 0.3, rng);
        Assert.Equal(16, output.Length);
    }

    [Fact]
    public void ReservoirLayer_Process_ActivationHistoryGrows()
    {
        var layer = new ReservoirLayer { Name = "Test", NeuronCount = 10, Level = 0 };
        var rng = new Random(42);
        for (int i = 0; i < 5; i++)
            layer.Process(new double[5], 0.9, 0.3, rng);
        Assert.Equal(5, layer.ActivationHistory.Count);
    }

    [Fact]
    public void ReservoirLayer_ActivationHistory_CapsBeyond200()
    {
        var layer = new ReservoirLayer { Name = "Test", NeuronCount = 8, Level = 0 };
        var rng = new Random(42);
        for (int i = 0; i < 210; i++)
            layer.Process(new double[4], 0.9, 0.3, rng);
        Assert.True(layer.ActivationHistory.Count <= 200);
    }

    [Fact]
    public void ReservoirLayer_InitializesOnFirstProcess()
    {
        var layer = new ReservoirLayer { Name = "Init", NeuronCount = 10, Level = 0 };
        Assert.Empty(layer.State);
        var rng = new Random(0);
        layer.Process(new double[4], 0.9, 0.3, rng);
        Assert.Equal(10, layer.State.Length);
    }
}
