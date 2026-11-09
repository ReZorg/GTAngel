using System;
using System.IO;
using System.Threading.Tasks;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Orchestrates a closed-loop autonomous training session:
///   1. Configure or auto-detect an external GTA3 engine (re3 by default).
///   2. Route GameState from the engine into the DTE/ESN training loop.
///   3. Let the training loop drive the engine through VigemControllerService input.
/// </summary>
public sealed class AutonomousTrainingService : IDisposable
{
    private readonly ILogger<AutonomousTrainingService> _logger;
    private readonly OpenRwEngineBridge _engine;
    private readonly VigemControllerService _controller;
    private readonly DteTrainingLoop _trainingLoop;

    private Action<DteTrainingState>? _stateUpdatedHandler;
    private Action<DteEpisodeResult>? _episodeCompleteHandler;
    private bool _disposed;

    public AutonomousTrainingOptions Options { get; }
    public AutonomousTrainingState State { get; } = new();

    public AutonomousTrainingService(
        ILogger<AutonomousTrainingService> logger,
        OpenRwEngineBridge engine,
        VigemControllerService controller,
        DteTrainingLoop trainingLoop,
        IConfiguration configuration)
    {
        _logger = logger;
        _engine = engine;
        _controller = controller;
        _trainingLoop = trainingLoop;
        Options = ReadOptions(configuration);
    }

    private static AutonomousTrainingOptions ReadOptions(IConfiguration configuration)
    {
        var section = configuration.GetSection("AutonomousTraining");
        var options = new AutonomousTrainingOptions();

        var exe = section["Re3ExecutablePath"] ?? section["ExecutablePath"];
        if (!string.IsNullOrWhiteSpace(exe))
            options.Re3ExecutablePath = exe;

        var data = section["Re3GameDataPath"] ?? section["GameDataPath"];
        if (!string.IsNullOrWhiteSpace(data))
            options.Re3GameDataPath = data;

        if (int.TryParse(section["Width"], out var width))
            options.Width = width;
        if (int.TryParse(section["Height"], out var height))
            options.Height = height;
        if (int.TryParse(section["TargetFps"], out var targetFps))
            options.TargetFps = targetFps;
        if (bool.TryParse(section["Headless"], out var headless))
            options.Headless = headless;
        if (bool.TryParse(section["DeterministicStep"], out var deterministicStep))
            options.DeterministicStep = deterministicStep;
        if (int.TryParse(section["MaxStepsPerEpisode"], out var maxSteps))
            options.MaxStepsPerEpisode = maxSteps;
        if (int.TryParse(section["MaxEpisodes"], out var maxEpisodes))
            options.MaxEpisodes = maxEpisodes;
        if (Enum.TryParse<DteTrainingMode>(section["TrainingMode"], true, out var mode))
            options.TrainingMode = mode;
        if (bool.TryParse(section["InitializeDtePipeline"], out var initPipeline))
            options.InitializeDtePipeline = initPipeline;
        if (bool.TryParse(section["AutoStart"], out var autoStart))
            options.AutoStart = autoStart;

        return options;
    }

    /// <summary>
    /// Start an autonomous training session.
    /// </summary>
    /// <returns>True if the session started successfully.</returns>
    public async Task<bool> StartAsync()
    {
        if (State.IsRunning)
        {
            _logger.LogWarning("Autonomous training is already running");
            return false;
        }

        try
        {
            // Configure engine rendering/resolution before detection/launch.
            _engine.RenderWidth = Options.Width;
            _engine.RenderHeight = Options.Height;
            _engine.TargetFps = Options.TargetFps;
            _engine.HeadlessMode = Options.Headless;
            _engine.DeterministicStepping = Options.DeterministicStep;

            // Use a configured executable path when available; otherwise auto-detect.
            if (!string.IsNullOrWhiteSpace(Options.Re3ExecutablePath) && File.Exists(Options.Re3ExecutablePath))
            {
                _engine.SetEnginePath(Options.Re3ExecutablePath, OpenRwEngineBridge.EngineType.Re3);

                var dataPath = !string.IsNullOrWhiteSpace(Options.Re3GameDataPath) && Directory.Exists(Options.Re3GameDataPath)
                    ? Options.Re3GameDataPath
                    : Path.GetDirectoryName(Options.Re3ExecutablePath)!;

                _engine.SetGameDataPath(dataPath);
            }
            else if (_engine.DetectedEngine == OpenRwEngineBridge.EngineType.None)
            {
                _engine.DetectEngines();
            }

            if (_engine.DetectedEngine == OpenRwEngineBridge.EngineType.None)
            {
                const string message = "No GTA3 engine could be detected or configured";
                _logger.LogWarning(message);
                State.LastError = message;
                return false;
            }

            // Prepare the controller so the DTE loop can send inputs to the engine window.
            _controller.Initialize();
            if (_engine.GameWindowHandle != nint.Zero)
                _controller.SetTargetWindow(_engine.GameWindowHandle);

            _trainingLoop.Config.TargetFps = Options.TargetFps;
            _trainingLoop.Config.MaxStepsPerEpisode = Options.MaxStepsPerEpisode;
            _trainingLoop.Config.MaxEpisodes = Options.MaxEpisodes;
            _trainingLoop.Config.TrainingMode = Options.TrainingMode;

            if (Options.InitializeDtePipeline && !_trainingLoop.State.IsInitialized)
            {
                _logger.LogInformation("Initializing DTE training pipeline...");
                await _trainingLoop.InitializeAsync();
            }

            AttachEventHandlers();

            _trainingLoop.Start();

            if (!_trainingLoop.State.IsRunning)
            {
                DetachEventHandlers();
                const string message = "DTE training loop could not start";
                _logger.LogWarning(message);
                State.LastError = message;
                return false;
            }

            State.IsRunning = true;
            State.EngineType = _engine.DetectedEngine.ToString();
            State.EnginePath = _engine.EnginePath;
            State.EngineWindowHandle = _engine.GameWindowHandle;
            State.LastError = null;

            _logger.LogInformation(
                "Autonomous training started: engine={Engine}, path={Path}",
                State.EngineType,
                State.EnginePath);

            return true;
        }
        catch (Exception ex)
        {
            State.LastError = ex.Message;
            _logger.LogError(ex, "Failed to start autonomous training");
            return false;
        }
    }

    /// <summary>
    /// Stop the autonomous training session and shut down the engine.
    /// </summary>
    public async Task<bool> StopAsync()
    {
        if (!State.IsRunning)
        {
            _logger.LogWarning("Autonomous training is not running");
            return false;
        }

        DetachEventHandlers();

        try
        {
            await _trainingLoop.StopAsync();
        }
        catch (Exception ex)
        {
            _logger.LogDebug(ex, "Training loop stop raised an exception");
        }

        try
        {
            _engine.Stop();
        }
        catch (Exception ex)
        {
            _logger.LogDebug(ex, "Engine stop raised an exception");
        }

        State.IsRunning = false;
        _logger.LogInformation("Autonomous training stopped");
        return true;
    }

    private void AttachEventHandlers()
    {
        DetachEventHandlers();

        _stateUpdatedHandler = state =>
        {
            if (state is null) return;
            State.TotalSteps = state.TotalSteps;
            State.StepsPerSecond = state.StepsPerSecond;
        };

        _episodeCompleteHandler = result =>
        {
            if (result is null) return;
            State.CurrentEpisode = result.EpisodeId;
            State.LastReward = result.TotalReward;
            State.TotalSteps = result.Steps;
        };

        _trainingLoop.OnStateUpdated += _stateUpdatedHandler;
        _trainingLoop.OnEpisodeComplete += _episodeCompleteHandler;
    }

    private void DetachEventHandlers()
    {
        if (_stateUpdatedHandler != null)
            _trainingLoop.OnStateUpdated -= _stateUpdatedHandler;
        if (_episodeCompleteHandler != null)
            _trainingLoop.OnEpisodeComplete -= _episodeCompleteHandler;

        _stateUpdatedHandler = null;
        _episodeCompleteHandler = null;
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;

        if (!State.IsRunning) return;

        try
        {
            // Avoid blocking the caller's synchronization context.
            Task.Run(async () => await StopAsync()).Wait(TimeSpan.FromSeconds(5));
        }
        catch (Exception ex)
        {
            _logger.LogDebug(ex, "Dispose while stopping autonomous training");
        }
    }
}

/// <summary>
/// Options for autonomous training, bound from the "AutonomousTraining" configuration section.
/// </summary>
public class AutonomousTrainingOptions
{
    public string? Re3ExecutablePath { get; set; }
    public string? Re3GameDataPath { get; set; }
    public int Width { get; set; } = 768;
    public int Height { get; set; } = 768;
    public int TargetFps { get; set; } = 30;
    public bool Headless { get; set; }
    public bool DeterministicStep { get; set; } = true;
    public int MaxStepsPerEpisode { get; set; } = 2000;
    public int MaxEpisodes { get; set; } = 0;
    public DteTrainingMode TrainingMode { get; set; } = DteTrainingMode.Hybrid;
    public bool InitializeDtePipeline { get; set; } = true;
    public bool AutoStart { get; set; }
}

/// <summary>
/// Observable runtime state for an autonomous training session.
/// </summary>
public class AutonomousTrainingState
{
    public bool IsRunning { get; set; }
    public string? EngineType { get; set; }
    public string? EnginePath { get; set; }
    public nint EngineWindowHandle { get; set; }
    public int CurrentEpisode { get; set; }
    public long TotalSteps { get; set; }
    public double StepsPerSecond { get; set; }
    public float LastReward { get; set; }
    public string? LastError { get; set; }
}
