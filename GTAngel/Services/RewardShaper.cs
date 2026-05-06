using System.Collections.Generic;

namespace GTAngel.Services;

/// <summary>
/// Domain-specific reward shaping for GTA3 Deep Tree Echo training.
///
/// Reward components:
///   1. Survival — staying alive is the baseline reward
///   2. Exploration — visiting new areas of the map
///   3. Driving Skill — smooth driving, speed control, avoiding crashes
///   4. Combat Effectiveness — successful combat encounters
///   5. Mission Progress — advancing through game missions
///   6. Economic — earning money, collecting pickups
///   7. Curiosity — intrinsic motivation for novel states (ICM-style)
///   8. Social — avoiding excessive wanted levels (stealth bonus)
///
/// Each component has a configurable weight and can be enabled/disabled.
/// The total reward is a weighted sum of all active components.
///
/// Supports potential-based reward shaping (PBRS) to preserve optimal
/// policy while accelerating learning: F(s,s') = γΦ(s') - Φ(s)
/// </summary>
public sealed class RewardShaper
{
    // Reward component weights
    public RewardWeights Weights { get; set; } = new();

    // State tracking for reward computation
    private float _prevHealth = 100f;
    private float _prevArmor = 0f;
    private int _prevMoney = 0;
    private int _prevWantedLevel = 0;
    private float _prevMissionProgress = 0f;
    private int _stepsSurvived = 0;
    private float _totalDistanceTraveled = 0f;
    private float _prevX, _prevY, _prevZ;
    private bool _wasInVehicle;
    private float _prevVehicleSpeed;
    private float _prevVehicleHealth;

    // Exploration tracking
    private readonly HashSet<(int, int)> _visitedCells = new();
    private const float CellSize = 50f; // 50 unit grid cells
    private int _prevVisitedCount;

    // Curiosity module (ICM-style)
    private readonly float[] _stateHistory = new float[22 * 100]; // Last 100 states
    private int _historyIndex;
    private float _avgStateSurprise;

    // Driving skill tracking
    private readonly Queue<float> _recentSpeeds = new();
    private readonly Queue<float> _recentSteeringChanges = new();
    private int _consecutiveCrashes;
    private int _smoothDrivingStreak;

    // Combat tracking
    private int _killCount;
    private int _deathCount;
    private float _damageDealt;
    private float _damageTaken;

    // Potential function for PBRS
    private float _prevPotential;
    private float _gamma = 0.99f;

    // Phase 1.3: Navigation Coverage Component — POI discovery and grid cell bonuses
    private float _pendingPOIBonus;
    private float _pendingNavCellBonus;

    /// <summary>
    /// Phase 1.3: Call when the avatar reaches a named POI. Adds +1.0 discovery bonus.
    /// </summary>
    public void NotifyPOIReached() => _pendingPOIBonus += 1.0f;

    /// <summary>
    /// Phase 1.3: Call when the avatar enters a new 50-UU navigation grid cell. Adds +0.5 curiosity reward.
    /// </summary>
    public void NotifyNewNavigationCell() => _pendingNavCellBonus += 0.5f;

    // Statistics
    public RewardBreakdown LastBreakdown { get; private set; } = new();
    public float CumulativeReward { get; private set; }
    public int TotalSteps { get; private set; }

    /// <summary>
    /// Compute the shaped reward for a state transition.
    /// </summary>
    /// <param name="prevState">Previous game state [22-dim].</param>
    /// <param name="currentState">Current game state [22-dim].</param>
    /// <param name="action">Action taken.</param>
    /// <param name="step">Step number in the episode.</param>
    /// <returns>Shaped reward value.</returns>
    public float ComputeReward(float[] prevState, float[] currentState, int action, int step)
    {
        TotalSteps++;
        _stepsSurvived++;

        // Parse state vectors
        var prev = ParseState(prevState);
        var curr = ParseState(currentState);

        var breakdown = new RewardBreakdown();

        // ── 1. Survival Reward ──────────────────────────────────────────
        breakdown.Survival = ComputeSurvivalReward(prev, curr);

        // ── 2. Exploration Reward ───────────────────────────────────────
        breakdown.Exploration = ComputeExplorationReward(curr);

        // ── 3. Driving Skill Reward ─────────────────────────────────────
        breakdown.DrivingSkill = ComputeDrivingReward(prev, curr);

        // ── 4. Combat Effectiveness ─────────────────────────────────────
        breakdown.Combat = ComputeCombatReward(prev, curr);

        // ── 5. Mission Progress ─────────────────────────────────────────
        breakdown.MissionProgress = ComputeMissionReward(prev, curr);

        // ── 6. Economic Reward ──────────────────────────────────────────
        breakdown.Economic = ComputeEconomicReward(prev, curr);

        // ── 7. Curiosity (Intrinsic Motivation) ─────────────────────────
        breakdown.Curiosity = ComputeCuriosityReward(currentState);

        // ── 8. Social / Stealth ─────────────────────────────────────────
        breakdown.Social = ComputeSocialReward(prev, curr);

        // ── Potential-Based Reward Shaping (PBRS) ───────────────────────
        float currentPotential = ComputePotential(curr);
        breakdown.PotentialShaping = _gamma * currentPotential - _prevPotential;
        _prevPotential = currentPotential;

        // ── 8. Navigation Coverage Bonus (Phase 1.3) ────────────────────
        // Consume pending POI discovery and cell-entry bonuses
        breakdown.NavigationBonus = _pendingPOIBonus + _pendingNavCellBonus;
        _pendingPOIBonus = 0f;
        _pendingNavCellBonus = 0f;

        // ── Weighted sum ────────────────────────────────────────────────
        float totalReward =
            Weights.Survival * breakdown.Survival +
            Weights.Exploration * breakdown.Exploration +
            Weights.DrivingSkill * breakdown.DrivingSkill +
            Weights.Combat * breakdown.Combat +
            Weights.MissionProgress * breakdown.MissionProgress +
            Weights.Economic * breakdown.Economic +
            Weights.Curiosity * breakdown.Curiosity +
            Weights.Social * breakdown.Social +
            Weights.PotentialShaping * breakdown.PotentialShaping +
            Weights.Navigation * breakdown.NavigationBonus;

        // Clip reward to prevent extreme values
        totalReward = Math.Clamp(totalReward, -10f, 10f);

        breakdown.Total = totalReward;
        LastBreakdown = breakdown;
        CumulativeReward += totalReward;

        // Update previous state
        _prevHealth = curr.Health;
        _prevArmor = curr.Armor;
        _prevMoney = curr.Money;
        _prevWantedLevel = curr.WantedLevel;
        _prevMissionProgress = curr.MissionProgress;
        _prevX = curr.X; _prevY = curr.Y; _prevZ = curr.Z;
        _wasInVehicle = curr.InVehicle;
        _prevVehicleSpeed = curr.VehicleSpeed;
        _prevVehicleHealth = curr.VehicleHealth;

        return totalReward;
    }

    /// <summary>
    /// Check if the current state is terminal (episode should end).
    /// </summary>
    public bool IsTerminal(float[] state)
    {
        var s = ParseState(state);
        return s.Health <= 0; // Death = terminal
    }

    /// <summary>
    /// Reset the reward shaper for a new episode.
    /// </summary>
    public void Reset()
    {
        _prevHealth = 100f;
        _prevArmor = 0f;
        _prevMoney = 0;
        _prevWantedLevel = 0;
        _prevMissionProgress = 0f;
        _stepsSurvived = 0;
        _totalDistanceTraveled = 0f;
        _prevX = _prevY = _prevZ = 0;
        _wasInVehicle = false;
        _prevVehicleSpeed = 0;
        _prevVehicleHealth = 0;
        _visitedCells.Clear();
        _prevVisitedCount = 0;
        _historyIndex = 0;
        _avgStateSurprise = 0;
        _recentSpeeds.Clear();
        _recentSteeringChanges.Clear();
        _consecutiveCrashes = 0;
        _smoothDrivingStreak = 0;
        _prevPotential = 0;
        _pendingPOIBonus = 0f;
        _pendingNavCellBonus = 0f;
        CumulativeReward = 0;
        TotalSteps = 0;
    }

    #region Reward Components

    private float ComputeSurvivalReward(GameStateView prev, GameStateView curr)
    {
        float reward = 0.01f; // Small positive reward for each step alive

        // Health loss penalty
        float healthDelta = curr.Health - _prevHealth;
        if (healthDelta < 0)
            reward += healthDelta * 0.05f; // Penalty proportional to damage

        // Death penalty
        if (curr.Health <= 0)
            reward -= 5.0f;

        // Armor bonus
        float armorDelta = curr.Armor - _prevArmor;
        if (armorDelta > 0)
            reward += armorDelta * 0.02f;

        return reward;
    }

    private float ComputeExplorationReward(GameStateView curr)
    {
        float reward = 0;

        // Grid-based exploration
        int cellX = (int)(curr.X / CellSize);
        int cellY = (int)(curr.Y / CellSize);
        var cell = (cellX, cellY);

        if (_visitedCells.Add(cell))
        {
            // New cell discovered!
            reward += 0.5f;

            // Bonus for exploring far from spawn
            float distFromOrigin = (float)Math.Sqrt(curr.X * curr.X + curr.Y * curr.Y);
            reward += Math.Min(distFromOrigin / 1000f, 0.5f);
        }

        // Distance traveled reward (encourages movement)
        float dx = curr.X - _prevX;
        float dy = curr.Y - _prevY;
        float dz = curr.Z - _prevZ;
        float dist = (float)Math.Sqrt(dx * dx + dy * dy + dz * dz);
        _totalDistanceTraveled += dist;

        if (dist > 0.5f && dist < 50f) // Moving but not teleporting
            reward += dist * 0.001f;

        // Stagnation penalty
        if (dist < 0.1f)
            reward -= 0.01f;

        return reward;
    }

    private float ComputeDrivingReward(GameStateView prev, GameStateView curr)
    {
        if (!curr.InVehicle) return 0;

        float reward = 0;

        // Speed reward (moderate speed is good)
        float speed = curr.VehicleSpeed;
        _recentSpeeds.Enqueue(speed);
        if (_recentSpeeds.Count > 30) _recentSpeeds.Dequeue();

        if (speed > 5f && speed < 80f)
        {
            reward += 0.02f; // Reward for driving at reasonable speed
            _smoothDrivingStreak++;
        }
        else if (speed > 80f)
        {
            reward += 0.01f; // Small reward for high speed (risky but exciting)
        }

        // Smooth driving bonus (low speed variance)
        if (_recentSpeeds.Count >= 10)
        {
            float avgSpeed = _recentSpeeds.Average();
            float speedVariance = _recentSpeeds.Select(s => (s - avgSpeed) * (s - avgSpeed)).Average();
            if (speedVariance < 100f)
                reward += 0.01f; // Smooth driving
        }

        // Crash penalty
        float healthDelta = curr.VehicleHealth - _prevVehicleHealth;
        if (healthDelta < -10f && _wasInVehicle)
        {
            reward -= 0.5f;
            _consecutiveCrashes++;
            _smoothDrivingStreak = 0;
        }
        else
        {
            _consecutiveCrashes = 0;
        }

        // Long smooth driving streak bonus
        if (_smoothDrivingStreak > 100)
            reward += 0.1f;

        // Getting into a vehicle
        if (curr.InVehicle && !_wasInVehicle)
            reward += 0.1f;

        return reward;
    }

    private float ComputeCombatReward(GameStateView prev, GameStateView curr)
    {
        float reward = 0;

        // Weapon switch (exploring combat options)
        if (curr.WeaponId != prev.WeaponId && curr.WeaponId > 0)
            reward += 0.05f;

        // Damage dealt vs taken ratio
        float healthLost = Math.Max(0, _prevHealth - curr.Health);
        if (healthLost > 0)
        {
            _damageTaken += healthLost;
            reward -= healthLost * 0.02f;
        }

        // Wanted level management
        if (curr.WantedLevel > _prevWantedLevel)
        {
            // Getting wanted is slightly negative (draws attention)
            reward -= 0.1f * (curr.WantedLevel - _prevWantedLevel);
        }
        else if (curr.WantedLevel < _prevWantedLevel)
        {
            // Losing wanted level is positive (escaped!)
            reward += 0.3f * (_prevWantedLevel - curr.WantedLevel);
        }

        return reward;
    }

    private float ComputeMissionReward(GameStateView prev, GameStateView curr)
    {
        float reward = 0;

        // Mission progress
        float progressDelta = curr.MissionProgress - _prevMissionProgress;
        if (progressDelta > 0)
        {
            reward += progressDelta * 5.0f; // Large reward for mission progress
        }

        // New mission started
        if (curr.MissionId != prev.MissionId && curr.MissionId > 0)
            reward += 1.0f;

        return reward;
    }

    private float ComputeEconomicReward(GameStateView prev, GameStateView curr)
    {
        float reward = 0;

        // Money earned
        int moneyDelta = curr.Money - _prevMoney;
        if (moneyDelta > 0)
        {
            reward += Math.Min(moneyDelta / 1000f, 1.0f); // Cap at 1.0
        }
        else if (moneyDelta < 0)
        {
            // Spending money is slightly negative (unless for mission)
            reward -= 0.01f;
        }

        return reward;
    }

    private float ComputeCuriosityReward(float[] state)
    {
        // ICM-style intrinsic curiosity: reward for states that are
        // surprising (different from recent history)
        float surprise = 0;
        int histLen = Math.Min(_historyIndex, 100);

        if (histLen > 5)
        {
            // Compare current state to recent states
            for (int h = Math.Max(0, histLen - 10); h < histLen; h++)
            {
                float dist = 0;
                for (int i = 0; i < Math.Min(state.Length, 22); i++)
                {
                    float diff = state[i] - _stateHistory[h * 22 + i];
                    dist += diff * diff;
                }
                surprise += (float)Math.Sqrt(dist / 22);
            }
            surprise /= Math.Min(10, histLen);
        }

        // Store current state in history
        int offset = (_historyIndex % 100) * 22;
        for (int i = 0; i < Math.Min(state.Length, 22); i++)
            _stateHistory[offset + i] = state[i];
        _historyIndex++;

        // Running average surprise
        _avgStateSurprise = _avgStateSurprise * 0.99f + surprise * 0.01f;

        // Reward is proportional to how surprising this state is
        // relative to the running average
        float curiosityReward = surprise > _avgStateSurprise * 1.5f ? 0.1f : 0;

        return curiosityReward;
    }

    private float ComputeSocialReward(GameStateView prev, GameStateView curr)
    {
        float reward = 0;

        // Low wanted level is good (stealth)
        if (curr.WantedLevel == 0)
            reward += 0.005f;

        // High wanted level is progressively worse
        if (curr.WantedLevel >= 3)
            reward -= 0.05f * curr.WantedLevel;

        return reward;
    }

    /// <summary>
    /// Potential function for PBRS.
    /// Higher potential = closer to desirable states.
    /// </summary>
    private float ComputePotential(GameStateView state)
    {
        float potential = 0;

        // Health potential
        potential += state.Health / 100f * 2f;

        // Exploration potential (visited cells)
        potential += _visitedCells.Count * 0.01f;

        // Money potential
        potential += Math.Min(state.Money / 10000f, 1f);

        // Mission progress potential
        potential += state.MissionProgress * 5f;

        // Distance from origin (exploration)
        float dist = (float)Math.Sqrt(state.X * state.X + state.Y * state.Y);
        potential += Math.Min(dist / 500f, 1f);

        return potential;
    }

    #endregion

    #region Helpers

    private struct GameStateView
    {
        public float X, Y, Z;
        public float Heading;
        public float VelX, VelY, VelZ;
        public float Health, Armor;
        public int WeaponId, WantedLevel;
        public bool InVehicle;
        public float VehicleHealth, VehicleSpeed;
        public int Money;
        public float CamX, CamY, CamZ, CamHeading, CamPitch;
        public int MissionId;
        public float MissionProgress;
    }

    private static GameStateView ParseState(float[] state)
    {
        if (state.Length < 15) return new GameStateView { Health = 100 };

        return new GameStateView
        {
            X = state[0], Y = state[1], Z = state[2],
            Heading = state.Length > 3 ? state[3] : 0,
            VelX = state.Length > 4 ? state[4] : 0,
            VelY = state.Length > 5 ? state[5] : 0,
            VelZ = state.Length > 6 ? state[6] : 0,
            Health = state.Length > 7 ? state[7] : 100,
            Armor = state.Length > 8 ? state[8] : 0,
            WeaponId = state.Length > 9 ? (int)state[9] : 0,
            WantedLevel = state.Length > 10 ? (int)state[10] : 0,
            InVehicle = state.Length > 11 && state[11] > 0.5f,
            VehicleHealth = state.Length > 12 ? state[12] : 0,
            VehicleSpeed = state.Length > 13 ? state[13] : 0,
            Money = state.Length > 14 ? (int)state[14] : 0,
            CamX = state.Length > 15 ? state[15] : 0,
            CamY = state.Length > 16 ? state[16] : 0,
            CamZ = state.Length > 17 ? state[17] : 0,
            CamHeading = state.Length > 18 ? state[18] : 0,
            CamPitch = state.Length > 19 ? state[19] : 0,
            MissionId = state.Length > 20 ? (int)state[20] : 0,
            MissionProgress = state.Length > 21 ? state[21] : 0,
        };
    }

    #endregion
}

#region Reward Configuration

/// <summary>
/// Configurable weights for each reward component.
/// </summary>
public class RewardWeights
{
    public float Survival { get; set; } = 1.0f;
    public float Exploration { get; set; } = 2.0f;
    public float DrivingSkill { get; set; } = 1.5f;
    public float Combat { get; set; } = 1.0f;
    public float MissionProgress { get; set; } = 3.0f;
    public float Economic { get; set; } = 0.5f;
    public float Curiosity { get; set; } = 1.0f;
    public float Social { get; set; } = 0.5f;
    public float PotentialShaping { get; set; } = 0.1f;
    public float Navigation { get; set; } = 1.0f;

    /// <summary>Preset: Exploration-focused (for early training).</summary>
    public static RewardWeights ExplorationFocused => new()
    {
        Survival = 0.5f, Exploration = 5.0f, DrivingSkill = 1.0f,
        Combat = 0.2f, MissionProgress = 1.0f, Economic = 0.3f,
        Curiosity = 3.0f, Social = 0.2f, PotentialShaping = 0.2f,
    };

    /// <summary>Preset: Mission-focused (for mid training).</summary>
    public static RewardWeights MissionFocused => new()
    {
        Survival = 1.5f, Exploration = 1.0f, DrivingSkill = 1.0f,
        Combat = 1.5f, MissionProgress = 5.0f, Economic = 1.0f,
        Curiosity = 0.5f, Social = 1.0f, PotentialShaping = 0.1f,
    };

    /// <summary>Preset: Driving-focused (for vehicle training).</summary>
    public static RewardWeights DrivingFocused => new()
    {
        Survival = 1.0f, Exploration = 2.0f, DrivingSkill = 5.0f,
        Combat = 0.1f, MissionProgress = 0.5f, Economic = 0.3f,
        Curiosity = 1.0f, Social = 0.5f, PotentialShaping = 0.1f,
    };

    /// <summary>Preset: Combat-focused (for combat training).</summary>
    public static RewardWeights CombatFocused => new()
    {
        Survival = 2.0f, Exploration = 0.5f, DrivingSkill = 0.5f,
        Combat = 5.0f, MissionProgress = 2.0f, Economic = 0.5f,
        Curiosity = 0.5f, Social = 0.3f, PotentialShaping = 0.1f,
    };
}

/// <summary>
/// Breakdown of reward components for visualization.
/// </summary>
public class RewardBreakdown
{
    public float Survival { get; set; }
    public float Exploration { get; set; }
    public float DrivingSkill { get; set; }
    public float Combat { get; set; }
    public float MissionProgress { get; set; }
    public float Economic { get; set; }
    public float Curiosity { get; set; }
    public float Social { get; set; }
    public float PotentialShaping { get; set; }
    /// <summary>Phase 1.3: POI discovery and navigation cell coverage bonus.</summary>
    public float NavigationBonus { get; set; }
    public float Total { get; set; }
}

#endregion
