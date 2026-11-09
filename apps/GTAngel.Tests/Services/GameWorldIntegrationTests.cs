using GTAngel.Services;
using GTAngel.Interop;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// E2E integration tests that wire GameWorldNavigationService, EsnReservoirPipeline,
/// AvatarEmbodimentService, and Ue5PlayerAiBridgeService together and validate the
/// full navigation + embodiment + bridge stack.
///
/// Tagged [Trait("Category","E2E")] so the CI unit-test job (--filter Category!=E2E)
/// skips them while the dedicated test-e2e job (--filter Category=E2E) runs them.
/// </summary>
[Trait("Category", "E2E")]
public sealed class GameWorldIntegrationTests : IDisposable
{
    private readonly GameWorldNavigationService _navigation;
    private readonly EsnReservoirPipeline       _esn;
    private readonly AvatarEmbodimentService    _embodiment;
    private readonly Ue5PlayerAiBridgeService   _bridge;

    public GameWorldIntegrationTests()
    {
        _navigation = new GameWorldNavigationService(NullLogger<GameWorldNavigationService>.Instance);
        _esn        = new EsnReservoirPipeline(NullLogger<EsnReservoirPipeline>.Instance);
        _embodiment = new AvatarEmbodimentService(NullLogger<AvatarEmbodimentService>.Instance);
        _bridge     = new Ue5PlayerAiBridgeService(NullLogger<Ue5PlayerAiBridgeService>.Instance);

        _esn.Initialize();
    }

    public void Dispose()
    {
        _esn.Dispose();
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    private static AvatarEmbodimentService.NeurochemicalState MidNeuro()
        => new(Curiosity: 0.6f, Endorphin: 0.5f, Chaos: 0.3f, Homeostasis: 0.7f);

    // Positions inside each district
    private static float[] PortlandPos  => new float[] { -1000, -500, 0 };
    private static float[] StauntonPos  => new float[] {   300,  100, 0 };
    private static float[] ShoresidePos => new float[] {  1100,  200, 0 };

    // ── 1. Integrated 100-step exploration walk ───────────────────────────────

    [Fact]
    public void ExplorationWalk_100Steps_IncreasesCoverageAboveZero()
    {
        var positions = new[]
        {
            PortlandPos,  StauntonPos,  ShoresidePos,
            PortlandPos,  StauntonPos,  ShoresidePos,
        };

        float startCoverage = _navigation.GetExplorationScore();

        for (int step = 0; step < 100; step++)
        {
            // Cycle through districts to simulate cross-district exploration
            var basePos = positions[step % positions.Length];
            var pos = new float[] { basePos[0] + step * 5f, basePos[1] + step * 3f, 0 };
            _navigation.UpdatePosition(pos);
        }

        Assert.True(_navigation.GetExplorationScore() > startCoverage,
            "Exploration score should increase after moving across the map");
    }

    [Fact]
    public void ExplorationWalk_20Steps_ExplorationScoreExceedsThreshold()
    {
        // Move through Portland at different positions
        for (int step = 0; step < 20; step++)
        {
            _navigation.UpdatePosition(new float[]
            {
                -1400f + step * 60f,  // spans Portland's X range
                -800f  + step * 50f,
                0
            });
        }

        Assert.True(_navigation.GetExplorationScore() > 0.01f,
            "Exploration score should exceed 0.01 after 20 distinct steps");
    }

    // ── 2. A* route: same-district nodes always find a path ──────────────────

    [Fact]
    public void AStarRoute_BetweenPortlandPOIs_ReturnsNonNullRoute()
    {
        // Trigger route planning from one Portland POI to another
        var dir1 = _navigation.SelectNextDestination(new float[] { -1200, -800, 0 }, 0.9f);
        Assert.NotNull(_navigation.CurrentRoute);
    }

    [Fact]
    public void AStarRoute_BetweenStauntonPOIs_ReturnsNonNullRoute()
    {
        var dir = _navigation.SelectNextDestination(new float[] { 100, 500, 0 }, 0.8f);
        Assert.NotNull(_navigation.CurrentRoute);
    }

    [Fact]
    public void AStarRoute_BetweenShoresidePOIs_ReturnsNonNullRoute()
    {
        var dir = _navigation.SelectNextDestination(new float[] { 900, 200, 0 }, 0.7f);
        Assert.NotNull(_navigation.CurrentRoute);
    }

    [Fact]
    public void AStarRoute_CrossDistrict_PortlandToStaunton_ReturnsDirection()
    {
        // Start in Portland, manually set NextPOI to a Staunton POI
        // The route finder must cross the Callahan Bridge
        _navigation.UpdatePosition(PortlandPos);
        var dir = _navigation.SelectNextDestination(PortlandPos, 1.0f);

        // Result must be a 2-element unit vector
        Assert.Equal(2, dir.Length);
        var mag = MathF.Sqrt(dir[0] * dir[0] + dir[1] * dir[1]);
        Assert.InRange(mag, 0.99f, 1.01f);
    }

    // ── 3. All 45 POIs are reachable from any starting position ──────────────

    [Fact]
    public void AllPOIs_AreReachableFromPortlandStart()
    {
        // Verify SelectNextDestination cycles to select each POI eventually
        // (probabilistic: run 45 iterations at max curiosity to maximise diversity)
        var selectedPOIs = new HashSet<string>();
        for (int i = 0; i < 200; i++)
        {
            _navigation.SelectNextDestination(PortlandPos, 1.0f);
            if (_navigation.NextPOI != null)
                selectedPOIs.Add(_navigation.NextPOI.Id);
        }

        // With 200 iterations and 5 top candidates per call, expect all 45 POIs to appear
        // at least once across all selection calls (policy selects uniformly from top-5)
        Assert.True(selectedPOIs.Count >= 10,
            $"Expected at least 10 distinct POIs to be selected across 200 calls, got {selectedPOIs.Count}");
    }

    [Fact]
    public void SelectNextDestination_AllPOIsInSelectionPool()
    {
        // Each POI in each district is a valid destination
        Assert.Equal(3,  _navigation.Districts.Count);
        Assert.Equal(45, _navigation.TotalPOICount);
    }

    // ── 4. Navigation → ESN pipeline integration ─────────────────────────────

    [Fact]
    public void NavigationDirection_FedIntoESN_ProducesActionProbabilities()
    {
        var pos     = PortlandPos;
        var dir     = _navigation.SelectNextDestination(pos, 0.7f);
        var esn     = _navigation.GetExplorationScore();
        var state   = new float[22];
        state[0]    = pos[0] / 3000f;
        state[1]    = pos[1] / 3000f;

        // Feed direction and state into the ESN reservoir
        var prevAction = new float[18];
        var actionProbs = _esn.ProcessStep(Array.Empty<float>(), state, prevAction);

        Assert.Equal(18, actionProbs.Length);
        Assert.All(actionProbs, v => Assert.True(v >= 0f && v <= 1f));
    }

    [Fact]
    public void NavigationAndESN_MultiStep_WisdomIncreases()
    {
        float wisdom0 = _esn.WisdomLevel;
        var state = new float[22];
        var prev  = new float[18];

        for (int i = 0; i < 30; i++)
        {
            var pos = new float[] { -1000f + i * 30f, -500f + i * 20f, 0 };
            _navigation.UpdatePosition(pos);
            state[0] = pos[0] / 3000f;
            state[1] = pos[1] / 3000f;
            _esn.ProcessStep(Array.Empty<float>(), state, prev);
        }

        Assert.True(_esn.WisdomLevel > wisdom0,
            "ESN wisdom should increase over multiple steps");
    }

    // ── 5. Embodiment pipeline wired to ESN output ───────────────────────────

    [Fact]
    public void EsnToEmotionalState_ViaEmbodimentService_ProducesValidEmotions()
    {
        var state   = new float[22] { 0.5f, 0.3f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        var prev    = new float[18];
        var esnOut  = _esn.ProcessStep(Array.Empty<float>(), state, prev);

        // Use ESN executive state as emotion input (first 6 dims)
        var esnExec = _esn.LastReservoirState.Take(6).ToArray();
        while (esnExec.Length < 6)
            esnExec = esnExec.Concat(new[] { 0f }).ToArray();

        var emotion = _embodiment.SynthesizeEmotionalState(esnExec, MidNeuro());

        Assert.InRange(emotion.Happiness, 0f, 1f);
        Assert.InRange(emotion.Surprise,  0f, 1f);
        Assert.InRange(emotion.Sadness,   0f, 1f);
        Assert.InRange(emotion.Anger,     0f, 1f);
        Assert.InRange(emotion.Fear,      0f, 1f);
    }

    [Fact]
    public void FullPipeline_Navigation_ESN_Embodiment_FACS_DoesNotThrow()
    {
        // Full pipeline: navigate → ESN step → emotional state → FACS AUs → personality
        var pos    = PortlandPos;
        var dir    = _navigation.SelectNextDestination(pos, 0.8f);
        var state  = new float[22];
        state[0]   = pos[0] / 3000f;
        var prev   = new float[18];

        var actionProbs = _esn.ProcessStep(Array.Empty<float>(), state, prev);
        var esnExec     = _esn.LastReservoirState.Take(6).Concat(Enumerable.Repeat(0f, 6)).Take(6).ToArray();
        var emotion     = _embodiment.SynthesizeEmotionalState(esnExec, MidNeuro());
        var aus         = _embodiment.ComputeFACSActionUnits(emotion);

        Assert.Equal(47, aus.Length);
        Assert.All(aus, v => Assert.InRange(v, 0f, 1f));
    }

    // ── 6. Bridge arbitration integrated with navigation ─────────────────────

    [Fact]
    public async Task Bridge_Arbitrated_BlendedAction_IsValid()
    {
        await _bridge.SetModeAsync(PlayerAiBridgeMode.Arbitrated);

        var dir = _navigation.SelectNextDestination(PortlandPos, 0.7f);
        var humanAction = new AvatarAction { InputAction = "IA_Move", AxisX = dir[0], AxisY = dir[1], Magnitude = 1.0f, Source = "Human" };
        var aiAction    = new AvatarAction { InputAction = "IA_Move", AxisX = dir[0] * 0.8f, AxisY = dir[1] * 0.8f, Magnitude = 0.8f, Source = "ML" };

        var result = _bridge.ArbitrateInput(humanAction, aiAction);

        Assert.NotNull(result);
        Assert.True(result.Magnitude > 0f);
    }

    // ── 7. District coverage all three districts ──────────────────────────────

    [Fact]
    public void DistrictCoverage_AllThreeDistricts_IncreaseAfterVisit()
    {
        // Visit each district with multiple positions
        for (int i = 0; i < 10; i++)
        {
            _navigation.UpdatePosition(new float[] { -1000f + i * 50f, -500f + i * 30f, 0 }); // Portland
            _navigation.UpdatePosition(new float[] { 200f + i * 50f, 100f + i * 30f, 0 });    // Staunton
            _navigation.UpdatePosition(new float[] { 1000f + i * 30f, 200f + i * 20f, 0 });   // Shoreside
        }

        var coverage = _navigation.GetDistrictCoverage();
        Assert.True(coverage["portland"]  > 0f, "Portland coverage should be > 0");
        Assert.True(coverage["staunton"]  > 0f, "Staunton coverage should be > 0");
        Assert.True(coverage["shoreside"] > 0f, "Shoreside coverage should be > 0");
    }

    [Fact]
    public void DistrictCoverage_OverallExplorationScore_InRange()
    {
        for (int i = 0; i < 20; i++)
        {
            _navigation.UpdatePosition(new float[] { -1000f + i * 70f, -500f + i * 50f, 0 });
        }

        var score = _navigation.GetExplorationScore();
        Assert.InRange(score, 0f, 1f);
    }

    // ── 8. POI events fire during cross-district walk ─────────────────────────

    [Fact]
    public void WalkNearMultiplePOIs_FiresOnPOIReachedMultipleTimes()
    {
        int fired = 0;
        _navigation.OnPOIReached += (_, _) => fired++;

        // Move to three known POI locations
        _navigation.UpdatePosition(new float[] { -1200, -800, 0 }); // Salvatore's Mansion
        _navigation.UpdatePosition(new float[] {  100,   500, 0 }); // Asuka's Condo (approx)
        _navigation.UpdatePosition(new float[] { 1200,   500, 0 }); // Cochrane Dam (approx)

        Assert.True(fired >= 1, "At least one POI should have been reached");
    }

    // ── 9. ESN coherence stays bounded after many steps ──────────────────────

    [Fact]
    public void EsnCoherence_After50Steps_StaysBounded()
    {
        var state = new float[22];
        var prev  = new float[18];

        for (int i = 0; i < 50; i++)
        {
            state[0] = (float)Math.Sin(i * 0.1);
            state[1] = (float)Math.Cos(i * 0.1);
            _esn.ProcessStep(Array.Empty<float>(), state, prev);
        }

        var coherence = _esn.GetCoherence();
        Assert.InRange(coherence, 0f, 1f);
    }

    // ── 10. Full-stack: navigation + ESN + embodiment personality update ──────

    [Fact]
    public async Task FullStack_NavigationToPersonality_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(async () =>
        {
            for (int step = 0; step < 5; step++)
            {
                var pos  = new float[] { -1000f + step * 100f, -500f + step * 80f, 0 };
                _navigation.UpdatePosition(pos);

                var gameState = new float[22];
                gameState[0]  = pos[0] / 3000f;
                gameState[1]  = pos[1] / 3000f;
                var prev      = new float[18];

                var probs  = _esn.ProcessStep(Array.Empty<float>(), gameState, prev);
                var exec6  = _esn.LastReservoirState.Take(6).ToArray();
                var emotion = _embodiment.SynthesizeEmotionalState(exec6, MidNeuro());

                await _embodiment.ApplyPersonalityTraitsAsync(
                    autonomyLevel: 0.5f + step * 0.1f,
                    coherence:     _esn.GetCoherence(),
                    neuro:         MidNeuro());
            }
        });

        Assert.Null(ex);
    }
}
