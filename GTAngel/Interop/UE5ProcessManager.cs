using System.IO;
using System.Diagnostics;
using System.IO.Pipes;
using System.Text;
using System.Text.Json;
using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Interop;

/// <summary>
/// UE5-upgraded process manager for GTAngel.
/// Replaces all UE4 (libUE4.so / NativeActivity / UE4CommandLine.txt) patterns
/// with UE5 equivalents:
///   - Enhanced Input System  (replaces legacy UE4 input)
///   - Chaos Physics          (replaces PhysX / NvCloth UE4 backend)
///   - Lumen Global Illumination (replaces UE4 baked/static GI)
///   - Nanite Virtualized Geometry (replaces UE4 LOD system)
///   - UE5 World Partition    (replaces UE4 level streaming)
///   - DTE 4E Avatar IPC      (new: avatar motor/vision commands)
///   - 768-resolution ML Vision capture (new: ML observation pipeline)
/// </summary>
public class UE5ProcessManager : IDisposable
{
    private readonly ILogger<UE5ProcessManager> _logger;
    private Process? _engineProcess;
    private NamedPipeServerStream? _pipeServer;
    private StreamWriter? _pipeWriter;
    private StreamReader? _pipeReader;
    private CancellationTokenSource? _cts;
    private Task? _messageLoopTask;

    // ── IPC pipe names ──────────────────────────────────────────────────────
    public const string PipeName        = "GTAngel_UE5_IPC";
    public const string AvatarPipeName  = "GTAngel_Avatar_IPC";
    public const string MLVisionPipe    = "GTAngel_MLVision_IPC";
    public const string SharedMemName   = "GTAngel_UE5_SharedState";

    // ── UE5 render resolution for ML Vision capture ─────────────────────────
    /// <summary>768×768 square viewport for ML observation (ViT / CLIP compatible)</summary>
    public const int MLVisionWidth  = 768;
    public const int MLVisionHeight = 768;

    // ── Main game viewport resolution ───────────────────────────────────────
    public const int GameResX = 1920;
    public const int GameResY = 1080;

    // ── Events ──────────────────────────────────────────────────────────────
    public event EventHandler<IntPtr>?   EngineWindowReady;
    public event EventHandler<int>?      EngineExited;
    public event EventHandler<UEMessage>? MessageReceived;
    public event EventHandler<AvatarObservation>? AvatarObservationReceived;

    public UEProcessState State { get; private set; } = UEProcessState.NotStarted;
    public IntPtr EngineWindowHandle { get; private set; }
    public int ProcessId => _engineProcess?.Id ?? 0;

    // ── UE5 feature flags ───────────────────────────────────────────────────
    public bool UseLumen         { get; set; } = true;
    public bool UseNanite        { get; set; } = true;
    public bool UseChaosPhysics  { get; set; } = true;
    public bool UseEnhancedInput { get; set; } = true;
    public bool UseWorldPartition{ get; set; } = true;
    public bool UseMLVisionCapture { get; set; } = true;

    public UE5ProcessManager(ILogger<UE5ProcessManager> logger)
    {
        _logger = logger;
    }

    // ── Launch ──────────────────────────────────────────────────────────────

    /// <summary>
    /// Launch the UE5 game engine process with full UE5 feature flags.
    /// UE4 equivalent: NativeActivity.onCreate() → System.loadLibrary("UE4")
    /// UE5 upgrade:    Direct Win64 process launch with Enhanced Input + Lumen + Nanite + Chaos
    /// </summary>
    public async Task<bool> StartAsync(string executablePath, string? extraArgs = null)
    {
        if (State == UEProcessState.Running)
        {
            _logger.LogWarning("UE5 engine already running (PID {Pid})", _engineProcess?.Id);
            return true;
        }

        if (!File.Exists(executablePath))
        {
            _logger.LogError("UE5 executable not found: {Path}", executablePath);
            State = UEProcessState.Error;
            return false;
        }

        _cts = new CancellationTokenSource();
        State = UEProcessState.Starting;

        try
        {
            await StartPipeServerAsync();

            var args = BuildUE5CommandLine(extraArgs);

            var startInfo = new ProcessStartInfo
            {
                FileName         = executablePath,
                Arguments        = args,
                UseShellExecute  = false,
                WorkingDirectory = Path.GetDirectoryName(executablePath)!,
                CreateNoWindow   = false
            };

            _engineProcess = Process.Start(startInfo)
                ?? throw new InvalidOperationException("Failed to start UE5 process");

            _engineProcess.EnableRaisingEvents = true;
            _engineProcess.Exited += OnProcessExited;

            _logger.LogInformation("UE5 engine started: PID {Pid}, Args: {Args}",
                _engineProcess.Id, args);

            // Wait for the engine window to appear (UE5 startup is ~3-8s)
            _messageLoopTask = Task.Run(() => WaitForEngineWindowAsync(_cts.Token));

            State = UEProcessState.Running;
            return true;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to launch UE5 engine");
            State = UEProcessState.Error;
            return false;
        }
    }

    // ── UE5 Command Line Builder ─────────────────────────────────────────────

    /// <summary>
    /// Build the UE5 command line.
    ///
    /// UE4 legacy (REMOVED):
    ///   -ResX=1920 -ResY=1080 -vulkan -Shipping
    ///   com.epicgames.ue4.GameActivity.bIsShippingBuild
    ///   com.epicgames.ue4.GameActivity.bSupportsVulkan
    ///
    /// UE5 upgrade:
    ///   -dx12                    → DirectX 12 RHI (UE5 default, replaces Vulkan on Windows)
    ///   -sm6                     → Shader Model 6 (required for Nanite + Lumen)
    ///   -lumen                   → Enable Lumen GI (replaces UE4 baked lighting)
    ///   -nanite                  → Enable Nanite geometry (replaces UE4 LOD)
    ///   -chaos                   → Enable Chaos Physics (replaces PhysX)
    ///   -enhancedinput           → UE5 Enhanced Input System (replaces legacy input)
    ///   -worldpartition          → UE5 World Partition streaming (replaces UE4 level streaming)
    ///   -ResX=1920 -ResY=1080    → Main game viewport
    ///   -SecondaryViewportResX=768 -SecondaryViewportResY=768 → ML Vision capture viewport
    ///   -GTAngel_Pipe            → IPC pipe for WPF↔UE5 commands
    ///   -GTAngel_AvatarPipe      → IPC pipe for avatar motor control
    ///   -GTAngel_MLVisionPipe    → IPC pipe for ML vision frame capture
    /// </summary>
    private string BuildUE5CommandLine(string? extraArgs)
    {
        var args = new List<string>
        {
            // ── Windowed mode ──────────────────────────────────────────────
            "-Windowed",
            $"-ResX={GameResX}",
            $"-ResY={GameResY}",

            // ── UE5 RHI: DirectX 12 + Shader Model 6 ─────────────────────
            // UE4 used Vulkan on Android; UE5 on Windows uses DX12 natively
            "-dx12",
            "-sm6",

            // ── UE5 Rendering Features ────────────────────────────────────
            // Lumen: software ray-traced global illumination (UE5 default)
            // Replaces UE4's Lightmass baked GI and SSGI approximations
            UseLumen  ? "-lumen"  : "-nolumen",

            // Nanite: virtualized micropolygon geometry (UE5 default)
            // Replaces UE4's manual LOD system and impostor meshes
            UseNanite ? "-nanite" : "-nonanite",

            // ── UE5 Physics: Chaos ────────────────────────────────────────
            // Chaos Physics replaces PhysX (deprecated in UE5)
            // Includes Chaos Cloth (replaces NvCloth), Chaos Vehicles
            UseChaosPhysics ? "-chaos" : string.Empty,

            // ── UE5 Enhanced Input System ─────────────────────────────────
            // Replaces UE4's legacy input system
            // Required for DTE Avatar virtual player controls
            UseEnhancedInput ? "-enhancedinput" : string.Empty,

            // ── UE5 World Partition ───────────────────────────────────────
            // Replaces UE4's level streaming with dynamic world partition
            UseWorldPartition ? "-worldpartition" : string.Empty,

            // ── GTAngel IPC channels ──────────────────────────────────────
            $"-GTAngel_Pipe={PipeName}",
            $"-GTAngel_AvatarPipe={AvatarPipeName}",

            // ── ML Vision: 768×768 secondary viewport ─────────────────────
            // UE5 supports multiple viewports; the secondary 768×768 viewport
            // is dedicated to ML observation (ViT/CLIP/ESN compatible)
            UseMLVisionCapture
                ? $"-GTAngel_MLVisionPipe={MLVisionPipe} -MLVisionResX={MLVisionWidth} -MLVisionResY={MLVisionHeight}"
                : string.Empty,

            // ── DTE Cognitive Avatar ──────────────────────────────────────
            "-GTAngel_DTE_Avatar=1",
            "-GTAngel_EmbodiedCognition=1",
        };

        if (!string.IsNullOrEmpty(extraArgs))
            args.Add(extraArgs);

        return string.Join(" ", args.Where(a => !string.IsNullOrEmpty(a))).Trim();
    }

    // ── Avatar Motor Control ─────────────────────────────────────────────────

    /// <summary>
    /// Send a virtual player control command to the UE5 avatar.
    /// Uses the Enhanced Input System (UE5) instead of legacy UE4 input.
    ///
    /// UE4 legacy: NativeCalls.HandleCustomTouchEvent(x, y, action)
    /// UE5 upgrade: Enhanced Input Action via named pipe → UE5 InputSubsystem
    /// </summary>
    public async Task SendAvatarActionAsync(AvatarAction action)
    {
        var cmd = new UEMessage
        {
            Action  = "AvatarAction",
            Key     = action.InputAction,
            Value   = action.Magnitude.ToString("F3"),
            Extras  = new[] { System.Text.Json.JsonSerializer.Serialize(action) }
        };
        await SendCommandAsync(cmd);
        _logger.LogDebug("Avatar action: {Action} mag={Mag}", action.InputAction, action.Magnitude);
    }

    /// <summary>
    /// Send a navigation waypoint to the DTE avatar for human-like exploration.
    /// The avatar will pathfind to the waypoint using UE5's NavMesh + Chaos physics.
    /// </summary>
    /// <summary>
    /// KSM Cycle 6: Send the current Player↔AI bridge mode to UE5
    /// </summary>
    public async Task SendPlayerAiModeAsync(GTA3DE.Wpf.Services.PlayerAiBridgeMode mode)
    {
        await SendCommandAsync(new UEMessage
        {
            Action = "SetPlayerAiMode",
            Key = mode.ToString(),
            Value = "1.0"
        });
        _logger.LogInformation("UE5 Player↔AI mode set to {Mode}", mode);
    }

    /// <summary>
    /// KSM Cycle 6: Passthrough for human gamepad/keyboard input
    /// </summary>
    public async Task PlayerInputPassthroughAsync(float axisX, float axisY, string action)
    {
        var cmd = new AvatarAction
        {
            InputAction = action,
            AxisX = axisX,
            AxisY = axisY,
            Magnitude = 1.0f,
            Source = "Human"
        };
        await SendAvatarActionAsync(cmd);
    }

    public async Task SendNavigationWaypointAsync(float x, float y, float z, string reason = "explore")
    {
        await SendCommandAsync(new UEMessage
        {
            Action = "NavigateTo",
            Key    = reason,
            Value  = $"{x:F2},{y:F2},{z:F2}"
        });
    }

    /// <summary>
    /// Request a 768×768 ML vision frame from the secondary viewport.
    /// The UE5 engine captures the current frame and sends it back via the ML vision pipe.
    /// </summary>
    public async Task RequestMLVisionFrameAsync()
    {
        await SendCommandAsync(new UEMessage
        {
            Action = "CaptureMLVisionFrame",
            Key    = "resolution",
            Value  = $"{MLVisionWidth}x{MLVisionHeight}"
        });
    }

    // ── Lifecycle ────────────────────────────────────────────────────────────

    public async Task PauseAsync()
    {
        await SendCommandAsync(new UEMessage { Action = "Pause" });
        State = UEProcessState.Paused;
    }

    public async Task ResumeAsync()
    {
        await SendCommandAsync(new UEMessage { Action = "Resume" });
        State = UEProcessState.Running;
    }

    public async Task StopAsync(int timeoutMs = 5000)
    {
        if (_engineProcess == null || _engineProcess.HasExited)
        {
            State = UEProcessState.Stopped;
            return;
        }

        State = UEProcessState.Stopping;
        try
        {
            await SendCommandAsync(new UEMessage { Action = "Shutdown" });
            if (!_engineProcess.WaitForExit(timeoutMs))
            {
                _logger.LogWarning("UE5 engine did not exit gracefully, killing");
                _engineProcess.Kill(entireProcessTree: true);
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error stopping UE5 engine");
            try { _engineProcess?.Kill(entireProcessTree: true); } catch { }
        }
        finally
        {
            State = UEProcessState.Stopped;
            Cleanup();
        }
    }

    // ── Internal helpers ─────────────────────────────────────────────────────

    private async Task SendCommandAsync(UEMessage msg)
    {
        if (_pipeWriter == null) return;
        try
        {
            var json = JsonSerializer.Serialize(msg);
            await _pipeWriter.WriteLineAsync(json);
        }
        catch (Exception ex)
        {
            _logger.LogWarning(ex, "Failed to send command to UE5 engine");
        }
    }

    private async Task StartPipeServerAsync()
    {
        _pipeServer = new NamedPipeServerStream(
            PipeName, PipeDirection.InOut, 1,
            PipeTransmissionMode.Byte, PipeOptions.Asynchronous);

        _ = Task.Run(async () =>
        {
            try
            {
                await _pipeServer.WaitForConnectionAsync(_cts!.Token);
                _pipeWriter = new StreamWriter(_pipeServer, Encoding.UTF8) { AutoFlush = true };
                _pipeReader = new StreamReader(_pipeServer, Encoding.UTF8);
                _logger.LogInformation("UE5 engine connected via named pipe");
                await MessageLoopAsync(_cts.Token);
            }
            catch (OperationCanceledException) { }
            catch (Exception ex) { _logger.LogError(ex, "UE5 pipe server error"); }
        });
    }

    private async Task MessageLoopAsync(CancellationToken ct)
    {
        while (!ct.IsCancellationRequested && _pipeReader != null)
        {
            try
            {
                var line = await _pipeReader.ReadLineAsync(ct);
                if (line == null) break;

                var msg = JsonSerializer.Deserialize<UEMessage>(line);
                if (msg == null) continue;

                // Handle ML vision observations
                if (msg.Action == "MLVisionFrame")
                {
                    var obsJson = msg.Extras.Length > 0 ? msg.Extras[0] : "{}";
                    var obs = JsonSerializer.Deserialize<AvatarObservation>(obsJson);
                    if (obs != null) AvatarObservationReceived?.Invoke(this, obs);
                }
                else
                {
                    MessageReceived?.Invoke(this, msg);
                }
            }
            catch (OperationCanceledException) { break; }
            catch (Exception ex) { _logger.LogWarning(ex, "Message loop error"); }
        }
    }

    private async Task WaitForEngineWindowAsync(CancellationToken ct)
    {
        // Poll for the engine window to appear (UE5 startup ~3-8 seconds)
        for (int i = 0; i < 120 && !ct.IsCancellationRequested; i++)
        {
            await Task.Delay(500, ct);
            if (_engineProcess == null || _engineProcess.HasExited) break;

            _engineProcess.Refresh();
            var hwnd = _engineProcess.MainWindowHandle;
            if (hwnd != IntPtr.Zero)
            {
                EngineWindowHandle = hwnd;
                EngineWindowReady?.Invoke(this, hwnd);
                _logger.LogInformation("UE5 engine window ready: HWND {Hwnd}", hwnd);
                return;
            }
        }
        _logger.LogWarning("UE5 engine window did not appear within timeout");
    }

    private void OnProcessExited(object? sender, EventArgs e)
    {
        var code = _engineProcess?.ExitCode ?? -1;
        _logger.LogInformation("UE5 engine exited with code {Code}", code);
        State = UEProcessState.Stopped;
        EngineExited?.Invoke(this, code);
        Cleanup();
    }

    private void Cleanup()
    {
        _cts?.Cancel();
        _pipeWriter?.Dispose();
        _pipeReader?.Dispose();
        _pipeServer?.Dispose();
        _pipeWriter = null;
        _pipeReader = null;
        _pipeServer = null;
    }

    public void Dispose()
    {
        Cleanup();
        _engineProcess?.Dispose();
        _cts?.Dispose();
    }
}

// ── Supporting types ─────────────────────────────────────────────────────────

/// <summary>
/// A virtual player control action sent to the UE5 Enhanced Input System.
/// Replaces UE4's legacy HandleCustomTouchEvent / keyboard injection.
/// </summary>
public class AvatarAction
{
    /// <summary>UE5 Enhanced Input Action name (e.g. "IA_Move", "IA_Look", "IA_Jump")</summary>
    public string InputAction { get; set; } = string.Empty;

    /// <summary>Axis magnitude [-1.0 .. 1.0] for analog inputs, 1.0 for digital</summary>
    public float Magnitude { get; set; } = 1.0f;

    /// <summary>X axis for 2D inputs (movement, look)</summary>
    public float AxisX { get; set; }

    /// <summary>Y axis for 2D inputs</summary>
    public float AxisY { get; set; }

    /// <summary>Duration to hold the action (0 = single frame)</summary>
    public float HoldDuration { get; set; }

    /// <summary>Source: "ML" (from ESN/RL policy), "Human" (passthrough), "Script" (waypoint)</summary>
    public string Source { get; set; } = "ML";
}

/// <summary>
/// An ML vision observation frame from the UE5 768×768 secondary viewport.
/// Used by the ESN reservoir and RL policy for embodied cognition.
/// </summary>
public class AvatarObservation
{
    /// <summary>Frame timestamp (engine time)</summary>
    public double Timestamp { get; set; }

    /// <summary>768×768 RGB frame encoded as base64 PNG</summary>
    public string? FrameBase64 { get; set; }

    /// <summary>Avatar world position (X, Y, Z)</summary>
    public float[] Position { get; set; } = new float[3];

    /// <summary>Avatar rotation (Pitch, Yaw, Roll)</summary>
    public float[] Rotation { get; set; } = new float[3];

    /// <summary>Avatar velocity vector</summary>
    public float[] Velocity { get; set; } = new float[3];

    /// <summary>Current UE5 Enhanced Input state (active actions)</summary>
    public string[] ActiveInputActions { get; set; } = Array.Empty<string>();

    /// <summary>Neurochemical state from the DTE NeurochemicalSystem</summary>
    public NeurochemicalSnapshot? NeurochemicalState { get; set; }

    /// <summary>Nearby objects detected by UE5 perception system</summary>
    public PerceivedObject[] PerceivedObjects { get; set; } = Array.Empty<PerceivedObject>();

    /// <summary>KSM Cycle 6: The current arbitration mode (Human, AI, Arbitrated)</summary>
    public string PlayerMode { get; set; } = "Human";

    /// <summary>KSM Cycle 6: Arbitration score (0.0 = AI, 1.0 = Human)</summary>
    public float ArbitrationScore { get; set; } = 1.0f;
}

/// <summary>Snapshot of the DTE NeurochemicalSystem state</summary>
public class NeurochemicalSnapshot
{
    public float Curiosity      { get; set; }
    public float Endorphin      { get; set; }
    public float ChaosIntensity { get; set; }
    public float Homeostasis    { get; set; }
    public float Abundance      { get; set; }
    public float Scarcity       { get; set; }
}

/// <summary>An object perceived by the UE5 AI Perception system</summary>
public class PerceivedObject
{
    public string Tag      { get; set; } = string.Empty;
    public float Distance  { get; set; }
    public float[] Location { get; set; } = new float[3];
    public bool IsVisible  { get; set; }
}
