using GTAngel.Interop;
using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for Ue5PlayerAiBridgeService:
///   1. Constructor — default state
///   2. SetModeAsync — transitions and events
///   3. ArbitrateInput — HumanOnly, AiOnly, Arbitrated modes
///   4. ProcessObservation — event firing, null-safety
///   5. Weight properties — correct defaults
/// </summary>
public sealed class Ue5PlayerAiBridgeServiceTests
{
    private readonly Ue5PlayerAiBridgeService _svc;

    public Ue5PlayerAiBridgeServiceTests()
    {
        _svc = new Ue5PlayerAiBridgeService(NullLogger<Ue5PlayerAiBridgeService>.Instance);
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    private static AvatarAction MakeHumanAction(float x = 1f, float y = 0f, float mag = 1f)
        => new() { InputAction = "IA_Move", AxisX = x, AxisY = y, Magnitude = mag, Source = "Human" };

    private static AvatarAction MakeAiAction(float x = 0f, float y = 1f, float mag = 0.8f)
        => new() { InputAction = "IA_Move", AxisX = x, AxisY = y, Magnitude = mag, Source = "ML" };

    private static AvatarObservation MakeObservation(float x = 0f, float y = 0f, float z = 0f)
        => new()
        {
            Position = new float[] { x, y, z },
            Rotation = new float[] { 0, 0, 0 },
            Velocity = new float[] { 0, 0, 0 },
            PerceivedObjects = Array.Empty<PerceivedObject>(),
            Timestamp = 0.0
        };

    // ── 1. Default state ──────────────────────────────────────────────────────

    [Fact]
    public void Constructor_DefaultMode_IsHumanOnly()
    {
        Assert.Equal(PlayerAiBridgeMode.HumanOnly, _svc.CurrentMode);
    }

    [Fact]
    public void HumanInputWeight_DefaultValue_IsHalf()
    {
        Assert.Equal(0.5f, _svc.HumanInputWeight);
    }

    [Fact]
    public void AiPolicyWeight_DefaultValue_IsHalf()
    {
        Assert.Equal(0.5f, _svc.AiPolicyWeight);
    }

    // ── 2. SetModeAsync ────────────────────────────────────────────────────────

    [Fact]
    public async Task SetModeAsync_ToAiOnly_ChangesCurrentMode()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.AiOnly);
        Assert.Equal(PlayerAiBridgeMode.AiOnly, _svc.CurrentMode);
    }

    [Fact]
    public async Task SetModeAsync_ToArbitrated_ChangesCurrentMode()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);
        Assert.Equal(PlayerAiBridgeMode.Arbitrated, _svc.CurrentMode);
    }

    [Fact]
    public async Task SetModeAsync_ToHumanOnly_ChangesCurrentMode()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.AiOnly);
        await _svc.SetModeAsync(PlayerAiBridgeMode.HumanOnly);
        Assert.Equal(PlayerAiBridgeMode.HumanOnly, _svc.CurrentMode);
    }

    [Fact]
    public async Task SetModeAsync_FiresOnModeChangedEvent()
    {
        int fired = 0;
        _svc.OnModeChanged += (_, _) => fired++;

        await _svc.SetModeAsync(PlayerAiBridgeMode.AiOnly);

        Assert.Equal(1, fired);
    }

    [Fact]
    public async Task SetModeAsync_EventCarriesNewMode()
    {
        PlayerAiBridgeMode? received = null;
        _svc.OnModeChanged += (_, m) => received = m;

        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);

        Assert.Equal(PlayerAiBridgeMode.Arbitrated, received);
    }

    [Fact]
    public async Task SetModeAsync_SameMode_DoesNotFireEvent()
    {
        int fired = 0;
        _svc.OnModeChanged += (_, _) => fired++;

        // Already HumanOnly — setting it again should not fire
        await _svc.SetModeAsync(PlayerAiBridgeMode.HumanOnly);

        Assert.Equal(0, fired);
    }

    [Fact]
    public async Task SetModeAsync_MultipleModeChanges_FiresEachTime()
    {
        int fired = 0;
        _svc.OnModeChanged += (_, _) => fired++;

        await _svc.SetModeAsync(PlayerAiBridgeMode.AiOnly);
        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);
        await _svc.SetModeAsync(PlayerAiBridgeMode.HumanOnly);

        Assert.Equal(3, fired);
    }

    // ── 3. ArbitrateInput — HumanOnly ─────────────────────────────────────────

    [Fact]
    public void ArbitrateInput_HumanOnly_WithHumanAction_ReturnsHumanSource()
    {
        // Default mode is HumanOnly
        var result = _svc.ArbitrateInput(MakeHumanAction(), MakeAiAction());
        Assert.Equal("Human", result.Source);
    }

    [Fact]
    public void ArbitrateInput_HumanOnly_MagnitudeMatchesHuman()
    {
        var human = MakeHumanAction(mag: 0.9f);
        var ai    = MakeAiAction(mag: 0.4f);
        var result = _svc.ArbitrateInput(human, ai);
        Assert.Equal(0.9f, result.Magnitude);
    }

    [Fact]
    public void ArbitrateInput_HumanOnly_AxisMatchesHuman()
    {
        var human = MakeHumanAction(x: 0.7f, y: 0.3f);
        var ai    = MakeAiAction(x: 0.1f, y: 0.9f);
        var result = _svc.ArbitrateInput(human, ai);
        Assert.Equal(0.7f, result.AxisX);
        Assert.Equal(0.3f, result.AxisY);
    }

    [Fact]
    public void ArbitrateInput_HumanOnly_WithNullHuman_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.ArbitrateInput(null, MakeAiAction()));
        Assert.Null(ex);
    }

    [Fact]
    public void ArbitrateInput_HumanOnly_WithBothNull_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.ArbitrateInput(null, null));
        Assert.Null(ex);
    }

    // ── 4. ArbitrateInput — AiOnly ────────────────────────────────────────────

    [Fact]
    public async Task ArbitrateInput_AiOnly_WithAiAction_ReturnsMLSource()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.AiOnly);
        var result = _svc.ArbitrateInput(MakeHumanAction(), MakeAiAction());
        Assert.Equal("ML", result.Source);
    }

    [Fact]
    public async Task ArbitrateInput_AiOnly_MagnitudeMatchesAi()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.AiOnly);
        var human = MakeHumanAction(mag: 0.9f);
        var ai    = MakeAiAction(mag: 0.4f);
        var result = _svc.ArbitrateInput(human, ai);
        Assert.Equal(0.4f, result.Magnitude);
    }

    [Fact]
    public async Task ArbitrateInput_AiOnly_AxisMatchesAi()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.AiOnly);
        var human = MakeHumanAction(x: 0.7f, y: 0.3f);
        var ai    = MakeAiAction(x: 0.1f, y: 0.9f);
        var result = _svc.ArbitrateInput(human, ai);
        Assert.Equal(0.1f, result.AxisX);
        Assert.Equal(0.9f, result.AxisY);
    }

    [Fact]
    public async Task ArbitrateInput_AiOnly_WithNullAi_DoesNotThrow()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.AiOnly);
        var ex = Record.Exception(() => _svc.ArbitrateInput(MakeHumanAction(), null));
        Assert.Null(ex);
    }

    // ── 5. ArbitrateInput — Arbitrated ────────────────────────────────────────

    [Fact]
    public async Task ArbitrateInput_Arbitrated_BlendsMagnitudeByEqualWeights()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);
        var human = MakeHumanAction(mag: 1.0f);
        var ai    = MakeAiAction(mag: 0.0f);
        var result = _svc.ArbitrateInput(human, ai);

        // With 0.5/0.5 weights: 1.0*0.5 + 0.0*0.5 = 0.5
        Assert.Equal(0.5f, result.Magnitude, precision: 5);
    }

    [Fact]
    public async Task ArbitrateInput_Arbitrated_BlendsAxisX()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);
        var human = MakeHumanAction(x: 1.0f);
        var ai    = MakeAiAction(x: 0.0f);
        var result = _svc.ArbitrateInput(human, ai);

        // (1.0 * 0.5) + (0.0 * 0.5) = 0.5
        Assert.Equal(0.5f, result.AxisX, precision: 5);
    }

    [Fact]
    public async Task ArbitrateInput_Arbitrated_BlendsAxisY()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);
        var human = MakeHumanAction(y: 0.0f);
        var ai    = MakeAiAction(y: 1.0f);
        var result = _svc.ArbitrateInput(human, ai);

        // (0.0 * 0.5) + (1.0 * 0.5) = 0.5
        Assert.Equal(0.5f, result.AxisY, precision: 5);
    }

    [Fact]
    public async Task ArbitrateInput_Arbitrated_WithNullHuman_FallsBackToAi()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);
        var ai    = MakeAiAction(mag: 0.6f);
        var result = _svc.ArbitrateInput(null, ai);
        Assert.Equal(0.6f, result.Magnitude);
    }

    [Fact]
    public async Task ArbitrateInput_Arbitrated_WithNullAi_FallsBackToHuman()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);
        var human = MakeHumanAction(mag: 0.7f);
        var result = _svc.ArbitrateInput(human, null);
        Assert.Equal(0.7f, result.Magnitude);
    }

    [Fact]
    public async Task ArbitrateInput_Arbitrated_WithBothNull_DoesNotThrow()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);
        var ex = Record.Exception(() => _svc.ArbitrateInput(null, null));
        Assert.Null(ex);
    }

    // ── 6. ArbitrateInput fires OnArbitrationScoreUpdated ─────────────────────

    [Fact]
    public void ArbitrateInput_HumanOnly_FiresArbitrationScoreEvent()
    {
        int fired = 0;
        _svc.OnArbitrationScoreUpdated += (_, _) => fired++;

        _svc.ArbitrateInput(MakeHumanAction(), MakeAiAction());

        Assert.Equal(1, fired);
    }

    [Fact]
    public void ArbitrateInput_HumanOnly_ArbitrationScoreIsOne()
    {
        float? score = null;
        _svc.OnArbitrationScoreUpdated += (_, s) => score = s;

        _svc.ArbitrateInput(MakeHumanAction(), MakeAiAction());

        Assert.Equal(1.0f, score);
    }

    [Fact]
    public async Task ArbitrateInput_AiOnly_ArbitrationScoreIsZero()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.AiOnly);
        float? score = null;
        _svc.OnArbitrationScoreUpdated += (_, s) => score = s;

        _svc.ArbitrateInput(MakeHumanAction(), MakeAiAction());

        Assert.Equal(0.0f, score);
    }

    [Fact]
    public async Task ArbitrateInput_Arbitrated_ArbitrationScoreIsHumanWeight()
    {
        await _svc.SetModeAsync(PlayerAiBridgeMode.Arbitrated);
        float? score = null;
        _svc.OnArbitrationScoreUpdated += (_, s) => score = s;

        _svc.ArbitrateInput(MakeHumanAction(), MakeAiAction());

        Assert.Equal(0.5f, score); // default HumanInputWeight
    }

    // ── 7. ProcessObservation ─────────────────────────────────────────────────

    [Fact]
    public void ProcessObservation_WithValidFeatures_FiresOnObservationFused()
    {
        int fired = 0;
        _svc.OnObservationFused += (_, _) => fired++;

        _svc.ProcessObservation(MakeObservation(), new float[] { 1f, 2f, 3f });

        Assert.Equal(1, fired);
    }

    [Fact]
    public void ProcessObservation_EmptyFeatures_FiresOnObservationFused()
    {
        int fired = 0;
        _svc.OnObservationFused += (_, _) => fired++;

        _svc.ProcessObservation(MakeObservation(), Array.Empty<float>());

        Assert.Equal(1, fired);
    }

    [Fact]
    public void ProcessObservation_NullFeatures_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            _svc.ProcessObservation(MakeObservation(), null!));
        Assert.Null(ex);
    }

    [Fact]
    public void ProcessObservation_FusedNormIsNonNegative()
    {
        float? norm = null;
        _svc.OnObservationFused += (_, n) => norm = n;

        _svc.ProcessObservation(MakeObservation(x: 100f), new float[] { 3f, 4f });

        Assert.True(norm.HasValue);
        Assert.True(norm!.Value >= 0f);
    }

    [Fact]
    public void ProcessObservation_NonZeroFeatures_NormIsPositive()
    {
        float? norm = null;
        _svc.OnObservationFused += (_, n) => norm = n;

        _svc.ProcessObservation(MakeObservation(), new float[] { 3f, 4f }); // norm = sqrt(9+16) = 5

        Assert.True(norm.HasValue && norm.Value > 0f);
    }

    // ── 8. SetUE5ProcessManager ───────────────────────────────────────────────

    [Fact]
    public void SetUE5ProcessManager_WithNull_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.SetUE5ProcessManager(null!));
        Assert.Null(ex);
    }
}
