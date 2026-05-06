using System;
using System.Text.Json;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using GTAngel.Interop;

namespace GTAngel.Services;

/// <summary>
/// KSM Cycle 6: UE5 Player↔AI Bridge Service
/// Implements the duality: Human Player vs DTE AI Agent, mediated by the ML Vision pipeline.
/// Provides input arbitration, mode toggling, and observation fusion.
/// </summary>
public class Ue5PlayerAiBridgeService
{
    private readonly ILogger<Ue5PlayerAiBridgeService> _logger;
    private UE5ProcessManager? _ue5;

    public PlayerAiBridgeMode CurrentMode { get; private set; } = PlayerAiBridgeMode.HumanOnly;

    // Arbitration weights (used in Arbitrated mode)
    public float HumanInputWeight { get; private set; } = 0.5f;
    public float AiPolicyWeight { get; private set; } = 0.5f;

    public event EventHandler<PlayerAiBridgeMode>? OnModeChanged;
    public event EventHandler<float>? OnArbitrationScoreUpdated;
    public event EventHandler<float>? OnObservationFused;
    /// <summary>Phase 4.3: Fired when mode transitions to AiOnly, marking an episode boundary.</summary>
    public event EventHandler<int>? OnEpisodeBoundary;

    // Episode boundary counter for telemetry
    private int _episodeBoundaryCount;

    public Ue5PlayerAiBridgeService(ILogger<Ue5PlayerAiBridgeService> logger)
    {
        _logger = logger;
    }

    public void SetUE5ProcessManager(UE5ProcessManager ue5)
    {
        _ue5 = ue5;
    }

    /// <summary>
    /// Toggle between the three player-AI modes.
    /// </summary>
    public async Task SetModeAsync(PlayerAiBridgeMode newMode)
    {
        if (CurrentMode == newMode) return;

        var prevMode = CurrentMode;
        CurrentMode = newMode;
        _logger.LogInformation("Player↔AI Bridge mode changed to: {Mode}", newMode);
        
        OnModeChanged?.Invoke(this, newMode);

        // Phase 4.3: Mode transition to AiOnly marks a new curriculum episode boundary
        if (newMode == PlayerAiBridgeMode.AiOnly && prevMode != PlayerAiBridgeMode.AiOnly)
        {
            _episodeBoundaryCount++;
            OnEpisodeBoundary?.Invoke(this, _episodeBoundaryCount);
            _logger.LogInformation("Episode boundary #{Count} triggered by mode switch to AiOnly", _episodeBoundaryCount);
        }

        if (_ue5 != null)
        {
            await _ue5.SendPlayerAiModeAsync(newMode);
        }
    }

    /// <summary>
    /// Phase 4.4: Dynamically adjust the human/AI arbitration weights.
    /// humanWeight in [0,1]: 0.0 = full AI, 1.0 = full human.
    /// Bind to a WPF Slider in AvatarView.xaml.
    /// </summary>
    public void UpdateArbitrationWeights(float humanWeight)
    {
        HumanInputWeight = Math.Clamp(humanWeight, 0f, 1f);
        AiPolicyWeight   = 1f - HumanInputWeight;
        OnArbitrationScoreUpdated?.Invoke(this, HumanInputWeight);
        _logger.LogDebug("Arbitration weights updated: Human={H:F2} AI={A:F2}", HumanInputWeight, AiPolicyWeight);
    }

    /// <summary>
    /// Receives an observation from UE5, fuses it with ML features, and updates telemetry.
    /// </summary>
    public void ProcessObservation(AvatarObservation obs, float[] mlFeatures)
    {
        // Calculate a dummy fusion norm for telemetry
        float norm = 0f;
        if (mlFeatures != null && mlFeatures.Length > 0)
        {
            foreach (var f in mlFeatures) norm += f * f;
            norm = (float)Math.Sqrt(norm);
        }
        
        // Add proprioception influence
        norm += (obs.Position[0] * 0.0001f);

        OnObservationFused?.Invoke(this, norm);
    }

    /// <summary>
    /// Arbitrates between a human input and an AI input based on the current mode.
    /// </summary>
    public AvatarAction ArbitrateInput(AvatarAction humanAction, AvatarAction aiAction)
    {
        var finalAction = new AvatarAction
        {
            InputAction = humanAction?.InputAction ?? aiAction?.InputAction ?? "None",
            Source = "Arbitrated"
        };

        float arbitrationScore = 0f;

        switch (CurrentMode)
        {
            case PlayerAiBridgeMode.HumanOnly:
                if (humanAction != null)
                {
                    finalAction.Magnitude = humanAction.Magnitude;
                    finalAction.AxisX = humanAction.AxisX;
                    finalAction.AxisY = humanAction.AxisY;
                    finalAction.Source = "Human";
                    arbitrationScore = 1.0f;
                }
                break;

            case PlayerAiBridgeMode.AiOnly:
                if (aiAction != null)
                {
                    finalAction.Magnitude = aiAction.Magnitude;
                    finalAction.AxisX = aiAction.AxisX;
                    finalAction.AxisY = aiAction.AxisY;
                    finalAction.Source = "ML";
                    arbitrationScore = 0.0f;
                }
                break;

            case PlayerAiBridgeMode.Arbitrated:
                if (humanAction != null && aiAction != null)
                {
                    // Blend inputs based on weights
                    finalAction.Magnitude = (humanAction.Magnitude * HumanInputWeight) + (aiAction.Magnitude * AiPolicyWeight);
                    finalAction.AxisX = (humanAction.AxisX * HumanInputWeight) + (aiAction.AxisX * AiPolicyWeight);
                    finalAction.AxisY = (humanAction.AxisY * HumanInputWeight) + (aiAction.AxisY * AiPolicyWeight);
                    arbitrationScore = HumanInputWeight; // 0.5 default
                }
                else if (humanAction != null)
                {
                    finalAction.Magnitude = humanAction.Magnitude;
                    finalAction.AxisX = humanAction.AxisX;
                    finalAction.AxisY = humanAction.AxisY;
                    arbitrationScore = 1.0f;
                }
                else if (aiAction != null)
                {
                    finalAction.Magnitude = aiAction.Magnitude;
                    finalAction.AxisX = aiAction.AxisX;
                    finalAction.AxisY = aiAction.AxisY;
                    arbitrationScore = 0.0f;
                }
                break;
        }

        OnArbitrationScoreUpdated?.Invoke(this, arbitrationScore);
        return finalAction;
    }
}

public enum PlayerAiBridgeMode
{
    HumanOnly,
    AiOnly,
    Arbitrated
}
