using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media.Imaging;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using GTA3DE.Wpf.Services;

namespace GTA3DE.Wpf.ViewModels;

/// <summary>
/// ViewModel for the Game Runtime tab.
/// Manages the live GTA3 connection, frame capture preview,
/// proprioceptive state display, and RL training loop.
/// </summary>
public partial class GameRuntimeViewModel : ObservableObject
{
    private readonly GameRuntimeService _runtime = new();

    // ========== Connection State ==========
    [ObservableProperty] private bool _isConnected;
    [ObservableProperty] private string _connectionStatus = "Disconnected";
    [ObservableProperty] private string _gameExecutablePath = string.Empty;

    // ========== Frame Capture ==========
    [ObservableProperty] private BitmapSource? _latestFrame;
    [ObservableProperty] private long _frameCount;
    [ObservableProperty] private string _captureStatus = "Idle";
    [ObservableProperty] private bool _isCapturing;
    [ObservableProperty] private int _captureWidth = 768;
    [ObservableProperty] private int _captureHeight = 768;
    [ObservableProperty] private int _targetFps = 15;

    // ========== Proprioceptive State ==========
    [ObservableProperty] private float _playerX;
    [ObservableProperty] private float _playerY;
    [ObservableProperty] private float _playerZ;
    [ObservableProperty] private float _playerHeading;
    [ObservableProperty] private float _playerHealth = 100f;
    [ObservableProperty] private float _playerArmor;
    [ObservableProperty] private int _wantedLevel;
    [ObservableProperty] private int _money;
    [ObservableProperty] private bool _isInVehicle;
    [ObservableProperty] private float _vehicleSpeed;
    [ObservableProperty] private float _velocityMagnitude;

    // ========== RL Training ==========
    [ObservableProperty] private bool _isRLRunning;
    [ObservableProperty] private int _rlEpisode;
    [ObservableProperty] private int _rlStep;
    [ObservableProperty] private float _rlEpisodeReward;
    [ObservableProperty] private float _rlBestReward;
    [ObservableProperty] private string _rlStatus = "Idle";
    [ObservableProperty] private int _actionSpaceSize = 15;

    // ========== Log ==========
    public ObservableCollection<string> RuntimeLog { get; } = new();

    // ========== Action Names ==========
    public static readonly string[] ActionNames =
    {
        "No-op", "Forward", "Stop Forward", "Backward", "Stop Backward",
        "Left", "Stop Left", "Right", "Stop Right", "Jump",
        "Sprint On", "Sprint Off", "Enter/Exit Vehicle", "Next Weapon", "Camera Toggle"
    };

    public GameRuntimeViewModel()
    {
        _runtime.OnFrameCaptured += OnFrameCaptured;
        _runtime.OnStateExtracted += OnStateExtracted;
        _runtime.OnLogMessage += msg => Application.Current.Dispatcher.Invoke(() =>
        {
            RuntimeLog.Insert(0, $"[{DateTime.Now:HH:mm:ss}] {msg}");
            if (RuntimeLog.Count > 500) RuntimeLog.RemoveAt(RuntimeLog.Count - 1);
        });
        _runtime.OnConnectionChanged += connected => Application.Current.Dispatcher.Invoke(() =>
        {
            IsConnected = connected;
            ConnectionStatus = connected ? "Connected to GTA3" : "Disconnected";
        });

        // Auto-detect common GTA3 paths
        AutoDetectGamePath();
    }

    private void AutoDetectGamePath()
    {
        string[] commonPaths =
        {
            @"C:\Program Files (x86)\Steam\steamapps\common\Grand Theft Auto 3\gta3.exe",
            @"C:\Program Files (x86)\Rockstar Games\Grand Theft Auto III\gta3.exe",
            @"C:\Games\GTA3\gta3.exe",
            @"D:\Games\GTA3\gta3.exe",
            @"E:\Games\GTA3\gta3.exe",
            @"C:\Program Files (x86)\GOG Galaxy\Games\Grand Theft Auto 3\gta3.exe",
        };

        foreach (var path in commonPaths)
        {
            if (File.Exists(path))
            {
                GameExecutablePath = path;
                Log($"Auto-detected GTA3 at: {path}");
                return;
            }
        }
    }

    // ========== Commands ==========

    [RelayCommand]
    private async Task ConnectToGameAsync()
    {
        ConnectionStatus = "Connecting...";
        _runtime.CaptureWidth = CaptureWidth;
        _runtime.CaptureHeight = CaptureHeight;
        _runtime.TargetFPS = TargetFps;

        var connected = await _runtime.ConnectOrLaunchAsync(
            string.IsNullOrEmpty(GameExecutablePath) ? null : GameExecutablePath);

        if (!connected)
        {
            ConnectionStatus = "Not found — running in simulation mode";
            Log("GTA3 not found. The system will operate in simulation mode with synthetic data.");
        }
    }

    [RelayCommand]
    private void StartCapture()
    {
        _runtime.CaptureWidth = CaptureWidth;
        _runtime.CaptureHeight = CaptureHeight;
        _runtime.TargetFPS = TargetFps;
        _runtime.StartFrameCapture();
        _runtime.StartStateExtraction();
        IsCapturing = true;
        CaptureStatus = $"Capturing at {TargetFps} FPS ({CaptureWidth}x{CaptureHeight})";
    }

    [RelayCommand]
    private void StopCapture()
    {
        _runtime.StopFrameCapture();
        _runtime.StopStateExtraction();
        IsCapturing = false;
        CaptureStatus = "Stopped";
    }

    [RelayCommand]
    private async Task StartRLTrainingAsync()
    {
        if (IsRLRunning) return;
        IsRLRunning = true;
        RlStatus = "Training...";
        Log("RL training loop started");

        // Ensure capture is running
        if (!IsCapturing) StartCapture();

        try
        {
            while (IsRLRunning)
            {
                RlEpisode++;
                RlEpisodeReward = 0;
                RlStep = 0;

                // Episode loop
                bool done = false;
                while (!done && IsRLRunning && RlStep < 1000)
                {
                    RlStep++;

                    // Select action (epsilon-greedy with random exploration)
                    int action = SelectAction();

                    // Execute step
                    var result = await _runtime.StepAsync(action);

                    RlEpisodeReward += result.Reward;
                    done = result.Done;

                    // Update display every 10 steps
                    if (RlStep % 10 == 0)
                    {
                        RlStatus = $"Episode {RlEpisode} | Step {RlStep} | Reward: {RlEpisodeReward:F2}";
                    }
                }

                if (RlEpisodeReward > RlBestReward)
                    RlBestReward = RlEpisodeReward;

                Log($"Episode {RlEpisode} complete: Steps={RlStep}, Reward={RlEpisodeReward:F2}, Best={RlBestReward:F2}");
            }
        }
        catch (Exception ex)
        {
            Log($"RL training error: {ex.Message}");
        }

        IsRLRunning = false;
        RlStatus = "Stopped";
    }

    [RelayCommand]
    private void StopRLTraining()
    {
        IsRLRunning = false;
        RlStatus = "Stopping...";
    }

    [RelayCommand]
    private void SendTestAction(string? actionIndexStr)
    {
        if (int.TryParse(actionIndexStr, out int idx))
        {
            _runtime.ExecuteAction(idx);
            Log($"Sent test action: {(idx < ActionNames.Length ? ActionNames[idx] : $"#{idx}")}");
        }
    }

    // ========== Action Selection ==========

    private readonly Random _rng = new();
    private double _epsilon = 0.3; // Exploration rate

    private int SelectAction()
    {
        // Epsilon-greedy: random action with probability epsilon
        if (_rng.NextDouble() < _epsilon)
            return _rng.Next(ActionSpaceSize);

        // Otherwise, use a simple heuristic (placeholder for DTE reservoir output)
        // In production, this would query the ESN reservoir for the best action
        return 1; // Default: move forward
    }

    // ========== Event Handlers ==========

    private void OnFrameCaptured(FrameData frame)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            LatestFrame = frame.Preview;
            FrameCount = frame.FrameNumber;
        });
    }

    private void OnStateExtracted(ProprioceptiveState state)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            PlayerX = state.PositionX;
            PlayerY = state.PositionY;
            PlayerZ = state.PositionZ;
            PlayerHeading = state.Heading;
            PlayerHealth = state.Health;
            PlayerArmor = state.Armor;
            WantedLevel = state.WantedLevel;
            Money = state.Money;
            IsInVehicle = state.IsInVehicle;
            VehicleSpeed = state.VehicleSpeed;
            VelocityMagnitude = MathF.Sqrt(
                state.VelocityX * state.VelocityX +
                state.VelocityY * state.VelocityY +
                state.VelocityZ * state.VelocityZ);
        });
    }

    private void Log(string message)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            RuntimeLog.Insert(0, $"[{DateTime.Now:HH:mm:ss}] {message}");
            if (RuntimeLog.Count > 500) RuntimeLog.RemoveAt(RuntimeLog.Count - 1);
        });
    }
}
