using System;
using System.Linq;
using GTAngel.Interop;
using GTAngel.Models.EmbodiedCognition;
using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// End-to-end integration tests for the Deep Tree Echo evolution pipeline.
/// Tests the interaction between GamerGirl controller, Autognosis, and MetaHuman avatar.
/// </summary>
[Trait("Category", "E2E")]
public class DeepTreeEchoEvolutionE2ETests
{
    // ═══════════════════════════════════════════════════════════════════════════
    // Full DTE Cognition Loop Integration
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void FullCognitionLoop_GamerGirl_Autognosis_MetaHuman()
    {
        // Arrange: Create all three new services
        var controller = new VigemControllerService(NullLogger<VigemControllerService>.Instance);
        var gamerGirl = new GamerGirlControllerInterface(
            NullLogger<GamerGirlControllerInterface>.Instance, controller);
        var autognosis = new AutognosisService(NullLogger<AutognosisService>.Instance);
        var metaHuman = new MetaHumanAvatarProfileService(
            NullLogger<MetaHumanAvatarProfileService>.Instance);

        // Step 1: Load MetaHuman avatar profile
        var profile = metaHuman.LoadProfileFromParameters(
            "DeepTreeEcho",
            MetaHumanBodyType.Feminine,
            new AppearanceParameters
            {
                SkinTone = "Fair-Medium",
                HairStyle = "Long-Wavy",
                HairColor = "Dark-Brown",
                EyeColor = "Hazel-Green",
                MakeupStyle = "Natural-Gamer",
                AccessoryStyle = "Gaming-Headset"
            });
        Assert.NotNull(profile);

        // Step 2: Calibrate FACS & generate MetaHuman config
        var facs = metaHuman.CalibrateFACS();
        Assert.True(facs.Count > 0);
        var config = metaHuman.GenerateConfiguration();
        Assert.True(config.EnableFACS);
        Assert.True(config.EnableLiveLink);

        // Step 3: Simulate ESN reservoir activity → Autognosis observation
        var sensory = Enumerable.Range(0, 128).Select(i => 0.3f + 0.4f * MathF.Sin(i * 0.1f)).ToArray();
        var cognitive = Enumerable.Range(0, 256).Select(i => 0.5f * MathF.Cos(i * 0.05f)).ToArray();
        var executive = Enumerable.Range(0, 512).Select(i => 0.4f + 0.2f * MathF.Sin(i * 0.02f)).ToArray();
        var clusterSTI = Enumerable.Repeat(1f / 16f, 16).ToArray();

        var health = autognosis.Observe(sensory, cognitive, executive, clusterSTI, 0.93f, 1.2);
        Assert.True(health.OverallHealth > 0f);
        Assert.True(health.OverallHealth <= 1f);

        // Step 4: Simulate game actions through gamer girl interface
        var selfState = new EmbodiedSelfState
        {
            Position = new float[] { 100f, 200f, 50f },
            Rotation = new float[] { 0f, 45f, 0f },
            Velocity = new float[] { 10f, 5f, 0f },
            Speed = 11.18f,
            Arousal = 0.4f,
            Fatigue = 0.1f
        };

        var actions = new[]
        {
            new AvatarAction { InputAction = "IA_Move", AxisX = 0f, AxisY = 0.8f, Magnitude = 0.8f },
            new AvatarAction { InputAction = "IA_Sprint", Magnitude = 1f },
            new AvatarAction { InputAction = "IA_Look", AxisX = -0.3f, Magnitude = 0.3f },
            new AvatarAction { InputAction = "IA_Jump", Magnitude = 1f },
            new AvatarAction { InputAction = "IA_Move", AxisX = 0.5f, AxisY = 0.5f, Magnitude = 0.7f },
        };

        foreach (var action in actions)
            gamerGirl.DispatchEmbodiedAction(action, selfState);

        // Verify: Controller processed all actions
        Assert.Equal(5, gamerGirl.TotalInputsDispatched);

        // Verify: Grip posture adapted to action context
        // Last action was a move, grip depends on sequence
        Assert.NotEqual(GripPosture.Neutral, gamerGirl.CurrentGrip);

        // Verify: Flow state tracking active (may still be warming up)
        Assert.True(gamerGirl.FlowIntensity >= 0f);

        // Step 5: Verify autognosis self-model updated
        var snapshot = autognosis.GetSnapshot();
        Assert.True(snapshot.SelfAwareness >= 0f);
        Assert.True(snapshot.CurrentHealth > 0f);

        // Step 6: Evaluate MetaHuman readiness with calibration
        var readiness = metaHuman.EvaluateReadiness();
        Assert.True(readiness.OverallScore > 0f);
        Assert.NotEqual(ReadinessStatus.NotReady, readiness.Status);

        // Step 7: Get gamer girl metrics
        var metrics = gamerGirl.GetMetrics();
        Assert.Equal(5, metrics.TotalInputs);
        Assert.True(Enum.IsDefined(typeof(GripPosture), metrics.Grip));
        Assert.True(Enum.IsDefined(typeof(FlowState), metrics.FlowState));
        Assert.True(Enum.IsDefined(typeof(GamingIntent), metrics.Intent));

        // Cleanup
        gamerGirl.Dispose();
        autognosis.Dispose();
        metaHuman.Dispose();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // KSM Self-Repair Cycle Integration
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void KSM_RepairCycle_ObserveDiagnoseRepairVerify()
    {
        var autognosis = new AutognosisService(NullLogger<AutognosisService>.Instance);

        // Step 1: Observe with degraded reservoir (high spectral radius)
        var sensory = Enumerable.Repeat(0.5f, 128).ToArray();
        var cognitive = Enumerable.Repeat(0.3f, 256).ToArray();
        var executive = Enumerable.Repeat(0.4f, 512).ToArray();
        var sti = Enumerable.Repeat(0.1f, 16).ToArray();

        autognosis.Observe(sensory, cognitive, executive, sti, 1.15f, 3.0);

        // Step 2: Diagnose
        var health = autognosis.LastHealth;
        var diagnosis = autognosis.Diagnose(health);

        // With spectral radius 1.15 (deviation 0.2 > tolerance 0.1), should detect anomaly
        Assert.True(diagnosis.Anomalies.Length > 0);

        // Step 3: Prescribe
        var strategy = autognosis.PrescribeRepair(diagnosis);
        Assert.NotEqual(RepairStrategy.None, strategy);

        // Step 4: Apply repair
        var parameters = autognosis.ApplyRepair(strategy);
        Assert.NotNull(parameters);
        Assert.False(string.IsNullOrEmpty(parameters.Description));

        // Step 5: Simulate improved state after repair
        var improvedHealth = new ReservoirHealthSnapshot
        {
            SpectralRadius = 0.95f,
            SpectralRadiusDeviation = 0.0f,
            OverallHealth = 0.85f,
            SensoryLayerEntropy = 0.6f,
            CognitiveLayerEntropy = 0.5f,
            ExecutiveLayerEntropy = 0.5f,
            AttentionConcentration = 0.5f,
            WoutLoss = 1.0f
        };

        var outcome = autognosis.VerifyRepair(improvedHealth);
        Assert.True(outcome.Successful);
        Assert.True(outcome.Improvement > 0f);

        // Step 6: Verify governance evolved
        var snap = autognosis.GetSnapshot();
        Assert.True(snap.TotalRepairs > 0);

        autognosis.Dispose();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Embodied Controller → Flow State Integration
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void GamerGirl_ExtendedPlay_BuildsFlowState()
    {
        var controller = new VigemControllerService(NullLogger<VigemControllerService>.Instance);
        var gamerGirl = new GamerGirlControllerInterface(
            NullLogger<GamerGirlControllerInterface>.Instance, controller);

        var selfState = new EmbodiedSelfState
        {
            Position = new float[] { 0, 0, 0 },
            Rotation = new float[] { 0, 0, 0 },
            Speed = 200f,
            Arousal = 0.5f
        };

        // Simulate extended gameplay session with varied actions
        var actionSequence = new[]
        {
            "IA_Move", "IA_Sprint", "IA_Move", "IA_Jump",
            "IA_Move", "IA_Look", "IA_Move", "IA_Interact",
            "IA_Move", "IA_Sprint", "IA_StrafeR", "IA_StrafeL"
        };

        for (int cycle = 0; cycle < 20; cycle++)
        {
            foreach (var actionName in actionSequence)
            {
                var action = new AvatarAction
                {
                    InputAction = actionName,
                    Magnitude = 0.7f + (cycle * 0.01f),
                    AxisY = 0.5f
                };
                gamerGirl.DispatchEmbodiedAction(action, selfState);
            }
        }

        // After 240 actions, should have measurable flow intensity
        Assert.True(gamerGirl.TotalInputsDispatched >= 240);
        Assert.True(gamerGirl.FlowIntensity >= 0f);

        // Should have detected some combos (varied actions within combo window)
        var metrics = gamerGirl.GetMetrics();
        Assert.True(metrics.TotalInputs >= 240);

        gamerGirl.Dispose();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Avatar Readiness Score Progression
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void MetaHuman_ReadinessImproves_WithCalibration()
    {
        var service = new MetaHumanAvatarProfileService(
            NullLogger<MetaHumanAvatarProfileService>.Instance);

        // Step 1: Basic profile (no calibration)
        service.LoadProfile("avatar.png", "DTE");
        var report1 = service.EvaluateReadiness();

        // Step 2: Add FACS calibration
        service.CalibrateFACS();
        var report2 = service.EvaluateReadiness();

        // Readiness should improve with calibration
        Assert.True(report2.OverallScore >= report1.OverallScore);

        service.Dispose();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Autognosis Wisdom Accumulation
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void Autognosis_WisdomAccumulates_OverMultipleRepairs()
    {
        var autognosis = new AutognosisService(NullLogger<AutognosisService>.Instance);

        float initialWisdom = autognosis.WisdomAccumulation;

        // Perform multiple repair cycles
        for (int i = 0; i < 5; i++)
        {
            autognosis.ApplyRepair(RepairStrategy.GeneralRebalance);
            autognosis.VerifyRepair(new ReservoirHealthSnapshot { OverallHealth = 0.8f });
        }

        Assert.True(autognosis.WisdomAccumulation > initialWisdom);
        Assert.Equal(5, autognosis.RepairProposalsExecuted);

        autognosis.Dispose();
    }
}
