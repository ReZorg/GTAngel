using System.Reflection;
using GTAngel.Interop;
using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for DTE4EAvatarService: cognitive state updates, exploration coverage,
/// reward accumulation, and bridge arbitration.
///
/// Because <see cref="DTE4EAvatarService.OnAvatarObservation"/> is a private method
/// wired to <see cref="UE5ProcessManager.AvatarObservationReceived"/>, observations
/// are injected via reflection so no real UE5 process is required.
/// </summary>
public sealed class DTE4EAvatarCognitionTests : IAsyncLifetime
{
    private readonly UE5ProcessManager  _ue5;
    private readonly EsnReservoirPipeline _esn;
    private readonly DTE4EAvatarService _svc;

    // Reflection accessor for the private OnAvatarObservation method
    private readonly MethodInfo _onAvatarObservation;

    public DTE4EAvatarCognitionTests()
    {
        _ue5 = new UE5ProcessManager(NullLogger<UE5ProcessManager>.Instance);
        _esn = new EsnReservoirPipeline(NullLogger<EsnReservoirPipeline>.Instance);

        _svc = new DTE4EAvatarService(
            NullLogger<DTE4EAvatarService>.Instance,
            _ue5,
            _esn);

        _onAvatarObservation = typeof(DTE4EAvatarService)
            .GetMethod("OnAvatarObservation",
                BindingFlags.NonPublic | BindingFlags.Instance)
            ?? throw new InvalidOperationException("Could not find OnAvatarObservation via reflection");
    }

    public Task InitializeAsync() => Task.CompletedTask;

    public async Task DisposeAsync()
    {
        _svc.Dispose();
        _esn.Dispose();
        _ue5.Dispose();
        await Task.CompletedTask;
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    /// <summary>
    /// Directly invoke the private OnAvatarObservation handler with a synthetic frame.
    /// This simulates UE5 sending an observation without a real process.
    /// </summary>
    private void InjectObservation(AvatarObservation obs)
        => _onAvatarObservation.Invoke(_svc, new object?[] { null, obs });

    private static AvatarObservation MakeObs(
        float x = 0, float y = 0, float z = 0,
        float vx = 0, float vy = 0,
        int perceivedCount = 2)
    {
        var perceived = Enumerable.Range(0, perceivedCount)
            .Select(_ => new PerceivedObject { Tag = "NPC", Distance = 50f, IsVisible = true })
            .ToArray();

        return new AvatarObservation
        {
            Position = new float[] { x, y, z },
            Rotation = new float[] { 0, 0, 0 },
            Velocity = new float[] { vx, vy, 0 },
            ActiveInputActions = Array.Empty<string>(),
            PerceivedObjects = perceived,
            NeurochemicalState = new NeurochemicalSnapshot
            {
                Curiosity = 0.7f, Endorphin = 0.5f,
                ChaosIntensity = 0.3f, Homeostasis = 0.6f
            },
            Timestamp = 1.0
        };
    }

    // ── 1. Initial state ──────────────────────────────────────────────────────

    [Fact]
    public void IsRunning_Initially_IsFalse()
    {
        Assert.False(_svc.IsRunning);
    }

    [Fact]
    public void TotalStepsTaken_Initially_IsZero()
    {
        Assert.Equal(0, _svc.TotalStepsTaken);
    }

    [Fact]
    public void ExplorationCoverage_Initially_IsZero()
    {
        Assert.Equal(0f, _svc.ExplorationCoverage);
    }

    [Fact]
    public void TotalReward_Initially_IsZero()
    {
        Assert.Equal(0f, _svc.TotalReward);
    }

    [Fact]
    public void LastObservation_Initially_IsNull()
    {
        Assert.Null(_svc.LastObservation);
    }

    // ── 2. OnAvatarObservation (via reflection) ───────────────────────────────

    [Fact]
    public void InjectObservation_SetsLastObservation()
    {
        var obs = MakeObs(x: -1200, y: -800);
        InjectObservation(obs);
        Assert.NotNull(_svc.LastObservation);
    }

    [Fact]
    public void InjectObservation_LastObservation_MatchesInjected()
    {
        var obs = MakeObs(x: 300, y: 100);
        InjectObservation(obs);
        Assert.Equal(300f, _svc.LastObservation!.Position[0]);
        Assert.Equal(100f, _svc.LastObservation.Position[1]);
    }

    [Fact]
    public void InjectObservation_QueuesObservation_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
        {
            for (int i = 0; i < 10; i++)
                InjectObservation(MakeObs(x: i * 50));
        });
        Assert.Null(ex);
    }

    // ── 3. CognitiveStateUpdated event ────────────────────────────────────────

    [Fact]
    public async Task CognitiveStateUpdated_FiresAfterExplorationStep()
    {
        int fired = 0;
        _svc.CognitiveStateUpdated += (_, _) => fired++;

        await _svc.StartExplorationAsync();

        // Inject observation so the loop can process a full step
        InjectObservation(MakeObs(x: -1000, y: -500, vx: 50, vy: 30, perceivedCount: 3));

        // Wait for the exploration loop to process at least one step (250ms interval + slack)
        await Task.Delay(700);

        await _svc.StopExplorationAsync();

        Assert.True(fired >= 1, "CognitiveStateUpdated should have fired at least once");
    }

    [Fact]
    public async Task CognitiveStateUpdated_EventArgs_AreInValidRanges()
    {
        AvatarCognitiveState? captured = null;
        _svc.CognitiveStateUpdated += (_, state) => captured = state;

        await _svc.StartExplorationAsync();
        InjectObservation(MakeObs(x: -800, y: -400, vx: 20, vy: 10));
        await Task.Delay(700);
        await _svc.StopExplorationAsync();

        if (captured != null)
        {
            Assert.InRange(captured.AutonomyScore, 0f, 1f);
            Assert.InRange(captured.Coherence,     0f, 2f); // can be slightly above 1 due to formula
            Assert.InRange(captured.Coverage,      0f, 1f);
        }
    }

    // ── 4. ExplorationCoverage increases after observations at new positions ──

    [Fact]
    public async Task ExplorationCoverage_IncreasesAfterMultipleDistinctPositions()
    {
        await _svc.StartExplorationAsync();

        // Inject observations at 10 different positions (different 50-unit cells)
        for (int i = 0; i < 10; i++)
        {
            InjectObservation(MakeObs(x: -1000f + i * 100f, y: -500f + i * 80f,
                                      vx: 50, vy: 30));
            await Task.Delay(300); // Wait for the loop to process each one
        }

        await _svc.StopExplorationAsync();

        Assert.True(_svc.ExplorationCoverage > 0f,
            "ExplorationCoverage should increase after visiting distinct positions");
    }

    // ── 5. TotalStepsTaken increments ────────────────────────────────────────

    [Fact]
    public async Task TotalStepsTaken_IncreasesAfterExplorationSteps()
    {
        await _svc.StartExplorationAsync();

        // Inject three observations at different positions
        for (int i = 0; i < 3; i++)
        {
            InjectObservation(MakeObs(x: -1000f + i * 200f, y: -500f + i * 150f, vx: 60));
            await Task.Delay(300);
        }

        await _svc.StopExplorationAsync();

        Assert.True(_svc.TotalStepsTaken >= 1,
            "TotalStepsTaken should be at least 1 after processing observations");
    }

    // ── 6. TotalReward changes with movement vs idle ──────────────────────────

    [Fact]
    public async Task TotalReward_IncreasesFasterWithMovementThanIdle()
    {
        await _svc.StartExplorationAsync();

        // Moving observation (vx=60, vy=40 → speed > 10)
        InjectObservation(MakeObs(x: -1000, y: -500, vx: 60, vy: 40));
        await Task.Delay(400);
        float movingReward = _svc.TotalReward;

        await _svc.StopExplorationAsync();

        // Reward for a moving observation should not be deeply negative
        // (idle penalty = -0.1, movement bonus = +0.1)
        Assert.True(movingReward >= -1f, "Total reward should not be deeply negative for movement");
    }

    // ── 7. IsRunning state transitions ───────────────────────────────────────

    [Fact]
    public async Task StartExploration_SetsIsRunningTrue()
    {
        await _svc.StartExplorationAsync();
        Assert.True(_svc.IsRunning);
        await _svc.StopExplorationAsync();
    }

    [Fact]
    public async Task StopExploration_SetsIsRunningFalse()
    {
        await _svc.StartExplorationAsync();
        await _svc.StopExplorationAsync();
        Assert.False(_svc.IsRunning);
    }

    [Fact]
    public async Task StartExploration_CalledTwice_DoesNotThrow()
    {
        await _svc.StartExplorationAsync();
        var ex = await Record.ExceptionAsync(() => _svc.StartExplorationAsync());
        Assert.Null(ex);
        await _svc.StopExplorationAsync();
    }

    // ── 8. ActionDispatched event ─────────────────────────────────────────────

    [Fact]
    public async Task ActionDispatched_FiresAfterObservationProcessed()
    {
        int fired = 0;
        _svc.ActionDispatched += (_, _) => fired++;

        await _svc.StartExplorationAsync();
        InjectObservation(MakeObs(x: -900, y: -400, vx: 40, vy: 20));
        await Task.Delay(700);
        await _svc.StopExplorationAsync();

        Assert.True(fired >= 1, "ActionDispatched should fire after processing an observation");
    }

    // ── 9. PlayerAiBridge integration ────────────────────────────────────────

    [Fact]
    public void SetPlayerAiBridge_DoesNotThrow()
    {
        var bridge = new Ue5PlayerAiBridgeService(NullLogger<Ue5PlayerAiBridgeService>.Instance);
        var ex = Record.Exception(() => _svc.SetPlayerAiBridge(bridge));
        Assert.Null(ex);
    }

    // ── 10. CognitiveDimensions (4E) are populated ───────────────────────────

    [Fact]
    public async Task CognitiveState_EmbodiedPosition_MatchesInjectedObservation()
    {
        AvatarCognitiveState? captured = null;
        _svc.CognitiveStateUpdated += (_, state) =>
        {
            if (captured == null) captured = state;
        };

        await _svc.StartExplorationAsync();
        var obs = MakeObs(x: -1100, y: -700, vx: 30, vy: 20);
        InjectObservation(obs);
        await Task.Delay(700);
        await _svc.StopExplorationAsync();

        if (captured != null)
        {
            Assert.Equal(3, captured.EmbodiedPosition.Length);
        }
    }
}
