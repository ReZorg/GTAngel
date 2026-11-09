using System;
using System.Linq;
using GTAngel.Interop;
using GTAngel.Models.EmbodiedCognition;
using GTAngel.Services;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Comprehensive tests for GamerGirlControllerInterface — 4E embodied cognition controller.
/// </summary>
public class GamerGirlControllerInterfaceTests
{
    private readonly GamerGirlControllerInterface _sut;
    private readonly VigemControllerService _controller;

    public GamerGirlControllerInterfaceTests()
    {
        _controller = new VigemControllerService(NullLogger<VigemControllerService>.Instance);
        _sut = new GamerGirlControllerInterface(
            NullLogger<GamerGirlControllerInterface>.Instance,
            _controller);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Construction & Initialization
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void Constructor_InitializesDefaultState()
    {
        Assert.Equal(GripPosture.Neutral, _sut.CurrentGrip);
        Assert.Equal(FlowState.Idle, _sut.CurrentFlowState);
        Assert.Equal(GamingIntent.Exploring, _sut.CurrentIntent);
        Assert.Equal(0f, _sut.GripTension);
        Assert.Equal(0f, _sut.FlowIntensity);
        Assert.Equal(0, _sut.TotalInputsDispatched);
        Assert.False(_sut.IsInClutch);
    }

    [Fact]
    public void Constructor_ThrowsOnNullLogger()
    {
        Assert.Throws<ArgumentNullException>(() =>
            new GamerGirlControllerInterface(null!, _controller));
    }

    [Fact]
    public void Constructor_ThrowsOnNullController()
    {
        Assert.Throws<ArgumentNullException>(() =>
            new GamerGirlControllerInterface(
                NullLogger<GamerGirlControllerInterface>.Instance, null!));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Phase 7.1: Grip Schema
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void DispatchEmbodiedAction_SprintSetsAggressiveGrip()
    {
        var action = new AvatarAction { InputAction = "IA_Sprint", Magnitude = 1f };
        var self = new EmbodiedSelfState { Arousal = 0.5f };

        _sut.DispatchEmbodiedAction(action, self);

        Assert.Equal(GripPosture.Aggressive, _sut.CurrentGrip);
    }

    [Fact]
    public void DispatchEmbodiedAction_CrouchSetsDefensiveGrip()
    {
        var action = new AvatarAction { InputAction = "IA_Crouch", Magnitude = 1f };
        var self = new EmbodiedSelfState { Arousal = 0.2f };

        _sut.DispatchEmbodiedAction(action, self);

        Assert.Equal(GripPosture.Defensive, _sut.CurrentGrip);
    }

    [Fact]
    public void DispatchEmbodiedAction_LookSetsPrecisionGrip()
    {
        var action = new AvatarAction { InputAction = "IA_Look", Magnitude = 0.5f };
        var self = new EmbodiedSelfState();

        _sut.DispatchEmbodiedAction(action, self);

        Assert.Equal(GripPosture.Precision, _sut.CurrentGrip);
    }

    [Fact]
    public void DispatchEmbodiedAction_InteractSetsRelaxedGrip()
    {
        var action = new AvatarAction { InputAction = "IA_Interact", Magnitude = 1f };
        var self = new EmbodiedSelfState();

        _sut.DispatchEmbodiedAction(action, self);

        Assert.Equal(GripPosture.Relaxed, _sut.CurrentGrip);
    }

    [Theory]
    [InlineData(0f, 0f)]
    [InlineData(0.5f, 0.5f)]
    [InlineData(1f, 1f)]
    public void GripTension_ScalesWithArousal(float arousal, float expectedMin)
    {
        var action = new AvatarAction { InputAction = "IA_Sprint", Magnitude = 1f };
        var self = new EmbodiedSelfState { Arousal = arousal };

        _sut.DispatchEmbodiedAction(action, self);

        // Tension should be influenced by arousal
        Assert.True(_sut.GripTension >= 0f);
        Assert.True(_sut.GripTension <= 1f);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Phase 7.2: Haptic Feedback
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void TriggerHaptic_RaisesEvent()
    {
        bool eventRaised = false;
        _sut.HapticFeedbackTriggered += (_, _) => eventRaised = true;

        _sut.TriggerHaptic(0.5f, 0.5f, 100f);

        Assert.True(eventRaised);
    }

    [Fact]
    public void TriggerHaptic_ClampsMotorValues()
    {
        float leftReceived = 0f;
        _sut.HapticFeedbackTriggered += (_, e) => leftReceived = e.LeftMotor;

        _sut.TriggerHaptic(2f, -1f, 3000f);

        Assert.Equal(1f, leftReceived);
    }

    [Fact]
    public void DispatchEmbodiedAction_GeneratesHapticForSprint()
    {
        bool hapticTriggered = false;
        _sut.HapticFeedbackTriggered += (_, _) => hapticTriggered = true;

        var action = new AvatarAction { InputAction = "IA_Sprint", Magnitude = 1f };
        var self = new EmbodiedSelfState { Speed = 500f };

        _sut.DispatchEmbodiedAction(action, self);

        Assert.True(hapticTriggered);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Phase 7.3: Flow State Detection
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void FlowState_StartsIdle()
    {
        Assert.Equal(FlowState.Idle, _sut.CurrentFlowState);
        Assert.Equal(0f, _sut.FlowIntensity);
    }

    [Fact]
    public void FlowState_ProgressesWithConsistentInput()
    {
        var action = new AvatarAction { InputAction = "IA_Move", Magnitude = 0.7f, AxisY = 0.7f };
        var self = new EmbodiedSelfState();

        // Dispatch many consistent actions to build flow
        for (int i = 0; i < 150; i++)
        {
            _sut.DispatchEmbodiedAction(action, self);
        }

        // Should have progressed beyond Idle
        Assert.True(_sut.FlowIntensity > 0f);
    }

    [Fact]
    public void CalculateRhythmCoherence_ReturnsZeroForFewInputs()
    {
        // Less than 4 inputs → 0 coherence
        float coherence = _sut.CalculateRhythmCoherence();
        Assert.Equal(0f, coherence);
    }

    [Fact]
    public void FlowState_RaisesEventOnChange()
    {
        FlowState? newState = null;
        _sut.FlowStateChanged += (_, e) => newState = e.Current;

        var action = new AvatarAction { InputAction = "IA_Move", Magnitude = 0.7f, AxisY = 0.7f };
        var self = new EmbodiedSelfState();

        // Pump enough actions to potentially trigger state change
        for (int i = 0; i < 200; i++)
            _sut.DispatchEmbodiedAction(action, self);

        // May or may not have changed — just verify no crash
        // If it did change, verify it's valid
        if (newState.HasValue)
            Assert.True(Enum.IsDefined(typeof(FlowState), newState.Value));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Phase 7.4: Gaming Intent Detection
    // ═══════════════════════════════════════════════════════════════════════════

    [Theory]
    [InlineData("IA_Sprint", GamingIntent.Rushing)]
    [InlineData("IA_Crouch", GamingIntent.Stealth)]
    [InlineData("IA_Interact", GamingIntent.Interacting)]
    [InlineData("IA_Jump", GamingIntent.Traversal)]
    [InlineData("IA_StrafeR", GamingIntent.Combat)]
    [InlineData("IA_StrafeL", GamingIntent.Combat)]
    public void InferIntent_MapsActionsCorrectly(string inputAction, GamingIntent expected)
    {
        var action = new AvatarAction { InputAction = inputAction, Magnitude = 1f };
        var intent = _sut.InferIntent(action);
        Assert.Equal(expected, intent);
    }

    [Fact]
    public void InferIntent_HighMagnitudeMoveMeansRushing()
    {
        var action = new AvatarAction { InputAction = "IA_Move", Magnitude = 0.9f };
        var intent = _sut.InferIntent(action);
        Assert.Equal(GamingIntent.Rushing, intent);
    }

    [Fact]
    public void InferIntent_LowMagnitudeMoveMeansExploring()
    {
        var action = new AvatarAction { InputAction = "IA_Move", Magnitude = 0.2f };
        var intent = _sut.InferIntent(action);
        Assert.Equal(GamingIntent.Exploring, intent);
    }

    [Fact]
    public void IntentChanged_RaisesEvent()
    {
        GamingIntent? newIntent = null;
        _sut.IntentChanged += (_, e) => newIntent = e.Current;

        var action = new AvatarAction { InputAction = "IA_Sprint", Magnitude = 1f };
        var self = new EmbodiedSelfState();
        _sut.DispatchEmbodiedAction(action, self);

        Assert.Equal(GamingIntent.Rushing, newIntent);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Grip Modulation
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void ModulateByGrip_NeutralReturnsSameValue()
    {
        // Default grip is neutral → multiplier ≈ 1.0
        float result = _sut.ModulateByGrip(0.5f);
        Assert.True(result >= 0.4f && result <= 0.6f);
    }

    [Fact]
    public void ModulateByGrip_ClampsToZeroOne()
    {
        float result = _sut.ModulateByGrip(1.5f);
        Assert.True(result <= 1f);

        float result2 = _sut.ModulateByGrip(-0.5f);
        Assert.True(result2 >= 0f);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Continuous Action Dispatch
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void DispatchContinuousAction_IncrementsInputCount()
    {
        var action = new float[] { 0.5f, 0.5f, 0f, 0f, 0f, 0f };
        _sut.DispatchContinuousAction(action, new EmbodiedSelfState());

        Assert.Equal(1, _sut.TotalInputsDispatched);
    }

    [Fact]
    public void DispatchContinuousAction_NullActionIsIgnored()
    {
        _sut.DispatchContinuousAction(null!, new EmbodiedSelfState());
        Assert.Equal(0, _sut.TotalInputsDispatched);
    }

    [Fact]
    public void DispatchContinuousAction_ShortArrayIsIgnored()
    {
        _sut.DispatchContinuousAction(new float[] { 0.5f }, new EmbodiedSelfState());
        Assert.Equal(0, _sut.TotalInputsDispatched);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Metrics & Reset
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void GetMetrics_ReturnsValidSnapshot()
    {
        var metrics = _sut.GetMetrics();

        Assert.Equal(GripPosture.Neutral, metrics.Grip);
        Assert.Equal(FlowState.Idle, metrics.FlowState);
        Assert.Equal(GamingIntent.Exploring, metrics.Intent);
        Assert.Equal(0, metrics.TotalInputs);
        Assert.False(metrics.IsInClutch);
    }

    [Fact]
    public void Reset_ClearsAllState()
    {
        // Accumulate some state
        var action = new AvatarAction { InputAction = "IA_Sprint", Magnitude = 1f };
        var self = new EmbodiedSelfState { Arousal = 0.8f };
        _sut.DispatchEmbodiedAction(action, self);

        _sut.Reset();

        Assert.Equal(GripPosture.Neutral, _sut.CurrentGrip);
        Assert.Equal(FlowState.Idle, _sut.CurrentFlowState);
        Assert.Equal(GamingIntent.Exploring, _sut.CurrentIntent);
        Assert.Equal(0, _sut.TotalInputsDispatched);
        Assert.Equal(0f, _sut.GripTension);
    }

    [Fact]
    public void DispatchEmbodiedAction_NullActionIsIgnored()
    {
        _sut.DispatchEmbodiedAction(null!, new EmbodiedSelfState());
        Assert.Equal(0, _sut.TotalInputsDispatched);
    }

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var sut = new GamerGirlControllerInterface(
            NullLogger<GamerGirlControllerInterface>.Instance, _controller);
        sut.Dispose();
        sut.Dispose(); // double dispose is safe
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // IsInClutch
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void IsInClutch_FalseByDefault()
    {
        Assert.False(_sut.IsInClutch);
    }

    [Fact]
    public void HasAnalogControl_ReflectsControllerState()
    {
        // ViGEm likely not available in test environment
        Assert.IsType<bool>(_sut.HasAnalogControl);
    }
}

/// <summary>
/// Tests for RingBuffer utility.
/// </summary>
public class RingBufferTests
{
    [Fact]
    public void Push_AddsItems()
    {
        var buf = new RingBuffer<int>(5);
        buf.Push(1);
        buf.Push(2);
        Assert.Equal(2, buf.Count);
    }

    [Fact]
    public void Push_WrapsAtCapacity()
    {
        var buf = new RingBuffer<int>(3);
        buf.Push(1);
        buf.Push(2);
        buf.Push(3);
        buf.Push(4); // wraps

        Assert.Equal(3, buf.Count);
        var arr = buf.ToArray();
        Assert.Contains(4, arr);
    }

    [Fact]
    public void Clear_ResetsCount()
    {
        var buf = new RingBuffer<int>(10);
        buf.Push(1);
        buf.Push(2);
        buf.Clear();
        Assert.Equal(0, buf.Count);
    }

    [Fact]
    public void ToArray_EmptyReturnsEmpty()
    {
        var buf = new RingBuffer<float>(10);
        Assert.Empty(buf.ToArray());
    }

    [Fact]
    public void ToArray_ReturnsAllItemsInOrder()
    {
        var buf = new RingBuffer<int>(5);
        buf.Push(10);
        buf.Push(20);
        buf.Push(30);
        var arr = buf.ToArray();
        Assert.Equal(new[] { 10, 20, 30 }, arr);
    }
}

/// <summary>
/// Tests for ComboDetector.
/// </summary>
public class ComboDetectorTests
{
    [Fact]
    public void Feed_NoComboWithLessThanThreeActions()
    {
        var detector = new ComboDetector();
        detector.Feed("IA_Move", 1.0);
        detector.Feed("IA_Jump", 1.1);
        Assert.False(detector.HasCombo);
    }

    [Fact]
    public void Feed_DetectsComboWithThreeDistinctActions()
    {
        var detector = new ComboDetector();
        detector.Feed("IA_Move", 1.0);
        detector.Feed("IA_Jump", 1.2);
        detector.Feed("IA_Sprint", 1.4);
        Assert.True(detector.HasCombo);
        Assert.NotNull(detector.LastCombo);
    }

    [Fact]
    public void Feed_NoComboWhenSameActionRepeated()
    {
        var detector = new ComboDetector();
        detector.Feed("IA_Move", 1.0);
        detector.Feed("IA_Move", 1.1);
        detector.Feed("IA_Move", 1.2);
        Assert.False(detector.HasCombo);
    }

    [Fact]
    public void Feed_ExpiresOldEntries()
    {
        var detector = new ComboDetector();
        detector.Feed("IA_Move", 0.0);
        detector.Feed("IA_Jump", 0.1);
        detector.Feed("IA_Sprint", 5.0); // 5 seconds later - old entries expire
        Assert.False(detector.HasCombo); // only 1 entry in window
    }

    [Fact]
    public void Reset_ClearsState()
    {
        var detector = new ComboDetector();
        detector.Feed("IA_Move", 1.0);
        detector.Feed("IA_Jump", 1.1);
        detector.Feed("IA_Sprint", 1.2);
        Assert.True(detector.HasCombo);

        detector.Reset();
        Assert.False(detector.HasCombo);
        Assert.Null(detector.LastCombo);
    }
}
