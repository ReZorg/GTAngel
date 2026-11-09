using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Windows;
using GTAngel.Organizer.Core;
using GTAngel.Organizer.Core.Models;
using GTAngel.Organizer.Core.Services;
using GTAngel.Organizer.Mvvm;
using Microsoft.Win32;

namespace GTAngel.Organizer.ViewModels;

public sealed class MainWindowViewModel : ObservableObject
{
    private readonly OrganizerEngine _engine = new();
    private readonly RecommendationWatcher _watcher = new();
    private CancellationTokenSource? _cancellation;
    private ScanResult? _scan;
    private OrganizationPlan? _plan;
    private string _workspacePath;
    private string _statusText = "Ready to scan. No files are changed during scanning.";
    private string _currentActivity = "Idle";
    private double _progressValue;
    private bool _isBusy;
    private JournalViewModel? _selectedJournal;
    private bool _includeUnknown;
    private bool _includeSource;
    private bool _looseRootOnly = true;
    private bool _renameConflicts;
    private string _watchStatus = WatcherLifecycle.Stopped.ToString();

    public MainWindowViewModel()
    {
        _workspacePath = DetectWorkspacePath();
        BrowseCommand = new RelayCommand(Browse, () => !IsBusy);
        ScanCommand = new AsyncRelayCommand(ScanAsync, () => !IsBusy && Directory.Exists(WorkspacePath));
        CancelCommand = new RelayCommand(Cancel, () => IsBusy);
        ExecuteCommand = new AsyncRelayCommand(ExecuteAsync, () => !IsBusy && _plan is not null && SelectedCount > 0);
        ExportCommand = new AsyncRelayCommand(ExportAsync, () => !IsBusy && _plan is not null);
        UndoCommand = new AsyncRelayCommand(UndoAsync, () => !IsBusy && SelectedJournal?.JournalPath is not null && SelectedJournal.Journal.Status is TransactionStatus.Completed or TransactionStatus.PartiallyCompleted);
        SelectSafeCommand = new RelayCommand(SelectSafe, () => !IsBusy && PlanEntries.Count > 0);
        ClearSelectionCommand = new RelayCommand(ClearSelection, () => !IsBusy && PlanEntries.Count > 0);
        OpenStateFolderCommand = new RelayCommand(OpenStateFolder, () => Directory.Exists(WorkspacePath));
        StartWatchCommand = new RelayCommand(StartWatch, () => !IsBusy && Directory.Exists(WorkspacePath) && _watcher.Lifecycle is WatcherLifecycle.Stopped or WatcherLifecycle.Faulted);
        PauseWatchCommand = new RelayCommand(PauseOrResumeWatch, () => _watcher.Lifecycle is WatcherLifecycle.Watching or WatcherLifecycle.Paused);
        StopWatchCommand = new AsyncRelayCommand(StopWatchAsync, () => _watcher.Lifecycle != WatcherLifecycle.Stopped);
        _watcher.RecommendationCreated += OnWatchRecommendationCreated;
        _watcher.LifecycleChanged += OnWatcherLifecycleChanged;
    }

    public ObservableCollection<PlanEntryViewModel> PlanEntries { get; } = [];
    public ObservableCollection<RepositoryComponent> Components { get; } = [];
    public ObservableCollection<JournalViewModel> History { get; } = [];
    public ObservableCollection<WatchRecommendation> WatchRecommendations { get; } = [];
    public ObservableCollection<string> ActivityLog { get; } = [];

    public RelayCommand BrowseCommand { get; }
    public AsyncRelayCommand ScanCommand { get; }
    public RelayCommand CancelCommand { get; }
    public AsyncRelayCommand ExecuteCommand { get; }
    public AsyncRelayCommand ExportCommand { get; }
    public AsyncRelayCommand UndoCommand { get; }
    public RelayCommand SelectSafeCommand { get; }
    public RelayCommand ClearSelectionCommand { get; }
    public RelayCommand OpenStateFolderCommand { get; }
    public RelayCommand StartWatchCommand { get; }
    public RelayCommand PauseWatchCommand { get; }
    public AsyncRelayCommand StopWatchCommand { get; }

    public string WorkspacePath
    {
        get => _workspacePath;
        set
        {
            if (!SetProperty(ref _workspacePath, value)) return;
            _scan = null;
            _plan = null;
            PlanEntries.Clear();
            Components.Clear();
            History.Clear();
            StatusText = "Workspace changed. Run a scan to create a new plan.";
            RefreshSummary();
            RefreshCommands();
        }
    }

    public string StatusText
    {
        get => _statusText;
        private set => SetProperty(ref _statusText, value);
    }

    public string CurrentActivity
    {
        get => _currentActivity;
        private set => SetProperty(ref _currentActivity, value);
    }

    public double ProgressValue
    {
        get => _progressValue;
        private set => SetProperty(ref _progressValue, value);
    }

    public bool IsBusy
    {
        get => _isBusy;
        private set
        {
            if (!SetProperty(ref _isBusy, value)) return;
            RefreshCommands();
        }
    }

    public JournalViewModel? SelectedJournal
    {
        get => _selectedJournal;
        set
        {
            if (!SetProperty(ref _selectedJournal, value)) return;
            UndoCommand.RaiseCanExecuteChanged();
        }
    }

    public bool IncludeUnknown
    {
        get => _includeUnknown;
        set => SetProperty(ref _includeUnknown, value);
    }

    public bool IncludeSource
    {
        get => _includeSource;
        set => SetProperty(ref _includeSource, value);
    }

    public bool LooseRootOnly
    {
        get => _looseRootOnly;
        set => SetProperty(ref _looseRootOnly, value);
    }

    public bool RenameConflicts
    {
        get => _renameConflicts;
        set => SetProperty(ref _renameConflicts, value);
    }

    public int FileCount => _scan?.Files.Count ?? 0;
    public int ComponentCount => _scan?.Components.Count ?? 0;
    public int ProtectedCount => _scan?.ProtectedFileCount ?? 0;
    public int ProposalCount => _plan?.Entries.Count ?? 0;
    public int SelectedCount => PlanEntries.Count(entry => entry.IsSelected);
    public int ConflictCount => _plan?.ConflictCount ?? 0;
    public int DuplicateCount => _plan?.DuplicateCount ?? 0;
    public string ScanDuration => _scan is null ? "—" : (_scan.CompletedAtUtc - _scan.StartedAtUtc).ToString("g");
    public string WatchStatus
    {
        get => _watchStatus;
        private set => SetProperty(ref _watchStatus, value);
    }
    public int WatchPendingCount => _watcher.PendingCount;
    public string PauseWatchLabel => _watcher.Lifecycle == WatcherLifecycle.Paused ? "Resume watch" : "Pause watch";

    private void Browse()
    {
        var dialog = new OpenFolderDialog
        {
            Title = "Choose the GTAngel workspace or another repository root",
            InitialDirectory = Directory.Exists(WorkspacePath) ? WorkspacePath : Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
            Multiselect = false
        };
        if (dialog.ShowDialog() == true)
        {
            WorkspacePath = dialog.FolderName;
        }
    }

    private async Task ScanAsync()
    {
        if (!Directory.Exists(WorkspacePath))
        {
            StatusText = "The selected workspace does not exist.";
            return;
        }

        _cancellation?.Dispose();
        _cancellation = new CancellationTokenSource();
        IsBusy = true;
        ProgressValue = 0;
        CurrentActivity = "Loading configuration";
        StatusText = "Scanning is read-only. Discovering repository and project boundaries…";
        ActivityLog.Insert(0, $"{DateTime.Now:T} Scan started: {WorkspacePath}");

        try
        {
            var saved = await _engine.LoadConfigAsync(WorkspacePath, _cancellation.Token);
            IncludeUnknown = saved.ProposeUnknownFiles;
            IncludeSource = saved.ProposeSourceFiles;
            LooseRootOnly = saved.ProposeLooseRootFilesOnly;
            RenameConflicts = saved.ConflictResolution == ConflictResolution.RenameWithSuffix;
            var config = BuildConfig(saved);
            var progress = new Progress<ScanProgress>(value =>
            {
                CurrentActivity = value.Stage;
                ProgressValue = value.Percent;
                if (!string.IsNullOrWhiteSpace(value.CurrentPath))
                {
                    StatusText = $"{value.Stage}: {value.ProcessedFiles:N0} of {Math.Max(value.DiscoveredFiles, value.ProcessedFiles):N0} — {Shorten(value.CurrentPath, 95)}";
                }
            });

            var result = await _engine.ScanAndPlanAsync(
                WorkspacePath,
                new ScanOptions
                {
                    IncludeSubdirectories = true,
                    IncludeHiddenFiles = false,
                    HashLooseFiles = true,
                    CatalogProtectedFiles = true,
                    MaximumFiles = 100_000,
                    MaximumDepth = 32
                },
                config,
                progress,
                _cancellation.Token);

            _scan = result.Scan;
            _plan = result.Plan;
            Components.Clear();
            foreach (var component in _scan.Components.OrderByDescending(component => component.FileCount))
            {
                Components.Add(component);
            }

            PlanEntries.Clear();
            foreach (var entry in _plan.Entries)
            {
                var item = new PlanEntryViewModel(entry);
                item.PropertyChanged += OnPlanEntryPropertyChanged;
                PlanEntries.Add(item);
            }

            await LoadHistoryAsync(_cancellation.Token);
            await _engine.SaveConfigAsync(WorkspacePath, config, _cancellation.Token);
            StatusText = _scan.WasTruncated
                ? $"Scan reached the configured limit. Cataloged {FileCount:N0} files and created {ProposalCount:N0} proposals."
                : $"Scan complete. Cataloged {FileCount:N0} files in {ComponentCount:N0} components and created {ProposalCount:N0} reversible proposals.";
            ActivityLog.Insert(0, $"{DateTime.Now:T} Scan complete: {FileCount:N0} files, {ProposalCount:N0} proposals, {ConflictCount:N0} conflicts");
        }
        catch (OperationCanceledException)
        {
            StatusText = "Scan cancelled. No files were changed.";
            ActivityLog.Insert(0, $"{DateTime.Now:T} Scan cancelled");
        }
        catch (Exception exception)
        {
            StatusText = $"Scan failed: {exception.Message}";
            ActivityLog.Insert(0, $"{DateTime.Now:T} Scan failed: {exception.Message}");
        }
        finally
        {
            IsBusy = false;
            CurrentActivity = "Idle";
            RefreshSummary();
            RefreshCommands();
        }
    }

    private async Task ExecuteAsync()
    {
        if (_plan is null || SelectedCount == 0) return;
        var answer = MessageBox.Show(
            $"Move {SelectedCount:N0} selected files?\n\nA transaction journal will be written before the first move, existing files will never be overwritten, and the transaction can be undone from History.",
            "Execute reversible organization plan",
            MessageBoxButton.YesNo,
            MessageBoxImage.Warning,
            MessageBoxResult.No);
        if (answer != MessageBoxResult.Yes) return;

        _cancellation?.Dispose();
        _cancellation = new CancellationTokenSource();
        IsBusy = true;
        CurrentActivity = "Executing";
        StatusText = "Preparing durable transaction journal…";
        try
        {
            var progress = new Progress<ScanProgress>(value =>
            {
                ProgressValue = value.Percent;
                StatusText = $"{value.Stage}: {value.ProcessedFiles:N0} of {value.DiscoveredFiles:N0} — {Shorten(value.CurrentPath ?? string.Empty, 95)}";
            });
            var journal = await _engine.ExecuteAsync(_plan, progress, _cancellation.Token);
            foreach (var entry in PlanEntries) entry.Refresh();
            await LoadHistoryAsync(CancellationToken.None);
            StatusText = $"Transaction {journal.Status}: {journal.Operations.Count(operation => operation.Status == PlanEntryStatus.Completed):N0} files moved.";
            ActivityLog.Insert(0, $"{DateTime.Now:T} Transaction {journal.Id:N}: {journal.Status}");
            if (journal.Status != TransactionStatus.Completed)
            {
                MessageBox.Show(string.Join(Environment.NewLine, journal.Errors), "Transaction stopped safely", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }
        catch (OperationCanceledException)
        {
            StatusText = "Execution cancelled. Review History for the journaled transaction state.";
        }
        catch (Exception exception)
        {
            StatusText = $"Execution failed safely: {exception.Message}";
            ActivityLog.Insert(0, $"{DateTime.Now:T} Execution failed: {exception.Message}");
        }
        finally
        {
            IsBusy = false;
            CurrentActivity = "Idle";
            RefreshSummary();
            RefreshCommands();
        }
    }

    private async Task ExportAsync()
    {
        if (_plan is null) return;
        IsBusy = true;
        try
        {
            var paths = await _engine.ExportPlanAsync(_plan);
            StatusText = $"Exported plan JSON and CSV to {Path.GetDirectoryName(paths.JsonPath)}";
            ActivityLog.Insert(0, $"{DateTime.Now:T} Plan exported: {paths.JsonPath}");
        }
        catch (Exception exception)
        {
            StatusText = $"Export failed: {exception.Message}";
        }
        finally
        {
            IsBusy = false;
        }
    }

    private async Task UndoAsync()
    {
        if (SelectedJournal?.JournalPath is null) return;
        var answer = MessageBox.Show(
            $"Undo transaction {SelectedJournal.Id}?\n\nUndo runs in reverse order and will stop rather than overwrite changed files.",
            "Undo transaction",
            MessageBoxButton.YesNo,
            MessageBoxImage.Question,
            MessageBoxResult.No);
        if (answer != MessageBoxResult.Yes) return;

        _cancellation?.Dispose();
        _cancellation = new CancellationTokenSource();
        IsBusy = true;
        try
        {
            var progress = new Progress<ScanProgress>(value =>
            {
                ProgressValue = value.Percent;
                StatusText = $"{value.Stage}: {value.ProcessedFiles:N0} of {value.DiscoveredFiles:N0}";
            });
            var journal = await _engine.UndoAsync(SelectedJournal.JournalPath, progress, _cancellation.Token);
            await LoadHistoryAsync(CancellationToken.None);
            StatusText = $"Undo {journal.Status}: {journal.Operations.Count(operation => operation.Status == PlanEntryStatus.Undone):N0} files restored.";
            ActivityLog.Insert(0, $"{DateTime.Now:T} Undo {journal.Id:N}: {journal.Status}");
        }
        catch (Exception exception)
        {
            StatusText = $"Undo stopped safely: {exception.Message}";
            ActivityLog.Insert(0, $"{DateTime.Now:T} Undo failed: {exception.Message}");
        }
        finally
        {
            IsBusy = false;
            RefreshCommands();
        }
    }

    private void SelectSafe()
    {
        if (_plan is null) return;
        foreach (var entry in _plan.Entries)
        {
            entry.IsSelected = entry.Status == PlanEntryStatus.Proposed
                && entry.Risk == RiskLevel.Low
                && !entry.HasConflict
                && !entry.IsDuplicate;
        }
        foreach (var item in PlanEntries) item.Refresh();
        RefreshSummary();
        RefreshCommands();
    }

    private void ClearSelection()
    {
        foreach (var entry in PlanEntries) entry.IsSelected = false;
        RefreshSummary();
        RefreshCommands();
    }

    private void Cancel() => _cancellation?.Cancel();

    private void StartWatch()
    {
        try
        {
            WatchRecommendations.Clear();
            _watcher.Start(WorkspacePath, BuildConfig(new OrganizationConfig()));
            ActivityLog.Insert(0, $"{DateTime.Now:T} Watch started: {WorkspacePath}");
            StatusText = "Watch mode is active. New files create pending recommendations; they are not moved automatically.";
        }
        catch (Exception exception)
        {
            StatusText = $"Watch could not start: {exception.Message}";
        }
        RefreshCommands();
    }

    private void PauseOrResumeWatch()
    {
        if (_watcher.Lifecycle == WatcherLifecycle.Watching)
        {
            _watcher.Pause();
            StatusText = "Watch mode paused.";
        }
        else if (_watcher.Lifecycle == WatcherLifecycle.Paused)
        {
            _watcher.Resume();
            StatusText = "Watch mode resumed.";
        }
        RefreshCommands();
    }

    private async Task StopWatchAsync()
    {
        await _watcher.StopAsync();
        StatusText = "Watch mode stopped.";
        ActivityLog.Insert(0, $"{DateTime.Now:T} Watch stopped");
        RefreshCommands();
    }

    private void OnWatchRecommendationCreated(object? sender, WatchRecommendation recommendation)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            WatchRecommendations.Insert(0, recommendation);
            while (WatchRecommendations.Count > 500) WatchRecommendations.RemoveAt(WatchRecommendations.Count - 1);
            OnPropertyChanged(nameof(WatchPendingCount));
            StatusText = $"Watch recommendation: {Path.GetFileName(recommendation.Path)} — {recommendation.Status}";
        });
    }

    private void OnWatcherLifecycleChanged(object? sender, WatcherLifecycle lifecycle)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            WatchStatus = lifecycle.ToString();
            OnPropertyChanged(nameof(PauseWatchLabel));
            OnPropertyChanged(nameof(WatchPendingCount));
            RefreshCommands();
        });
    }

    private void OpenStateFolder()
    {
        var path = Path.Combine(WorkspacePath, ".gtangel-organizer");
        Directory.CreateDirectory(path);
        Process.Start(new ProcessStartInfo { FileName = path, UseShellExecute = true });
    }

    private async Task LoadHistoryAsync(CancellationToken cancellationToken)
    {
        var items = await _engine.LoadHistoryAsync(WorkspacePath, cancellationToken: cancellationToken);
        History.Clear();
        foreach (var item in items)
        {
            History.Add(new JournalViewModel(item));
        }
        SelectedJournal = History.FirstOrDefault();
    }

    private OrganizationConfig BuildConfig(OrganizationConfig basis) => basis with
    {
        ProposeUnknownFiles = IncludeUnknown,
        ProposeSourceFiles = IncludeSource,
        ProposeLooseRootFilesOnly = LooseRootOnly,
        ConflictResolution = RenameConflicts ? ConflictResolution.RenameWithSuffix : ConflictResolution.Review
    };

    private void OnPlanEntryPropertyChanged(object? sender, PropertyChangedEventArgs eventArgs)
    {
        if (eventArgs.PropertyName == nameof(PlanEntryViewModel.IsSelected))
        {
            RefreshSummary();
            RefreshCommands();
        }
    }

    private void RefreshSummary()
    {
        OnPropertyChanged(nameof(FileCount));
        OnPropertyChanged(nameof(ComponentCount));
        OnPropertyChanged(nameof(ProtectedCount));
        OnPropertyChanged(nameof(ProposalCount));
        OnPropertyChanged(nameof(SelectedCount));
        OnPropertyChanged(nameof(ConflictCount));
        OnPropertyChanged(nameof(DuplicateCount));
        OnPropertyChanged(nameof(ScanDuration));
    }

    private void RefreshCommands()
    {
        BrowseCommand.RaiseCanExecuteChanged();
        ScanCommand.RaiseCanExecuteChanged();
        CancelCommand.RaiseCanExecuteChanged();
        ExecuteCommand.RaiseCanExecuteChanged();
        ExportCommand.RaiseCanExecuteChanged();
        UndoCommand.RaiseCanExecuteChanged();
        SelectSafeCommand.RaiseCanExecuteChanged();
        ClearSelectionCommand.RaiseCanExecuteChanged();
        OpenStateFolderCommand.RaiseCanExecuteChanged();
        StartWatchCommand.RaiseCanExecuteChanged();
        PauseWatchCommand.RaiseCanExecuteChanged();
        StopWatchCommand.RaiseCanExecuteChanged();
    }

    public async ValueTask DisposeAsync()
    {
        _watcher.RecommendationCreated -= OnWatchRecommendationCreated;
        _watcher.LifecycleChanged -= OnWatcherLifecycleChanged;
        await _watcher.DisposeAsync();
        _cancellation?.Dispose();
    }

    private static string DetectWorkspacePath()
    {
        var commandLine = Environment.GetCommandLineArgs().Skip(1).FirstOrDefault(Directory.Exists);
        if (!string.IsNullOrWhiteSpace(commandLine)) return Path.GetFullPath(commandLine);
        var current = new DirectoryInfo(Environment.CurrentDirectory);
        while (current is not null)
        {
            if (File.Exists(Path.Combine(current.FullName, "GTAngel.sln"))) return current.FullName;
            current = current.Parent;
        }

        current = new DirectoryInfo(AppContext.BaseDirectory);
        while (current is not null)
        {
            if (File.Exists(Path.Combine(current.FullName, "GTAngel.sln"))) return current.FullName;
            current = current.Parent;
        }

        return Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
    }

    private static string Shorten(string value, int maximum)
    {
        if (value.Length <= maximum) return value;
        return "…" + value[^Math.Max(1, maximum - 1)..];
    }
}
