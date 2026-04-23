using System.Collections.ObjectModel;
using System.IO;
using System.Windows.Media;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using GTAngel.Interop;
using GTAngel.Services;

namespace GTAngel.ViewModels;

/// <summary>
/// Game viewport view model with full UE engine process management.
/// Translated from: GameActivity UE4 game surface management.
///
/// On Android:
///   GameActivity.onCreate() loads libUE4.so via NativeActivity
///   NativeCalls provides JNI bridge for Java ↔ C++ communication
///   SurfaceView provides the rendering surface
///   Touch/sensor events forwarded via NativeCalls.HandleCustomTouchEvent
///
/// On Windows (WPF):
///   UEProcessManager launches the game .exe as a child process
///   Named pipe IPC replaces the JNI NativeCalls bridge
///   UEWindowHost embeds the game HWND via Win32 SetParent
///   Keyboard/mouse events forwarded via the embedded HWND
///
/// Key JNI translations:
///   NativeCalls.CallNativeToEmbedded → UEProcessManager.SendCommandAsync
///   NativeCalls.KeepAwake → NativeInterop.KeepAwake (SetThreadExecutionState)
///   NativeCalls.AllowSleep → NativeInterop.AllowSleep
///   NativeCalls.WebViewVisible → UEProcessManager.SetWebViewVisibleAsync
///   NativeCalls.SetNamedObject → UEProcessManager.SetNamedObjectAsync
///   NativeCalls.ForwardNotification → UEProcessManager.ForwardNotificationAsync
/// </summary>
public partial class GameViewModel : ObservableObject
{
    private readonly ILogger<GameViewModel> _logger;
    private readonly AppStateService _state;
    private readonly AudioService _audio;
    private readonly FileSystemService _fileSystem;
    private readonly UEProcessManager _processManager;
    private readonly UEAssetPipeline _assetPipeline;

    [ObservableProperty]
    private bool _isGameNotRunning = true;

    [ObservableProperty]
    private bool _isGameLoading;

    [ObservableProperty]
    private string _engineStatusText = "Ready to launch";

    [ObservableProperty]
    private string _loadingStatusText = "Initializing engine...";

    [ObservableProperty]
    private string _fpsText = "";

    [ObservableProperty]
    private string _engineStateText = "Idle";

    [ObservableProperty]
    private Brush _engineStateColor = Brushes.Gray;

    [ObservableProperty]
    private bool _hasDiscoveredProjects;

    [ObservableProperty]
    private string? _selectedProjectPath;

    public ObservableCollection<DiscoveredUEProject> DiscoveredProjects { get; } = new();

    /// <summary>Fires when the engine window HWND is ready for embedding</summary>
    public event EventHandler<IntPtr>? EngineWindowReady;

    /// <summary>Fires when the engine window is detached (process exited)</summary>
    public event EventHandler? EngineDetached;

    public GameViewModel(
        ILogger<GameViewModel> logger,
        AppStateService state,
        AudioService audio,
        FileSystemService fileSystem)
    {
        _logger = logger;
        _state = state;
        _audio = audio;
        _fileSystem = fileSystem;

        _processManager = new UEProcessManager(
            App.Services.GetRequiredService<ILogger<UEProcessManager>>());

        _assetPipeline = new UEAssetPipeline(
            App.Services.GetRequiredService<ILogger<UEAssetPipeline>>(),
            fileSystem.GameDataPath);

        // Subscribe to engine events
        _processManager.EngineWindowReady += OnEngineWindowReady;
        _processManager.EngineExited += OnEngineExited;
        _processManager.MessageReceived += OnEngineMessage;
    }

    /// <summary>
    /// Discover local UE projects that could be launched.
    /// Searches UnrealEngineCog and common project directories.
    /// </summary>
    public void DiscoverProjects()
    {
        DiscoveredProjects.Clear();

        // Check the standard game data path first
        var gamePaths = _fileSystem.GetAllGameExecutablePaths();
        foreach (var path in gamePaths)
        {
            if (File.Exists(path))
            {
                DiscoveredProjects.Add(new DiscoveredUEProject
                {
                    Name = $"GTA3DE ({Path.GetFileName(path)})",
                    ProjectFilePath = path,
                    RootPath = Path.GetDirectoryName(path)!,
                    HasBinaries = true,
                    HasContent = true,
                    EngineVersion = "UE4"
                });
            }
        }

        // Discover UE projects on the system
        var projects = _assetPipeline.DiscoverLocalProjects();
        foreach (var project in projects)
        {
            DiscoveredProjects.Add(project);
        }

        HasDiscoveredProjects = DiscoveredProjects.Count > 0;

        if (HasDiscoveredProjects)
        {
            EngineStatusText = $"Found {DiscoveredProjects.Count} UE project(s). Select one or click Launch.";
        }
        else
        {
            EngineStatusText = "No UE projects found. Place game files in GameData or build from UnrealEngineCog.";
        }

        _logger.LogInformation("Discovered {Count} UE projects", DiscoveredProjects.Count);
    }

    /// <summary>
    /// Select a discovered UE project to launch.
    /// </summary>
    [RelayCommand]
    private void SelectProject(DiscoveredUEProject project)
    {
        if (project.HasBinaries)
        {
            // Find the executable in the project's Binaries/Win64 directory
            var binDir = Path.Combine(project.RootPath, "Binaries", "Win64");
            var exes = Directory.Exists(binDir)
                ? Directory.GetFiles(binDir, "*-Win64-Shipping.exe")
                    .Concat(Directory.GetFiles(binDir, "*.exe"))
                    .ToArray()
                : Array.Empty<string>();

            if (exes.Length > 0)
            {
                SelectedProjectPath = exes[0];
                EngineStatusText = $"Selected: {project.Name} ({Path.GetFileName(SelectedProjectPath)})";
            }
            else
            {
                SelectedProjectPath = project.ProjectFilePath;
                EngineStatusText = $"Selected: {project.Name} (needs build)";
            }
        }
        else
        {
            SelectedProjectPath = project.ProjectFilePath;
            EngineStatusText = $"Selected: {project.Name} — needs compilation from source";
        }

        _logger.LogInformation("Selected project: {Name} at {Path}", project.Name, SelectedProjectPath);
    }

    /// <summary>
    /// Launch the game engine.
    /// Replaces: NativeActivity loading libUE4.so and starting the UE4 game loop.
    ///
    /// Android flow:
    ///   1. NativeActivity.onCreate() → System.loadLibrary("UE4")
    ///   2. ANativeActivity_onCreate (C entry point)
    ///   3. FAndroidAppEntry::PlatformInit()
    ///   4. FEngineLoop::Init() → FEngineLoop::Tick()
    ///
    /// Windows flow:
    ///   1. UEProcessManager.StartAsync() launches the .exe
    ///   2. Waits for the game window to appear
    ///   3. Embeds the window via UEWindowHost.EmbedEngineWindow()
    ///   4. Establishes named pipe IPC
    /// </summary>
    [RelayCommand]
    private async Task LaunchGameAsync()
    {
        _logger.LogInformation("Launching game engine");
        IsGameLoading = true;
        IsGameNotRunning = false;
        LoadingStatusText = "Starting engine process...";
        EngineStateText = "Starting";
        EngineStateColor = Brushes.Yellow;

        try
        {
            // Replaces: NativeCalls.KeepAwake("GameActivity", true)
            NativeInterop.KeepAwake();

            // Determine the executable path
            var gamePath = SelectedProjectPath ?? _fileSystem.GetGameExecutablePath();

            if (string.IsNullOrEmpty(gamePath) || !File.Exists(gamePath))
            {
                // Check if we have an APKM to extract assets from
                var apkmPath = FindApkmFile();
                if (apkmPath != null)
                {
                    LoadingStatusText = "Extracting game assets from APKM...";
                    var extractProgress = new Progress<double>(p =>
                        LoadingStatusText = $"Extracting assets: {p:P0}");
                    await _assetPipeline.ExtractApkmAssetsAsync(apkmPath, extractProgress);

                    // Verify assets after extraction
                    var verification = _assetPipeline.VerifyAssets();
                    if (verification.NeedsRecooking)
                    {
                        LoadingStatusText = "Assets extracted (Android format). Windows re-cooking required.";
                        _logger.LogWarning("Extracted assets are Android-cooked. Need Windows re-cooking.");
                    }

                    gamePath = _fileSystem.GetGameExecutablePath();
                }

                if (!File.Exists(gamePath))
                {
                    _logger.LogWarning("Game executable not found at {Path}", gamePath);
                    EngineStatusText = $"Game executable not found.\nExpected: {gamePath}\n\n" +
                        "To run the game, either:\n" +
                        "1. Place a Windows-compiled UE4 game in the GameData folder\n" +
                        "2. Build the Gameface project from UnrealEngineCog\n" +
                        "3. Select a discovered UE project above";
                    IsGameLoading = false;
                    IsGameNotRunning = true;
                    EngineStateText = "No executable";
                    EngineStateColor = Brushes.Red;
                    NativeInterop.AllowSleep();
                    return;
                }
            }

            // Build UE4-equivalent command line
            // Replaces: UE4CommandLine.txt content
            var commandLine = BuildUECommandLine();

            LoadingStatusText = "Launching engine process...";
            var success = await _processManager.StartAsync(gamePath, commandLine);

            if (success)
            {
                LoadingStatusText = "Engine running";
                EngineStateText = "Running";
                EngineStateColor = Brushes.LimeGreen;
                _logger.LogInformation("Engine launched successfully, PID: {Pid}", _processManager.ProcessId);
            }
            else
            {
                LoadingStatusText = "Failed to start engine";
                EngineStatusText = "Engine failed to start. Check logs for details.";
                EngineStateText = "Error";
                EngineStateColor = Brushes.Red;
                IsGameNotRunning = true;
                NativeInterop.AllowSleep();
            }

            IsGameLoading = false;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to launch game");
            EngineStatusText = $"Launch error: {ex.Message}";
            IsGameLoading = false;
            IsGameNotRunning = true;
            EngineStateText = "Error";
            EngineStateColor = Brushes.Red;
            NativeInterop.AllowSleep();
        }
    }

    /// <summary>
    /// Stop the engine process.
    /// Replaces: GameActivity.onDestroy() → native cleanup
    /// </summary>
    public async Task StopEngineAsync()
    {
        if (_processManager.State == UEProcessState.Running ||
            _processManager.State == UEProcessState.Paused)
        {
            EngineStateText = "Stopping";
            EngineStateColor = Brushes.Orange;
            await _processManager.StopAsync();
        }

        NativeInterop.AllowSleep();
        IsGameNotRunning = true;
        EngineStateText = "Stopped";
        EngineStateColor = Brushes.Gray;
    }

    /// <summary>
    /// Build UE4-equivalent command line arguments.
    /// Replaces: UE4CommandLine.txt and nativeSetConfigRulesVariables
    /// </summary>
    private string BuildUECommandLine()
    {
        var args = new List<string>();

        // Replaces: ../../../Gameface/Gameface.uproject from UE4CommandLine.txt
        // On Windows, the project path is relative to the executable
        args.Add("-Windowed");
        args.Add("-ResX=1920");
        args.Add("-ResY=1080");

        // Replaces: nativeSetConfigRulesVariables
        if (NativeInterop.GetMetaDataBoolean("com.epicgames.ue4.GameActivity.bIsShippingBuild"))
        {
            args.Add("-Shipping");
        }

        // Replaces: nativeSupportsVulkan check
        if (NativeInterop.GetMetaDataBoolean("com.epicgames.ue4.GameActivity.bSupportsVulkan"))
        {
            args.Add("-vulkan");
        }

        return string.Join(" ", args);
    }

    /// <summary>
    /// Find an APKM file in common locations for asset extraction.
    /// </summary>
    private string? FindApkmFile()
    {
        var searchPaths = new[]
        {
            Path.Combine(_fileSystem.AppDataPath, "gta3.apkm"),
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "Downloads", "gta3.apkm"),
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "Downloads", "gta3..apkm"),
        };

        return searchPaths.FirstOrDefault(File.Exists);
    }

    /// <summary>
    /// Engine window is ready — forward to the view for embedding.
    /// Replaces: SurfaceView.surfaceCreated() callback
    /// </summary>
    private void OnEngineWindowReady(object? sender, IntPtr hwnd)
    {
        _logger.LogInformation("Engine window ready: HWND {Hwnd}", hwnd);
        EngineWindowReady?.Invoke(this, hwnd);
    }

    /// <summary>
    /// Engine process exited.
    /// Replaces: NativeActivity.onDestroy() / process crash handling
    /// </summary>
    private void OnEngineExited(object? sender, int exitCode)
    {
        _logger.LogInformation("Engine exited with code {Code}", exitCode);

        IsGameNotRunning = true;
        EngineStateText = exitCode == 0 ? "Exited" : $"Crashed (code {exitCode})";
        EngineStateColor = exitCode == 0 ? Brushes.Gray : Brushes.Red;
        EngineStatusText = exitCode == 0
            ? "Game exited normally."
            : $"Game crashed with exit code {exitCode}. Check logs.";
        FpsText = "";

        NativeInterop.AllowSleep();
        EngineDetached?.Invoke(this, EventArgs.Empty);
    }

    /// <summary>
    /// Handle messages from the engine via named pipe IPC.
    /// Replaces: NativeCalls.CallNativeToEmbedded callbacks
    /// </summary>
    private void OnEngineMessage(object? sender, UEMessage message)
    {
        switch (message.Action)
        {
            case "FPS":
                FpsText = $"{message.Value} FPS";
                break;

            case "WebViewVisible":
                // Replaces: NativeCalls.WebViewVisible(visible)
                // The engine wants to show/hide the browser overlay
                _logger.LogDebug("Engine requests WebView visible: {Value}", message.Value);
                break;

            case "AllowBackButton":
                // Replaces: NativeCalls.AllowJavaBackButtonEvent(allow)
                _logger.LogDebug("Engine allows back button: {Value}", message.Value);
                break;

            case "ForwardNotification":
                // Replaces: NativeCalls.ForwardNotification(json)
                _logger.LogDebug("Engine notification: {Json}", message.Json);
                break;

            case "RouteServiceIntent":
                // Replaces: NativeCalls.RouteServiceIntent(action, data)
                _logger.LogDebug("Engine service intent: {Key} → {Value}", message.Key, message.Value);
                break;

            case "Log":
                _logger.LogDebug("[UE] {Message}", message.Value);
                break;

            default:
                _logger.LogDebug("Unhandled engine message: {Action}", message.Action);
                break;
        }
    }
}
