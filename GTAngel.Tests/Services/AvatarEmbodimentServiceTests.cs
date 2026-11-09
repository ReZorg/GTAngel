using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for AvatarEmbodimentService:
///   1. Constructor — initialises without throwing
///   2. SynthesizeEmotionalState — maps ESN output to Ekman emotions
///   3. ComputeFACSActionUnits — maps EmotionalState to AU vector
///   4. ApplyPersonalityTraitsAsync — maps cognitive state to personality
///   5. Event contracts — OnEmotionalStateUpdated, OnFACSAUsUpdated, OnPersonalityTraitsUpdated
///   6. Edge cases — short ESN vectors, zero inputs, clamping
/// </summary>
public sealed class AvatarEmbodimentServiceTests : IAsyncLifetime
{
    private readonly AvatarEmbodimentService _svc;

    public AvatarEmbodimentServiceTests()
    {
        _svc = new AvatarEmbodimentService(NullLogger<AvatarEmbodimentService>.Instance);
    }

    public Task InitializeAsync() => Task.CompletedTask;

    public async Task DisposeAsync()
    {
        await _svc.StopAsync();
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    private static float[] MakeEsnOutput(
        float valence = 0.5f, float arousal = 0.5f, float dominance = 0.5f,
        float curiosity = 0.5f, float fear = 0.0f, float social = 0.5f)
        => new[] { valence, arousal, dominance, curiosity, fear, social };

    private static AvatarEmbodimentService.NeurochemicalState DefaultNeuro()
        => new(Curiosity: 0.5f, Endorphin: 0.5f, Chaos: 0.3f, Homeostasis: 0.7f);

    // ── 1. Constructor ────────────────────────────────────────────────────────

    [Fact]
    public void Constructor_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            new AvatarEmbodimentService(NullLogger<AvatarEmbodimentService>.Instance));
        Assert.Null(ex);
    }

    // ── 2. SynthesizeEmotionalState ───────────────────────────────────────────

    [Fact]
    public void SynthesizeEmotionalState_HighValence_HighHappiness()
    {
        var esn = MakeEsnOutput(valence: 1.0f, arousal: 0.5f, social: 1.0f, fear: 0.0f);
        var emotion = _svc.SynthesizeEmotionalState(esn, DefaultNeuro());

        // Happiness should be the dominant emotion for high valence + high social + low fear
        Assert.True(emotion.Happiness > 0f, "Happiness should be positive for high valence");
    }

    [Fact]
    public void SynthesizeEmotionalState_LowValenceLowArousal_HighSadness()
    {
        var esn = MakeEsnOutput(valence: 0.0f, arousal: 0.0f, dominance: 0.0f);
        var emotion = _svc.SynthesizeEmotionalState(esn, DefaultNeuro());

        // Sadness = (1-valence)*(1-arousal)*(1-dominance) — should dominate
        Assert.True(emotion.Sadness > 0f, "Sadness should be positive for low valence+arousal+dominance");
    }

    [Fact]
    public void SynthesizeEmotionalState_LowValenceHighArousalHighDominance_HighAnger()
    {
        var esn = MakeEsnOutput(valence: 0.0f, arousal: 1.0f, dominance: 1.0f, fear: 0.0f);
        var emotion = _svc.SynthesizeEmotionalState(esn, DefaultNeuro());

        // Anger = (1-valence)*arousal*dominance
        Assert.True(emotion.Anger > 0f, "Anger should be positive for low valence, high arousal+dominance");
    }

    [Fact]
    public void SynthesizeEmotionalState_HighFearLowDominance_HighFear()
    {
        var esn = MakeEsnOutput(fear: 1.0f, dominance: 0.0f);
        var emotion = _svc.SynthesizeEmotionalState(esn, DefaultNeuro());

        Assert.True(emotion.Fear > 0f, "Fear should be positive for high fear signal");
    }

    [Fact]
    public void SynthesizeEmotionalState_HighArousalHighCuriosity_HighSurprise()
    {
        var esn = MakeEsnOutput(valence: 0.5f, arousal: 1.0f, curiosity: 1.0f);
        var emotion = _svc.SynthesizeEmotionalState(esn, DefaultNeuro());

        Assert.True(emotion.Surprise > 0f, "Surprise should be positive for high arousal+curiosity");
    }

    [Fact]
    public void SynthesizeEmotionalState_AllEmotionsInUnitRange()
    {
        var esn = MakeEsnOutput(0.7f, 0.6f, 0.4f, 0.8f, 0.3f, 0.9f);
        var emotion = _svc.SynthesizeEmotionalState(esn, DefaultNeuro());

        Assert.InRange(emotion.Happiness, 0f, 1f);
        Assert.InRange(emotion.Surprise,  0f, 1f);
        Assert.InRange(emotion.Sadness,   0f, 1f);
        Assert.InRange(emotion.Anger,     0f, 1f);
        Assert.InRange(emotion.Fear,      0f, 1f);
    }

    [Fact]
    public void SynthesizeEmotionalState_ShortEsnVector_ReturnsCurrentEmotion()
    {
        // Should gracefully return the current (default) emotion without throwing
        var emotion = _svc.SynthesizeEmotionalState(new float[] { 0.5f, 0.5f }, DefaultNeuro());
        Assert.NotNull(emotion);
    }

    [Fact]
    public void SynthesizeEmotionalState_EmptyEsnVector_DoesNotThrow()
    {
        var ex = Record.Exception(() =>
            _svc.SynthesizeEmotionalState(Array.Empty<float>(), DefaultNeuro()));
        Assert.Null(ex);
    }

    [Fact]
    public void SynthesizeEmotionalState_FiresOnEmotionalStateUpdatedEvent()
    {
        int fired = 0;
        _svc.OnEmotionalStateUpdated += (_, _) => fired++;

        _svc.SynthesizeEmotionalState(MakeEsnOutput(), DefaultNeuro());

        Assert.Equal(1, fired);
    }

    [Fact]
    public void SynthesizeEmotionalState_EventCarriesCorrectEmotionValues()
    {
        AvatarEmbodimentService.EmotionalState? received = null;
        _svc.OnEmotionalStateUpdated += (_, e) => received = e;

        var esn = MakeEsnOutput(valence: 1.0f, social: 1.0f);
        var returned = _svc.SynthesizeEmotionalState(esn, DefaultNeuro());

        Assert.NotNull(received);
        Assert.Equal(returned.Happiness, received!.Happiness);
    }

    [Fact]
    public void SynthesizeEmotionalState_MaxEmotion_IsNormalisedToOne()
    {
        var esn = MakeEsnOutput(valence: 1.0f, social: 1.0f, fear: 0.0f, arousal: 0.0f);
        var emotion = _svc.SynthesizeEmotionalState(esn, DefaultNeuro());

        float maxEmotion = Math.Max(emotion.Happiness,
                           Math.Max(emotion.Surprise,
                           Math.Max(emotion.Sadness,
                           Math.Max(emotion.Anger, emotion.Fear))));
        // After normalisation the maximum emotion value should be exactly 1.0
        Assert.Equal(1.0f, maxEmotion, precision: 5);
    }

    // ── 3. ComputeFACSActionUnits ─────────────────────────────────────────────

    [Fact]
    public void ComputeFACSActionUnits_Returns47ElementArray()
    {
        var emotion = new AvatarEmbodimentService.EmotionalState(Happiness: 0.8f);
        var aus = _svc.ComputeFACSActionUnits(emotion);
        Assert.Equal(47, aus.Length);
    }

    [Fact]
    public void ComputeFACSActionUnits_AllValuesInUnitRange()
    {
        var emotion = new AvatarEmbodimentService.EmotionalState(
            Happiness: 0.8f, Surprise: 0.5f, Sadness: 0.1f, Anger: 0.1f, Fear: 0.0f);
        var aus = _svc.ComputeFACSActionUnits(emotion);
        Assert.All(aus, v => Assert.InRange(v, 0f, 1f));
    }

    [Fact]
    public void ComputeFACSActionUnits_HappinessEmotion_ActivatesSmileAUs()
    {
        var emotion = new AvatarEmbodimentService.EmotionalState(Happiness: 1.0f);
        var aus = _svc.ComputeFACSActionUnits(emotion);

        // AU12 = LipCornerPuller (Smile), should be active for happiness
        Assert.True(aus[12] > 0f, "AU12 (lip corner puller) should activate for happiness");
    }

    [Fact]
    public void ComputeFACSActionUnits_AngerEmotion_ActivatesBrowLowererAU()
    {
        var emotion = new AvatarEmbodimentService.EmotionalState(Anger: 1.0f);
        var aus = _svc.ComputeFACSActionUnits(emotion);

        // AU4 = BrowLowerer, should be active for anger
        Assert.True(aus[4] > 0f, "AU4 (brow lowerer) should activate for anger");
    }

    [Fact]
    public void ComputeFACSActionUnits_SurpriseEmotion_ActivatesUpperLidRaiser()
    {
        var emotion = new AvatarEmbodimentService.EmotionalState(Surprise: 1.0f);
        var aus = _svc.ComputeFACSActionUnits(emotion);

        // AU5 = UpperLidRaiser, should be active for surprise
        Assert.True(aus[5] > 0f, "AU5 (upper lid raiser) should activate for surprise");
    }

    [Fact]
    public void ComputeFACSActionUnits_ZeroEmotions_AllAUsAreZero()
    {
        var emotion = new AvatarEmbodimentService.EmotionalState(
            Happiness: 0f, Surprise: 0f, Sadness: 0f, Anger: 0f, Fear: 0f);
        var aus = _svc.ComputeFACSActionUnits(emotion);
        Assert.All(aus, v => Assert.Equal(0f, v));
    }

    [Fact]
    public void ComputeFACSActionUnits_FiresOnFACSAUsUpdatedEvent()
    {
        int fired = 0;
        _svc.OnFACSAUsUpdated += (_, _) => fired++;

        _svc.ComputeFACSActionUnits(new AvatarEmbodimentService.EmotionalState(Happiness: 0.5f));

        Assert.Equal(1, fired);
    }

    [Fact]
    public void ComputeFACSActionUnits_MultipleEmotionsBlendCorrectly()
    {
        var emotion = new AvatarEmbodimentService.EmotionalState(
            Happiness: 0.5f, Anger: 0.5f);
        var aus = _svc.ComputeFACSActionUnits(emotion);

        // Both happiness (AU12) and anger (AU4) should activate
        Assert.True(aus[12] > 0f, "Happiness AU12 should activate");
        Assert.True(aus[4]  > 0f, "Anger AU4 should activate");
    }

    [Fact]
    public void ComputeFACSActionUnits_AUsDoNotExceedOne()
    {
        // Even with all emotions at max, AUs should be clamped to 1
        var emotion = new AvatarEmbodimentService.EmotionalState(
            Happiness: 1f, Surprise: 1f, Sadness: 1f, Anger: 1f, Fear: 1f);
        var aus = _svc.ComputeFACSActionUnits(emotion);
        Assert.All(aus, v => Assert.True(v <= 1f, $"AU value {v} exceeds 1.0"));
    }

    // ── 4. ApplyPersonalityTraitsAsync ────────────────────────────────────────

    [Fact]
    public async Task ApplyPersonalityTraitsAsync_HighAutonomyCoherence_HighConfidence()
    {
        AvatarEmbodimentService.PersonalityTraits? received = null;
        _svc.OnPersonalityTraitsUpdated += (_, t) => received = t;

        await _svc.ApplyPersonalityTraitsAsync(
            autonomyLevel: 1.0f, coherence: 1.0f, DefaultNeuro());

        Assert.NotNull(received);
        // Confidence = clamp(0.6 + 0.1*autonomy + 0.1*coherence)
        Assert.True(received!.Confidence >= 0.7f, "High autonomy+coherence → high confidence");
    }

    [Fact]
    public async Task ApplyPersonalityTraitsAsync_HighCuriosity_HighPlayfulness()
    {
        AvatarEmbodimentService.PersonalityTraits? received = null;
        _svc.OnPersonalityTraitsUpdated += (_, t) => received = t;

        var neuro = new AvatarEmbodimentService.NeurochemicalState(Curiosity: 1.0f);
        await _svc.ApplyPersonalityTraitsAsync(0.5f, 0.5f, neuro);

        Assert.NotNull(received);
        Assert.True(received!.Playfulness >= 0.8f, "High curiosity → high playfulness");
    }

    [Fact]
    public async Task ApplyPersonalityTraitsAsync_HighChaos_HighSass()
    {
        AvatarEmbodimentService.PersonalityTraits? received = null;
        _svc.OnPersonalityTraitsUpdated += (_, t) => received = t;

        var neuro = new AvatarEmbodimentService.NeurochemicalState(Chaos: 1.0f);
        await _svc.ApplyPersonalityTraitsAsync(0.5f, 0.5f, neuro);

        Assert.NotNull(received);
        Assert.True(received!.Sass >= 0.7f, "High chaos → high sass");
    }

    [Fact]
    public async Task ApplyPersonalityTraitsAsync_AllTraitsInUnitRange()
    {
        AvatarEmbodimentService.PersonalityTraits? received = null;
        _svc.OnPersonalityTraitsUpdated += (_, t) => received = t;

        await _svc.ApplyPersonalityTraitsAsync(0.7f, 0.8f, DefaultNeuro());

        Assert.NotNull(received);
        Assert.InRange(received!.Confidence,  0f, 1f);
        Assert.InRange(received.Charm,        0f, 1f);
        Assert.InRange(received.Playfulness,  0f, 1f);
        Assert.InRange(received.Wit,          0f, 1f);
        Assert.InRange(received.Sass,         0f, 1f);
    }

    [Fact]
    public async Task ApplyPersonalityTraitsAsync_FiresOnPersonalityTraitsUpdatedEvent()
    {
        int fired = 0;
        _svc.OnPersonalityTraitsUpdated += (_, _) => fired++;

        await _svc.ApplyPersonalityTraitsAsync(0.5f, 0.5f, DefaultNeuro());

        Assert.Equal(1, fired);
    }

    [Fact]
    public async Task ApplyPersonalityTraitsAsync_HighCoherence_HighCharm()
    {
        AvatarEmbodimentService.PersonalityTraits? received = null;
        _svc.OnPersonalityTraitsUpdated += (_, t) => received = t;

        await _svc.ApplyPersonalityTraitsAsync(0.5f, 1.0f, DefaultNeuro());

        Assert.NotNull(received);
        // Charm = clamp(0.7 + coherence*0.2) → ~0.9
        Assert.True(received!.Charm >= 0.85f, "High coherence → high charm");
    }

    // ── 5. Lifecycle ──────────────────────────────────────────────────────────

    [Fact]
    public async Task StopAsync_CalledWithoutStart_DoesNotThrow()
    {
        var svc = new AvatarEmbodimentService(NullLogger<AvatarEmbodimentService>.Instance);
        var ex = await Record.ExceptionAsync(() => svc.StopAsync());
        Assert.Null(ex);
    }
}
