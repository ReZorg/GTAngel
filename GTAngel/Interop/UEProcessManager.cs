using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Text;
using System.Text.Json;
using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Interop;

/// <summary>
/// Manages the UE4/UE5 game engine process lifecycle.
/// Translated from: GameActivity (NativeActivity) which loads libUE4.so via dlopen/JNI.
///
/// On Android, the UE4 engine runs in-process as a native library loaded by NativeActivity.
/// On Windows, we launch the UE4/UE5 game as a separate process and communicate via:
///   1. Named pipes (bidirectional IPC replacing JNI NativeCalls)
///   2. Win32 window embedding (replacing SurfaceView)
///   3. Shared memory (replacing Android SharedPreferences for game state)
///
/// JNI Bridge Translation:
///   Java → NativeCalls.CallNativeToEmbedded(action, id, key, val, extras, json)
///   becomes: C# → UEProcessManager.SendCommand(action, id, key, val, extras, json)
///   via named pipe: \\.\pipe\GTA3DE_IPC
///
/// Lifecycle Translation:
///   GameActivity.onCreate()  → StartAsync() — launch process
///   GameActivity.onResume()  → Resume() — send resume command
///   GameActivity.onPause()   → Pause() — send pause command
///   GameActivity.onDestroy() → StopAsync() — terminate process
///   NativeCalls.SetNamedObject() → SendCommand("SetNamedObject", ...)
/// </summary>
public class UEProcessManager : IDisposable
{
    private readonly ILogger<UEProcessManager> _logger;
    private Process? _engineProcess;
    private NamedPipeServerStream? _pipeServer;
    private StreamWriter? _pipeWriter;
    private StreamReader? _pipeReader;
    private CancellationTokenSource? _cts;
    private Task? _messageLoopTask;

    /// <summary>Named pipe identifier for IPC (replaces JNI bridge)</summary>
    public const string PipeName = "GTA3DE_IPC";

    /// <summary>Shared memory name for game state (replaces SharedPreferences)</summary>
    public const string SharedMemoryName = "GTA3DE_SharedState";

    /// <summary>Fires when the engine process starts and its main window is available</summary>
    public event EventHandler<IntPtr>? EngineWindowReady;

    /// <summary>Fires when the engine process exits</summary>
    public event EventHandler<int>? EngineExited;

    /// <summary>Fires when a message is received from the engine via named pipe</summary>
    public event EventHandler<UEMessage>? MessageReceived;

    /// <summary>Current engine process state</summary>
    public UEProcessState State { get; private set; } = UEProcessState.NotStarted;

    /// <summary>The engine process main window handle (for embedding)</summary>
    public IntPtr EngineWindowHandle { get; private set; }

    /// <summary>The engine process ID</summary>
    public int ProcessId => _engineProcess?.Id ?? 0;

    public UEProcessManager(ILogger<UEProcessManager> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Launch the UE4/UE5 game engine process.
    /// Replaces: NativeActivity.onCreate() → System.loadLibrary("UE4") → native main loop
    ///
    /// The Android flow:
    ///   1. NativeActivity loads libUE4.so
    ///   2. Calls ANativeActivity_onCreate (C entry point)
    ///   3. UE4 creates EGL surface from the Activity's SurfaceView
    ///   4. FEngineLoop::Init() → FEngineLoop::Tick() game loop starts
    ///
    /// The Windows flow:
    ///   1. Launch the UE4/UE5 game .exe as a child process
    ///   2. Pass command-line args equivalent to UE4CommandLine.txt
    ///   3. Wait for the game window to appear
    ///   4. Embed the game window into our WPF host
    ///   5. Establish named pipe IPC
    /// </summary>
    public async Task<bool> StartAsync(string executablePath, string? commandLine = null)
    {
        if (State == UEProcessState.Running)
        {
            _logger.LogWarning("Engine already running (PID {Pid})", _engineProcess?.Id);
            return true;
        }

        if (!File.Exists(executablePath))
        {
            _logger.LogError("Engine executable not found: {Path}", executablePath);
            State = UEProcessState.Error;
            return false;
        }

        _cts = new CancellationTokenSource();
        State = UEProcessState.Starting;

        try
        {
            // Start the named pipe server before launching the process
            await StartPipeServerAsync();

            // Build command line arguments
            // Replaces: UE4CommandLine.txt content: ../../../Gameface/Gameface.uproject
            var args = BuildCommandLineArgs(executablePath, commandLine);

            var startInfo = new ProcessStartInfo
            {
                FileName = executablePath,
                Arguments = args,
                UseShellExecute = false,
                WorkingDirectory = Path.GetDirectoryName(executablePath)!,
                // Pass the pipe name so the engine can connect back
                Environment =
                {
                    ["GTA3DE_PIPE"] = PipeName,
                    ["GTA3DE_SHARED_MEMORY"] = SharedMemoryName,
                    // Replaces: nativeSetAndroidVersionInformation
                    ["GTA3DE_HOST_VERSION"] = "1.84.3",
                    ["GTA3DE_HOST_PLATFORM"] = "Windows"
                }
            };

            _engineProcess = Process.Start(startInfo);
            if (_engineProcess == null)
            {
                _logger.LogError("Failed to start engine process");
                State = UEProcessState.Error;
                return false;
            }

            _engineProcess.EnableRaisingEvents = true;
            _engineProcess.Exited += OnEngineProcessExited;

            _logger.LogInformation("Engine process started: PID {Pid}, Path: {Path}",
                _engineProcess.Id, executablePath);

            // Wait for the engine window to appear
            // Replaces: waiting for SurfaceView.surfaceCreated() callback
            EngineWindowHandle = await WaitForEngineWindowAsync(_cts.Token);

            if (EngineWindowHandle != IntPtr.Zero)
            {
                State = UEProcessState.Running;
                EngineWindowReady?.Invoke(this, EngineWindowHandle);

                // Send initial configuration
                // Replaces: nativeSetGlobalActivity, nativeSetAndroidStartupState,
                //           nativeSetObbFilePaths, nativeSetObbInfo
                await SendInitialConfigurationAsync();

                // Start the message receive loop
                _messageLoopTask = Task.Run(() => MessageReceiveLoopAsync(_cts.Token));

                _logger.LogInformation("Engine window ready: HWND {Hwnd}", EngineWindowHandle);
                return true;
            }
            else
            {
                _logger.LogWarning("Engine window not found within timeout");
                State = UEProcessState.Error;
                return false;
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to start engine");
            State = UEProcessState.Error;
            return false;
        }
    }

    /// <summary>
    /// Send a command to the engine via named pipe.
    /// Replaces: NativeCalls.CallNativeToEmbedded(action, id, key, val, extras, json)
    /// This is the primary IPC mechanism replacing the JNI bridge.
    /// </summary>
    public async Task SendCommandAsync(string action, int id = 0,
        string? key = null, string? value = null,
        string[]? extras = null, string? json = null)
    {
        if (_pipeWriter == null || State != UEProcessState.Running)
        {
            _logger.LogWarning("Cannot send command: engine not running");
            return;
        }

        var message = new UEMessage
        {
            Action = action,
            Id = id,
            Key = key ?? string.Empty,
            Value = value ?? string.Empty,
            Extras = extras ?? Array.Empty<string>(),
            Json = json ?? string.Empty,
            Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
        };

        var serialized = JsonSerializer.Serialize(message);
        await _pipeWriter.WriteLineAsync(serialized);
        await _pipeWriter.FlushAsync();

        _logger.LogDebug("Sent command: {Action} (id={Id})", action, id);
    }

    /// <summary>
    /// Pause the engine.
    /// Replaces: GameActivity.onPause() → native onPause callback
    /// </summary>
    public async Task PauseAsync()
    {
        if (State != UEProcessState.Running) return;
        State = UEProcessState.Paused;
        await SendCommandAsync("Pause");
        _logger.LogDebug("Engine paused");
    }

    /// <summary>
    /// Resume the engine.
    /// Replaces: GameActivity.onResume() → nativeResumeMainInit
    /// </summary>
    public async Task ResumeAsync()
    {
        if (State != UEProcessState.Paused) return;
        State = UEProcessState.Running;
        await SendCommandAsync("Resume");
        _logger.LogDebug("Engine resumed");
    }

    /// <summary>
    /// Send a console command to the engine.
    /// Replaces: nativeConsoleCommand(String command)
    /// </summary>
    public async Task SendConsoleCommandAsync(string command)
    {
        await SendCommandAsync("ConsoleCommand", key: command);
    }

    /// <summary>
    /// Notify engine of window size change.
    /// Replaces: nativeSetWindowInfo(boolean, int, int, int, int)
    /// </summary>
    public async Task SetWindowInfoAsync(bool hasFocus, int x, int y, int width, int height)
    {
        await SendCommandAsync("SetWindowInfo",
            json: JsonSerializer.Serialize(new { hasFocus, x, y, width, height }));
    }

    /// <summary>
    /// Notify engine of safe zone (notch/cutout) info.
    /// Replaces: nativeSetSafezoneInfo(boolean, int, int, int, int)
    /// </summary>
    public async Task SetSafezoneInfoAsync(bool hasSafezone, int left, int top, int right, int bottom)
    {
        await SendCommandAsync("SetSafezoneInfo",
            json: JsonSerializer.Serialize(new { hasSafezone, left, top, right, bottom }));
    }

    /// <summary>
    /// Set OBB/asset file paths for the engine.
    /// Replaces: nativeSetObbFilePaths(String internalPath, String externalPath)
    /// </summary>
    public async Task SetAssetPathsAsync(string internalPath, string externalPath)
    {
        await SendCommandAsync("SetObbFilePaths",
            key: internalPath, value: externalPath);
    }

    /// <summary>
    /// Forward a notification to the engine.
    /// Replaces: NativeCalls.ForwardNotification(String json)
    /// </summary>
    public async Task ForwardNotificationAsync(string notificationJson)
    {
        await SendCommandAsync("ForwardNotification", json: notificationJson);
    }

    /// <summary>
    /// Set a named object in the engine's object registry.
    /// Replaces: NativeCalls.SetNamedObject(String name, Object obj)
    /// </summary>
    public async Task SetNamedObjectAsync(string name, string serializedObject)
    {
        await SendCommandAsync("SetNamedObject", key: name, value: serializedObject);
    }

    /// <summary>
    /// Handle custom touch/input event.
    /// Replaces: NativeCalls.HandleCustomTouchEvent(int deviceId, int action, int source, int pointerId, float x, float y)
    /// On Windows this translates mouse/touch events to the engine's input format.
    /// </summary>
    public async Task HandleInputEventAsync(int deviceId, int action, int source, int pointerId, float x, float y)
    {
        await SendCommandAsync("HandleCustomTouchEvent",
            json: JsonSerializer.Serialize(new { deviceId, action, source, pointerId, x, y }));
    }

    /// <summary>
    /// Route a service intent to the engine.
    /// Replaces: NativeCalls.RouteServiceIntent(String action, String data)
    /// </summary>
    public async Task RouteServiceIntentAsync(string action, string data)
    {
        await SendCommandAsync("RouteServiceIntent", key: action, value: data);
    }

    /// <summary>
    /// Log to the engine's logging system.
    /// Replaces: NativeCalls.UELogLog/UELogWarning/UELogError/UELogVerbose
    /// </summary>
    public async Task UELogAsync(string message, UELogLevel level = UELogLevel.Log)
    {
        await SendCommandAsync($"UELog{level}", value: message);
    }

    /// <summary>
    /// Notify engine of WebView visibility change.
    /// Replaces: NativeCalls.WebViewVisible(boolean visible)
    /// </summary>
    public async Task SetWebViewVisibleAsync(bool visible)
    {
        await SendCommandAsync("WebViewVisible", value: visible.ToString());
    }

    /// <summary>
    /// Allow or disallow back button events.
    /// Replaces: NativeCalls.AllowJavaBackButtonEvent(boolean allow)
    /// </summary>
    public async Task AllowBackButtonAsync(bool allow)
    {
        await SendCommandAsync("AllowBackButtonEvent", value: allow.ToString());
    }

    /// <summary>
    /// Stop the engine process gracefully.
    /// Replaces: GameActivity.onDestroy()
    /// </summary>
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
            // Send graceful shutdown command
            await SendCommandAsync("Shutdown");

            // Wait for graceful exit
            if (!_engineProcess.WaitForExit(timeoutMs))
            {
                _logger.LogWarning("Engine did not exit gracefully, killing process");
                _engineProcess.Kill(entireProcessTree: true);
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error stopping engine");
            try { _engineProcess.Kill(entireProcessTree: true); } catch { }
        }
        finally
        {
            State = UEProcessState.Stopped;
            Cleanup();
        }
    }

    private string BuildCommandLineArgs(string executablePath, string? commandLine)
    {
        var sb = new StringBuilder();

        // Replaces UE4CommandLine.txt: ../../../Gameface/Gameface.uproject
        if (!string.IsNullOrEmpty(commandLine))
        {
            sb.Append(commandLine);
        }

        // Add IPC pipe name for the engine to connect back
        sb.Append($" -GTA3DE_Pipe={PipeName}");

        // Replaces: nativeSetAndroidStartupState flags
        sb.Append(" -Windowed");
        sb.Append(" -ResX=1920 -ResY=1080");

        // Replaces: nativeIsShippingBuild check
        sb.Append(" -Shipping");

        // Replaces: nativeSupportsNEON → equivalent SIMD on x86
        // (UE4 auto-detects SSE/AVX on Windows)

        return sb.ToString().Trim();
    }

    private async Task StartPipeServerAsync()
    {
        _pipeServer = new NamedPipeServerStream(
            PipeName,
            PipeDirection.InOut,
            1,
            PipeTransmissionMode.Byte,
            PipeOptions.Asynchronous);

        // Don't block — the engine will connect when ready
        _ = Task.Run(async () =>
        {
            try
            {
                await _pipeServer.WaitForConnectionAsync(_cts!.Token);
                _pipeWriter = new StreamWriter(_pipeServer, Encoding.UTF8) { AutoFlush = true };
                _pipeReader = new StreamReader(_pipeServer, Encoding.UTF8);
                _logger.LogInformation("Engine connected via named pipe");
            }
            catch (OperationCanceledException) { }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Pipe server error");
            }
        });
    }

    private async Task<IntPtr> WaitForEngineWindowAsync(CancellationToken ct)
    {
        const int maxWaitMs = 30000;
        const int pollIntervalMs = 250;
        var elapsed = 0;

        while (elapsed < maxWaitMs && !ct.IsCancellationRequested)
        {
            if (_engineProcess == null || _engineProcess.HasExited)
                return IntPtr.Zero;

            _engineProcess.Refresh();
            if (_engineProcess.MainWindowHandle != IntPtr.Zero)
                return _engineProcess.MainWindowHandle;

            await Task.Delay(pollIntervalMs, ct);
            elapsed += pollIntervalMs;
        }

        return IntPtr.Zero;
    }

    private async Task SendInitialConfigurationAsync()
    {
        // Replaces: nativeSetGlobalActivity (pass host reference)
        await SendCommandAsync("SetHostInfo",
            json: JsonSerializer.Serialize(new
            {
                HostPid = Environment.ProcessId,
                HostVersion = "1.84.3",
                Platform = "Windows",
                PipeName
            }));

        // Replaces: nativeSetAndroidVersionInformation
        await SendCommandAsync("SetVersionInfo",
            json: JsonSerializer.Serialize(new
            {
                SdkVersion = Environment.OSVersion.Version.ToString(),
                DeviceModel = Environment.MachineName,
                DeviceManufacturer = "PC"
            }));

        // Replaces: nativeSetConfigRulesVariables
        await SendCommandAsync("SetConfigRules",
            json: JsonSerializer.Serialize(new
            {
                IsShippingBuild = true,
                SupportsVulkan = true,
                DepthBufferPreference = 0
            }));

        // Replaces: nativeOnInitialDownloadCompleted
        await SendCommandAsync("InitialDownloadCompleted");
    }

    private async Task MessageReceiveLoopAsync(CancellationToken ct)
    {
        while (!ct.IsCancellationRequested && _pipeReader != null)
        {
            try
            {
                var line = await _pipeReader.ReadLineAsync(ct);
                if (line == null) break;

                var message = JsonSerializer.Deserialize<UEMessage>(line);
                if (message != null)
                {
                    _logger.LogDebug("Received from engine: {Action}", message.Action);
                    MessageReceived?.Invoke(this, message);
                }
            }
            catch (OperationCanceledException) { break; }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error reading from engine pipe");
            }
        }
    }

    private void OnEngineProcessExited(object? sender, EventArgs e)
    {
        var exitCode = _engineProcess?.ExitCode ?? -1;
        _logger.LogInformation("Engine process exited with code {Code}", exitCode);
        State = UEProcessState.Stopped;
        EngineExited?.Invoke(this, exitCode);
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
        EngineWindowHandle = IntPtr.Zero;
    }

    public void Dispose()
    {
        StopAsync(2000).GetAwaiter().GetResult();
        _cts?.Dispose();
        GC.SuppressFinalize(this);
    }
}

/// <summary>
/// IPC message format for communication between WPF host and UE engine.
/// Replaces the JNI NativeCalls interface with a JSON-over-pipe protocol.
/// Each field maps to a parameter of NativeCalls.CallNativeToEmbedded().
/// </summary>
public class UEMessage
{
    public string Action { get; set; } = string.Empty;
    public int Id { get; set; }
    public string Key { get; set; } = string.Empty;
    public string Value { get; set; } = string.Empty;
    public string[] Extras { get; set; } = Array.Empty<string>();
    public string Json { get; set; } = string.Empty;
    public long Timestamp { get; set; }
}

/// <summary>Engine process lifecycle states</summary>
public enum UEProcessState
{
    NotStarted,
    Starting,
    Running,
    Paused,
    Stopping,
    Stopped,
    Error
}

/// <summary>UE log levels matching NativeCalls.UELog* methods</summary>
public enum UELogLevel
{
    Verbose,
    Log,
    Warning,
    Error
}
