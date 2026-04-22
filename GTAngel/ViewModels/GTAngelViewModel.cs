using System.Collections.ObjectModel;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using GTA3DE.Wpf.Models;
using GTA3DE.Wpf.Services;
using LiveChartsCore;
using LiveChartsCore.Defaults;
using LiveChartsCore.SkiaSharpView;
using LiveChartsCore.SkiaSharpView.Painting;
using Microsoft.Extensions.Logging;
using SkiaSharp;

namespace GTA3DE.Wpf.ViewModels;

/// <summary>
/// GTAngel ViewModel — the guardian angel cognitive dashboard.
///
/// Composition: /dte-ksm-evo-autogenesis ( /gta3-ue5-wpf ) → "GTAngel"
///
/// Exposes all observable properties for the GTAngelPage XAML bindings:
///   - Guardian Angel status (phase, loop step, KSM step)
///   - Autogenesis metrics (experiments, keep ratio, best metric)
///   - Cognitive state (stream coherence, property coherence, autonomy level)
///   - Alexander's 15 Properties live scores
///   - Experiment log
///   - System log
///   - Live charts (coherence, reward, property radar)
/// </summary>
public partial class GTAngelViewModel : ObservableObject
{
    private readonly GTAngelService _angel;

    // ── Parameterless constructor for XAML instantiation (TrainingDashboard tab) ──
    public GTAngelViewModel() : this(ResolveOrCreate()) { }

    private static GTAngelService ResolveOrCreate()
    {
        try
        {
            return Microsoft.Extensions.DependencyInjection.ServiceProviderServiceExtensions
                .GetRequiredService<GTAngelService>(App.Services);
        }
        catch
        {
            // Fallback for design-time or when DI not yet initialized
            var engine = new TrainingEngine();
            return new GTAngelService(
                Microsoft.Extensions.Logging.Abstractions.NullLogger<GTAngelService>.Instance,
                engine);
        }
    }

    // ── Guardian Angel Identity ───────────────────────────────────────────────
    [ObservableProperty] private string _angelName = "GTAngel";
    [ObservableProperty] private string _angelSubtitle = "Guardian Angel Cognitive Orchestrator";
    [ObservableProperty] private string _angelEmoji = "👼";
    [ObservableProperty] private string _phaseLabel = "Idle";
    [ObservableProperty] private string _loopStepLabel = "Observe";
    [ObservableProperty] private int _loopStepIndex;
    [ObservableProperty] private string _ksmStepLabel = "0. Observe";
    [ObservableProperty] private int _ksmStepIndex;
    [ObservableProperty] private string _alertMessage = string.Empty;
    [ObservableProperty] private bool _hasAlert;

    // ── Control State ─────────────────────────────────────────────────────────
    [ObservableProperty] private bool _isRunning;
    [ObservableProperty] private bool _isPaused;
    [ObservableProperty] private bool _isHalted;
    [ObservableProperty] private string _elapsedTime = "00:00:00";

    // ── Configuration ─────────────────────────────────────────────────────────
    [ObservableProperty] private int _maxExperiments = 30;
    [ObservableProperty] private double _spectralRadius = 0.90;
    [ObservableProperty] private double _leakingRate = 0.30;
    [ObservableProperty] private string _targetLevelLabel = "Autonomous";

    // ── Cognitive Metrics ─────────────────────────────────────────────────────
    [ObservableProperty] private double _streamCoherence = 0.5;
    [ObservableProperty] private double _propertyCoherence = 0.5;
    [ObservableProperty] private string _autonomyLevelLabel = "Level 0: Reactive";
    [ObservableProperty] private double _autonomyProgress;

    // ── Experiment Stats ──────────────────────────────────────────────────────
    [ObservableProperty] private int _totalExperiments;
    [ObservableProperty] private int _keptExperiments;
    [ObservableProperty] private int _discardedExperiments;
    [ObservableProperty] private double _keepRatio;
    [ObservableProperty] private double _bestMetric;
    [ObservableProperty] private string _statusMessage = "Ready";

    // ── Collections ───────────────────────────────────────────────────────────
    public ObservableCollection<AutogenesisExperiment> ExperimentLog { get; } = new();
    public ObservableCollection<AlexanderPropertyVM> PropertyScores { get; } = new();
    public ObservableCollection<string> LogMessages { get; } = new();
    public ObservableCollection<GTAngelLoopStepVM> LoopSteps { get; } = new();
    public ObservableCollection<KsmStepVM> KsmSteps { get; } = new();

    // ── Charts ────────────────────────────────────────────────────────────────
    public ObservableCollection<ISeries> CoherenceSeries { get; } = new();
    public ObservableCollection<ISeries> ExperimentMetricSeries { get; } = new();
    public Axis[] CoherenceXAxes { get; } = { new Axis { Name = "Experiment", LabelsPaint = null } };
    public Axis[] MetricXAxes { get; } = { new Axis { Name = "Experiment", LabelsPaint = null } };

    private readonly ObservableCollection<ObservablePoint> _coherencePoints = new();
    private readonly ObservableCollection<ObservablePoint> _propertyPoints = new();
    private readonly ObservableCollection<ObservablePoint> _metricPoints = new();

    private System.Threading.Timer? _elapsedTimer;

    public GTAngelViewModel(GTAngelService angel)
    {
        _angel = angel;

        InitializeLoopSteps();
        InitializeKsmSteps();
        InitializePropertyScores();
        InitializeCharts();

        // Wire service events
        _angel.OnStateUpdated += OnAngelStateUpdated;
        _angel.OnExperimentCompleted += OnExperimentCompleted;
        _angel.OnLogMessage += OnLogMessage;
        _angel.OnLoopStepChanged += OnLoopStepChanged;
        _angel.OnGuardianAlert += OnGuardianAlert;
    }

    // ── Commands ──────────────────────────────────────────────────────────────

    [RelayCommand]
    private async Task StartAngelAsync()
    {
        var config = new GTAngelConfig
        {
            TargetAutonomyLevel = ParseTargetLevel(TargetLevelLabel),
            MaxExperiments = MaxExperiments,
            SpectralRadius = SpectralRadius,
            LeakingRate = LeakingRate,
            EnableGuardianAlerts = true
        };

        ExperimentLog.Clear();
        LogMessages.Clear();
        _coherencePoints.Clear();
        _propertyPoints.Clear();
        _metricPoints.Clear();
        TotalExperiments = 0;
        KeptExperiments = 0;
        DiscardedExperiments = 0;
        BestMetric = 0;
        HasAlert = false;
        AlertMessage = string.Empty;

        // Reset loop step highlights
        foreach (var step in LoopSteps) step.IsActive = false;
        foreach (var step in KsmSteps) step.IsActive = false;

        StartElapsedTimer();
        await _angel.StartAsync(config);
    }

    [RelayCommand]
    private async Task StopAngelAsync()
    {
        await _angel.StopAsync();
        StopElapsedTimer();
    }

    [RelayCommand]
    private void TogglePause()
    {
        _angel.TogglePause();
    }

    [RelayCommand]
    private void DismissAlert()
    {
        HasAlert = false;
        AlertMessage = string.Empty;
    }

    [RelayCommand]
    private void ResetStats()
    {
        if (IsRunning) return;
        ExperimentLog.Clear();
        LogMessages.Clear();
        _coherencePoints.Clear();
        _propertyPoints.Clear();
        _metricPoints.Clear();
        TotalExperiments = 0;
        KeptExperiments = 0;
        DiscardedExperiments = 0;
        BestMetric = 0;
        HasAlert = false;
        StatusMessage = "Stats reset.";
    }

    // ── Event Handlers ────────────────────────────────────────────────────────

    private void OnAngelStateUpdated(GTAngelState state)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            IsRunning = state.IsRunning;
            IsPaused = state.IsPaused;
            IsHalted = state.IsHalted;
            PhaseLabel = state.Phase.ToString();
            KsmStepIndex = state.CurrentKsmStep;
            KsmStepLabel = $"{state.CurrentKsmStep}. {state.CurrentKsmStepName}";
            StreamCoherence = state.StreamCoherence;
            PropertyCoherence = state.PropertyCoherence;
            TotalExperiments = state.TotalExperiments;
            KeptExperiments = state.KeptExperiments;
            DiscardedExperiments = state.DiscardedExperiments;
            KeepRatio = state.KeepRatio;
            BestMetric = state.BestMetric;

            // Autonomy level
            AutonomyLevelLabel = $"Level {(int)state.AutonomyLevel}: {state.AutonomyLevel}";
            AutonomyProgress = (int)state.AutonomyLevel / 5.0;

            // KSM step highlight
            foreach (var step in KsmSteps)
                step.IsActive = step.Index == state.CurrentKsmStep;

            // Chart data
            var ep = state.TotalExperiments;
            _coherencePoints.Add(new ObservablePoint(ep, state.StreamCoherence));
            _propertyPoints.Add(new ObservablePoint(ep, state.PropertyCoherence));
            if (_coherencePoints.Count > 200) _coherencePoints.RemoveAt(0);
            if (_propertyPoints.Count > 200) _propertyPoints.RemoveAt(0);

            // Update property scores
            UpdatePropertyScores(state);

            StatusMessage = $"Phase: {state.Phase} | KSM: {state.CurrentKsmStepName} | Coherence: {state.StreamCoherence:F3}";
        });
    }

    private void OnExperimentCompleted(AutogenesisExperiment exp)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            ExperimentLog.Insert(0, exp);
            if (ExperimentLog.Count > 500) ExperimentLog.RemoveAt(ExperimentLog.Count - 1);

            _metricPoints.Add(new ObservablePoint(exp.ExperimentId, exp.PrimaryMetric));
            if (_metricPoints.Count > 200) _metricPoints.RemoveAt(0);
        });
    }

    private void OnLogMessage(string msg)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            LogMessages.Insert(0, $"[{DateTime.Now:HH:mm:ss}] {msg}");
            if (LogMessages.Count > 500) LogMessages.RemoveAt(LogMessages.Count - 1);
        });
    }

    private void OnLoopStepChanged(GTAngelLoopStep step)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            LoopStepIndex = (int)step;
            LoopStepLabel = step.ToString();
            foreach (var s in LoopSteps)
                s.IsActive = s.Index == (int)step;
        });
    }

    private void OnGuardianAlert(string alert)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            AlertMessage = alert;
            HasAlert = true;
        });
    }

    // ── Initialization ────────────────────────────────────────────────────────

    private void InitializeLoopSteps()
    {
        var steps = new[]
        {
            ("Observe", "Levels of Scale", "#00BCD4"),
            ("Edit", "Good Shape", "#4CAF50"),
            ("Run", "Contrast", "#FF9800"),
            ("Assess", "Not-Separateness", "#9C27B0"),
            ("Decide", "Gradients", "#F44336"),
            ("Log", "Echoes", "#607D8B"),
        };

        for (int i = 0; i < steps.Length; i++)
        {
            LoopSteps.Add(new GTAngelLoopStepVM
            {
                Index = i,
                Name = steps[i].Item1,
                Property = steps[i].Item2,
                Color = steps[i].Item3,
                IsActive = i == 0
            });
        }
    }

    private void InitializeKsmSteps()
    {
        var colors = new[]
        {
            "#00BCD4", "#00BCD4", "#00BCD4",
            "#4CAF50", "#4CAF50", "#4CAF50",
            "#FF9800", "#FF9800", "#FF9800",
            "#9C27B0", "#9C27B0", "#9C27B0"
        };

        for (int i = 0; i < 12; i++)
        {
            KsmSteps.Add(new KsmStepVM
            {
                Index = i,
                Name = $"{i}. {AutogenesisExperiment.KsmStepNames[i]}",
                Color = colors[i],
                IsActive = false
            });
        }
    }

    private void InitializePropertyScores()
    {
        foreach (var prop in AlexanderProperty.CreateAll())
        {
            PropertyScores.Add(new AlexanderPropertyVM
            {
                Index = prop.Index,
                Name = prop.Name,
                Description = prop.Description,
                Score = 0.5
            });
        }
    }

    private void InitializeCharts()
    {
        CoherenceSeries.Add(new LineSeries<ObservablePoint>
        {
            Values = _coherencePoints,
            Name = "Stream Coherence",
            Stroke = new SolidColorPaint(SKColors.DeepSkyBlue) { StrokeThickness = 2 },
            Fill = new SolidColorPaint(SKColor.Parse("#1500BCD4")),
            GeometrySize = 3,
            GeometryStroke = new SolidColorPaint(SKColors.DeepSkyBlue) { StrokeThickness = 2 }
        });

        CoherenceSeries.Add(new LineSeries<ObservablePoint>
        {
            Values = _propertyPoints,
            Name = "Property Coherence",
            Stroke = new SolidColorPaint(SKColors.LimeGreen) { StrokeThickness = 2 },
            Fill = new SolidColorPaint(SKColor.Parse("#1532CD32")),
            GeometrySize = 3,
            GeometryStroke = new SolidColorPaint(SKColors.LimeGreen) { StrokeThickness = 2 }
        });

        ExperimentMetricSeries.Add(new LineSeries<ObservablePoint>
        {
            Values = _metricPoints,
            Name = "Primary Metric",
            Stroke = new SolidColorPaint(SKColors.Gold) { StrokeThickness = 2 },
            Fill = new SolidColorPaint(SKColor.Parse("#15FFD700")),
            GeometrySize = 4,
            GeometryStroke = new SolidColorPaint(SKColors.Gold) { StrokeThickness = 2 }
        });
    }

    private void UpdatePropertyScores(GTAngelState state)
    {
        // Simulate property score updates based on coherence
        var rng = new Random();
        foreach (var prop in PropertyScores)
        {
            var delta = (rng.NextDouble() - 0.5) * 0.02;
            prop.Score = Math.Clamp(prop.Score + delta, 0.0, 1.0);
        }
    }

    // ── Elapsed Timer ─────────────────────────────────────────────────────────

    private void StartElapsedTimer()
    {
        _elapsedTimer = new System.Threading.Timer(_ =>
        {
            if (_angel.State.IsRunning)
            {
                var elapsed = _angel.State.Elapsed;
                Application.Current.Dispatcher.Invoke(() =>
                    ElapsedTime = $"{elapsed.Hours:D2}:{elapsed.Minutes:D2}:{elapsed.Seconds:D2}");
            }
        }, null, TimeSpan.Zero, TimeSpan.FromSeconds(1));
    }

    private void StopElapsedTimer()
    {
        _elapsedTimer?.Dispose();
        _elapsedTimer = null;
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    private static AutonomyLevel ParseTargetLevel(string label) => label switch
    {
        "Reactive" => AutonomyLevel.Reactive,
        "Adaptive" => AutonomyLevel.Adaptive,
        "Strategic" => AutonomyLevel.Strategic,
        "Cognitive" => AutonomyLevel.Cognitive,
        "Embodied" => AutonomyLevel.Embodied,
        _ => AutonomyLevel.Autonomous
    };
}

// ── Supporting View Models ────────────────────────────────────────────────────
// Note: AlexanderPropertyVM is defined in MainViewModel.cs (shared)

public partial class GTAngelLoopStepVM : ObservableObject
{
    public int Index { get; set; }
    public string Name { get; set; } = string.Empty;
    public string Property { get; set; } = string.Empty;
    public string Color { get; set; } = "#FFFFFF";

    [ObservableProperty] private bool _isActive;
}

public partial class KsmStepVM : ObservableObject
{
    public int Index { get; set; }
    public string Name { get; set; } = string.Empty;
    public string Color { get; set; } = "#FFFFFF";

    [ObservableProperty] private bool _isActive;
}
