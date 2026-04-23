using System;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace GTAngel.Services;

/// <summary>
/// Manages the GTA3 game runtime, frame capture pipeline, virtual controller input,
/// and proprioceptive state extraction for Deep Tree Echo RL training.
/// 
/// Architecture:
///   [GTA3 Process] → [DXGI/BitBlt Frame Capture] → [Vision Pipeline] → [DTE Reservoir]
///   [DTE Reservoir] → [Action Selection] → [Virtual Controller / SendInput] → [GTA3 Process]
///   [GTA3 Process] → [Memory Reader] → [Proprioceptive State] → [DTE Sensory Integration]
/// </summary>
public class GameRuntimeService : IDisposable
{
    // ========== P/Invoke for Win32 APIs ==========
    [DllImport("user32.dll")] static extern IntPtr FindWindow(string? lpClassName, string? lpWindowName);
    [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] static extern IntPtr GetDC(IntPtr hWnd);
    [DllImport("user32.dll")] static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);
    [DllImport("user32.dll")] static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] static extern bool MoveWindow(IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);
    [DllImport("user32.dll")] static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll")] static extern uint SendInput(uint nInputs, INPUT[] pInputs, int cbSize);
    [DllImport("user32.dll")] static extern bool PostMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);
    [DllImport("gdi32.dll")] static extern IntPtr CreateCompatibleDC(IntPtr hdc);
    [DllImport("gdi32.dll")] static extern IntPtr CreateCompatibleBitmap(IntPtr hdc, int nWidth, int nHeight);
    [DllImport("gdi32.dll")] static extern IntPtr SelectObject(IntPtr hdc, IntPtr hgdiobj);
    [DllImport("gdi32.dll")] static extern bool BitBlt(IntPtr hdcDest, int xDest, int yDest, int wDest, int hDest, IntPtr hdcSrc, int xSrc, int ySrc, uint dwRop);
    [DllImport("gdi32.dll")] static extern bool DeleteObject(IntPtr hObject);
    [DllImport("gdi32.dll")] static extern bool DeleteDC(IntPtr hdc);
    [DllImport("kernel32.dll")] static extern IntPtr OpenProcess(uint dwDesiredAccess, bool bInheritHandle, int dwProcessId);
    [DllImport("kernel32.dll")] static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int dwSize, out int lpNumberOfBytesRead);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr hObject);
    [DllImport("user32.dll")] static extern short GetAsyncKeyState(int vKey);

    const uint SRCCOPY = 0x00CC0020;
    const uint PROCESS_VM_READ = 0x0010;
    const uint PROCESS_QUERY_INFORMATION = 0x0400;
    const uint WM_KEYDOWN = 0x0100;
    const uint WM_KEYUP = 0x0101;

    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left, Top, Right, Bottom; }

    [StructLayout(LayoutKind.Sequential)]
    public struct INPUT
    {
        public uint type;
        public INPUTUNION u;
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct INPUTUNION
    {
        [FieldOffset(0)] public KEYBDINPUT ki;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct KEYBDINPUT
    {
        public ushort wVk;
        public ushort wScan;
        public uint dwFlags;
        public uint time;
        public IntPtr dwExtraInfo;
    }

    // ========== Configuration ==========
    public int CaptureWidth { get; set; } = 768;
    public int CaptureHeight { get; set; } = 768;
    public int TargetFPS { get; set; } = 15;  // 15 FPS is sufficient for RL training
    public string GameWindowTitle { get; set; } = "GTA3";
    public string[] AlternateWindowTitles { get; set; } = { "GTA: III", "Grand Theft Auto III", "re3", "GTA3 Definitive Edition" };

    // ========== State ==========
    private Process? _gameProcess;
    private IntPtr _gameWindowHandle;
    private IntPtr _processHandle;
    private CancellationTokenSource? _captureCts;
    private CancellationTokenSource? _stateCts;
    private bool _isRunning;
    private bool _disposed;

    // ========== Frame Buffer ==========
    private readonly ConcurrentQueue<FrameData> _frameBuffer = new();
    private const int MaxFrameBufferSize = 5;

    // ========== Events ==========
    public event Action<FrameData>? OnFrameCaptured;
    public event Action<ProprioceptiveState>? OnStateExtracted;
    public event Action<string>? OnLogMessage;
    public event Action<bool>? OnConnectionChanged;

    // ========== GTA3 Memory Addresses (Original PC v1.0) ==========
    // These are well-documented by the GTA modding community
    private static class GTA3Addresses
    {
        // Player position (CVector - 3 floats)
        public static readonly IntPtr PlayerPosX = new(0x00B73424);
        public static readonly IntPtr PlayerPosY = new(0x00B73428);
        public static readonly IntPtr PlayerPosZ = new(0x00B7342C);

        // Player heading (float, radians)
        public static readonly IntPtr PlayerHeading = new(0x00B73478);

        // Player health (float)
        public static readonly IntPtr PlayerHealth = new(0x00B73488);

        // Player armor (float)
        public static readonly IntPtr PlayerArmor = new(0x00B7348C);

        // Current weapon ID (int32)
        public static readonly IntPtr CurrentWeapon = new(0x00B73490);

        // Vehicle pointer (0 = on foot)
        public static readonly IntPtr InVehiclePtr = new(0x00B73404);

        // Vehicle speed (float, when in vehicle)
        public static readonly IntPtr VehicleSpeed = new(0x00B6F07C);

        // Wanted level (int32)
        public static readonly IntPtr WantedLevel = new(0x00B7349C);

        // Game timer (uint32, milliseconds)
        public static readonly IntPtr GameTimer = new(0x00885B48);

        // Camera position
        public static readonly IntPtr CameraPosX = new(0x00B6F028);
        public static readonly IntPtr CameraPosY = new(0x00B6F02C);
        public static readonly IntPtr CameraPosZ = new(0x00B6F030);

        // Money (int32)
        public static readonly IntPtr Money = new(0x00B7CE50);
    }

    // ========== Key Mappings for GTA3 ==========
    public static class GTA3Keys
    {
        // Movement
        public const ushort Forward = 0x57;   // W
        public const ushort Backward = 0x53;  // S
        public const ushort Left = 0x41;      // A
        public const ushort Right = 0x44;     // D
        public const ushort Jump = 0x20;      // Space
        public const ushort Sprint = 0x10;    // Shift

        // Combat
        public const ushort Attack = 0x01;    // Left Mouse (via SendInput)
        public const ushort AimLock = 0x02;   // Right Mouse
        public const ushort NextWeapon = 0x45; // E
        public const ushort PrevWeapon = 0x51; // Q

        // Vehicle
        public const ushort Accelerate = 0x57; // W
        public const ushort Brake = 0x53;      // S
        public const ushort SteerLeft = 0x41;  // A
        public const ushort SteerRight = 0x44; // D
        public const ushort EnterVehicle = 0x46; // F
        public const ushort Horn = 0x48;       // H
        public const ushort Handbrake = 0x20;  // Space

        // Camera
        public const ushort CameraMode = 0x56; // V
        public const ushort LookBehind = 0x43; // C
    }

    // ========== Game Launch ==========

    /// <summary>
    /// Attempts to find an already-running GTA3 instance or launches a new one.
    /// Supports re3, original GTA3, and Definitive Edition.
    /// </summary>
    public async Task<bool> ConnectOrLaunchAsync(string? executablePath = null)
    {
        Log("Searching for running GTA3 instance...");

        // Try to find existing window
        _gameWindowHandle = FindGameWindow();
        if (_gameWindowHandle != IntPtr.Zero)
        {
            _gameProcess = GetProcessFromWindow();
            if (_gameProcess != null)
            {
                Log($"Found running GTA3: PID {_gameProcess.Id}, Window: {_gameWindowHandle}");
                await ConfigureGameWindowAsync();
                OpenProcessHandle();
                _isRunning = true;
                OnConnectionChanged?.Invoke(true);
                return true;
            }
        }

        // Try to launch if path provided
        if (!string.IsNullOrEmpty(executablePath) && File.Exists(executablePath))
        {
            Log($"Launching GTA3 from: {executablePath}");
            var startInfo = new ProcessStartInfo
            {
                FileName = executablePath,
                WorkingDirectory = Path.GetDirectoryName(executablePath),
                UseShellExecute = true
            };

            _gameProcess = Process.Start(startInfo);
            if (_gameProcess != null)
            {
                Log("Waiting for game window to appear...");
                for (int i = 0; i < 60; i++) // Wait up to 60 seconds
                {
                    await Task.Delay(1000);
                    _gameWindowHandle = FindGameWindow();
                    if (_gameWindowHandle != IntPtr.Zero) break;
                }

                if (_gameWindowHandle != IntPtr.Zero)
                {
                    await ConfigureGameWindowAsync();
                    OpenProcessHandle();
                    _isRunning = true;
                    OnConnectionChanged?.Invoke(true);
                    Log("GTA3 launched and connected successfully.");
                    return true;
                }
            }
        }

        Log("Could not find or launch GTA3. Entering simulation mode.");
        _isRunning = false;
        OnConnectionChanged?.Invoke(false);
        return false;
    }

    private IntPtr FindGameWindow()
    {
        var handle = FindWindow(null, GameWindowTitle);
        if (handle != IntPtr.Zero) return handle;

        foreach (var title in AlternateWindowTitles)
        {
            handle = FindWindow(null, title);
            if (handle != IntPtr.Zero)
            {
                GameWindowTitle = title;
                return handle;
            }
        }
        return IntPtr.Zero;
    }

    private Process? GetProcessFromWindow()
    {
        foreach (var proc in Process.GetProcesses())
        {
            try
            {
                if (proc.MainWindowHandle == _gameWindowHandle)
                    return proc;
            }
            catch { }
        }
        return null;
    }

    private void OpenProcessHandle()
    {
        if (_gameProcess != null)
        {
            _processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, false, _gameProcess.Id);
            if (_processHandle == IntPtr.Zero)
                Log("Warning: Could not open process for memory reading. Proprioceptive feedback will be simulated.");
        }
    }

    private async Task ConfigureGameWindowAsync()
    {
        // Set window to 768x768 windowed mode for ML training
        Log($"Configuring game window to {CaptureWidth}x{CaptureHeight}...");
        await Task.Delay(500);

        ShowWindow(_gameWindowHandle, 1); // SW_SHOWNORMAL
        MoveWindow(_gameWindowHandle, 0, 0, CaptureWidth + 16, CaptureHeight + 39, true); // Account for window borders
        await Task.Delay(200);
    }

    // ========== Frame Capture Pipeline ==========

    /// <summary>
    /// Starts the continuous frame capture loop using BitBlt (GDI).
    /// For higher performance, DXGI Desktop Duplication should be used via SharpDX.
    /// This GDI implementation works universally with all rendering backends.
    /// </summary>
    public void StartFrameCapture()
    {
        if (_captureCts != null) return;
        _captureCts = new CancellationTokenSource();
        var token = _captureCts.Token;

        Task.Run(async () =>
        {
            Log($"Frame capture started at {TargetFPS} FPS target ({CaptureWidth}x{CaptureHeight})");
            var frameInterval = TimeSpan.FromMilliseconds(1000.0 / TargetFPS);
            var sw = Stopwatch.StartNew();
            long frameCount = 0;
            var lastFpsReport = sw.ElapsedMilliseconds;

            while (!token.IsCancellationRequested)
            {
                var frameStart = sw.ElapsedMilliseconds;

                try
                {
                    var frame = CaptureFrame();
                    if (frame != null)
                    {
                        // Keep buffer small for low-latency RL
                        while (_frameBuffer.Count >= MaxFrameBufferSize)
                            _frameBuffer.TryDequeue(out _);

                        _frameBuffer.Enqueue(frame);
                        OnFrameCaptured?.Invoke(frame);
                        frameCount++;
                    }

                    // FPS reporting every 5 seconds
                    if (sw.ElapsedMilliseconds - lastFpsReport > 5000)
                    {
                        var elapsed = (sw.ElapsedMilliseconds - lastFpsReport) / 1000.0;
                        Log($"Frame capture: {frameCount / elapsed:F1} FPS, buffer: {_frameBuffer.Count}");
                        frameCount = 0;
                        lastFpsReport = sw.ElapsedMilliseconds;
                    }
                }
                catch (Exception ex)
                {
                    Log($"Frame capture error: {ex.Message}");
                }

                // Frame rate limiting
                var elapsed2 = sw.ElapsedMilliseconds - frameStart;
                var sleepMs = (int)(frameInterval.TotalMilliseconds - elapsed2);
                if (sleepMs > 0) await Task.Delay(sleepMs, token);
            }

            Log("Frame capture stopped.");
        }, token);
    }

    public void StopFrameCapture()
    {
        _captureCts?.Cancel();
        _captureCts = null;
    }

    private FrameData? CaptureFrame()
    {
        if (_gameWindowHandle == IntPtr.Zero) return null;

        GetClientRect(_gameWindowHandle, out var clientRect);
        int srcWidth = clientRect.Right - clientRect.Left;
        int srcHeight = clientRect.Bottom - clientRect.Top;
        if (srcWidth <= 0 || srcHeight <= 0) return null;

        IntPtr hdcWindow = GetDC(_gameWindowHandle);
        IntPtr hdcMemDC = CreateCompatibleDC(hdcWindow);
        IntPtr hBitmap = CreateCompatibleBitmap(hdcWindow, srcWidth, srcHeight);
        IntPtr hOld = SelectObject(hdcMemDC, hBitmap);

        BitBlt(hdcMemDC, 0, 0, srcWidth, srcHeight, hdcWindow, 0, 0, SRCCOPY);

        SelectObject(hdcMemDC, hOld);

        // Convert to managed bitmap and resize to target dimensions
        using var bitmap = System.Drawing.Image.FromHbitmap(hBitmap);
        using var resized = new Bitmap(CaptureWidth, CaptureHeight);
        using (var g = Graphics.FromImage(resized))
        {
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.Bilinear;
            g.DrawImage(bitmap, 0, 0, CaptureWidth, CaptureHeight);
        }

        // Extract raw pixel data for ML pipeline (RGB float array)
        var bmpData = resized.LockBits(
            new Rectangle(0, 0, CaptureWidth, CaptureHeight),
            ImageLockMode.ReadOnly,
            System.Drawing.Imaging.PixelFormat.Format24bppRgb);

        int byteCount = bmpData.Stride * CaptureHeight;
        byte[] rawPixels = new byte[byteCount];
        Marshal.Copy(bmpData.Scan0, rawPixels, 0, byteCount);
        resized.UnlockBits(bmpData);

        // Convert to normalized float array [0,1] for neural network input
        float[] normalizedPixels = new float[CaptureWidth * CaptureHeight * 3];
        int pixelIdx = 0;
        for (int y = 0; y < CaptureHeight; y++)
        {
            int rowOffset = y * bmpData.Stride;
            for (int x = 0; x < CaptureWidth; x++)
            {
                int srcIdx = rowOffset + x * 3;
                // BGR to RGB conversion + normalization
                normalizedPixels[pixelIdx++] = rawPixels[srcIdx + 2] / 255f; // R
                normalizedPixels[pixelIdx++] = rawPixels[srcIdx + 1] / 255f; // G
                normalizedPixels[pixelIdx++] = rawPixels[srcIdx + 0] / 255f; // B
            }
        }

        // Create WPF-compatible preview image
        var previewSource = CreateBitmapSource(resized);

        // Cleanup GDI objects
        DeleteObject(hBitmap);
        DeleteDC(hdcMemDC);
        ReleaseDC(_gameWindowHandle, hdcWindow);

        return new FrameData
        {
            Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
            Width = CaptureWidth,
            Height = CaptureHeight,
            NormalizedPixels = normalizedPixels,
            Preview = previewSource,
            FrameNumber = Interlocked.Increment(ref _frameCounter)
        };
    }

    private long _frameCounter;

    private static BitmapSource CreateBitmapSource(Bitmap bitmap)
    {
        var bmpData = bitmap.LockBits(
            new Rectangle(0, 0, bitmap.Width, bitmap.Height),
            ImageLockMode.ReadOnly,
            System.Drawing.Imaging.PixelFormat.Format24bppRgb);

        var source = BitmapSource.Create(
            bitmap.Width, bitmap.Height,
            96, 96,
            PixelFormats.Bgr24,
            null,
            bmpData.Scan0,
            bmpData.Stride * bitmap.Height,
            bmpData.Stride);

        bitmap.UnlockBits(bmpData);
        source.Freeze(); // Make thread-safe for WPF binding
        return source;
    }

    // ========== Proprioceptive State Extraction ==========

    /// <summary>
    /// Starts continuous memory reading to extract the player's proprioceptive state.
    /// Runs at 30 Hz (faster than frame capture) for responsive feedback.
    /// </summary>
    public void StartStateExtraction()
    {
        if (_stateCts != null) return;
        _stateCts = new CancellationTokenSource();
        var token = _stateCts.Token;

        Task.Run(async () =>
        {
            Log("Proprioceptive state extraction started (30 Hz)");
            while (!token.IsCancellationRequested)
            {
                try
                {
                    var state = ExtractProprioceptiveState();
                    OnStateExtracted?.Invoke(state);
                }
                catch (Exception ex)
                {
                    Log($"State extraction error: {ex.Message}");
                }
                await Task.Delay(33, token); // ~30 Hz
            }
            Log("State extraction stopped.");
        }, token);
    }

    public void StopStateExtraction()
    {
        _stateCts?.Cancel();
        _stateCts = null;
    }

    private ProprioceptiveState ExtractProprioceptiveState()
    {
        var state = new ProprioceptiveState { Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() };

        if (_processHandle == IntPtr.Zero || _gameProcess == null || _gameProcess.HasExited)
        {
            // Return simulated state when not connected
            state.IsSimulated = true;
            return state;
        }

        state.PositionX = ReadFloat(GTA3Addresses.PlayerPosX);
        state.PositionY = ReadFloat(GTA3Addresses.PlayerPosY);
        state.PositionZ = ReadFloat(GTA3Addresses.PlayerPosZ);
        state.Heading = ReadFloat(GTA3Addresses.PlayerHeading);
        state.Health = ReadFloat(GTA3Addresses.PlayerHealth);
        state.Armor = ReadFloat(GTA3Addresses.PlayerArmor);
        state.CurrentWeapon = ReadInt32(GTA3Addresses.CurrentWeapon);
        state.IsInVehicle = ReadInt32(GTA3Addresses.InVehiclePtr) != 0;
        state.VehicleSpeed = state.IsInVehicle ? ReadFloat(GTA3Addresses.VehicleSpeed) : 0f;
        state.WantedLevel = ReadInt32(GTA3Addresses.WantedLevel);
        state.GameTime = ReadUInt32(GTA3Addresses.GameTimer);
        state.Money = ReadInt32(GTA3Addresses.Money);
        state.CameraX = ReadFloat(GTA3Addresses.CameraPosX);
        state.CameraY = ReadFloat(GTA3Addresses.CameraPosY);
        state.CameraZ = ReadFloat(GTA3Addresses.CameraPosZ);

        // Compute velocity from position delta
        if (_lastState != null && !_lastState.IsSimulated)
        {
            float dt = (state.Timestamp - _lastState.Timestamp) / 1000f;
            if (dt > 0)
            {
                state.VelocityX = (state.PositionX - _lastState.PositionX) / dt;
                state.VelocityY = (state.PositionY - _lastState.PositionY) / dt;
                state.VelocityZ = (state.PositionZ - _lastState.PositionZ) / dt;
            }
        }
        _lastState = state;

        return state;
    }

    private ProprioceptiveState? _lastState;

    private float ReadFloat(IntPtr address)
    {
        byte[] buffer = new byte[4];
        ReadProcessMemory(_processHandle, address, buffer, 4, out _);
        return BitConverter.ToSingle(buffer, 0);
    }

    private int ReadInt32(IntPtr address)
    {
        byte[] buffer = new byte[4];
        ReadProcessMemory(_processHandle, address, buffer, 4, out _);
        return BitConverter.ToInt32(buffer, 0);
    }

    private uint ReadUInt32(IntPtr address)
    {
        byte[] buffer = new byte[4];
        ReadProcessMemory(_processHandle, address, buffer, 4, out _);
        return BitConverter.ToUInt32(buffer, 0);
    }

    // ========== Action Injection (Virtual Controller) ==========

    /// <summary>
    /// Executes a discrete action in the game by sending keyboard/mouse inputs.
    /// Maps DTE cognitive action indices to GTA3 controls.
    /// </summary>
    public void ExecuteAction(int actionIndex)
    {
        if (_gameWindowHandle == IntPtr.Zero) return;
        SetForegroundWindow(_gameWindowHandle);

        switch (actionIndex)
        {
            case 0: /* No-op */ break;
            case 1: SendKey(GTA3Keys.Forward, true); break;    // Move forward
            case 2: SendKey(GTA3Keys.Forward, false); break;   // Stop forward
            case 3: SendKey(GTA3Keys.Backward, true); break;   // Move backward
            case 4: SendKey(GTA3Keys.Backward, false); break;  // Stop backward
            case 5: SendKey(GTA3Keys.Left, true); break;       // Turn left
            case 6: SendKey(GTA3Keys.Left, false); break;      // Stop left
            case 7: SendKey(GTA3Keys.Right, true); break;      // Turn right
            case 8: SendKey(GTA3Keys.Right, false); break;     // Stop right
            case 9: SendKey(GTA3Keys.Jump, true); SendKey(GTA3Keys.Jump, false); break; // Jump
            case 10: SendKey(GTA3Keys.Sprint, true); break;    // Sprint on
            case 11: SendKey(GTA3Keys.Sprint, false); break;   // Sprint off
            case 12: SendKey(GTA3Keys.EnterVehicle, true); SendKey(GTA3Keys.EnterVehicle, false); break; // Enter/Exit vehicle
            case 13: SendKey(GTA3Keys.NextWeapon, true); SendKey(GTA3Keys.NextWeapon, false); break; // Next weapon
            case 14: SendKey(GTA3Keys.CameraMode, true); SendKey(GTA3Keys.CameraMode, false); break; // Camera toggle
        }
    }

    /// <summary>
    /// Executes a continuous action vector (for policy gradient / actor-critic methods).
    /// Maps float values to analog-like input via timed key presses.
    /// [0] = forward/backward (-1 to 1)
    /// [1] = left/right (-1 to 1)
    /// [2] = attack (0 or 1)
    /// [3] = jump (0 or 1)
    /// [4] = sprint (0 or 1)
    /// </summary>
    public void ExecuteContinuousAction(float[] actionVector)
    {
        if (_gameWindowHandle == IntPtr.Zero || actionVector.Length < 5) return;

        // Forward/backward
        SendKey(GTA3Keys.Forward, actionVector[0] > 0.3f);
        SendKey(GTA3Keys.Backward, actionVector[0] < -0.3f);

        // Left/right
        SendKey(GTA3Keys.Left, actionVector[1] < -0.3f);
        SendKey(GTA3Keys.Right, actionVector[1] > 0.3f);

        // Attack
        if (actionVector[2] > 0.5f)
        {
            SendKey(0x01, true);  // Left mouse down
            SendKey(0x01, false); // Left mouse up
        }

        // Jump
        if (actionVector[3] > 0.5f)
        {
            SendKey(GTA3Keys.Jump, true);
            SendKey(GTA3Keys.Jump, false);
        }

        // Sprint
        SendKey(GTA3Keys.Sprint, actionVector[4] > 0.5f);
    }

    private void SendKey(ushort vkCode, bool keyDown)
    {
        PostMessage(_gameWindowHandle, keyDown ? WM_KEYDOWN : WM_KEYUP, (IntPtr)vkCode, IntPtr.Zero);
    }

    // ========== RL Environment Interface ==========

    /// <summary>
    /// Gets the latest frame from the buffer (non-blocking).
    /// Returns null if no frame is available.
    /// </summary>
    public FrameData? GetLatestFrame()
    {
        FrameData? latest = null;
        while (_frameBuffer.TryDequeue(out var frame))
            latest = frame;
        return latest;
    }

    /// <summary>
    /// Performs one RL step: execute action, capture frame, extract state, compute reward.
    /// This is the main interface for the DTE training loop.
    /// </summary>
    public async Task<RLStepResult> StepAsync(int action)
    {
        var prevState = _lastState ?? new ProprioceptiveState { IsSimulated = true };

        // Execute action
        ExecuteAction(action);

        // Wait for game to process the action (1-2 frames)
        await Task.Delay(1000 / TargetFPS);

        // Capture new observation
        var frame = GetLatestFrame();
        var newState = ExtractProprioceptiveState();

        // Compute reward
        float reward = ComputeReward(prevState, newState, action);
        bool done = newState.Health <= 0;

        return new RLStepResult
        {
            Frame = frame,
            State = newState,
            Reward = reward,
            Done = done,
            Action = action
        };
    }

    /// <summary>
    /// Computes the reward signal based on state transitions.
    /// Reward shaping for exploration, survival, and mission progress.
    /// </summary>
    private float ComputeReward(ProprioceptiveState prev, ProprioceptiveState curr, int action)
    {
        float reward = 0f;

        if (curr.IsSimulated) return 0f;

        // Survival reward (small positive for staying alive)
        reward += 0.01f;

        // Health penalty
        float healthDelta = curr.Health - prev.Health;
        if (healthDelta < 0) reward += healthDelta * 0.1f;

        // Movement reward (encourage exploration)
        float dx = curr.PositionX - prev.PositionX;
        float dy = curr.PositionY - prev.PositionY;
        float distance = MathF.Sqrt(dx * dx + dy * dy);
        reward += MathF.Min(distance * 0.01f, 0.1f);

        // Money reward
        int moneyDelta = curr.Money - prev.Money;
        if (moneyDelta > 0) reward += MathF.Min(moneyDelta * 0.001f, 1.0f);

        // Wanted level penalty (discourages excessive chaos during early training)
        if (curr.WantedLevel > prev.WantedLevel)
            reward -= 0.2f * curr.WantedLevel;

        // Death penalty
        if (curr.Health <= 0) reward -= 5.0f;

        return reward;
    }

    // ========== Utility ==========

    public bool IsConnected => _isRunning && _gameProcess != null && !_gameProcess.HasExited;

    private void Log(string message) => OnLogMessage?.Invoke(message);

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;

        StopFrameCapture();
        StopStateExtraction();

        if (_processHandle != IntPtr.Zero)
            CloseHandle(_processHandle);

        GC.SuppressFinalize(this);
    }
}

// ========== Data Models ==========

/// <summary>
/// A single captured frame from the game, ready for the vision pipeline.
/// </summary>
public class FrameData
{
    public long Timestamp { get; set; }
    public long FrameNumber { get; set; }
    public int Width { get; set; }
    public int Height { get; set; }

    /// <summary>
    /// Normalized RGB pixel data [0,1] in row-major order.
    /// Shape: [Height, Width, 3] flattened to [Height * Width * 3]
    /// </summary>
    public float[] NormalizedPixels { get; set; } = Array.Empty<float>();

    /// <summary>
    /// WPF-compatible preview image for the dashboard.
    /// </summary>
    public BitmapSource? Preview { get; set; }
}

/// <summary>
/// Proprioceptive state extracted from GTA3 process memory.
/// Maps to the DTE SensoryInputIntegration's Proprioceptive modality.
/// </summary>
public class ProprioceptiveState
{
    public long Timestamp { get; set; }
    public bool IsSimulated { get; set; }

    // Position
    public float PositionX { get; set; }
    public float PositionY { get; set; }
    public float PositionZ { get; set; }
    public float Heading { get; set; }

    // Velocity (computed from position delta)
    public float VelocityX { get; set; }
    public float VelocityY { get; set; }
    public float VelocityZ { get; set; }

    // Health & Status
    public float Health { get; set; } = 100f;
    public float Armor { get; set; }
    public int CurrentWeapon { get; set; }
    public int WantedLevel { get; set; }
    public int Money { get; set; }

    // Vehicle
    public bool IsInVehicle { get; set; }
    public float VehicleSpeed { get; set; }

    // Camera
    public float CameraX { get; set; }
    public float CameraY { get; set; }
    public float CameraZ { get; set; }

    // Game state
    public uint GameTime { get; set; }

    /// <summary>
    /// Converts the proprioceptive state to a normalized float vector
    /// suitable for the ESN reservoir input.
    /// </summary>
    public float[] ToFeatureVector()
    {
        return new float[]
        {
            PositionX / 4000f,       // Normalize to GTA3 map bounds
            PositionY / 4000f,
            PositionZ / 100f,
            Heading / (2f * MathF.PI),
            VelocityX / 50f,
            VelocityY / 50f,
            VelocityZ / 20f,
            Health / 200f,
            Armor / 100f,
            CurrentWeapon / 15f,     // 15 weapon types
            WantedLevel / 6f,
            IsInVehicle ? 1f : 0f,
            VehicleSpeed / 200f,
            Money / 1000000f
        };
    }
}

/// <summary>
/// Result of a single RL step in the game environment.
/// </summary>
public class RLStepResult
{
    public FrameData? Frame { get; set; }
    public ProprioceptiveState State { get; set; } = new();
    public float Reward { get; set; }
    public bool Done { get; set; }
    public int Action { get; set; }
}
