// Services/UE5LaunchOrchestrator.cs
// KSM Cycle 2: /echo-wpf-ksm-evolve | Target: UE5 Build & Asset Integration
// Alexander Properties strengthened: P2 Strong Centres, P4 Alternating Repetition,
//                                    P8 Deep Interlock, P12 The Void, P14 Not-Separateness
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using GTAngel.Interop;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

// ── Launch Stage Enum ─────────────────────────────────────────────────────────
public enum UE5LaunchStage
{
    Idle,
    Validating,   // Stage 1: Validate engine path and required binaries
    Building,     // Stage 2: Build/verify cognitive plugin modules
    Launching,    // Stage 3: Launch UnrealEditor.exe with ML Vision flags
    Connecting,   // Stage 4: Connect named IPC pipe for DTE state exchange
    Ready,        // All stages complete — UE5 is running and connected
    Failed,       // One or more stages failed
}

// ── Launch Result ─────────────────────────────────────────────────────────────
public sealed record UE5LaunchResult(
    bool Success,
    UE5LaunchStage FailedAtStage,
    string Message,
    TimeSpan Duration
);

/// <summary>
/// KSM Cycle 2 — UE5 Build &amp; Asset Integration centre transformation.
/// Implements a 4-stage pipeline: Validate → Build → Launch → Connect.
/// Emits granular progress events consumed by AvatarViewModel for live UI updates.
/// Wires the UE5 process lifecycle into the DTE cognitive state loop.
/// </summary>
public sealed class UE5LaunchOrchestrator : IDisposable
{
    // ── Constants ─────────────────────────────────────────────────────────────
    private const string DefaultEnginePath   = @".\Engine";
    private const string EditorBinary        = @"Engine\Binaries\Win64\UnrealEditor.exe";
    private const int    MlVisionWidth       = 768;
    private const int    MlVisionHeight      = 768;
    private const int    IpcConnectTimeoutMs = 15_000;
    private const int    LaunchTimeoutMs     = 60_000;
    private static readonly TimeSpan ImportTimeout = TimeSpan.FromMinutes(10);

    // ── State ─────────────────────────────────────────────────────────────────
    private readonly ILogger<UE5LaunchOrchestrator> _logger;
    private readonly AppConfiguration _config;
    private readonly UE5ProcessManager _processManager;
    private CancellationTokenSource? _cts;
    private Process? _ueProcess;
    private volatile UE5LaunchStage _currentStage = UE5LaunchStage.Idle;
    private volatile bool _isReady;

    // ── Events ────────────────────────────────────────────────────────────────
    /// <summary>Fired when the launch stage changes.</summary>
    public event Action<UE5LaunchStage, string>? OnStageChanged;
    /// <summary>Fired when the full launch pipeline completes (success or failure).</summary>
    public event Action<UE5LaunchResult>? OnLaunchComplete;
    /// <summary>Fired for each log line from the UE5 process stdout.</summary>
    public event Action<string>? OnLogLine;
    /// <summary>Fired when UE5 process exits unexpectedly.</summary>
    public event Action<int>? OnProcessExited;

    // ── Properties ────────────────────────────────────────────────────────────
    public UE5LaunchStage CurrentStage => _currentStage;
    public bool IsReady => _isReady;
    public string EnginePath => _config.Ue5EnginePath ?? DefaultEnginePath;
    public Process? UEProcess => _ueProcess;

    public UE5LaunchOrchestrator(
        ILogger<UE5LaunchOrchestrator> logger,
        AppConfiguration config,
        UE5ProcessManager processManager)
    {
        _logger = logger;
        _config = config;
        _processManager = processManager;
    }

    // ── Public API ────────────────────────────────────────────────────────────

    /// <summary>
    /// Run the full 4-stage launch pipeline asynchronously.
    /// Returns a UE5LaunchResult indicating success or the stage at which it failed.
    /// </summary>
    public async Task<UE5LaunchResult> LaunchAsync(CancellationToken externalCt = default)
    {
        _cts = CancellationTokenSource.CreateLinkedTokenSource(externalCt);
        var ct = _cts.Token;
        var sw = Stopwatch.StartNew();

        try
        {
            // ── Stage 1: Validate ─────────────────────────────────────────────
            SetStage(UE5LaunchStage.Validating, "Validating engine path and binaries...");
            var validateResult = await ValidateEngineAsync(ct);
            if (!validateResult.success)
                return Fail(UE5LaunchStage.Validating, validateResult.message, sw.Elapsed);

            // ── Stage 2: Build ────────────────────────────────────────────────
            SetStage(UE5LaunchStage.Building, "Verifying cognitive plugin modules...");
            var buildResult = await VerifyCognitivePluginsAsync(ct);
            if (!buildResult.success)
                return Fail(UE5LaunchStage.Building, buildResult.message, sw.Elapsed);

            SetStage(UE5LaunchStage.Building, "Ensuring Arc Angel Unreal assets are imported...");
            var importResult = await EnsureArcAngelAssetsAsync(ct);
            if (!importResult.success)
                return Fail(UE5LaunchStage.Building, importResult.message, sw.Elapsed);

            await _processManager.EnsureIpcServerAsync(ct);
            Log("✓ Shared GTAngel_UE5_IPC command/observation server started");

            // ── Stage 3: Launch ───────────────────────────────────────────────
            SetStage(UE5LaunchStage.Launching, "Launching UnrealEditor with ML Vision pipeline...");
            var launchResult = await LaunchEditorAsync(ct);
            if (!launchResult.success)
                return Fail(UE5LaunchStage.Launching, launchResult.message, sw.Elapsed);

            // ── Stage 4: Connect ──────────────────────────────────────────────
            SetStage(UE5LaunchStage.Connecting, "Connecting DTE cognitive IPC pipe...");
            var connectResult = await WaitForSharedIpcAsync(ct);
            if (!connectResult.success)
                return Fail(UE5LaunchStage.Connecting, connectResult.message, sw.Elapsed);

            // ── Ready ─────────────────────────────────────────────────────────
            _isReady = true;
            SetStage(UE5LaunchStage.Ready, "UE5 ready — Arc Angel command/observation transport connected");
            var result = new UE5LaunchResult(true, UE5LaunchStage.Ready,
                "UE5 launched and connected successfully", sw.Elapsed);
            OnLaunchComplete?.Invoke(result);
            return result;
        }
        catch (OperationCanceledException)
        {
            return Fail(_currentStage, "Launch cancelled by user", sw.Elapsed);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "UE5LaunchOrchestrator unexpected error at stage {Stage}", _currentStage);
            return Fail(_currentStage, ex.Message, sw.Elapsed);
        }
    }

    /// <summary>Stop the UE5 process and disconnect the IPC pipe.</summary>
    public void Stop()
    {
        _cts?.Cancel();
        _isReady = false;
        try
        {
            if (_ueProcess is { HasExited: false })
            {
                _ueProcess.Kill(entireProcessTree: true);
                _logger.LogInformation("UE5 process terminated");
            }
        }
        catch (Exception ex) { _logger.LogWarning(ex, "Error stopping UE5 process"); }
        SetStage(UE5LaunchStage.Idle, "Stopped");
    }

    /// <summary>Send a command to UE5 via the IPC pipe.</summary>
    public async Task SendCommandAsync(string commandType, string payload, string? extra = null)
    {
        if (!_processManager.IsIpcConnected)
        {
            _logger.LogWarning("Cannot send command — IPC pipe not connected");
            return;
        }
        await _processManager.SendExternalCommandAsync(commandType, payload, extra);
    }

    // ── Stage Implementations ─────────────────────────────────────────────────

    private async Task<(bool success, string message)> ValidateEngineAsync(CancellationToken ct)
    {
        await Task.Delay(200, ct); // Simulate async I/O

        var enginePath = EnginePath;
        Log($"Engine path: {enginePath}");

        if (!Directory.Exists(enginePath))
        {
            var msg = $"Engine path not found: {enginePath}";
            Log($"✗ {msg}");
            return (false, msg);
        }

        var editorExe = Path.Combine(enginePath, EditorBinary);
        if (!File.Exists(editorExe))
        {
            var msg = $"UnrealEditor.exe not found at: {editorExe}";
            Log($"✗ {msg}");
            return (false, msg);
        }

        Log($"✓ Engine binary: {editorExe}");

        string projectPath = GetGamefaceProjectPath();
        if (!File.Exists(projectPath))
        {
            var msg = $"Gameface project not found: {projectPath}";
            Log($"✗ {msg}");
            return (false, msg);
        }
        Log($"✓ Gameface project: {projectPath}");

        // Check for cognitive source modules
        var sourceDir = Path.Combine(enginePath, "Source");
        var modules = new[] { "Avatar", "Neurochemical", "Personality", "Environment" };
        foreach (var m in modules)
        {
            var mPath = Path.Combine(sourceDir, m);
            Log(Directory.Exists(mPath)
                ? $"  ✓ Module: {m}"
                : $"  ⚠ Module not found: {m} (will use stubs)");
        }

        return (true, "Engine validated");
    }

    private async Task<(bool success, string message)> VerifyCognitivePluginsAsync(CancellationToken ct)
    {
        await Task.Delay(300, ct);

        // Check for pre-built plugin binaries
        var pluginPaths = new[]
        {
            Path.Combine(AppContext.BaseDirectory, "Assets", "Gameface", "Plugins", "GTAngelRuntime"),
            Path.Combine(EnginePath, "Plugins", "GTAngelCognitive"),
            Path.Combine(EnginePath, "Plugins", "DTEReservoir"),
            Path.Combine(EnginePath, "Plugins", "MLVisionPipe"),
        };

        int found = 0;
        foreach (var p in pluginPaths)
        {
            if (Directory.Exists(p))
            {
                Log($"  ✓ Plugin: {Path.GetFileName(p)}");
                found++;
            }
            else
            {
                Log($"  ⚠ Plugin not found: {Path.GetFileName(p)} (will use engine defaults)");
            }
        }

        Log($"✓ Plugin verification complete ({found}/{pluginPaths.Length} found)");
        return (true, $"Plugins verified ({found}/{pluginPaths.Length} present)");
    }

    private async Task<(bool success, string message)> LaunchEditorAsync(CancellationToken ct)
    {
        var editorExe = Path.Combine(EnginePath, EditorBinary);
        var projectPath = GetGamefaceProjectPath();
        if (!File.Exists(projectPath))
            return (false, $"Gameface project not found: {projectPath}");

        // Build launch arguments with UE5 cognitive flags
        var args = string.Join(" ", new[]
        {
            $"\"{projectPath}\"",
            "\"/Engine/Maps/Entry?game=/Script/Engine.GameModeBase\"",
            "-game",
            "-dx12",
            "-SM6",
            "-lumen",
            "-nanite",
            "-ChaosPhysics",
            "-nophysx",
            "-WorldPartition",
            "-EnhancedInput",
            $"-ResX=1280",
            $"-ResY=720",
            $"-MLResX={MlVisionWidth}",
            $"-MLResY={MlVisionHeight}",
            $"-GTAngel_Pipe={UE5ProcessManager.PipeName}",
            $"-GTAngel_AvatarPipe={UE5ProcessManager.AvatarPipeName}",
            "-GTAngel_DTE_Avatar=1",
            "-GTAngel_EmbodiedCognition=1",
            "-DTECognitive",
            "-log",
            "-unattended",
        });

        Log($"Launching: {Path.GetFileName(editorExe)} {args[..Math.Min(80, args.Length)]}...");

        try
        {
            var psi = new ProcessStartInfo(editorExe, args)
            {
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                CreateNoWindow = true,
            };

            _ueProcess = new Process { StartInfo = psi, EnableRaisingEvents = true };
            _ueProcess.OutputDataReceived += (_, e) => { if (e.Data != null) Log(e.Data); };
            _ueProcess.ErrorDataReceived  += (_, e) => { if (e.Data != null) Log($"[ERR] {e.Data}"); };
            _ueProcess.Exited += (_, _) =>
            {
                var code = _ueProcess.ExitCode;
                _isReady = false;
                SetStage(UE5LaunchStage.Idle, $"UE5 process exited (code {code})");
                OnProcessExited?.Invoke(code);
            };

            _ueProcess.Start();
            _ueProcess.BeginOutputReadLine();
            _ueProcess.BeginErrorReadLine();

            Log($"✓ UE5 process started (PID {_ueProcess.Id})");

            // Wait briefly for process to initialise before IPC connect
            await Task.Delay(2000, ct);

            if (_ueProcess.HasExited)
                return (false, $"UE5 process exited immediately (code {_ueProcess.ExitCode})");

            return (true, $"UE5 launched (PID {_ueProcess.Id})");
        }
        catch (Exception ex)
        {
            Log($"✗ UE5 launch failed: {ex.Message}");
            return (false, ex.Message);
        }
    }

    private async Task<(bool success, string message)> EnsureArcAngelAssetsAsync(CancellationToken ct)
    {
        string projectPath = GetGamefaceProjectPath();
        string projectDirectory = Path.GetDirectoryName(projectPath)!;
        string contentDirectory = Path.Combine(
            projectDirectory, "Content", "GTAngel", "Avatars", "ArcAngelEcho");
        string meshAsset = Path.Combine(contentDirectory, "SK_ArcAngelEcho.uasset");
        string walkAsset = Path.Combine(contentDirectory, "A_ArcAngelEcho_Walk.uasset");
        string runAsset = Path.Combine(contentDirectory, "A_ArcAngelEcho_Run.uasset");
        if (File.Exists(meshAsset) && File.Exists(walkAsset) && File.Exists(runAsset))
        {
            Log("✓ Arc Angel Unreal assets already imported");
            return (true, "Arc Angel assets present");
        }

        string importer = Path.Combine(
            projectDirectory, "Plugins", "GTAngelRuntime", "Content", "Python",
            "import_arc_angel_echo.py");
        if (!File.Exists(importer))
            return (false, $"Arc Angel importer not found: {importer}");

        string editorExe = Path.Combine(EnginePath, EditorBinary);
        var psi = new ProcessStartInfo(editorExe)
        {
            UseShellExecute = false,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            CreateNoWindow = true,
        };
        psi.ArgumentList.Add(projectPath);
        psi.ArgumentList.Add("-run=pythonscript");
        psi.ArgumentList.Add($"-script={importer}");
        psi.ArgumentList.Add("-unattended");
        psi.ArgumentList.Add("-nullrhi");
        psi.ArgumentList.Add("-nosplash");
        psi.ArgumentList.Add("-stdout");
        psi.ArgumentList.Add("-FullStdOutLogOutput");

        Log("Importing Arc Angel FBX, animations and PBR textures into Gameface...");
        using var process = new Process { StartInfo = psi };
        process.Start();
        Task<string> stdout = process.StandardOutput.ReadToEndAsync(ct);
        Task<string> stderr = process.StandardError.ReadToEndAsync(ct);
        try
        {
            await process.WaitForExitAsync(ct).WaitAsync(ImportTimeout, ct);
        }
        catch (TimeoutException)
        {
            try { process.Kill(entireProcessTree: true); } catch { }
            return (false, $"Arc Angel import exceeded {ImportTimeout.TotalMinutes:F0} minutes");
        }

        string output = await stdout;
        string errors = await stderr;
        foreach (string line in output.Split('\n', StringSplitOptions.RemoveEmptyEntries).TakeLast(20))
            Log(line.TrimEnd());
        if (process.ExitCode != 0)
        {
            string detail = errors.Split('\n', StringSplitOptions.RemoveEmptyEntries).LastOrDefault()
                ?? "No error output";
            return (false, $"Arc Angel importer failed with exit code {process.ExitCode}: {detail}");
        }

        if (!File.Exists(meshAsset) || !File.Exists(walkAsset) || !File.Exists(runAsset))
            return (false, "Arc Angel importer exited successfully but required UE assets were not generated");

        Log("✓ Arc Angel mesh, locomotion and PBR assets imported");
        return (true, "Arc Angel assets imported");
    }

    private async Task<(bool success, string message)> WaitForSharedIpcAsync(CancellationToken ct)
    {
        Log($"Waiting for IPC client: {UE5ProcessManager.PipeName}...");
        bool connected = await _processManager.WaitForIpcConnectionAsync(
            TimeSpan.FromMilliseconds(IpcConnectTimeoutMs), ct);
        if (!connected)
            return (false, $"Timed out waiting for {UE5ProcessManager.PipeName}");

        Log("✓ Shared GTAngel IPC connected");
        return (true, "Shared GTAngel IPC connected");
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    private static string GetGamefaceProjectPath() => Path.Combine(
        AppContext.BaseDirectory, "Assets", "Gameface", "Gameface.uproject");

    private void SetStage(UE5LaunchStage stage, string message)
    {
        _currentStage = stage;
        _logger.LogInformation("[UE5 Stage {Stage}] {Message}", stage, message);
        OnStageChanged?.Invoke(stage, message);
    }

    private void Log(string line)
    {
        _logger.LogDebug("[UE5] {Line}", line);
        OnLogLine?.Invoke(line);
    }

    private UE5LaunchResult Fail(UE5LaunchStage stage, string message, TimeSpan elapsed)
    {
        _isReady = false;
        SetStage(UE5LaunchStage.Failed, $"Failed at {stage}: {message}");
        var result = new UE5LaunchResult(false, stage, message, elapsed);
        OnLaunchComplete?.Invoke(result);
        return result;
    }

    public void Dispose()
    {
        Stop();
        _cts?.Dispose();
        _ueProcess?.Dispose();
    }
}
