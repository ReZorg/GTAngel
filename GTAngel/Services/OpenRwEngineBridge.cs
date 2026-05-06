using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text.Json;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Bridge to OpenRW (open-source GTA3 engine reimplementation) and re3 (reverse-engineered GTA3).
/// Manages engine detection, configuration, launching at 768x768 resolution, and IPC for training.
/// 
/// Supported engines (in priority order):
///   1. OpenRW  — Full open-source reimplementation (rwengine/openrw)
///   2. re3     — Reverse-engineered source port (halpz/re3)
///   3. GTA3 DE — Original Definitive Edition (via UE process manager)
///   4. GTA3 OG — Original 2001 release (via compatibility shim)
/// 
/// OpenRW/re3 advantages for ML training:
///   - Custom resolution support (768x768 native)
///   - Direct memory access without ReadProcessMemory
///   - Headless rendering mode for accelerated training
///   - Custom rendering hooks for segmentation maps
///   - Deterministic frame stepping (pause → step → capture → resume)
///   - Built-in scripting for reward signal extraction
/// </summary>
public sealed class OpenRwEngineBridge : IDisposable
{
    private readonly ILogger<OpenRwEngineBridge> _logger;
    private Process? _engineProcess;
    private bool _disposed;

    // Engine detection results
    public EngineType DetectedEngine { get; private set; } = EngineType.None;
    public string? EnginePath { get; private set; }
    public string? GameDataPath { get; private set; }
    public bool IsRunning => _engineProcess is { HasExited: false };
    public Process? GameProcess => _engineProcess;
    public nint GameWindowHandle => _engineProcess?.MainWindowHandle ?? nint.Zero;

    // Training-specific configuration
    public int RenderWidth { get; set; } = 768;
    public int RenderHeight { get; set; } = 768;
    public bool HeadlessMode { get; set; }
    public bool DeterministicStepping { get; set; } = true;
    public int TargetFps { get; set; } = 30;

    // IPC for training control
    private string? _ipcPipeName;
    private System.IO.Pipes.NamedPipeServerStream? _ipcServer;

    public enum EngineType
    {
        None,
        OpenRW,
        Re3,
        GTA3DE,
        GTA3Original,
    }

    /// <summary>
    /// Configuration for the game engine.
    /// </summary>
    public class EngineConfig
    {
        public int Width { get; set; } = 768;
        public int Height { get; set; } = 768;
        public bool Fullscreen { get; set; }
        public bool VSync { get; set; }
        public int TargetFps { get; set; } = 30;
        public bool Headless { get; set; }
        public bool DeterministicStep { get; set; } = true;
        public string? GameDataPath { get; set; }
        public string? SavePath { get; set; }
        public string? ModsPath { get; set; }
        public bool EnableSegmentationOutput { get; set; }
        public bool EnableDepthBuffer { get; set; }
        public bool EnableRewardSignals { get; set; }
    }

    /// <summary>
    /// Game state extracted from the engine for RL training.
    /// </summary>
    public class GameState
    {
        public float PlayerX { get; set; }
        public float PlayerY { get; set; }
        public float PlayerZ { get; set; }
        public float PlayerHeading { get; set; }
        public float PlayerHealth { get; set; }
        public float PlayerArmor { get; set; }
        public int PlayerMoney { get; set; }
        public int WantedLevel { get; set; }
        public int CurrentWeapon { get; set; }
        public bool InVehicle { get; set; }
        public float VehicleHealth { get; set; }
        public float VehicleSpeed { get; set; }
        public float VelocityX { get; set; }
        public float VelocityY { get; set; }
        public float VelocityZ { get; set; }
        public string CurrentIsland { get; set; } = "Portland";
        public int GameHour { get; set; }
        public int GameMinute { get; set; }
        public string Weather { get; set; } = "Sunny";
        public bool IsDead { get; set; }
        public bool IsArrested { get; set; }
        public int MissionIndex { get; set; }
        public float DistanceTraveled { get; set; }

        /// <summary>
        /// Convert to a normalized feature vector for the ESN reservoir.
        /// </summary>
        public float[] ToFeatureVector()
        {
            return new float[]
            {
                PlayerX / 4000f,        // Normalize to map bounds
                PlayerY / 4000f,
                PlayerZ / 100f,
                PlayerHeading / 360f,
                PlayerHealth / 100f,
                PlayerArmor / 100f,
                PlayerMoney / 1000000f,
                WantedLevel / 6f,
                CurrentWeapon / 15f,
                InVehicle ? 1f : 0f,
                VehicleHealth / 1000f,
                VehicleSpeed / 200f,
                VelocityX / 50f,
                VelocityY / 50f,
                VelocityZ / 50f,
                CurrentIsland switch { "Portland" => 0f, "Staunton" => 0.5f, "Shoreside" => 1f, _ => 0f },
                GameHour / 24f,
                (Weather switch { "Sunny" => 0, "Cloudy" => 1, "Rainy" => 2, "Foggy" => 3, _ => 0 }) / 3f,
                IsDead ? 1f : 0f,
                IsArrested ? 1f : 0f,
                MissionIndex / 100f,
                DistanceTraveled / 10000f,
            };
        }
    }

    public OpenRwEngineBridge(ILogger<OpenRwEngineBridge> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Detect available game engines on the system.
    /// </summary>
    public EngineType DetectEngines()
    {
        _logger.LogInformation("Scanning for GTA3 game engines...");

        // 1. Check for OpenRW
        var openrwPath = FindExecutable("openrw", new[]
        {
            @"C:\OpenRW\openrw.exe",
            @"C:\Program Files\OpenRW\openrw.exe",
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "OpenRW", "openrw.exe"),
            Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "engines", "openrw", "openrw.exe"),
        });

        if (openrwPath != null)
        {
            DetectedEngine = EngineType.OpenRW;
            EnginePath = openrwPath;
            _logger.LogInformation("Found OpenRW at: {Path}", openrwPath);
            return DetectedEngine;
        }

        // 2. Check for re3
        var re3Path = FindExecutable("re3", new[]
        {
            @"C:\re3\re3.exe",
            @"C:\Program Files\re3\re3.exe",
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "re3", "re3.exe"),
            Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "engines", "re3", "re3.exe"),
        });

        if (re3Path != null)
        {
            DetectedEngine = EngineType.Re3;
            EnginePath = re3Path;
            _logger.LogInformation("Found re3 at: {Path}", re3Path);
            return DetectedEngine;
        }

        // 3. Check for GTA3 Definitive Edition
        var dePaths = new[]
        {
            @"C:\Program Files\Rockstar Games\Grand Theft Auto III - Definitive Edition",
            @"C:\Program Files (x86)\Steam\steamapps\common\Grand Theft Auto III - Definitive Edition",
            @"E:\Games\GTA3DE",
        };

        foreach (var path in dePaths)
        {
            if (Directory.Exists(path))
            {
                var exe = Directory.GetFiles(path, "*.exe", SearchOption.TopDirectoryOnly)
                    .FirstOrDefault(f => f.Contains("GTA", StringComparison.OrdinalIgnoreCase));
                if (exe != null)
                {
                    DetectedEngine = EngineType.GTA3DE;
                    EnginePath = exe;
                    GameDataPath = path;
                    _logger.LogInformation("Found GTA3 Definitive Edition at: {Path}", path);
                    return DetectedEngine;
                }
            }
        }

        // 4. Check for original GTA3
        var ogPaths = new[]
        {
            @"C:\Program Files\Rockstar Games\Grand Theft Auto III",
            @"C:\Program Files (x86)\Steam\steamapps\common\Grand Theft Auto III",
            @"C:\GOG Games\Grand Theft Auto III",
        };

        foreach (var path in ogPaths)
        {
            var exe = Path.Combine(path, "gta3.exe");
            if (File.Exists(exe))
            {
                DetectedEngine = EngineType.GTA3Original;
                EnginePath = exe;
                GameDataPath = path;
                _logger.LogInformation("Found GTA3 Original at: {Path}", path);
                return DetectedEngine;
            }
        }

        _logger.LogWarning("No GTA3 engine found. Running in simulation mode.");
        DetectedEngine = EngineType.None;
        return DetectedEngine;
    }

    /// <summary>
    /// Set a custom engine path.
    /// </summary>
    public void SetEnginePath(string path, EngineType type)
    {
        if (File.Exists(path))
        {
            EnginePath = path;
            DetectedEngine = type;
            GameDataPath = Path.GetDirectoryName(path);
            _logger.LogInformation("Engine path set to: {Path} ({Type})", path, type);
        }
    }

    /// <summary>
    /// Set the game data directory (where GTA3 data files are located).
    /// </summary>
    public void SetGameDataPath(string path)
    {
        if (Directory.Exists(path))
        {
            GameDataPath = path;
            _logger.LogInformation("Game data path set to: {Path}", path);
        }
    }

    /// <summary>
    /// Launch the game engine with training configuration.
    /// </summary>
    public async Task<bool> LaunchAsync(EngineConfig? config = null)
    {
        config ??= new EngineConfig
        {
            Width = RenderWidth,
            Height = RenderHeight,
            TargetFps = TargetFps,
            Headless = HeadlessMode,
            DeterministicStep = DeterministicStepping,
            GameDataPath = GameDataPath,
        };

        if (DetectedEngine == EngineType.None)
        {
            _logger.LogWarning("No engine detected. Use DetectEngines() or SetEnginePath() first.");
            return false;
        }

        try
        {
            // Set up IPC pipe for training control
            _ipcPipeName = $"angelclaw_gta3_training_{Process.GetCurrentProcess().Id}";

            var args = BuildLaunchArguments(config);
            _logger.LogInformation("Launching {Engine} with args: {Args}", DetectedEngine, args);

            var startInfo = new ProcessStartInfo
            {
                FileName = EnginePath!,
                Arguments = args,
                WorkingDirectory = GameDataPath ?? Path.GetDirectoryName(EnginePath)!,
                UseShellExecute = false,
                CreateNoWindow = config.Headless,
            };

            // Set environment variables for training mode
            startInfo.EnvironmentVariables["ANGELCLAW_TRAINING"] = "1";
            startInfo.EnvironmentVariables["ANGELCLAW_IPC_PIPE"] = _ipcPipeName;
            startInfo.EnvironmentVariables["ANGELCLAW_RENDER_W"] = config.Width.ToString();
            startInfo.EnvironmentVariables["ANGELCLAW_RENDER_H"] = config.Height.ToString();

            _engineProcess = Process.Start(startInfo);

            if (_engineProcess == null)
            {
                _logger.LogError("Failed to start engine process");
                return false;
            }

            _logger.LogInformation("Engine started (PID: {Pid})", _engineProcess.Id);

            // Wait for the window to appear
            await WaitForWindowAsync(TimeSpan.FromSeconds(30));

            return true;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to launch engine");
            return false;
        }
    }

    /// <summary>
    /// Stop the game engine.
    /// </summary>
    public void Stop()
    {
        if (_engineProcess is { HasExited: false })
        {
            try
            {
                _engineProcess.CloseMainWindow();
                if (!_engineProcess.WaitForExit(5000))
                {
                    _engineProcess.Kill();
                }
                _logger.LogInformation("Engine stopped");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error stopping engine");
            }
        }
    }

    /// <summary>
    /// Read the current game state from the engine.
    /// For OpenRW/re3, uses shared memory or IPC.
    /// For GTA3 DE/OG, uses ReadProcessMemory.
    /// </summary>
    public GameState ReadGameState()
    {
        if (_engineProcess is null or { HasExited: true })
            return new GameState();

        return DetectedEngine switch
        {
            EngineType.OpenRW => ReadGameStateOpenRw(),
            EngineType.Re3 => ReadGameStateRe3(),
            EngineType.GTA3DE => ReadGameStateProcessMemory(),
            EngineType.GTA3Original => ReadGameStateProcessMemory(),
            _ => new GameState(),
        };
    }

    /// <summary>
    /// Send a training step command: pause → execute action → advance one frame → capture.
    /// Only available with OpenRW/re3 in deterministic stepping mode.
    /// </summary>
    public async Task<GameState> StepAsync()
    {
        if (DeterministicStepping && (DetectedEngine == EngineType.OpenRW || DetectedEngine == EngineType.Re3))
        {
            // Send step command via IPC
            await SendIpcCommandAsync("step");
        }
        else
        {
            // For non-deterministic engines, just wait one frame
            await Task.Delay(1000 / TargetFps);
        }

        return ReadGameState();
    }

    /// <summary>
    /// Reset the game to a known starting state (for episode boundaries).
    /// </summary>
    public async Task ResetAsync(string? saveName = null)
    {
        if (DetectedEngine == EngineType.OpenRW || DetectedEngine == EngineType.Re3)
        {
            await SendIpcCommandAsync(saveName != null ? $"load:{saveName}" : "reset");
        }
        else
        {
            // For DE/OG, we'd need to use input injection to load a save
            _logger.LogWarning("Reset not supported for {Engine} — use save/load manually", DetectedEngine);
        }
    }

    /// <summary>
    /// Generate the OpenRW build configuration for 768x768 training resolution.
    /// This creates a CMake configuration that can be used to compile OpenRW.
    /// </summary>
    public string GenerateOpenRwBuildConfig()
    {
        return $@"# OpenRW Build Configuration for AngelClaw DTE Training
# Generated by GTAngelClaw.Wpf — {DateTime.Now:yyyy-MM-dd HH:mm}
#
# Build instructions:
#   git clone https://github.com/rwengine/openrw.git
#   cd openrw && mkdir build && cd build
#   cmake .. -G ""Visual Studio 17 2022"" \
#     -DCMAKE_BUILD_TYPE=Release \
#     -DOPENRW_DEFAULT_WIDTH={RenderWidth} \
#     -DOPENRW_DEFAULT_HEIGHT={RenderHeight} \
#     -DOPENRW_TRAINING_MODE=ON \
#     -DOPENRW_HEADLESS=ON \
#     -DOPENRW_IPC_PIPE=ON
#   cmake --build . --config Release

cmake_minimum_required(VERSION 3.16)

# Training resolution override
set(OPENRW_DEFAULT_WIDTH {RenderWidth} CACHE STRING ""Default render width"")
set(OPENRW_DEFAULT_HEIGHT {RenderHeight} CACHE STRING ""Default render height"")

# Training mode features
option(OPENRW_TRAINING_MODE ""Enable ML training hooks"" ON)
option(OPENRW_HEADLESS ""Enable headless rendering"" OFF)
option(OPENRW_IPC_PIPE ""Enable named pipe IPC for training control"" ON)
option(OPENRW_SEGMENTATION ""Enable semantic segmentation output"" ON)
option(OPENRW_DEPTH_BUFFER ""Enable depth buffer output"" ON)

# Dependencies (use vcpkg on Windows)
# vcpkg install sdl2 openal-soft bullet3 glm ffmpeg boost-filesystem boost-program-options

# Compile with training hooks
add_definitions(-DANGELCLAW_TRAINING)
add_definitions(-DRENDER_WIDTH=${{OPENRW_DEFAULT_WIDTH}})
add_definitions(-DRENDER_HEIGHT=${{OPENRW_DEFAULT_HEIGHT}})
";
    }

    /// <summary>
    /// Generate the re3 build configuration for 768x768 training resolution.
    /// </summary>
    public string GenerateRe3BuildConfig()
    {
        return $@"# re3 Build Configuration for AngelClaw DTE Training
# Generated by GTAngelClaw.Wpf — {DateTime.Now:yyyy-MM-dd HH:mm}
#
# Build instructions:
#   git clone https://github.com/halpz/re3.git
#   Open re3.sln in Visual Studio 2022
#   Set configuration to Release|x64
#   Apply the following defines in project properties:
#
# Preprocessor Definitions:
#   DEFAULT_SCREEN_WIDTH={RenderWidth}
#   DEFAULT_SCREEN_HEIGHT={RenderHeight}
#   ANGELCLAW_TRAINING=1
#   TRAINING_IPC_PIPE=1
#
# In src/core/config.h, modify:
#   #define DEFAULT_SCREEN_WIDTH {RenderWidth}
#   #define DEFAULT_SCREEN_HEIGHT {RenderHeight}
#   #define DEFAULT_ASPECT_RATIO (1.0f)  // Square for ML
#
# In src/render/Renderer.cpp, add frame capture hook:
#   void CRenderer::RenderFrame() {{
#       ... existing code ...
#       #ifdef ANGELCLAW_TRAINING
#       AngelClawTraining::OnFrameRendered(backBuffer, {RenderWidth}, {RenderHeight});
#       #endif
#   }}
";
    }

    #region Private Implementation

    private string BuildLaunchArguments(EngineConfig config)
    {
        return DetectedEngine switch
        {
            EngineType.OpenRW => $"--width {config.Width} --height {config.Height} " +
                                 $"--gamedata \"{config.GameDataPath}\" " +
                                 (config.Headless ? "--headless " : "") +
                                 (config.DeterministicStep ? "--step-mode " : "") +
                                 $"--ipc-pipe {_ipcPipeName}",

            EngineType.Re3 => $"-width {config.Width} -height {config.Height} " +
                              (config.Headless ? "-headless " : "") +
                              $"-windowed",

            EngineType.GTA3DE => "", // DE uses its own launcher

            EngineType.GTA3Original => $"-norandom -nointro -width {config.Width} -height {config.Height}",

            _ => ""
        };
    }

    private async Task WaitForWindowAsync(TimeSpan timeout)
    {
        var sw = Stopwatch.StartNew();
        while (sw.Elapsed < timeout)
        {
            if (_engineProcess is { HasExited: false } && _engineProcess.MainWindowHandle != nint.Zero)
            {
                _logger.LogInformation("Engine window appeared after {Ms}ms", sw.ElapsedMilliseconds);
                return;
            }
            await Task.Delay(500);
            _engineProcess?.Refresh();
        }
        _logger.LogWarning("Engine window did not appear within {Sec}s", timeout.TotalSeconds);
    }

    private GameState ReadGameStateOpenRw()
    {
        // OpenRW exposes game state via shared memory or IPC
        // In production, this reads from a memory-mapped file
        return ReadGameStateFromIpc();
    }

    private GameState ReadGameStateRe3()
    {
        // re3 with training hooks exposes state via IPC
        return ReadGameStateFromIpc();
    }

    private GameState ReadGameStateFromIpc()
    {
        // Phase 5.4: Parse 22-dim game state from named IPC pipe
        // Protocol: the engine sends a JSON line with fields matching GameState properties
        // Falls back to a default state if the pipe is not connected or read fails
        if (_ipcServer == null)
            return new GameState { PlayerHealth = 100, CurrentIsland = "Portland" };

        try
        {
            if (!_ipcServer.IsConnected)
                return new GameState { PlayerHealth = 100, CurrentIsland = "Portland" };

            // Use a 4KB buffer to handle realistic JSON message sizes
            var buffer = new byte[4096];
            int bytesRead = 0;

            // Non-blocking peek: only read if data is available
            if (_ipcServer.InBufferSize == 0)
                return new GameState { PlayerHealth = 100, CurrentIsland = "Portland" };

            int toRead = Math.Min(buffer.Length, _ipcServer.InBufferSize);
            bytesRead = _ipcServer.Read(buffer, 0, toRead);
            if (bytesRead == 0)
                return new GameState { PlayerHealth = 100, CurrentIsland = "Portland" };

            // Find the end of the first complete JSON object (terminated by '}' or newline)
            var raw = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
            // Trim to the last '}' to handle any trailing data or partial messages
            int jsonEnd = raw.LastIndexOf('}');
            if (jsonEnd < 0)
                return new GameState { PlayerHealth = 100, CurrentIsland = "Portland" };

            var json = raw[..(jsonEnd + 1)];
            var doc = System.Text.Json.JsonDocument.Parse(json);
            var root = doc.RootElement;

            return new GameState
            {
                PlayerX          = root.TryGetProperty("x",        out var px)  ? px.GetSingle()  : 0f,
                PlayerY          = root.TryGetProperty("y",        out var py)  ? py.GetSingle()  : 0f,
                PlayerZ          = root.TryGetProperty("z",        out var pz)  ? pz.GetSingle()  : 0f,
                PlayerHeading    = root.TryGetProperty("heading",  out var ph)  ? ph.GetSingle()  : 0f,
                PlayerHealth     = root.TryGetProperty("health",   out var hlt) ? hlt.GetSingle() : 100f,
                PlayerArmor      = root.TryGetProperty("armor",    out var arm) ? arm.GetSingle() : 0f,
                PlayerMoney      = root.TryGetProperty("money",    out var mon) ? mon.GetInt32()  : 0,
                WantedLevel      = root.TryGetProperty("wanted",   out var wnt) ? wnt.GetInt32()  : 0,
                CurrentWeapon    = root.TryGetProperty("weapon",   out var wpn) ? wpn.GetInt32()  : 0,
                InVehicle        = root.TryGetProperty("inVehicle",out var inv) && inv.GetBoolean(),
                VehicleHealth    = root.TryGetProperty("vehHealth",out var vh)  ? vh.GetSingle()  : 0f,
                VehicleSpeed     = root.TryGetProperty("vehSpeed", out var vs)  ? vs.GetSingle()  : 0f,
                VelocityX        = root.TryGetProperty("velX",     out var vx)  ? vx.GetSingle()  : 0f,
                VelocityY        = root.TryGetProperty("velY",     out var vy)  ? vy.GetSingle()  : 0f,
                VelocityZ        = root.TryGetProperty("velZ",     out var vz2) ? vz2.GetSingle() : 0f,
                CurrentIsland    = root.TryGetProperty("island",   out var isl) ? isl.GetString() ?? "Portland" : "Portland",
                GameHour         = root.TryGetProperty("hour",     out var hr)  ? hr.GetInt32()   : 12,
                GameMinute       = root.TryGetProperty("minute",   out var mn)  ? mn.GetInt32()   : 0,
                Weather          = root.TryGetProperty("weather",  out var wx)  ? wx.GetString() ?? "Sunny" : "Sunny",
                IsDead           = root.TryGetProperty("isDead",   out var id2) && id2.GetBoolean(),
                IsArrested       = root.TryGetProperty("arrested", out var arr) && arr.GetBoolean(),
                MissionIndex     = root.TryGetProperty("mission",  out var mis) ? mis.GetInt32()  : 0,
                DistanceTraveled = root.TryGetProperty("dist",     out var dst) ? dst.GetSingle() : 0f,
            };
        }
        catch (Exception ex)
        {
            _logger.LogDebug(ex, "IPC GameState parse failed — using defaults");
            return new GameState { PlayerHealth = 100, CurrentIsland = "Portland" };
        }
    }

    [DllImport("kernel32.dll")]
    private static extern bool ReadProcessMemory(nint hProcess, nint lpBaseAddress,
        byte[] lpBuffer, int dwSize, out int lpNumberOfBytesRead);

    [DllImport("kernel32.dll")]
    private static extern nint OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

    [DllImport("kernel32.dll")]
    private static extern bool CloseHandle(nint hObject);

    private const int PROCESS_VM_READ = 0x0010;

    private GameState ReadGameStateProcessMemory()
    {
        if (_engineProcess is null or { HasExited: true })
            return new GameState();

        var state = new GameState();
        nint hProcess = nint.Zero;

        try
        {
            hProcess = OpenProcess(PROCESS_VM_READ, false, _engineProcess.Id);
            if (hProcess == nint.Zero) return state;

            // GTA3 memory addresses (these are for the original PC version)
            // DE uses different addresses due to UE4 wrapper
            var baseAddr = _engineProcess.MainModule?.BaseAddress ?? nint.Zero;

            // Read player position (CWorld::Players[0].m_pPed->GetPosition())
            // These offsets are approximate and engine-version-specific
            state.PlayerHealth = ReadFloat(hProcess, baseAddr + 0x94AD28);
            state.PlayerArmor = ReadFloat(hProcess, baseAddr + 0x94AD2C);
            state.PlayerMoney = ReadInt32(hProcess, baseAddr + 0x94ADD8);
            state.WantedLevel = ReadInt32(hProcess, baseAddr + 0x94ADC0);

            // Position from CMatrix
            var pedPtr = ReadIntPtr(hProcess, baseAddr + 0x94AD28 - 0x18);
            if (pedPtr != nint.Zero)
            {
                var matrixPtr = ReadIntPtr(hProcess, pedPtr + 0x14);
                if (matrixPtr != nint.Zero)
                {
                    state.PlayerX = ReadFloat(hProcess, matrixPtr + 0x30);
                    state.PlayerY = ReadFloat(hProcess, matrixPtr + 0x34);
                    state.PlayerZ = ReadFloat(hProcess, matrixPtr + 0x38);
                }
            }
        }
        catch (Exception ex)
        {
            _logger.LogDebug(ex, "ReadProcessMemory failed");
        }
        finally
        {
            if (hProcess != nint.Zero) CloseHandle(hProcess);
        }

        return state;
    }

    private static float ReadFloat(nint hProcess, nint address)
    {
        var buffer = new byte[4];
        ReadProcessMemory(hProcess, address, buffer, 4, out _);
        return BitConverter.ToSingle(buffer, 0);
    }

    private static int ReadInt32(nint hProcess, nint address)
    {
        var buffer = new byte[4];
        ReadProcessMemory(hProcess, address, buffer, 4, out _);
        return BitConverter.ToInt32(buffer, 0);
    }

    private static nint ReadIntPtr(nint hProcess, nint address)
    {
        var buffer = new byte[nint.Size];
        ReadProcessMemory(hProcess, address, buffer, nint.Size, out _);
        return nint.Size == 8 ? (nint)BitConverter.ToInt64(buffer, 0) : (nint)BitConverter.ToInt32(buffer, 0);
    }

    private static string? FindExecutable(string name, string[] searchPaths)
    {
        // Check explicit paths
        foreach (var path in searchPaths)
        {
            if (File.Exists(path)) return path;
        }

        // Check PATH
        var pathEnv = Environment.GetEnvironmentVariable("PATH") ?? "";
        foreach (var dir in pathEnv.Split(Path.PathSeparator))
        {
            var fullPath = Path.Combine(dir, $"{name}.exe");
            if (File.Exists(fullPath)) return fullPath;
        }

        return null;
    }

    private async Task SendIpcCommandAsync(string command)
    {
        // In production, sends command via named pipe to the engine
        _logger.LogDebug("IPC command: {Command}", command);
        await Task.CompletedTask;
    }

    #endregion

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;

        Stop();
        _ipcServer?.Dispose();
        _engineProcess?.Dispose();

        _logger.LogInformation("OpenRwEngineBridge disposed");
    }
}
