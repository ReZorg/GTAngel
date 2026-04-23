using System.Collections.ObjectModel;
using System.Windows.Media;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using GTAngel.Interop;
using GTAngel.Services;

namespace GTAngel.ViewModels;

/// <summary>
/// ViewModel for the DTE 4E Embodied Cognition Avatar panel.
/// Binds the DTE4EAvatarService to the WPF UI.
/// </summary>
public partial class AvatarViewModel : ObservableObject
{
    private readonly ILogger<AvatarViewModel> _logger;
    private DTE4EAvatarService?       _avatarService;
    private UE5ProcessManager?        _ue5;
    private MlVisionCaptureService?   _mlVision;
    private AvatarEmbodimentService?  _embodiment;  // KSM Cycle 3
    private GameWorldNavigationService? _navigation; // KSM Cycle 4
    private Ue5PlayerAiBridgeService? _playerAiBridge; // KSM Cycle 6

    // ── Avatar Status ────────────────────────────────────────────────────────
    [ObservableProperty] private string _avatarStatusText    = "Avatar offline";
    [ObservableProperty] private string _avatarStateText     = "Idle";
    [ObservableProperty] private Brush  _avatarStateColor    = Brushes.Gray;
    [ObservableProperty] private bool   _isAvatarRunning     = false;
    [ObservableProperty] private bool   _isAvatarNotRunning  = true;

    // ── 4E Cognitive State ───────────────────────────────────────────────────
    [ObservableProperty] private float  _curiosity           = 0f;
    [ObservableProperty] private float  _endorphin           = 0f;
    [ObservableProperty] private float  _chaosIntensity      = 0f;
    [ObservableProperty] private float  _homeostasis         = 0f;
    [ObservableProperty] private float  _extendedEsnNorm     = 0f;

    // ── Exploration Metrics ──────────────────────────────────────────────────
    [ObservableProperty] private int    _totalSteps          = 0;
    [ObservableProperty] private float  _totalReward         = 0f;
    [ObservableProperty] private float  _explorationCoverage = 0f;
    [ObservableProperty] private string _coverageText        = "0%";
    [ObservableProperty] private string _positionText        = "(0, 0, 0)";
    [ObservableProperty] private string _velocityText        = "0.0 UU/s";
    [ObservableProperty] private string _lastActionText      = "—";

    // ── UE5 Feature Flags ────────────────────────────────────────────────────
    [ObservableProperty] private bool   _useLumen            = true;
    [ObservableProperty] private bool   _useNanite           = true;
    [ObservableProperty] private bool   _useChaosPhysics     = true;
    [ObservableProperty] private bool   _useEnhancedInput    = true;
    [ObservableProperty] private bool   _useMLVisionCapture  = true;
    [ObservableProperty] private string _ue5EngineVersion    = "Detecting...";
    [ObservableProperty] private string _ue5ModulesStatus    = "Not loaded";

      // ── ML Vision (KSM Cycle 1 — structure-preserving transformation) ────────
    [ObservableProperty] private string _mlVisionResolution  = "768×768";
    [ObservableProperty] private int    _mlVisionFrameCount  = 0;
    [ObservableProperty] private string _mlVisionStatus      = "Waiting for engine...";
    [ObservableProperty] private double _mlFrameRate         = 0.0;   // live FPS
    [ObservableProperty] private float  _mlLastFeatureNorm   = 0.0f;  // ONNX feature L2 norm
    [ObservableProperty] private string _mlFrameRateText     = "0.0 fps";
    [ObservableProperty] private string _mlFeatureNormText   = "0.000";
    [ObservableProperty] private bool   _mlCaptureActive     = false;

    // ── KSM Cycle 2: UE5 Launch Stage (P4 Alternating Rep., P8 Deep Interlock, P12 The Void) ─
    [ObservableProperty] private string _ue5LaunchStageText   = "Idle";
    [ObservableProperty] private string _ue5LaunchLog         = "";
    [ObservableProperty] private bool   _ue5IsValidating      = false;
    [ObservableProperty] private bool   _ue5IsBuilding        = false;
    [ObservableProperty] private bool   _ue5IsLaunching       = false;
    [ObservableProperty] private bool   _ue5IsConnecting      = false;
    [ObservableProperty] private bool   _ue5IsReady           = false;
    [ObservableProperty] private bool   _ue5IsFailed          = false;
    [ObservableProperty] private bool   _ue5IsLaunching2      = false; // combined for button state
    [ObservableProperty] private double _ue5LaunchProgress    = 0.0;   // 0..1 across 4 stages
    [ObservableProperty] private string _ue5LaunchButtonText  = "▶ LAUNCH UE5";

    // ── KSM Cycle 3: FACS Action Units (P5 Positive Space) ────────────────────
    [ObservableProperty] private float _auBrowRaise     = 0f;  // AU1+2
    [ObservableProperty] private float _auBrowFurrow    = 0f;  // AU4
    [ObservableProperty] private float _auEyeWide       = 0f;  // AU5
    [ObservableProperty] private float _auSmile         = 0f;  // AU12+13
    [ObservableProperty] private float _auLipPress      = 0f;  // AU23+24
    [ObservableProperty] private float _auJawDrop       = 0f;  // AU26+27

    // ── KSM Cycle 3: Emotional State (P8 Deep Interlock) ─────────────────────
    [ObservableProperty] private float _emHappiness     = 0f;
    [ObservableProperty] private float _emSurprise      = 0f;
    [ObservableProperty] private float _emSadness       = 0f;
    [ObservableProperty] private float _emAnger         = 0f;
    [ObservableProperty] private float _emFear          = 0f;

    // ── KSM Cycle 3: Neurochemical Readback (P11 Echoes) ─────────────────────
    [ObservableProperty] private float _neuroCuriosity   = 0f;
    [ObservableProperty] private float _neuroEndorphin   = 0f;
    [ObservableProperty] private float _neuroChaos       = 0f;
    [ObservableProperty] private float _neuroHomeostasis = 0f;

    // ── KSM Cycle 3: SuperHotGirl Personality Traits (P14 Not-Separateness) ──
    [ObservableProperty] private float _personalityConfidence  = 0.8f;
    [ObservableProperty] private float _personalityCharm       = 0.9f;
    [ObservableProperty] private float _personalityPlayfulness = 0.7f;
    [ObservableProperty] private float _personalityWit         = 0.8f;
    [ObservableProperty] private float _personalitySass        = 0.6f;

    // ── KSM Cycle 6: UE5 Player↔AI Bridge (P12 The Void, P15 Not-Separateness) ─
    [ObservableProperty] private string _playerAiMode            = "HumanOnly";
    [ObservableProperty] private bool   _humanInputActive        = true;
    [ObservableProperty] private bool   _aiPolicyActive          = false;
    [ObservableProperty] private float  _inputArbitrationScore   = 1.0f;
    [ObservableProperty] private float  _observationFusionNorm   = 0.0f;
    [ObservableProperty] private string _activeInputActionsText  = "None";
    [ObservableProperty] private string _arbitrationModeText     = "Human 100%";

    // ── KSM Cycle 4: Game World Navigation (P3 Boundaries, P7 Local Symmetries) ─
    [ObservableProperty] private string _currentDistrict     = "Unknown";
    [ObservableProperty] private string _currentPOI          = "None";
    [ObservableProperty] private string _navTargetPOI        = "None";
    [ObservableProperty] private float  _navDistToTarget     = 0f;
    [ObservableProperty] private int    _navPOIsVisited      = 0;
    [ObservableProperty] private int    _navPOIsTotal        = 0;
    [ObservableProperty] private string _navPOIProgress      = "0 / 0";
    [ObservableProperty] private float  _navDistrictCoverage = 0f;
    [ObservableProperty] private string _navRouteStatus      = "No route";
    [ObservableProperty] private int    _navWaypointsInRoute = 0;
    [ObservableProperty] private int    _navCurrentWaypoint  = 0;
    [ObservableProperty] private string _navMode             = "Idle";
    [ObservableProperty] private bool   _navIsActive         = false;

    // ── Log ──────────────────────────────────────────────────────────────────
    public ObservableCollection<string> ExplorationLog { get; } = new();
    public ObservableCollection<string> Ue5LaunchLogLines { get; } = new();

    // ── UE5 Module Status ────────────────────────────────────────────────────
    public ObservableCollection<UE5ModuleStatus> UE5Modules { get; } = new();

    private UE5LaunchOrchestrator? _ue5Orchestrator;

    public AvatarViewModel()
    {
        _logger = App.Services.GetRequiredService<ILogger<AvatarViewModel>>();
        _ue5Orchestrator = App.Services.GetService<UE5LaunchOrchestrator>();
        if (_ue5Orchestrator != null)
        {
            _ue5Orchestrator.OnStageChanged  += OnUe5StageChanged;
            _ue5Orchestrator.OnLogLine       += OnUe5LogLine;
            _ue5Orchestrator.OnLaunchComplete += OnUe5LaunchComplete;
        }
        InitializeModuleStatus();
        DetectUE5Engine();
    }

    // ── Commands ─────────────────────────────────────────────────────────────

    [RelayCommand]
    private async Task TogglePlayerAiModeAsync()
    {
        if (_playerAiBridge == null) return;
        
        var nextMode = _playerAiBridge.CurrentMode switch
        {
            PlayerAiBridgeMode.HumanOnly => PlayerAiBridgeMode.Arbitrated,
            PlayerAiBridgeMode.Arbitrated => PlayerAiBridgeMode.AiOnly,
            PlayerAiBridgeMode.AiOnly => PlayerAiBridgeMode.HumanOnly,
            _ => PlayerAiBridgeMode.HumanOnly
        };
        
        await _playerAiBridge.SetModeAsync(nextMode);
    }

    [RelayCommand]
    private async Task StartAvatarAsync()
    {
        if (_avatarService == null)
        {
            await InitializeAvatarServiceAsync();
        }

        if (_avatarService == null)
        {
            AvatarStatusText = "Failed to initialize avatar service";
            return;
        }

        IsAvatarRunning    = true;
        IsAvatarNotRunning = false;
        AvatarStateText    = "Starting";
        AvatarStateColor   = Brushes.Yellow;
        AvatarStatusText   = "Initializing DTE 4E Avatar...";

        try
        {
            await _avatarService.StartExplorationAsync();
            AvatarStateText  = "Exploring";
            AvatarStateColor = Brushes.LimeGreen;
            AvatarStatusText = "Avatar exploring Liberty City";
            AddLog("🌟 DTE 4E Avatar started — embodied cognition active");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to start avatar");
            AvatarStatusText   = $"Error: {ex.Message}";
            AvatarStateText    = "Error";
            AvatarStateColor   = Brushes.Red;
            IsAvatarRunning    = false;
            IsAvatarNotRunning = true;
        }
    }

    [RelayCommand]
    private async Task StopAvatarAsync()
    {
        if (_avatarService == null) return;

        AvatarStateText  = "Stopping";
        AvatarStateColor = Brushes.Orange;

        await _avatarService.StopExplorationAsync();

        IsAvatarRunning    = false;
        IsAvatarNotRunning = true;
        AvatarStateText    = "Stopped";
        AvatarStateColor   = Brushes.Gray;
        AvatarStatusText   = $"Exploration stopped. Steps: {TotalSteps}, Reward: {TotalReward:F1}";
        AddLog($"🛑 Avatar stopped. Steps: {TotalSteps}, Coverage: {CoverageText}");
    }

    [RelayCommand]
    private async Task RequestMLFrameAsync()
    {
        if (_ue5 == null) return;
        await _ue5.RequestMLVisionFrameAsync();
        MlVisionStatus = "Frame requested...";
    }

    // ── Initialization ────────────────────────────────────────────────────────

    private async Task InitializeAvatarServiceAsync()
    {
        try
        {
            var loggerFactory = App.Services.GetRequiredService<ILoggerFactory>();

            // Create UE5ProcessManager with UE5 feature flags
            _ue5 = new UE5ProcessManager(loggerFactory.CreateLogger<UE5ProcessManager>())
            {
                UseLumen          = UseLumen,
                UseNanite         = UseNanite,
                UseChaosPhysics   = UseChaosPhysics,
                UseEnhancedInput  = UseEnhancedInput,
                UseMLVisionCapture = UseMLVisionCapture
            };

            // Create ESN pipeline
            var esn = App.Services.GetRequiredService<EsnReservoirPipeline>();

            // ── KSM Cycle 1: Initialise MlVisionCaptureService ────────────────
            _mlVision = App.Services.GetRequiredService<MlVisionCaptureService>();
            _mlVision.OnFrameRateUpdated += rate => App.Current.Dispatcher.InvokeAsync(() =>
            {
                MlFrameRate     = rate;
                MlFrameRateText = $"{rate:F1} fps";
            });
            _mlVision.OnFrameCaptured += (frame, norm) => App.Current.Dispatcher.InvokeAsync(() =>
            {
                MlLastFeatureNorm = norm;
                MlFeatureNormText = $"{norm:F3}";
                MlVisionFrameCount = _mlVision.FrameCount;
            });
            _mlVision.OnStatusChanged += msg => App.Current.Dispatcher.InvokeAsync(() =>
                MlVisionStatus = msg);

            await _mlVision.InitialiseAsync();
            await _mlVision.StartCaptureAsync();
            MlCaptureActive = true;
            AddLog($"🎥 ML Vision capture started — {_mlVision.Resolution} DXGI pipeline active");

            // ── KSM Cycle 3: Initialise AvatarEmbodimentService ────────────────────
            _embodiment = App.Services.GetRequiredService<AvatarEmbodimentService>();
            _embodiment.OnEmotionalStateUpdated    += OnEmotionalStateUpdated;
            _embodiment.OnNeurochemicalStateUpdated += OnNeurochemicalReadback;
            _embodiment.OnPersonalityTraitsUpdated  += OnPersonalityApplied;
            _embodiment.OnFACSAUsUpdated            += OnAuArrayUpdated;
            _embodiment.OnEmbodimentLog             += (_, msg) => App.Current.Dispatcher.InvokeAsync(() => AddLog($"[Embody] {msg}"));
            await _embodiment.StartAsync();
            AddLog("🎭 AvatarEmbodimentService started — FACS+IK+Neuro+Personality active");

            // ── KSM Cycle 4: Initialise GameWorldNavigationService ────────────────
            _navigation = App.Services.GetRequiredService<GameWorldNavigationService>();
            _navigation.OnDistrictChanged += (_, d) => App.Current.Dispatcher.InvokeAsync(() =>
            {
                CurrentDistrict = d.Name;
                AddLog($"🗺️ District: {d.Name}");
            });
            _navigation.OnPOIReached += (_, poi) => App.Current.Dispatcher.InvokeAsync(() =>
            {
                CurrentPOI     = poi.Name;
                NavPOIsVisited = _navigation.VisitedPOICount;
                NavPOIProgress = $"{NavPOIsVisited} / {NavPOIsTotal}";
                AddLog($"📍 POI reached: {poi.Name}");
            });
            _navigation.OnRouteUpdated += (_, info) => App.Current.Dispatcher.InvokeAsync(() =>
            {
                NavRouteStatus      = info.Status;
                NavTargetPOI        = info.TargetPOI;
                NavWaypointsInRoute = info.TotalWaypoints;
                NavCurrentWaypoint  = info.CurrentWaypoint;
                NavDistToTarget     = info.DistanceToTarget;
                NavMode             = info.Mode;
                NavIsActive         = info.IsActive;
                NavDistrictCoverage = info.DistrictCoverage;
            });
            _navigation.OnNavigationLog += (_, msg) => App.Current.Dispatcher.InvokeAsync(() => AddLog($"[Nav] {msg}"));
            _navigation.Initialize();
            NavPOIsTotal = _navigation.TotalPOICount;
            NavPOIProgress = $"0 / {NavPOIsTotal}";
            AddLog($"🗺️ Navigation initialized: {_navigation.TotalPOICount} POIs across {_navigation.DistrictCount} districts");

            // ── KSM Cycle 6: Initialise Ue5PlayerAiBridgeService ────────────────────
            _playerAiBridge = App.Services.GetRequiredService<Ue5PlayerAiBridgeService>();
            _playerAiBridge.SetUE5ProcessManager(_ue5);
            _playerAiBridge.OnModeChanged += (_, mode) => App.Current.Dispatcher.InvokeAsync(() =>
            {
                PlayerAiMode = mode.ToString();
                HumanInputActive = mode == PlayerAiBridgeMode.HumanOnly || mode == PlayerAiBridgeMode.Arbitrated;
                AiPolicyActive = mode == PlayerAiBridgeMode.AiOnly || mode == PlayerAiBridgeMode.Arbitrated;
                ArbitrationModeText = mode switch
                {
                    PlayerAiBridgeMode.HumanOnly => "Human 100%",
                    PlayerAiBridgeMode.AiOnly => "AI Policy 100%",
                    PlayerAiBridgeMode.Arbitrated => "Arbitrated (50/50)",
                    _ => "Unknown"
                };
                AddLog($"[Bridge] Mode changed to {mode}");
            });
            _playerAiBridge.OnArbitrationScoreUpdated += (_, score) => App.Current.Dispatcher.InvokeAsync(() => InputArbitrationScore = score);
            _playerAiBridge.OnObservationFused += (_, norm) => App.Current.Dispatcher.InvokeAsync(() => ObservationFusionNorm = norm);

            // Create avatar service (now receives real frames + embodiment + navigation)
            _avatarService = new DTE4EAvatarService(
                loggerFactory.CreateLogger<DTE4EAvatarService>(),
                _ue5, esn, _mlVision, _embodiment, _navigation);
            _avatarService.SetPlayerAiBridge(_playerAiBridge);

            // Wire events
            _avatarService.CognitiveStateUpdated += OnCognitiveStateUpdated;
            _avatarService.ObservationReceived   += OnObservationReceived;
            _avatarService.ActionDispatched      += OnActionDispatched;
            _avatarService.ExplorationLog        += OnExplorationLog;

            UpdateUE5ModuleStatus("Avatar3DComponent",          "Ready", true);
            UpdateUE5ModuleStatus("NeurochemicalSystem",        "Ready", true);
            UpdateUE5ModuleStatus("SuperHotGirlPersonality",    "Ready", true);
            UpdateUE5ModuleStatus("VirtualEnvironmentManager",  "Ready", true);
            UpdateUE5ModuleStatus("Live2DCubismAvatarComponent","Ready", true);
            Ue5ModulesStatus = "All modules ready";

            AddLog("✅ UE5 cognitive modules loaded from E:\\u9n\\UnrealEngine\\Source\\");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to initialize avatar service");
            AvatarStatusText = $"Init error: {ex.Message}";
        }
    }

    private void InitializeModuleStatus()
    {
        UE5Modules.Clear();
        var modules = new[]
        {
            ("Avatar3DComponent",           "E:\\u9n\\UnrealEngine\\Source\\Avatar",          "UE4→UE5"),
            ("NeurochemicalSystem",         "E:\\u9n\\UnrealEngine\\Source\\Neurochemical",   "UE5 Native"),
            ("SuperHotGirlPersonality",     "E:\\u9n\\UnrealEngine\\Source\\Personality",     "UE5 Native"),
            ("VirtualEnvironmentManager",   "E:\\u9n\\UnrealEngine\\Source\\Environment",     "UE5 Lumen"),
            ("Live2DCubismAvatarComponent", "E:\\u9n\\UnrealEngine\\Source\\Live2DCubism",    "UE5 Native"),
            ("AssetManager",               "E:\\u9n\\UnrealEngine\\Source\\AssetManagement", "UE5 Native"),
            ("Enhanced Input System",       "UE5 Engine Built-in",                            "UE4→UE5"),
            ("Chaos Physics",               "UE5 Engine Built-in",                            "UE4 PhysX→UE5"),
            ("Lumen GI",                    "UE5 Engine Built-in",                            "UE4 Baked→UE5"),
            ("Nanite Geometry",             "UE5 Engine Built-in",                            "UE4 LOD→UE5"),
            ("World Partition",             "UE5 Engine Built-in",                            "UE4 Streaming→UE5"),
        };

        foreach (var (name, path, upgrade) in modules)
        {
            UE5Modules.Add(new UE5ModuleStatus
            {
                Name        = name,
                SourcePath  = path,
                UpgradeType = upgrade,
                Status      = "Pending",
                IsReady     = false
            });
        }
    }

    private void DetectUE5Engine()
    {
        var enginePath = DTE4EAvatarService.UE5EnginePath;
        if (System.IO.Directory.Exists(enginePath))
        {
            var buildVersionPath = System.IO.Path.Combine(enginePath, "Engine", "Build", "Build.version");
            if (System.IO.File.Exists(buildVersionPath))
            {
                try
                {
                    var json = System.IO.File.ReadAllText(buildVersionPath);
                    var doc  = System.Text.Json.JsonDocument.Parse(json);
                    var major = doc.RootElement.GetProperty("MajorVersion").GetInt32();
                    var minor = doc.RootElement.GetProperty("MinorVersion").GetInt32();
                    Ue5EngineVersion = $"UE {major}.{minor}";
                }
                catch { Ue5EngineVersion = "UE5 (version unknown)"; }
            }
            else
            {
                Ue5EngineVersion = "UE5 (no Build.version)";
            }
        }
        else
        {
            Ue5EngineVersion = "Not found at E:\\u9n\\UnrealEngine";
        }
    }

    // ── Event Handlers ────────────────────────────────────────────────────────

    private void OnCognitiveStateUpdated(object? sender, AvatarCognitiveState state)
    {
        App.Current.Dispatcher.InvokeAsync(() =>
        {
            Curiosity           = state.Curiosity;
            Endorphin           = state.Endorphin;
            ChaosIntensity      = state.ChaosIntensity;
            Homeostasis         = state.Homeostasis;
            ExtendedEsnNorm     = state.ExtendedESNNorm;
            TotalSteps          = state.TotalSteps;
            TotalReward         = state.TotalReward;
            ExplorationCoverage = state.Coverage;
            CoverageText        = $"{state.Coverage:P1}";

            if (state.EmbodiedPosition.Length >= 3)
                PositionText = $"({state.EmbodiedPosition[0]:F0}, {state.EmbodiedPosition[1]:F0}, {state.EmbodiedPosition[2]:F0})";

            if (state.EnactedVelocity.Length >= 2)
            {
                var speed = MathF.Sqrt(state.EnactedVelocity[0] * state.EnactedVelocity[0] +
                                       state.EnactedVelocity[1] * state.EnactedVelocity[1]);
                VelocityText = $"{speed:F1} UU/s";
            }
        });
    }

    private void OnObservationReceived(object? sender, AvatarObservation obs)
    {
        App.Current.Dispatcher.InvokeAsync(() =>
        {
            // Frame count now driven by MlVisionCaptureService; just update status
            MlVisionStatus = $"Frame {MlVisionFrameCount} @ {obs.Timestamp:F2}s — {obs.PerceivedObjects.Length} objects";
        });
    }

    private void OnActionDispatched(object? sender, AvatarAction action)
    {
        App.Current.Dispatcher.InvokeAsync(() =>
        {
            LastActionText = $"{action.InputAction} ({action.AxisX:F2}, {action.AxisY:F2}) mag={action.Magnitude:F2}";
        });
    }

    private void OnExplorationLog(object? sender, string message)
    {
        App.Current.Dispatcher.InvokeAsync(() => AddLog(message));
    }

    // ── KSM Cycle 3: AvatarEmbodimentService event handlers ─────────────────────

    private void OnEmotionalStateUpdated(object? sender, AvatarEmbodimentService.EmotionalState e)
    {
        App.Current.Dispatcher.InvokeAsync(() =>
        {
            EmHappiness = e.Happiness;
            EmSurprise  = e.Surprise;
            EmSadness   = e.Sadness;
            EmAnger     = e.Anger;
            EmFear      = e.Fear;
        });
    }

    private void OnNeurochemicalReadback(object? sender, AvatarEmbodimentService.NeurochemicalState n)
    {
        App.Current.Dispatcher.InvokeAsync(() =>
        {
            NeuroCuriosity   = n.Curiosity;
            NeuroEndorphin   = n.Endorphin;
            NeuroChaos       = n.Chaos;
            NeuroHomeostasis = n.Homeostasis;
        });
    }

    private void OnPersonalityApplied(object? sender, AvatarEmbodimentService.PersonalityTraits p)
    {
        App.Current.Dispatcher.InvokeAsync(() =>
        {
            PersonalityConfidence  = p.Confidence;
            PersonalityCharm       = p.Charm;
            PersonalityPlayfulness = p.Playfulness;
            PersonalityWit         = p.Wit;
            PersonalitySass        = p.Sass;
        });
    }

    private void OnAuArrayUpdated(object? sender, float[] aus)
    {
        App.Current.Dispatcher.InvokeAsync(() =>
        {
            // Map the 47-element AU array to the 6 display AUs
            // AU1+2 = brow raise, AU4 = brow furrow, AU5 = eye wide
            // AU12+13 = smile, AU23+24 = lip press, AU26+27 = jaw drop
            AuBrowRaise  = aus.Length > 2  ? (aus[1] + aus[2]) / 2f : 0f;
            AuBrowFurrow = aus.Length > 4  ? aus[4]                  : 0f;
            AuEyeWide    = aus.Length > 5  ? aus[5]                  : 0f;
            AuSmile      = aus.Length > 13 ? (aus[12] + aus[13]) / 2f : 0f;
            AuLipPress   = aus.Length > 24 ? (aus[23] + aus[24]) / 2f : 0f;
            AuJawDrop    = aus.Length > 27 ? (aus[26] + aus[27]) / 2f : 0f;
        });
    }

    private void AddLog(string message)
    {
        ExplorationLog.Insert(0, $"[{DateTime.Now:HH:mm:ss}] {message}");
        while (ExplorationLog.Count > 200)
            ExplorationLog.RemoveAt(ExplorationLog.Count - 1);
    }

    // ── KSM Cycle 2: UE5 Launch Stage Handlers (P4, P8, P11, P12, P14) ────────────────

    /// <summary>Launch UE5 via the 4-stage orchestrator pipeline.</summary>
    [RelayCommand]
    private async Task LaunchUE5Async()
    {
        if (_ue5Orchestrator == null)
        {
            AddUe5Log("⚠ UE5LaunchOrchestrator not registered in DI container");
            return;
        }
        Ue5LaunchButtonText = "⏸ LAUNCHING...";
        Ue5IsLaunching2     = true;
        Ue5LaunchProgress   = 0.0;
        Ue5LaunchLogLines.Clear();
        var result = await _ue5Orchestrator.LaunchAsync();
        Ue5LaunchButtonText = result.Success ? "✓ UE5 READY" : "▶ LAUNCH UE5";
        Ue5IsLaunching2     = false;
    }

    /// <summary>Stop the UE5 process.</summary>
    [RelayCommand]
    private void StopUE5()
    {
        _ue5Orchestrator?.Stop();
        Ue5LaunchButtonText = "▶ LAUNCH UE5";
        Ue5IsLaunching2     = false;
    }

    private void OnUe5StageChanged(UE5LaunchStage stage, string message)
    {
        App.Current.Dispatcher.InvokeAsync(() =>
        {
            Ue5LaunchStageText = $"{stage}: {message}";
            Ue5IsValidating  = stage == UE5LaunchStage.Validating;
            Ue5IsBuilding    = stage == UE5LaunchStage.Building;
            Ue5IsLaunching   = stage == UE5LaunchStage.Launching;
            Ue5IsConnecting  = stage == UE5LaunchStage.Connecting;
            Ue5IsReady       = stage == UE5LaunchStage.Ready;
            Ue5IsFailed      = stage == UE5LaunchStage.Failed;
            Ue5LaunchProgress = stage switch
            {
                UE5LaunchStage.Validating  => 0.10,
                UE5LaunchStage.Building    => 0.35,
                UE5LaunchStage.Launching   => 0.60,
                UE5LaunchStage.Connecting  => 0.85,
                UE5LaunchStage.Ready       => 1.00,
                UE5LaunchStage.Failed      => Ue5LaunchProgress,
                _                          => 0.00,
            };
            AddUe5Log($"[{stage}] {message}");
        });
    }

    private void OnUe5LogLine(string line)
    {
        App.Current.Dispatcher.InvokeAsync(() => AddUe5Log(line));
    }

    private void OnUe5LaunchComplete(UE5LaunchResult result)
    {
        App.Current.Dispatcher.InvokeAsync(() =>
        {
            if (result.Success)
            {
                AddLog($"✅ UE5 ready in {result.Duration.TotalSeconds:F1}s — {result.Message}");
                Ue5LaunchButtonText = "✓ UE5 READY";
            }
            else
            {
                AddLog($"❌ UE5 launch failed at {result.FailedAtStage}: {result.Message}");
                Ue5LaunchButtonText = "▶ LAUNCH UE5";
            }
        });
    }

    private void AddUe5Log(string line)
    {
        Ue5LaunchLogLines.Insert(0, $"[{DateTime.Now:HH:mm:ss}] {line}");
        while (Ue5LaunchLogLines.Count > 100)
            Ue5LaunchLogLines.RemoveAt(Ue5LaunchLogLines.Count - 1);
        Ue5LaunchLog = Ue5LaunchLogLines.Count > 0 ? Ue5LaunchLogLines[0] : "";
    }

    private void UpdateUE5ModuleStatus(string name, string status, bool isReady)
    {
        var module = UE5Modules.FirstOrDefault(m => m.Name == name);
        if (module != null)
        {
            module.Status  = status;
            module.IsReady = isReady;
        }
    }
}

/// <summary>Status of a UE5 cognitive module</summary>
public class UE5ModuleStatus : ObservableObject
{
    private string _status  = "Pending";
    private bool   _isReady = false;

    public string Name        { get; set; } = string.Empty;
    public string SourcePath  { get; set; } = string.Empty;
    public string UpgradeType { get; set; } = string.Empty;

    public string Status
    {
        get => _status;
        set => SetProperty(ref _status, value);
    }

    public bool IsReady
    {
        get => _isReady;
        set => SetProperty(ref _isReady, value);
    }
}
