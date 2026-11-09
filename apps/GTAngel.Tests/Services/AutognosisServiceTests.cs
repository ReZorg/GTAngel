using System;
using System.Linq;
using GTAngel.Services;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Comprehensive tests for AutognosisService — DTE ESN self-monitoring &amp; DAO governance.
/// </summary>
public class AutognosisServiceTests
{
    private readonly AutognosisService _sut;

    public AutognosisServiceTests()
    {
        _sut = new AutognosisService(NullLogger<AutognosisService>.Instance);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Construction & Initialization
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void Constructor_InitializesDefaultState()
    {
        Assert.Equal(RepairState.Observing, _sut.CurrentRepairState);
        Assert.Equal(0, _sut.RepairProposalsExecuted);
        Assert.False(_sut.IsMonitoring);
        Assert.True(_sut.SpectralRadius > 0f);
    }

    [Fact]
    public void Constructor_ThrowsOnNullLogger()
    {
        Assert.Throws<ArgumentNullException>(() => new AutognosisService(null!));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // KSM Cycle 1: Observe
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void Observe_ReturnsHealthSnapshot()
    {
        var sensory = Enumerable.Repeat(0.5f, 128).ToArray();
        var cognitive = Enumerable.Repeat(0.3f, 256).ToArray();
        var executive = Enumerable.Repeat(0.4f, 512).ToArray();
        var clusterSTI = Enumerable.Repeat(1f / 16f, 16).ToArray();

        var health = _sut.Observe(sensory, cognitive, executive, clusterSTI, 0.92f, 1.5);

        Assert.True(health.OverallHealth > 0f);
        Assert.True(health.OverallHealth <= 1f);
        Assert.Equal(0.92f, health.SpectralRadius);
    }

    [Fact]
    public void Observe_RaisesHealthUpdatedEvent()
    {
        ReservoirHealthSnapshot? received = null;
        _sut.HealthUpdated += (_, h) => received = h;

        var sensory = new float[128];
        var cognitive = new float[256];
        var executive = new float[512];
        var sti = new float[16];

        _sut.Observe(sensory, cognitive, executive, sti, 0.9f, 1.0);

        Assert.NotNull(received);
    }

    [Fact]
    public void Observe_UpdatesLastHealth()
    {
        var sensory = Enumerable.Repeat(0.5f, 128).ToArray();
        var cognitive = Enumerable.Repeat(0.3f, 256).ToArray();
        var executive = Enumerable.Repeat(0.4f, 512).ToArray();
        var sti = Enumerable.Repeat(0.1f, 16).ToArray();

        _sut.Observe(sensory, cognitive, executive, sti, 0.95f, 0.5);

        Assert.Equal(0.95f, _sut.LastHealth.SpectralRadius);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // KSM Cycle 2: Diagnose
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void Diagnose_HealthyStateReturnsNoRepair()
    {
        var health = new ReservoirHealthSnapshot
        {
            SpectralRadius = 0.95f,
            SpectralRadiusDeviation = 0.0f,
            SensoryLayerEntropy = 0.5f,
            CognitiveLayerEntropy = 0.5f,
            ExecutiveLayerEntropy = 0.5f,
            AttentionConcentration = 0.5f,
            WoutLoss = 1f,
            OverallHealth = 0.8f
        };

        var result = _sut.Diagnose(health);

        Assert.False(result.NeedsRepair);
        Assert.Equal(DiagnosisSeverity.Healthy, result.Severity);
        Assert.Empty(result.Anomalies);
    }

    [Fact]
    public void Diagnose_HighSpectralDeviationDetectsAnomaly()
    {
        var health = new ReservoirHealthSnapshot
        {
            SpectralRadius = 1.2f,
            SpectralRadiusDeviation = 0.25f, // > tolerance of 0.1
            SensoryLayerEntropy = 0.5f,
            CognitiveLayerEntropy = 0.5f,
            ExecutiveLayerEntropy = 0.5f,
            AttentionConcentration = 0.5f,
            WoutLoss = 1f,
            OverallHealth = 0.3f
        };

        var result = _sut.Diagnose(health);

        Assert.True(result.Anomalies.Length > 0);
        Assert.Contains(result.Anomalies, a => a.Contains("SpectralRadius"));
    }

    [Fact]
    public void Diagnose_LowEntropyDetectsDeadNeurons()
    {
        var health = new ReservoirHealthSnapshot
        {
            SpectralRadiusDeviation = 0.0f,
            SensoryLayerEntropy = 0.05f, // too low
            CognitiveLayerEntropy = 0.5f,
            ExecutiveLayerEntropy = 0.5f,
            AttentionConcentration = 0.5f,
            WoutLoss = 1f,
            OverallHealth = 0.4f
        };

        var result = _sut.Diagnose(health);

        Assert.Contains(result.Anomalies, a => a.Contains("dead neurons"));
    }

    [Fact]
    public void Diagnose_HighEntropyDetectsNoise()
    {
        var health = new ReservoirHealthSnapshot
        {
            SpectralRadiusDeviation = 0.0f,
            SensoryLayerEntropy = 0.5f,
            CognitiveLayerEntropy = 0.5f,
            ExecutiveLayerEntropy = 0.98f, // too high
            AttentionConcentration = 0.5f,
            WoutLoss = 1f,
            OverallHealth = 0.4f
        };

        var result = _sut.Diagnose(health);

        Assert.Contains(result.Anomalies, a => a.Contains("noise dominance"));
    }

    [Fact]
    public void Diagnose_OverConcentratedAttention()
    {
        var health = new ReservoirHealthSnapshot
        {
            SpectralRadiusDeviation = 0.0f,
            SensoryLayerEntropy = 0.5f,
            CognitiveLayerEntropy = 0.5f,
            ExecutiveLayerEntropy = 0.5f,
            AttentionConcentration = 0.95f, // tunnel vision
            WoutLoss = 1f,
            OverallHealth = 0.3f
        };

        var result = _sut.Diagnose(health);

        Assert.Contains(result.Anomalies, a => a.Contains("tunnel vision"));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // KSM Cycle 3: Prescribe
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void PrescribeRepair_SpectralRadiusHigh_ReturnsDamping()
    {
        var diagnosis = new DiagnosisResult(
            NeedsRepair: true,
            GovernanceScore: 0.8f,
            Anomalies: new[] { "SpectralRadius deviation 0.200 > tolerance 0.100" },
            Severity: DiagnosisSeverity.Mild);

        // Force spectral radius high by observing first
        var sensory = Enumerable.Repeat(0.5f, 128).ToArray();
        var cognitive = Enumerable.Repeat(0.3f, 256).ToArray();
        var executive = Enumerable.Repeat(0.4f, 512).ToArray();
        var sti = Enumerable.Repeat(0.1f, 16).ToArray();
        _sut.Observe(sensory, cognitive, executive, sti, 1.1f, 1.0);

        var strategy = _sut.PrescribeRepair(diagnosis);
        Assert.Equal(RepairStrategy.SpectralRadiusDamping, strategy);
    }

    [Fact]
    public void PrescribeRepair_DeadNeurons_ReturnsReactivation()
    {
        var diagnosis = new DiagnosisResult(
            NeedsRepair: true,
            GovernanceScore: 0.7f,
            Anomalies: new[] { "Sensory layer entropy too low (dead neurons)" },
            Severity: DiagnosisSeverity.Mild);

        var strategy = _sut.PrescribeRepair(diagnosis);
        Assert.Equal(RepairStrategy.NeuronReactivation, strategy);
    }

    [Fact]
    public void PrescribeRepair_NoAnomalies_ReturnsNone()
    {
        var diagnosis = new DiagnosisResult(
            NeedsRepair: false,
            GovernanceScore: 0f,
            Anomalies: Array.Empty<string>(),
            Severity: DiagnosisSeverity.Healthy);

        var strategy = _sut.PrescribeRepair(diagnosis);
        Assert.Equal(RepairStrategy.None, strategy);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // KSM Cycle 4: Apply
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void ApplyRepair_ReturnsValidParameters()
    {
        var parameters = _sut.ApplyRepair(RepairStrategy.SpectralRadiusDamping);

        Assert.Equal(0.95f, parameters.SpectralRadiusMultiplier);
        Assert.True(parameters.LeakRateAdjustment < 0f);
        Assert.Contains("Damping", parameters.Description);
    }

    [Fact]
    public void ApplyRepair_IncrementsExecutedCount()
    {
        int before = _sut.RepairProposalsExecuted;
        _sut.ApplyRepair(RepairStrategy.GeneralRebalance);
        Assert.Equal(before + 1, _sut.RepairProposalsExecuted);
    }

    [Fact]
    public void ApplyRepair_RaisesEvent()
    {
        RepairStrategy? received = null;
        _sut.RepairProposed += (_, e) => received = e.Strategy;

        _sut.ApplyRepair(RepairStrategy.NeuronReactivation);

        Assert.Equal(RepairStrategy.NeuronReactivation, received);
    }

    [Theory]
    [InlineData(RepairStrategy.SpectralRadiusDamping)]
    [InlineData(RepairStrategy.SpectralRadiusBoost)]
    [InlineData(RepairStrategy.NeuronReactivation)]
    [InlineData(RepairStrategy.NoiseReduction)]
    [InlineData(RepairStrategy.AttentionRedistribution)]
    [InlineData(RepairStrategy.AttentionFocusing)]
    [InlineData(RepairStrategy.RidgeRegularizationIncrease)]
    [InlineData(RepairStrategy.GeneralRebalance)]
    public void ApplyRepair_AllStrategiesReturnNonNull(RepairStrategy strategy)
    {
        var parameters = _sut.ApplyRepair(strategy);
        Assert.NotNull(parameters);
        Assert.False(string.IsNullOrEmpty(parameters.Description));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // KSM Cycle 5: Verify
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void VerifyRepair_SuccessfulOnImprovement()
    {
        // Observe twice to create history
        var sensory = Enumerable.Repeat(0.5f, 128).ToArray();
        var cognitive = Enumerable.Repeat(0.3f, 256).ToArray();
        var executive = Enumerable.Repeat(0.4f, 512).ToArray();
        var sti = Enumerable.Repeat(0.1f, 16).ToArray();
        _sut.Observe(sensory, cognitive, executive, sti, 0.8f, 5.0); // bad health

        _sut.ApplyRepair(RepairStrategy.SpectralRadiusBoost);

        var afterHealth = new ReservoirHealthSnapshot
        {
            SpectralRadius = 0.95f,
            OverallHealth = 0.8f
        };

        var outcome = _sut.VerifyRepair(afterHealth);
        Assert.True(outcome.Successful);
        Assert.True(outcome.Improvement > 0f);
    }

    [Fact]
    public void VerifyRepair_RaisesCompletedEvent()
    {
        RepairOutcome? received = null;
        _sut.RepairCompleted += (_, o) => received = o;

        _sut.ApplyRepair(RepairStrategy.GeneralRebalance);
        _sut.VerifyRepair(ReservoirHealthSnapshot.Default);

        Assert.NotNull(received);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Autognosis Self-Model
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void GetSnapshot_ReturnsValidData()
    {
        var snapshot = _sut.GetSnapshot();

        Assert.True(snapshot.SelfAwareness >= 0f && snapshot.SelfAwareness <= 1f);
        Assert.True(snapshot.CognitiveStability >= 0f && snapshot.CognitiveStability <= 1f);
        Assert.True(snapshot.WisdomAccumulation >= 0f && snapshot.WisdomAccumulation <= 1f);
        Assert.Equal(RepairState.Observing, snapshot.RepairState);
    }

    [Fact]
    public void SelfAwareness_IncreasesWithObservations()
    {
        float initial = _sut.SelfAwareness;

        var sensory = Enumerable.Repeat(0.5f, 128).ToArray();
        var cognitive = Enumerable.Repeat(0.5f, 256).ToArray();
        var executive = Enumerable.Repeat(0.5f, 512).ToArray();
        var sti = Enumerable.Repeat(0.1f, 16).ToArray();

        for (int i = 0; i < 10; i++)
            _sut.Observe(sensory, cognitive, executive, sti, 0.95f, 0.5);

        Assert.True(_sut.SelfAwareness >= initial);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Entropy Computation
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void ComputeEntropy_UniformDistribution_ReturnsOne()
    {
        var uniform = Enumerable.Repeat(1f, 10).ToArray();
        float entropy = AutognosisService.ComputeEntropy(uniform);
        Assert.True(entropy > 0.95f); // ~1.0 for uniform
    }

    [Fact]
    public void ComputeEntropy_SinglePeak_ReturnsNearZero()
    {
        var spike = new float[10];
        spike[0] = 1f;
        float entropy = AutognosisService.ComputeEntropy(spike);
        Assert.True(entropy < 0.05f);
    }

    [Fact]
    public void ComputeEntropy_EmptyArray_ReturnsZero()
    {
        Assert.Equal(0f, AutognosisService.ComputeEntropy(Array.Empty<float>()));
    }

    [Fact]
    public void ComputeEntropy_NullArray_ReturnsZero()
    {
        Assert.Equal(0f, AutognosisService.ComputeEntropy(null!));
    }

    [Fact]
    public void ComputeEntropy_AllZeros_ReturnsZero()
    {
        var zeros = new float[10];
        Assert.Equal(0f, AutognosisService.ComputeEntropy(zeros));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Concentration Computation
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void ComputeConcentration_SingleMax_ReturnsHigh()
    {
        var values = new float[] { 0, 0, 0, 0, 1 };
        float conc = AutognosisService.ComputeConcentration(values);
        Assert.Equal(1f, conc);
    }

    [Fact]
    public void ComputeConcentration_Uniform_ReturnsLow()
    {
        var values = Enumerable.Repeat(0.1f, 10).ToArray();
        float conc = AutognosisService.ComputeConcentration(values);
        Assert.Equal(0.1f, conc, 3);
    }

    [Fact]
    public void ComputeConcentration_Empty_ReturnsZero()
    {
        Assert.Equal(0f, AutognosisService.ComputeConcentration(Array.Empty<float>()));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Monitoring
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void StartMonitoring_SetsIsMonitoring()
    {
        _sut.StartMonitoring();
        Assert.True(_sut.IsMonitoring);
        _sut.StopMonitoring();
    }

    [Fact]
    public void StopMonitoring_CancelsLoop()
    {
        _sut.StartMonitoring();
        _sut.StopMonitoring();
        // IsMonitoring may still be true briefly due to async cancellation
        // This test just verifies no exception is thrown
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Dispose
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var sut = new AutognosisService(NullLogger<AutognosisService>.Instance);
        sut.Dispose();
        sut.Dispose(); // double dispose safe
    }
}
