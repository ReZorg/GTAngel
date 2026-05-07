using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using System.Reflection;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for OpenRwEngineBridge — initial state, properties, GameState feature vector,
/// engine detection on a clean machine, and configuration helpers.
/// </summary>
public class OpenRwEngineBridgeTests : IDisposable
{
    private readonly OpenRwEngineBridge _svc =
        new(NullLogger<OpenRwEngineBridge>.Instance);

    // ── Initial state ─────────────────────────────────────────────────────────

    [Fact]
    public void DetectedEngine_Initially_IsNone()
    {
        Assert.Equal(OpenRwEngineBridge.EngineType.None, _svc.DetectedEngine);
    }

    [Fact]
    public void EnginePath_Initially_IsNull()
    {
        Assert.Null(_svc.EnginePath);
    }

    [Fact]
    public void GameDataPath_Initially_IsNull()
    {
        Assert.Null(_svc.GameDataPath);
    }

    [Fact]
    public void IsRunning_Initially_IsFalse()
    {
        Assert.False(_svc.IsRunning);
    }

    [Fact]
    public void GameProcess_Initially_IsNull()
    {
        Assert.Null(_svc.GameProcess);
    }

    [Fact]
    public void GameWindowHandle_Initially_IsZero()
    {
        Assert.Equal(nint.Zero, _svc.GameWindowHandle);
    }

    // ── Default configuration ─────────────────────────────────────────────────

    [Fact]
    public void RenderWidth_DefaultIs768()
    {
        Assert.Equal(768, _svc.RenderWidth);
    }

    [Fact]
    public void RenderHeight_DefaultIs768()
    {
        Assert.Equal(768, _svc.RenderHeight);
    }

    [Fact]
    public void HeadlessMode_DefaultIsFalse()
    {
        Assert.False(_svc.HeadlessMode);
    }

    [Fact]
    public void DeterministicStepping_DefaultIsTrue()
    {
        Assert.True(_svc.DeterministicStepping);
    }

    [Fact]
    public void TargetFps_DefaultIs30()
    {
        Assert.Equal(30, _svc.TargetFps);
    }

    // ── DetectEngines on a build machine (no GTA3 installed) ─────────────────

    [Fact]
    public void DetectEngines_OnCleanMachine_ReturnsNone()
    {
        var result = _svc.DetectEngines();
        // On a CI / build machine there is no GTA3 installed
        Assert.Equal(OpenRwEngineBridge.EngineType.None, result);
    }

    [Fact]
    public void DetectEngines_SetsDetectedEngineProperty()
    {
        _svc.DetectEngines();
        // Property must be consistent with the return value
        Assert.Equal(OpenRwEngineBridge.EngineType.None, _svc.DetectedEngine);
    }

    // ── Configuration mutators ────────────────────────────────────────────────

    [Fact]
    public void RenderWidth_CanBeChanged()
    {
        _svc.RenderWidth = 1920;
        Assert.Equal(1920, _svc.RenderWidth);
    }

    [Fact]
    public void RenderHeight_CanBeChanged()
    {
        _svc.RenderHeight = 1080;
        Assert.Equal(1080, _svc.RenderHeight);
    }

    [Fact]
    public void HeadlessMode_CanBeEnabled()
    {
        _svc.HeadlessMode = true;
        Assert.True(_svc.HeadlessMode);
    }

    [Fact]
    public void TargetFps_CanBeChanged()
    {
        _svc.TargetFps = 60;
        Assert.Equal(60, _svc.TargetFps);
    }

    [Fact]
    public void SetEnginePath_WithExistingFile_SetsDetectedEngineAndDataPath()
    {
        string directory = Path.Combine(Path.GetTempPath(), "GTAngel.Tests", Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(directory);
        string exe = Path.Combine(directory, "openrw.exe");
        File.WriteAllText(exe, string.Empty);

        _svc.SetEnginePath(exe, OpenRwEngineBridge.EngineType.OpenRW);

        Assert.Equal(OpenRwEngineBridge.EngineType.OpenRW, _svc.DetectedEngine);
        Assert.Equal(exe, _svc.EnginePath);
        Assert.Equal(directory, _svc.GameDataPath);
    }

    [Fact]
    public void SetGameDataPath_WithInvalidDirectory_LeavesPathUnchanged()
    {
        _svc.SetGameDataPath("/path/that/does/not/exist");
        Assert.Null(_svc.GameDataPath);
    }

    [Fact]
    public async Task LaunchAsync_WhenNoEngineDetected_ReturnsFalse()
    {
        Assert.False(await _svc.LaunchAsync());
    }

    // ── GameState.ToFeatureVector ─────────────────────────────────────────────

    [Fact]
    public void GameState_ToFeatureVector_Returns22Elements()
    {
        var state = new OpenRwEngineBridge.GameState();
        var vector = state.ToFeatureVector();
        Assert.Equal(22, vector.Length);
    }

    [Fact]
    public void GameState_ToFeatureVector_AllValuesInRange()
    {
        var state = new OpenRwEngineBridge.GameState
        {
            PlayerX = 1000f,
            PlayerY = -500f,
            PlayerZ = 20f,
            PlayerHealth = 75f,
            PlayerArmor = 50f,
            PlayerMoney = 500_000,
            WantedLevel = 3,
            CurrentWeapon = 7,
            InVehicle = true,
            VehicleHealth = 800f,
            VehicleSpeed = 100f,
        };

        var vector = state.ToFeatureVector();

        // All values should be normalised — not necessarily [0,1] due to
        // signed axes, but none should be NaN or Infinity
        foreach (float v in vector)
        {
            Assert.False(float.IsNaN(v), $"NaN in feature vector");
            Assert.False(float.IsInfinity(v), $"Infinity in feature vector");
        }
    }

    [Fact]
    public void GameState_InVehicle_True_SetsOneInVector()
    {
        var state = new OpenRwEngineBridge.GameState { InVehicle = true };
        var vector = state.ToFeatureVector();
        // InVehicle maps to index 9 (counting from 0)
        Assert.Equal(1f, vector[9]);
    }

    [Fact]
    public void GameState_InVehicle_False_SetsZeroInVector()
    {
        var state = new OpenRwEngineBridge.GameState { InVehicle = false };
        var vector = state.ToFeatureVector();
        Assert.Equal(0f, vector[9]);
    }

    [Fact]
    public void GameState_DefaultIsland_Portland_MapsToZero()
    {
        var state = new OpenRwEngineBridge.GameState { CurrentIsland = "Portland" };
        var vector = state.ToFeatureVector();
        Assert.Equal(0f, vector[15]);
    }

    [Fact]
    public void GameState_Island_Staunton_MapsToHalf()
    {
        var state = new OpenRwEngineBridge.GameState { CurrentIsland = "Staunton" };
        var vector = state.ToFeatureVector();
        Assert.Equal(0.5f, vector[15]);
    }

    [Fact]
    public void GameState_Island_Shoreside_MapsToOne()
    {
        var state = new OpenRwEngineBridge.GameState { CurrentIsland = "Shoreside" };
        var vector = state.ToFeatureVector();
        Assert.Equal(1f, vector[15]);
    }

    [Fact]
    public void ReadGameState_WhenNoProcess_ReturnsDefaultState()
    {
        var state = _svc.ReadGameState();
        Assert.Equal(0f, state.PlayerX);
        Assert.Equal("Portland", state.CurrentIsland);
    }

    [Fact]
    public void GenerateOpenRwBuildConfig_UsesConfiguredResolution()
    {
        _svc.RenderWidth = 640;
        _svc.RenderHeight = 640;

        var config = _svc.GenerateOpenRwBuildConfig();

        Assert.Contains("OPENRW_DEFAULT_WIDTH=640", config);
        Assert.Contains("OPENRW_DEFAULT_HEIGHT=640", config);
    }

    [Fact]
    public void GenerateRe3BuildConfig_UsesConfiguredResolution()
    {
        _svc.RenderWidth = 512;
        _svc.RenderHeight = 512;

        var config = _svc.GenerateRe3BuildConfig();

        Assert.Contains("DEFAULT_SCREEN_WIDTH=512", config);
        Assert.Contains("DEFAULT_SCREEN_HEIGHT=512", config);
    }

    [Fact]
    public void FindFirstJsonObjectEnd_PrivateHelper_HandlesNestedObjectsAndEscapes()
    {
        var method = typeof(OpenRwEngineBridge).GetMethod("FindFirstJsonObjectEnd", BindingFlags.NonPublic | BindingFlags.Static);
        const string raw = "{\"outer\":{\"message\":\"brace } and quote \\\" ok\"}} trailing";

        var end = Assert.IsType<int>(method!.Invoke(null, [raw])!);

        Assert.Equal(raw.IndexOf("}}", StringComparison.Ordinal) + 1, end);
    }

    // ── Stop / Dispose ────────────────────────────────────────────────────────

    [Fact]
    public void Stop_WhenNoProcess_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.Stop());
        Assert.Null(ex);
    }

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var svc2 = new OpenRwEngineBridge(NullLogger<OpenRwEngineBridge>.Instance);
        var ex = Record.Exception(() => svc2.Dispose());
        Assert.Null(ex);
    }

    // ── EngineConfig defaults ─────────────────────────────────────────────────

    [Fact]
    public void EngineConfig_DefaultWidth_Is768()
    {
        var config = new OpenRwEngineBridge.EngineConfig();
        Assert.Equal(768, config.Width);
    }

    [Fact]
    public void EngineConfig_DefaultHeight_Is768()
    {
        var config = new OpenRwEngineBridge.EngineConfig();
        Assert.Equal(768, config.Height);
    }

    [Fact]
    public void EngineConfig_DefaultTargetFps_Is30()
    {
        var config = new OpenRwEngineBridge.EngineConfig();
        Assert.Equal(30, config.TargetFps);
    }

    [Fact]
    public void EngineConfig_DefaultDeterministicStep_IsTrue()
    {
        var config = new OpenRwEngineBridge.EngineConfig();
        Assert.True(config.DeterministicStep);
    }

    public void Dispose() => _svc.Dispose();
}
