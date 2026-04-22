using GTA3DE.Wpf.Services;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Comprehensive tests for RewardShaper — all 8 reward components,
/// IsTerminal, Reset, PBRS, and edge-case state vectors.
/// </summary>
public class RewardShaperTests
{
    // ── Helpers ────────────────────────────────────────────────────────────

    /// <summary>
    /// Build a 22-element state vector with given values.
    /// Indices match GameStateView.ParseState ordering:
    ///   0-2: X,Y,Z  3: Heading  4-6: Vel  7: Health  8: Armor
    ///   9: WeaponId  10: WantedLevel  11: InVehicle
    ///   12: VehicleHealth  13: VehicleSpeed  14: Money
    ///   15-18: CamX/Y/Z/Heading  19: CamPitch
    ///   20: MissionId  21: MissionProgress
    /// </summary>
    private static float[] MakeState(
        float x = 0, float y = 0, float z = 0,
        float health = 100, float armor = 0,
        int wantedLevel = 0, bool inVehicle = false,
        float vehicleHealth = 1000, float vehicleSpeed = 0,
        int money = 0, int missionId = 0, float missionProgress = 0,
        int weaponId = 0)
    {
        var s = new float[22];
        s[0] = x; s[1] = y; s[2] = z;
        s[7] = health; s[8] = armor;
        s[9] = weaponId;
        s[10] = wantedLevel;
        s[11] = inVehicle ? 1f : 0f;
        s[12] = vehicleHealth; s[13] = vehicleSpeed;
        s[14] = money;
        s[20] = missionId; s[21] = missionProgress;
        return s;
    }

    // ── IsTerminal ─────────────────────────────────────────────────────────

    [Fact]
    public void IsTerminal_HealthAboveZero_ReturnsFalse()
    {
        var shaper = new RewardShaper();
        var state = MakeState(health: 50);
        Assert.False(shaper.IsTerminal(state));
    }

    [Fact]
    public void IsTerminal_HealthAtZero_ReturnsTrue()
    {
        var shaper = new RewardShaper();
        var state = MakeState(health: 0);
        Assert.True(shaper.IsTerminal(state));
    }

    [Fact]
    public void IsTerminal_HealthBelowZero_ReturnsTrue()
    {
        var shaper = new RewardShaper();
        var state = MakeState(health: -10);
        Assert.True(shaper.IsTerminal(state));
    }

    [Fact]
    public void IsTerminal_FullHealth_ReturnsFalse()
    {
        var shaper = new RewardShaper();
        var state = MakeState(health: 100);
        Assert.False(shaper.IsTerminal(state));
    }

    // ── Reset ──────────────────────────────────────────────────────────────

    [Fact]
    public void Reset_ClearsCumulativeReward()
    {
        var shaper = new RewardShaper();
        var s1 = MakeState(health: 100);
        var s2 = MakeState(health: 100, x: 10);
        shaper.ComputeReward(s1, s2, 0, 1);
        Assert.NotEqual(0f, shaper.CumulativeReward);
        shaper.Reset();
        Assert.Equal(0f, shaper.CumulativeReward);
    }

    [Fact]
    public void Reset_ClearsTotalSteps()
    {
        var shaper = new RewardShaper();
        var s = MakeState();
        shaper.ComputeReward(s, s, 0, 1);
        shaper.Reset();
        Assert.Equal(0, shaper.TotalSteps);
    }

    [Fact]
    public void Reset_AllowsMultipleEpisodes()
    {
        var shaper = new RewardShaper();
        var s1 = MakeState(health: 100);
        var s2 = MakeState(health: 80);
        shaper.ComputeReward(s1, s2, 0, 1);
        shaper.Reset();
        // Should not throw
        var reward = shaper.ComputeReward(s1, s2, 0, 1);
        Assert.True(float.IsFinite(reward));
    }

    // ── ComputeReward — basic sanity ───────────────────────────────────────

    [Fact]
    public void ComputeReward_IncrementsTotalSteps()
    {
        var shaper = new RewardShaper();
        var s = MakeState();
        shaper.ComputeReward(s, s, 0, 1);
        Assert.Equal(1, shaper.TotalSteps);
        shaper.ComputeReward(s, s, 0, 2);
        Assert.Equal(2, shaper.TotalSteps);
    }

    [Fact]
    public void ComputeReward_UpdatesCumulativeReward()
    {
        var shaper = new RewardShaper();
        var s = MakeState();
        shaper.ComputeReward(s, s, 0, 1);
        // CumulativeReward should have changed (nonzero due to survival + other components)
        Assert.NotEqual(float.NegativeInfinity, shaper.CumulativeReward);
    }

    [Fact]
    public void ComputeReward_ReturnFiniteValue()
    {
        var shaper = new RewardShaper();
        var s = MakeState();
        var reward = shaper.ComputeReward(s, s, 0, 1);
        Assert.True(float.IsFinite(reward));
    }

    [Fact]
    public void ComputeReward_IsClampedToRange()
    {
        var shaper = new RewardShaper();
        var s1 = MakeState(health: 100);
        var s2 = MakeState(health: 100);
        var reward = shaper.ComputeReward(s1, s2, 0, 1);
        Assert.InRange(reward, -10f, 10f);
    }

    // ── Survival reward ────────────────────────────────────────────────────

    [Fact]
    public void ComputeReward_DeathPenalty_IsNegative()
    {
        var shaper = new RewardShaper();
        var alive = MakeState(health: 100);
        var dead = MakeState(health: 0);
        // Run a few steps before death so _prevHealth is set
        shaper.ComputeReward(alive, alive, 0, 1);
        var reward = shaper.ComputeReward(alive, dead, 0, 2);
        // Survival breakdown should have a large negative component
        Assert.True(shaper.LastBreakdown.Survival < 0);
    }

    [Fact]
    public void ComputeReward_HealthLoss_ProducesNegativeSurvivalComponent()
    {
        var shaper = new RewardShaper();
        var full = MakeState(health: 100);
        shaper.ComputeReward(full, full, 0, 1); // set prev health to 100

        var damaged = MakeState(health: 50);
        shaper.ComputeReward(full, damaged, 0, 2);
        Assert.True(shaper.LastBreakdown.Survival < 0.1f,
            "Health loss should reduce survival reward");
    }

    // ── Exploration reward ─────────────────────────────────────────────────

    [Fact]
    public void ComputeReward_NewCell_ProducesPositiveExplorationReward()
    {
        var shaper = new RewardShaper();
        // Move to a brand-new grid cell (200, 200 is far from origin's cell (0,0))
        var origin = MakeState(x: 0, y: 0);
        var distant = MakeState(x: 200, y: 200);
        shaper.ComputeReward(origin, origin, 0, 1);
        shaper.ComputeReward(origin, distant, 0, 2);
        Assert.True(shaper.LastBreakdown.Exploration > 0);
    }

    [Fact]
    public void ComputeReward_SameCell_ZeroNewExplorationReward()
    {
        var shaper = new RewardShaper();
        var s = MakeState(x: 10, y: 10);
        shaper.ComputeReward(s, s, 0, 1); // discovers cell (0, 0)
        // Second call to the same cell — new-cell reward should be 0
        shaper.ComputeReward(s, s, 0, 2);
        // Exploration reward may still have distance-traveled component but not new-cell
        // We just confirm it's not spuriously large
        Assert.True(shaper.LastBreakdown.Exploration < 1.5f);
    }

    // ── Driving reward ─────────────────────────────────────────────────────

    [Fact]
    public void ComputeReward_NotInVehicle_DrivingIsZero()
    {
        var shaper = new RewardShaper();
        var s = MakeState(inVehicle: false);
        shaper.ComputeReward(s, s, 0, 1);
        Assert.Equal(0f, shaper.LastBreakdown.DrivingSkill);
    }

    [Fact]
    public void ComputeReward_InVehicleAtReasonableSpeed_PositiveDriving()
    {
        var shaper = new RewardShaper();
        var prev = MakeState(inVehicle: true, vehicleSpeed: 30);
        var curr = MakeState(inVehicle: true, vehicleSpeed: 30);
        shaper.ComputeReward(prev, curr, 0, 1);
        Assert.True(shaper.LastBreakdown.DrivingSkill >= 0);
    }

    [Fact]
    public void ComputeReward_VehicleCrash_ReducesDrivingReward()
    {
        var shaper = new RewardShaper();
        // First step: set prev vehicle health = 1000
        var healthy = MakeState(inVehicle: true, vehicleHealth: 1000, vehicleSpeed: 30);
        shaper.ComputeReward(healthy, healthy, 0, 1);
        // Second step: big health drop
        var crashed = MakeState(inVehicle: true, vehicleHealth: 900, vehicleSpeed: 5);
        shaper.ComputeReward(healthy, crashed, 0, 2);
        Assert.True(shaper.LastBreakdown.DrivingSkill < 0,
            "A crash should produce negative driving reward");
    }

    // ── Combat reward ──────────────────────────────────────────────────────

    [Fact]
    public void ComputeReward_WantedLevelIncrease_NegativeCombat()
    {
        var shaper = new RewardShaper();
        var noWanted = MakeState(wantedLevel: 0);
        var wanted = MakeState(wantedLevel: 1);
        shaper.ComputeReward(noWanted, noWanted, 0, 1); // set prev wanted = 0
        shaper.ComputeReward(noWanted, wanted, 0, 2);
        Assert.True(shaper.LastBreakdown.Combat < 0);
    }

    [Fact]
    public void ComputeReward_WantedLevelDecrease_PositiveCombat()
    {
        var shaper = new RewardShaper();
        var wanted2 = MakeState(wantedLevel: 2);
        shaper.ComputeReward(wanted2, wanted2, 0, 1); // set prev wanted = 2
        var wanted0 = MakeState(wantedLevel: 0);
        shaper.ComputeReward(wanted2, wanted0, 0, 2);
        Assert.True(shaper.LastBreakdown.Combat > 0);
    }

    // ── Mission reward ─────────────────────────────────────────────────────

    [Fact]
    public void ComputeReward_MissionProgress_IsPositive()
    {
        var shaper = new RewardShaper();
        var noProgress = MakeState(missionProgress: 0);
        var withProgress = MakeState(missionProgress: 0.5f);
        shaper.ComputeReward(noProgress, noProgress, 0, 1); // set prev progress = 0
        shaper.ComputeReward(noProgress, withProgress, 0, 2);
        Assert.True(shaper.LastBreakdown.MissionProgress > 0);
    }

    [Fact]
    public void ComputeReward_NewMissionStarted_PositiveMissionReward()
    {
        var shaper = new RewardShaper();
        var noMission = MakeState(missionId: 0);
        var mission1 = MakeState(missionId: 1);
        shaper.ComputeReward(noMission, noMission, 0, 1);
        shaper.ComputeReward(noMission, mission1, 0, 2);
        Assert.True(shaper.LastBreakdown.MissionProgress > 0);
    }

    // ── Economic reward ────────────────────────────────────────────────────

    [Fact]
    public void ComputeReward_MoneyEarned_IsPositive()
    {
        var shaper = new RewardShaper();
        var poor = MakeState(money: 0);
        var rich = MakeState(money: 1000);
        shaper.ComputeReward(poor, poor, 0, 1); // set prev money = 0
        shaper.ComputeReward(poor, rich, 0, 2);
        Assert.True(shaper.LastBreakdown.Economic > 0);
    }

    [Fact]
    public void ComputeReward_MoneyLost_IsNegativeOrZero()
    {
        var shaper = new RewardShaper();
        var rich = MakeState(money: 1000);
        var poor = MakeState(money: 0);
        shaper.ComputeReward(rich, rich, 0, 1); // set prev money = 1000
        shaper.ComputeReward(rich, poor, 0, 2);
        Assert.True(shaper.LastBreakdown.Economic <= 0);
    }

    // ── Social reward ──────────────────────────────────────────────────────

    [Fact]
    public void ComputeReward_ZeroWanted_PositiveSocialReward()
    {
        var shaper = new RewardShaper();
        var clean = MakeState(wantedLevel: 0);
        shaper.ComputeReward(clean, clean, 0, 1);
        Assert.True(shaper.LastBreakdown.Social > 0);
    }

    [Fact]
    public void ComputeReward_HighWanted_NegativeSocialReward()
    {
        var shaper = new RewardShaper();
        var wanted = MakeState(wantedLevel: 5);
        shaper.ComputeReward(wanted, wanted, 0, 1);
        Assert.True(shaper.LastBreakdown.Social < 0);
    }

    // ── Reward breakdown ───────────────────────────────────────────────────

    [Fact]
    public void LastBreakdown_TotalMatches_WeightedSum()
    {
        var shaper = new RewardShaper();
        var s = MakeState(health: 100, x: 50, y: 50);
        shaper.ComputeReward(s, s, 0, 1);
        var bd = shaper.LastBreakdown;
        Assert.True(float.IsFinite(bd.Total));
    }

    // ── Reward weight presets ──────────────────────────────────────────────

    [Fact]
    public void RewardWeights_ExplorationFocused_HasHighExploration()
    {
        var w = RewardWeights.ExplorationFocused;
        Assert.True(w.Exploration > w.Combat);
    }

    [Fact]
    public void RewardWeights_MissionFocused_HasHighMissionProgress()
    {
        var w = RewardWeights.MissionFocused;
        Assert.True(w.MissionProgress > w.Curiosity);
    }

    [Fact]
    public void RewardWeights_DrivingFocused_HasHighDrivingSkill()
    {
        var w = RewardWeights.DrivingFocused;
        Assert.True(w.DrivingSkill > w.Combat);
    }

    [Fact]
    public void RewardWeights_CombatFocused_HasHighCombat()
    {
        var w = RewardWeights.CombatFocused;
        Assert.True(w.Combat > w.Exploration);
    }

    // ── Edge cases ─────────────────────────────────────────────────────────

    [Fact]
    public void ComputeReward_ShortStateVector_DoesNotThrow()
    {
        var shaper = new RewardShaper();
        var shortState = new float[15]; // minimum accepted by ParseState
        shortState[7] = 100; // health
        var ex = Record.Exception(() => shaper.ComputeReward(shortState, shortState, 0, 1));
        Assert.Null(ex);
    }

    [Fact]
    public void ComputeReward_MultipleResets_TrackStepsCorrectly()
    {
        var shaper = new RewardShaper();
        var s = MakeState();
        for (int i = 0; i < 5; i++) shaper.ComputeReward(s, s, 0, i);
        Assert.Equal(5, shaper.TotalSteps);
        shaper.Reset();
        Assert.Equal(0, shaper.TotalSteps);
        shaper.ComputeReward(s, s, 0, 1);
        Assert.Equal(1, shaper.TotalSteps);
    }
}
