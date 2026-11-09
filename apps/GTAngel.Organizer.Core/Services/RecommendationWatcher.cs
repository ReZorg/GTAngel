using System.Collections.Concurrent;
using System.Security.Cryptography;
using GTAngel.Organizer.Core.Models;

namespace GTAngel.Organizer.Core.Services;

public sealed class RecommendationWatcher : IAsyncDisposable
{
    private sealed record PendingChange(string Path, string ChangeType, DateTime LastSeenUtc);

    private readonly FileClassifier _classifier;
    private readonly ConcurrentDictionary<string, PendingChange> _pending = new(StringComparer.OrdinalIgnoreCase);
    private readonly SemaphoreSlim _processingGate = new(1, 1);
    private FileSystemWatcher? _watcher;
    private Timer? _timer;
    private string? _root;
    private OrganizationConfig _config = new();
    private WatcherLifecycle _lifecycle = WatcherLifecycle.Stopped;

    public RecommendationWatcher(FileClassifier? classifier = null)
    {
        _classifier = classifier ?? new FileClassifier();
    }

    public event EventHandler<WatchRecommendation>? RecommendationCreated;
    public event EventHandler<WatcherLifecycle>? LifecycleChanged;

    public WatcherLifecycle Lifecycle => _lifecycle;
    public int PendingCount => _pending.Count;

    public void Start(string rootPath, OrganizationConfig? config = null)
    {
        if (_lifecycle is not WatcherLifecycle.Stopped and not WatcherLifecycle.Faulted)
        {
            throw new InvalidOperationException($"Watcher is already {_lifecycle}.");
        }

        _root = PathSafety.NormalizeRoot(rootPath);
        if (!Directory.Exists(_root)) throw new DirectoryNotFoundException(_root);
        _config = config ?? new OrganizationConfig();
        SetLifecycle(WatcherLifecycle.Starting);

        try
        {
            _watcher = new FileSystemWatcher(_root)
            {
                IncludeSubdirectories = !_config.WatchLooseRootOnly,
                NotifyFilter = NotifyFilters.FileName | NotifyFilters.CreationTime | NotifyFilters.LastWrite | NotifyFilters.Size,
                Filter = "*.*",
                InternalBufferSize = 32 * 1024,
                EnableRaisingEvents = false
            };
            _watcher.Created += OnCreated;
            _watcher.Changed += OnChanged;
            _watcher.Renamed += OnRenamed;
            _watcher.Error += OnError;
            _timer = new Timer(_ => _ = ProcessPendingAsync(), null, 500, 500);
            _watcher.EnableRaisingEvents = true;
            SetLifecycle(WatcherLifecycle.Watching);
        }
        catch
        {
            SetLifecycle(WatcherLifecycle.Faulted);
            DisposeWatcher();
            throw;
        }
    }

    public void Pause()
    {
        if (_lifecycle != WatcherLifecycle.Watching) return;
        if (_watcher is not null) _watcher.EnableRaisingEvents = false;
        SetLifecycle(WatcherLifecycle.Paused);
    }

    public void Resume()
    {
        if (_lifecycle != WatcherLifecycle.Paused) return;
        if (_watcher is not null) _watcher.EnableRaisingEvents = true;
        SetLifecycle(WatcherLifecycle.Watching);
    }

    public async Task StopAsync()
    {
        if (_lifecycle == WatcherLifecycle.Stopped) return;
        SetLifecycle(WatcherLifecycle.Stopping);
        if (_watcher is not null) _watcher.EnableRaisingEvents = false;
        _timer?.Change(Timeout.Infinite, Timeout.Infinite);
        await _processingGate.WaitAsync().ConfigureAwait(false);
        _processingGate.Release();
        _pending.Clear();
        DisposeWatcher();
        SetLifecycle(WatcherLifecycle.Stopped);
    }

    private void OnCreated(object sender, FileSystemEventArgs eventArgs) => Queue(eventArgs.FullPath, "Created");
    private void OnChanged(object sender, FileSystemEventArgs eventArgs) => Queue(eventArgs.FullPath, "Changed");
    private void OnRenamed(object sender, RenamedEventArgs eventArgs) => Queue(eventArgs.FullPath, "Renamed");

    private void Queue(string path, string changeType)
    {
        if (_lifecycle != WatcherLifecycle.Watching || _root is null) return;
        if (Directory.Exists(path) || !PathSafety.IsContainedBy(path, _root)) return;
        if (ContainsExcludedSegment(path, _root)) return;
        _pending[path] = new PendingChange(path, changeType, DateTime.UtcNow);
    }

    private async Task ProcessPendingAsync()
    {
        if (_lifecycle != WatcherLifecycle.Watching || _root is null) return;
        if (!await _processingGate.WaitAsync(0).ConfigureAwait(false)) return;
        try
        {
            var cutoff = DateTime.UtcNow.AddMilliseconds(-Math.Max(250, _config.WatchDebounceMilliseconds));
            var ready = _pending.Values.Where(change => change.LastSeenUtc <= cutoff).ToArray();
            foreach (var change in ready)
            {
                if (!_pending.TryRemove(change.Path, out _)) continue;
                try
                {
                    if (!File.Exists(change.Path)) continue;
                    var file = new FileInfo(change.Path);
                    if (!IsStable(file))
                    {
                        _pending[change.Path] = change with { LastSeenUtc = DateTime.UtcNow };
                        continue;
                    }

                    var isManifest = PathSafety.TryGetManifest(change.Path, out _, out _);
                    var isRootStructural = string.Equals(file.DirectoryName, _root, StringComparison.OrdinalIgnoreCase)
                        && IsStructuralFile(file.Name);
                    var isProtected = isManifest || isRootStructural;
                    var hash = string.Equals(file.DirectoryName, _root, StringComparison.OrdinalIgnoreCase) && !isProtected
                        ? ComputeSha256(file.FullName)
                        : null;
                    var classification = _classifier.Classify(
                        file,
                        _root,
                        file.DirectoryName,
                        isProtected,
                        isManifest ? "project manifest" : isRootStructural ? "workspace structural file" : null,
                        hash);
                    RecommendationCreated?.Invoke(this, new WatchRecommendation(
                        DateTime.UtcNow,
                        change.Path,
                        change.ChangeType,
                        classification,
                        isProtected ? "Protected" : "Pending review"));
                }
                catch (Exception exception)
                {
                    RecommendationCreated?.Invoke(this, new WatchRecommendation(
                        DateTime.UtcNow,
                        change.Path,
                        change.ChangeType,
                        null,
                        "Failed",
                        exception.Message));
                }
            }
        }
        finally
        {
            _processingGate.Release();
        }
    }

    private void OnError(object sender, ErrorEventArgs eventArgs)
    {
        SetLifecycle(WatcherLifecycle.Faulted);
        RecommendationCreated?.Invoke(this, new WatchRecommendation(
            DateTime.UtcNow,
            _root ?? string.Empty,
            "Watcher error",
            null,
            "Failed",
            eventArgs.GetException().Message));
    }

    private void SetLifecycle(WatcherLifecycle value)
    {
        _lifecycle = value;
        LifecycleChanged?.Invoke(this, value);
    }

    private void DisposeWatcher()
    {
        _timer?.Dispose();
        _timer = null;
        if (_watcher is not null)
        {
            _watcher.Created -= OnCreated;
            _watcher.Changed -= OnChanged;
            _watcher.Renamed -= OnRenamed;
            _watcher.Error -= OnError;
            _watcher.Dispose();
            _watcher = null;
        }
    }

    private static bool ContainsExcludedSegment(string path, string root)
    {
        var relative = Path.GetRelativePath(root, path);
        return relative.Split(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar)
            .Any(PathSafety.ExcludedDirectoryNames.Contains);
    }

    private static bool IsStable(FileInfo file)
    {
        try
        {
            using var stream = new FileStream(file.FullName, FileMode.Open, FileAccess.Read, FileShare.Read);
            return stream.Length == file.Length;
        }
        catch (IOException)
        {
            return false;
        }
    }

    private static bool IsStructuralFile(string name) =>
        name.Equals("README.md", StringComparison.OrdinalIgnoreCase)
        || name.Equals("CHANGELOG.md", StringComparison.OrdinalIgnoreCase)
        || name.Equals("AGENTS.md", StringComparison.OrdinalIgnoreCase)
        || name.Equals(".gitignore", StringComparison.OrdinalIgnoreCase)
        || name.Equals(".editorconfig", StringComparison.OrdinalIgnoreCase)
        || name.Equals("global.json", StringComparison.OrdinalIgnoreCase);

    private static string ComputeSha256(string path)
    {
        using var stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read, 131_072, FileOptions.SequentialScan);
        return Convert.ToHexString(SHA256.HashData(stream));
    }

    public async ValueTask DisposeAsync()
    {
        await StopAsync().ConfigureAwait(false);
        _processingGate.Dispose();
    }
}
