using System.Collections.ObjectModel;
using System.IO;
using System.Windows;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using GTAngel.Models;
using GTAngel.Services;
using LiveChartsCore;
using LiveChartsCore.Defaults;
using LiveChartsCore.SkiaSharpView;
using LiveChartsCore.SkiaSharpView.Painting;
using SkiaSharp;

namespace GTAngel.ViewModels;

public partial class MainViewModel : ObservableObject
{
    private readonly AssetCatalogService _catalogService = new();
    private readonly TrainingEngine _trainingEngine = new();
    private DteCognitiveCoreService? _cognitiveCore;

    // ========== Navigation ==========
    [ObservableProperty] private int _selectedTabIndex;
    [ObservableProperty] private string _statusMessage = "Ready";
    [ObservableProperty] private bool _isBusy;

    // ========== Asset Browser ==========
    [ObservableProperty] private string _assetArchivePath = string.Empty;
    [ObservableProperty] private bool _isAssetLoaded;
    [ObservableProperty] private string _assetSearchQuery = string.Empty;
    [ObservableProperty] private AssetCategory? _selectedCategory;
    [ObservableProperty] private string? _selectedGame;
    [ObservableProperty] private AssetCatalogSummary? _catalogSummary;

    public ObservableCollection<GameWorldAsset> FilteredAssets { get; } = new();
    public ObservableCollection<GameWorldMap> Maps { get; } = new();
    public ObservableCollection<AssetCategoryInfo> CategoryBreakdown { get; } = new();

    // ========== Training Dashboard ==========
    [ObservableProperty] private bool _isTrainingRunning;
    [ObservableProperty] private string _autonomyLevelText = "Level 0: Reactive";
    [ObservableProperty] private double _autonomyProgress;
    [ObservableProperty] private string _cognitiveModeName = "Exploration";
    [ObservableProperty] private int _currentCycleStep;
    [ObservableProperty] private string _cycleStepName = "Perception";
    [ObservableProperty] private double _streamCoherence;
    [ObservableProperty] private double _propertyCoherence;
    [ObservableProperty] private double _valence;
    [ObservableProperty] private double _arousal;
    [ObservableProperty] private double _embodied;
    [ObservableProperty] private double _embedded;
    [ObservableProperty] private double _enacted;
    [ObservableProperty] private double _extended;
    [ObservableProperty] private double _wisdomLevel;
    [ObservableProperty] private int _totalExperiments;
    [ObservableProperty] private int _keptExperiments;
    [ObservableProperty] private int _discardedExperiments;
    [ObservableProperty] private double _bestReward;
    [ObservableProperty] private double _spectralRadius = 0.9;
    [ObservableProperty] private double _leakingRate = 0.3;
    [ObservableProperty] private int _maxExperiments = 30;
    [ObservableProperty] private string _targetAutonomyLevel = "Autonomous";

    // ========== KSM Cycle 5: DTE Cognitive Core Telemetry ==========
    [ObservableProperty] private string _ecanTopNeurons = "C0(0.00), C1(0.00), C2(0.00)";
    [ObservableProperty] private double _ecanAttentionBudget;
    [ObservableProperty] private int _mosesPatternCount;
    [ObservableProperty] private double _mosesTopFitness;
    [ObservableProperty] private double _woutTrainingLoss = 1.0;
    [ObservableProperty] private int _woutSampleCount;
    [ObservableProperty] private double _thompsonAlpha = 1.0;
    [ObservableProperty] private double _thompsonBeta = 1.0;
    [ObservableProperty] private double _cognitiveCoherence;
    [ObservableProperty] private double _attentionEntropy;
    [ObservableProperty] private double _policyEntropy;
    [ObservableProperty] private double _patternDiversity;
    // Phase 8.3: 16 cluster STI values for bar chart + Wout convergence status
    [ObservableProperty] private string _woutConverged = "No";
    public ObservableCollection<float> ClusterStiValues { get; } = new(new float[16]);

    public ObservableCollection<AutogenesisExperiment> ExperimentLog { get; } = new();
    public ObservableCollection<AlexanderPropertyVM> PropertyScores { get; } = new();
    public ObservableCollection<string> LogMessages { get; } = new();

    // ========== Charts ==========
    public ObservableCollection<ISeries> RewardSeries { get; } = new();
    public ObservableCollection<ISeries> CoherenceSeries { get; } = new();
    public ObservableCollection<ISeries> ReservoirSeries { get; } = new();
    public ObservableCollection<ISeries> AssetDistributionSeries { get; } = new();

    // Chart Axes (LiveCharts2 Axis must be defined in code, not XAML)
    public Axis[] RewardXAxes { get; } = new[] { new Axis { Name = "Episode", LabelsPaint = null } };
    public Axis[] ReservoirXAxes { get; } = new[] { new Axis { Name = "Step", LabelsPaint = null } };
    public Axis[] CoherenceXAxes { get; } = new[] { new Axis { Name = "Episode", LabelsPaint = null } };

    private readonly ObservableCollection<ObservablePoint> _rewardPoints = new();
    private readonly ObservableCollection<ObservablePoint> _coherencePoints = new();
    private readonly ObservableCollection<ObservablePoint> _sensoryPoints = new();
    private readonly ObservableCollection<ObservablePoint> _cognitivePoints = new();
    private readonly ObservableCollection<ObservablePoint> _executivePoints = new();

    public MainViewModel()
    {
        InitializeCharts();
        InitializePropertyScores();

        // Phase 8.3: Wire DteCognitiveCoreService for real-time telemetry
        _cognitiveCore = App.Services.GetService<DteCognitiveCoreService>();
        if (_cognitiveCore != null)
        {
            _cognitiveCore.OnAttentionUpdated     += OnCognitiveCoreAttentionUpdated;
            _cognitiveCore.OnPatternMined         += OnCognitiveCorePatternMined;
            _cognitiveCore.OnWoutTrained          += OnCognitiveCoreWoutTrained;
            _cognitiveCore.OnCognitiveCoherenceUpdated += OnCognitiveCoreCoherenceUpdated;
        }

        _trainingEngine.OnCognitiveStateChanged += OnCognitiveStateChanged;
        _trainingEngine.OnEpisodeCompleted += OnEpisodeCompleted;
        _trainingEngine.OnExperimentCompleted += OnExperimentCompleted;
        _trainingEngine.OnLogMessage += msg => Application.Current.Dispatcher.Invoke(() =>
        {
            LogMessages.Insert(0, $"[{DateTime.Now:HH:mm:ss}] {msg}");
            if (LogMessages.Count > 500) LogMessages.RemoveAt(LogMessages.Count - 1);
        });

        // Auto-detect asset path
        var defaultPath = @"E:\u9n\angelclaw\GTA3DE.Assets.zip";
        if (File.Exists(defaultPath))
            AssetArchivePath = defaultPath;
    }

    private void InitializeCharts()
    {
        RewardSeries.Add(new LineSeries<ObservablePoint>
        {
            Values = _rewardPoints,
            Name = "Total Reward",
            Stroke = new SolidColorPaint(SKColors.DeepSkyBlue) { StrokeThickness = 2 },
            Fill = null,
            GeometrySize = 4,
            GeometryStroke = new SolidColorPaint(SKColors.DeepSkyBlue) { StrokeThickness = 2 }
        });

        CoherenceSeries.Add(new LineSeries<ObservablePoint>
        {
            Values = _coherencePoints,
            Name = "Stream Coherence",
            Stroke = new SolidColorPaint(SKColors.LimeGreen) { StrokeThickness = 2 },
            Fill = null,
            GeometrySize = 3
        });

        ReservoirSeries.Add(new LineSeries<ObservablePoint>
        {
            Values = _sensoryPoints,
            Name = "Sensory (128)",
            Stroke = new SolidColorPaint(SKColors.Coral) { StrokeThickness = 1.5f },
            Fill = null,
            GeometrySize = 0
        });
        ReservoirSeries.Add(new LineSeries<ObservablePoint>
        {
            Values = _cognitivePoints,
            Name = "Cognitive (256)",
            Stroke = new SolidColorPaint(SKColors.MediumPurple) { StrokeThickness = 1.5f },
            Fill = null,
            GeometrySize = 0
        });
        ReservoirSeries.Add(new LineSeries<ObservablePoint>
        {
            Values = _executivePoints,
            Name = "Executive (512)",
            Stroke = new SolidColorPaint(SKColors.Gold) { StrokeThickness = 1.5f },
            Fill = null,
            GeometrySize = 0
        });
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

    // ========== Asset Browser Commands ==========

    [RelayCommand]
    private async Task LoadAssetsAsync()
    {
        if (string.IsNullOrEmpty(AssetArchivePath) || !File.Exists(AssetArchivePath))
        {
            StatusMessage = "Asset archive not found. Please set the correct path.";
            return;
        }

        IsBusy = true;
        StatusMessage = "Scanning asset archive...";

        try
        {
            var progress = new Progress<string>(msg => StatusMessage = msg);
            CatalogSummary = await _catalogService.ScanArchiveAsync(AssetArchivePath, progress);
            IsAssetLoaded = true;

            // Populate filtered assets
            FilteredAssets.Clear();
            foreach (var asset in _catalogService.Assets.Take(500))
                FilteredAssets.Add(asset);

            // Populate maps
            Maps.Clear();
            foreach (var map in _catalogService.Maps)
                Maps.Add(map);

            // Build category breakdown
            UpdateCategoryBreakdown();
            UpdateAssetDistributionChart();

            StatusMessage = $"Loaded {CatalogSummary.TotalAssets} assets ({CatalogSummary.TotalSizeFormatted})";
        }
        catch (Exception ex)
        {
            StatusMessage = $"Error: {ex.Message}";
        }
        finally
        {
            IsBusy = false;
        }
    }

    [RelayCommand]
    private void FilterAssets()
    {
        FilteredAssets.Clear();
        IEnumerable<GameWorldAsset> results = _catalogService.Assets;

        if (!string.IsNullOrWhiteSpace(AssetSearchQuery))
            results = _catalogService.Search(AssetSearchQuery);
        if (SelectedCategory.HasValue)
            results = results.Where(a => a.Category == SelectedCategory.Value);
        if (!string.IsNullOrEmpty(SelectedGame))
            results = results.Where(a => a.GameTitle == SelectedGame);

        foreach (var asset in results.Take(500))
            FilteredAssets.Add(asset);

        StatusMessage = $"Showing {FilteredAssets.Count} assets";
    }

    [RelayCommand]
    private void ClearFilter()
    {
        AssetSearchQuery = string.Empty;
        SelectedCategory = null;
        SelectedGame = null;
        FilterAssets();
    }

    private void UpdateCategoryBreakdown()
    {
        CategoryBreakdown.Clear();
        if (CatalogSummary == null) return;

        foreach (var kvp in CatalogSummary.CountByCategory.OrderByDescending(x => x.Value))
        {
            var size = CatalogSummary.SizeByCategory.GetValueOrDefault(kvp.Key, 0);
            CategoryBreakdown.Add(new AssetCategoryInfo
            {
                Category = kvp.Key,
                Count = kvp.Value,
                SizeBytes = size,
                Percentage = (double)kvp.Value / CatalogSummary.TotalAssets
            });
        }
    }

    private void UpdateAssetDistributionChart()
    {
        AssetDistributionSeries.Clear();
        if (CatalogSummary == null) return;

        var topCategories = CatalogSummary.CountByCategory
            .OrderByDescending(x => x.Value)
            .Take(8)
            .ToList();

        var colors = new[] {
            SKColors.DeepSkyBlue, SKColors.Coral, SKColors.LimeGreen, SKColors.Gold,
            SKColors.MediumPurple, SKColors.OrangeRed, SKColors.Teal, SKColors.HotPink
        };

        for (int i = 0; i < topCategories.Count; i++)
        {
            var cat = topCategories[i];
            AssetDistributionSeries.Add(new PieSeries<double>
            {
                Values = new[] { (double)cat.Value },
                Name = cat.Key.ToString(),
                Fill = new SolidColorPaint(colors[i % colors.Length])
            });
        }
    }

    // ========== Training Commands ==========

    [RelayCommand]
    private async Task StartTrainingAsync()
    {
        if (IsTrainingRunning) return;

        IsTrainingRunning = true;
        StatusMessage = "Starting autogenesis training loop...";
        SelectedTabIndex = 1; // Switch to training tab

        var config = new TrainingConfig
        {
            AssetArchivePath = AssetArchivePath,
            TargetLevel = Enum.TryParse<AutonomyLevel>(TargetAutonomyLevel, out var level) ? level : AutonomyLevel.Autonomous,
            MaxExperiments = MaxExperiments,
            MinCoherence = 0.15,
            MinPropertyCoherence = 0.60,
            MaxParameterDelta = 0.20,
            ExplorationRate = 0.3
        };

        _trainingEngine.ESN.SpectralRadius = SpectralRadius;
        _trainingEngine.ESN.LeakingRate = LeakingRate;

        var progress = new Progress<string>(msg => StatusMessage = msg);

        try
        {
            await _trainingEngine.StartAutogenesisAsync(config, progress);
        }
        catch (Exception ex)
        {
            StatusMessage = $"Training error: {ex.Message}";
        }
        finally
        {
            IsTrainingRunning = false;
            StatusMessage = "Training complete";
        }
    }

    [RelayCommand]
    private void StopTraining()
    {
        _trainingEngine.Stop();
        IsTrainingRunning = false;
        StatusMessage = "Training stopped";
    }

    [RelayCommand]
    private void ResetTraining()
    {
        _trainingEngine.ESN.Reset();
        ExperimentLog.Clear();
        _rewardPoints.Clear();
        _coherencePoints.Clear();
        _sensoryPoints.Clear();
        _cognitivePoints.Clear();
        _executivePoints.Clear();
        LogMessages.Clear();
        TotalExperiments = 0;
        KeptExperiments = 0;
        DiscardedExperiments = 0;
        BestReward = 0;
        AutonomyLevelText = "Level 0: Reactive";
        AutonomyProgress = 0;
        StatusMessage = "Training reset";
    }

    // ========== Event Handlers ==========

    private void OnCognitiveStateChanged(CognitiveState state)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            CognitiveModeName = state.Mode.ToString();
            CurrentCycleStep = state.CurrentCycleStep;
            CycleStepName = state.CycleStepName;
            Valence = state.Valence;
            Arousal = state.Arousal;
            Embodied = state.Embodied;
            Embedded = state.Embedded;
            Enacted = state.Enacted;
            Extended = state.Extended;
            WisdomLevel = state.WisdomLevel;
            StreamCoherence = _trainingEngine.ESN.StreamCoherence;

            // Update reservoir chart
            var esn = _trainingEngine.ESN;
            int step = esn.TotalSteps;
            if (esn.Layers.Count >= 3)
            {
                _sensoryPoints.Add(new ObservablePoint(step, esn.Layers[0].Activation));
                _cognitivePoints.Add(new ObservablePoint(step, esn.Layers[1].Activation));
                _executivePoints.Add(new ObservablePoint(step, esn.Layers[2].Activation));

                if (_sensoryPoints.Count > 300)
                {
                    _sensoryPoints.RemoveAt(0);
                    _cognitivePoints.RemoveAt(0);
                    _executivePoints.RemoveAt(0);
                }
            }
        });
    }

    private void OnEpisodeCompleted(TrainingEpisode episode)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            _rewardPoints.Add(new ObservablePoint(episode.EpisodeId, episode.TotalReward));
            _coherencePoints.Add(new ObservablePoint(episode.EpisodeId, episode.StreamCoherence));
        });
    }

    private void OnExperimentCompleted(AutogenesisExperiment experiment)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            ExperimentLog.Insert(0, experiment);
            if (ExperimentLog.Count > 200) ExperimentLog.RemoveAt(ExperimentLog.Count - 1);

            var stats = _trainingEngine.Stats;
            TotalExperiments = stats.ExperimentsRun;
            KeptExperiments = stats.ExperimentsKept;
            DiscardedExperiments = stats.ExperimentsDiscarded;
            BestReward = stats.BestReward;
            AutonomyLevelText = $"Level {(int)stats.CurrentAutonomyLevel}: {stats.CurrentAutonomyLevel}";
            AutonomyProgress = (int)stats.CurrentAutonomyLevel / 5.0;
            PropertyCoherence = _trainingEngine.ComputeOverallPropertyCoherence();
            SpectralRadius = _trainingEngine.ESN.SpectralRadius;
            LeakingRate = _trainingEngine.ESN.LeakingRate;

            // Update property scores
            for (int i = 0; i < _trainingEngine.Properties.Count && i < PropertyScores.Count; i++)
            {
                PropertyScores[i].Score = _trainingEngine.Properties[i].Score;
                PropertyScores[i].Delta = _trainingEngine.Properties[i].Delta;
            }
        });
    }

    // Phase 8.3: Cognitive Core telemetry event handlers

    private void OnCognitiveCoreAttentionUpdated(object? sender, AttentionUpdatedEventArgs e)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            EcanAttentionBudget = e.Snapshot.AttentionBudget;
            AttentionEntropy    = e.Snapshot.AttentionEntropy;
            EcanTopNeurons      = e.Snapshot.TopNeuronClusters;

            // Update cluster STI bar chart values (16 values)
            var sti = e.Snapshot.ClusterSTI;
            for (int i = 0; i < ClusterStiValues.Count && i < sti.Length; i++)
                ClusterStiValues[i] = sti[i];
        });
    }

    private void OnCognitiveCorePatternMined(object? sender, PatternMinedEventArgs e)
    {
        if (_cognitiveCore == null) return;
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            MosesPatternCount = _cognitiveCore.PatternCount;
            PatternDiversity  = _cognitiveCore.GetPatternDiversity();
            MosesTopFitness   = e.Pattern.Fitness;
        });
    }

    private void OnCognitiveCoreWoutTrained(object? sender, WoutTrainedEventArgs e)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            WoutTrainingLoss = e.Snapshot.RidgeLoss;
            WoutSampleCount  = e.Snapshot.SampleCount;
            WoutConverged    = e.Snapshot.IsConverged ? "Yes ✓" : "No";
        });
    }

    private void OnCognitiveCoreCoherenceUpdated(object? sender, CognitiveCoherenceUpdatedEventArgs e)
    {
        Application.Current.Dispatcher.InvokeAsync(() =>
        {
            CognitiveCoherence = e.Snapshot.Coherence;
        });
    }
}

// ========== Helper ViewModels ==========

public partial class AlexanderPropertyVM : ObservableObject
{
    [ObservableProperty] private int _index;
    [ObservableProperty] private string _name = string.Empty;
    [ObservableProperty] private string _description = string.Empty;
    [ObservableProperty] private double _score;
    [ObservableProperty] private double _delta;

    public string ScoreText => $"{Score:P0}";
    public string DeltaText => Delta >= 0 ? $"+{Delta:F3}" : $"{Delta:F3}";
}

public class AssetCategoryInfo
{
    public AssetCategory Category { get; set; }
    public int Count { get; set; }
    public long SizeBytes { get; set; }
    public double Percentage { get; set; }

    public string SizeFormatted => SizeBytes switch
    {
        < 1024 * 1024 => $"{SizeBytes / 1024.0:F1} KB",
        _ => $"{SizeBytes / (1024.0 * 1024.0):F1} MB"
    };

    public string PercentageText => $"{Percentage:P1}";
}
