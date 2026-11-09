using System.Text.Json;
using System.Text.Json.Serialization;
using GTAngel.Organizer.Core;
using GTAngel.Organizer.Core.Models;
using GTAngel.Organizer.Core.Services;

if (args.Length >= 2 && args[0].Equals("--scan-only", StringComparison.OrdinalIgnoreCase))
{
    await RunRealScanAsync(args[1]);
    return;
}

var tests = new (string Name, Func<Task> Run)[]
{
    ("classification and protected boundaries", TestClassificationAndBoundariesAsync),
    ("duplicate and conflict planning", TestDuplicateAndConflictPlanningAsync),
    ("journaled execution and undo", TestExecuteAndUndoAsync),
    ("stale-plan fail-fast recovery", TestStalePlanAsync),
    ("watched-folder pending recommendation", TestWatcherAsync)
};

var failures = new List<string>();
foreach (var test in tests)
{
    try
    {
        await test.Run();
        Console.WriteLine($"PASS | {test.Name}");
    }
    catch (Exception exception)
    {
        failures.Add($"{test.Name}: {exception.Message}");
        Console.WriteLine($"FAIL | {test.Name} | {exception}");
    }
}

Console.WriteLine($"RESULT | passed={tests.Length - failures.Count} failed={failures.Count}");
if (failures.Count > 0)
{
    Environment.ExitCode = 1;
}

static async Task TestClassificationAndBoundariesAsync()
{
    using var workspace = new TemporaryWorkspace();
    workspace.Write("README.md", "# Protected workspace readme");
    workspace.Write("avatar.png", "image-content");
    workspace.Write("paper.pdf", "pdf-content");
    workspace.Write("organize.ps1", "Write-Output 'safe'");
    workspace.Write("module/package.json", "{\"name\":\"module\"}");
    workspace.Write("module/src/service.ts", "export const service = true;");
    workspace.Write("plugin/Cognitive.uplugin", "{}");
    workspace.Write("plugin/Source/Cognitive.cpp", "int main() { return 0; }");
    Directory.CreateDirectory(Path.Combine(workspace.Root, "node_modules"));
    workspace.Write("node_modules/ignored.js", "ignored");

    var engine = new OrganizerEngine();
    var (scan, plan) = await engine.ScanAndPlanAsync(workspace.Root);

    Assert(scan.Files.All(file => !file.RelativePath.Contains("node_modules", StringComparison.OrdinalIgnoreCase)), "Dependency directory was not excluded.");
    Assert(scan.Components.Any(component => component.Name == "module" && component.Kind == ComponentKind.NodeProject), "Node component was not detected.");
    Assert(scan.Components.Any(component => component.Name == "plugin" && component.Kind == ComponentKind.UnrealPlugin), "Unreal plugin was not detected.");
    Assert(scan.Files.Single(file => file.RelativePath == "README.md").IsProtected, "Workspace README must be protected.");
    Assert(scan.Files.Single(file => file.RelativePath.Replace('\\', '/') == "module/src/service.ts").IsProtected, "Project source must be protected.");
    Assert(plan.Entries.Any(entry => entry.RelativeSourcePath == "avatar.png" && entry.RelativeDestinationPath.Replace('\\', '/') == "assets/images/avatar.png"), "Image proposal is incorrect.");
    Assert(plan.Entries.Any(entry => entry.RelativeSourcePath == "paper.pdf" && entry.RelativeDestinationPath.Replace('\\', '/') == "docs/reference/research/paper.pdf"), "Reference proposal is incorrect.");
    Assert(plan.Entries.Any(entry => entry.RelativeSourcePath == "organize.ps1" && entry.RelativeDestinationPath.Replace('\\', '/') == "tools/scripts/organize.ps1"), "Automation proposal is incorrect.");
    Assert(plan.Entries.All(entry => !entry.RelativeSourcePath.Contains("module", StringComparison.OrdinalIgnoreCase)), "Protected project files entered the move plan.");
}

static async Task TestDuplicateAndConflictPlanningAsync()
{
    using var workspace = new TemporaryWorkspace();
    workspace.Write("copy-a.pdf", "same");
    workspace.Write("copy-b.pdf", "same");
    workspace.Write("logo.png", "new-content");
    workspace.Write("assets/images/logo.png", "existing-content");

    var engine = new OrganizerEngine();
    var (scan, reviewPlan) = await engine.ScanAndPlanAsync(workspace.Root);
    var duplicateEntries = reviewPlan.Entries.Where(entry => entry.RelativeSourcePath.StartsWith("copy-", StringComparison.OrdinalIgnoreCase)).ToArray();
    Assert(duplicateEntries.Length == 2 && duplicateEntries.All(entry => entry.IsDuplicate && !entry.IsSelected), "Duplicate root files must be review-only.");
    var conflict = reviewPlan.Entries.Single(entry => entry.RelativeSourcePath == "logo.png");
    Assert(conflict.HasConflict && conflict.Status == PlanEntryStatus.Conflict && !conflict.IsSelected, "Existing destination conflict was not blocked.");

    var renamePlan = new OrganizationPlanner().CreatePlan(scan, new OrganizationConfig { ConflictResolution = ConflictResolution.RenameWithSuffix });
    var renamed = renamePlan.Entries.Single(entry => entry.RelativeSourcePath == "logo.png");
    Assert(!renamed.HasConflict && renamed.RelativeDestinationPath.Replace('\\', '/').EndsWith("assets/images/logo (2).png", StringComparison.OrdinalIgnoreCase), "Numeric conflict resolution was not deterministic.");
}

static async Task TestExecuteAndUndoAsync()
{
    using var workspace = new TemporaryWorkspace();
    workspace.Write("portrait.png", "reversible-content");
    var engine = new OrganizerEngine();
    var (_, plan) = await engine.ScanAndPlanAsync(workspace.Root);
    var entry = plan.Entries.Single(item => item.RelativeSourcePath == "portrait.png");
    entry.IsSelected = true;

    var journal = await engine.ExecuteAsync(plan);
    Assert(journal.Status == TransactionStatus.Completed, "Transaction did not complete.");
    Assert(File.Exists(Path.Combine(workspace.Root, "assets", "images", "portrait.png")), "Destination file was not created.");
    Assert(!File.Exists(Path.Combine(workspace.Root, "portrait.png")), "Source file still exists after move.");
    Assert(journal.JournalPath is not null && File.Exists(journal.JournalPath), "Durable journal was not written.");

    var undone = await engine.UndoAsync(journal.JournalPath!);
    Assert(undone.Status == TransactionStatus.Undone, "Undo did not complete.");
    Assert(File.Exists(Path.Combine(workspace.Root, "portrait.png")), "Original file was not restored.");
    Assert(!File.Exists(Path.Combine(workspace.Root, "assets", "images", "portrait.png")), "Moved destination remains after undo.");
}

static async Task TestStalePlanAsync()
{
    using var workspace = new TemporaryWorkspace();
    workspace.Write("stale.png", "original");
    var engine = new OrganizerEngine();
    var (_, plan) = await engine.ScanAndPlanAsync(workspace.Root);
    var entry = plan.Entries.Single(item => item.RelativeSourcePath == "stale.png");
    entry.IsSelected = true;
    await Task.Delay(1100);
    File.AppendAllText(entry.SourcePath, "-changed");

    var journal = await engine.ExecuteAsync(plan);
    Assert(journal.Status == TransactionStatus.Failed, "Stale plan should fail before moving changed content.");
    Assert(File.Exists(entry.SourcePath), "Changed source was moved despite stale-plan protection.");
    Assert(!File.Exists(entry.DestinationPath), "Destination exists after stale-plan failure.");
    Assert(journal.Errors.Any(error => error.Contains("changed after planning", StringComparison.OrdinalIgnoreCase)), "Stale-plan reason was not journaled.");
}

static async Task TestWatcherAsync()
{
    using var workspace = new TemporaryWorkspace();
    await using var watcher = new RecommendationWatcher();
    var completion = new TaskCompletionSource<WatchRecommendation>(TaskCreationOptions.RunContinuationsAsynchronously);
    watcher.RecommendationCreated += (_, recommendation) =>
    {
        if (Path.GetFileName(recommendation.Path).Equals("watched.png", StringComparison.OrdinalIgnoreCase))
        {
            completion.TrySetResult(recommendation);
        }
    };

    watcher.Start(workspace.Root, new OrganizationConfig { WatchDebounceMilliseconds = 300, WatchLooseRootOnly = true });
    workspace.Write("watched.png", "watch-content");
    var finished = await Task.WhenAny(completion.Task, Task.Delay(TimeSpan.FromSeconds(8)));
    Assert(finished == completion.Task, "Watcher did not emit a debounced recommendation.");
    var recommendation = await completion.Task;
    Assert(recommendation.Status == "Pending review", "Watcher should create a pending review recommendation.");
    Assert(recommendation.Classification?.FileType == FileTypeCategory.Image, "Watcher classification is incorrect.");
    Assert(File.Exists(Path.Combine(workspace.Root, "watched.png")), "Watcher moved a file without approval.");
    Assert(!File.Exists(Path.Combine(workspace.Root, "assets", "images", "watched.png")), "Watcher created a destination without approval.");
    await watcher.StopAsync();
    Assert(watcher.Lifecycle == WatcherLifecycle.Stopped, "Watcher lifecycle did not stop cleanly.");
}

static async Task RunRealScanAsync(string rootPath)
{
    var engine = new OrganizerEngine();
    var progress = new Progress<ScanProgress>(value =>
    {
        if (value.ProcessedFiles % 5000 == 0 || value.Percent >= 100)
        {
            Console.Error.WriteLine($"PROGRESS | {value.Stage} | {value.ProcessedFiles:N0} | {value.Percent:0.0}%");
        }
    });
    var (scan, plan) = await engine.ScanAndPlanAsync(
        rootPath,
        new ScanOptions { MaximumFiles = 100_000, MaximumDepth = 32, HashLooseFiles = true },
        new OrganizationConfig(),
        progress);
    var summary = new
    {
        scan.RootPath,
        files = scan.Files.Count,
        components = scan.Components.Count,
        protectedFiles = scan.ProtectedFileCount,
        looseFiles = scan.LooseFileCount,
        proposals = plan.Entries.Count,
        selected = plan.SelectedCount,
        conflicts = plan.ConflictCount,
        duplicates = plan.DuplicateCount,
        scan.WasTruncated,
        warnings = scan.Warnings.Count,
        componentsDetail = scan.Components.OrderBy(component => component.RootPath, StringComparer.OrdinalIgnoreCase).Select(component => new
        {
            component.Name,
            relativeRoot = Path.GetRelativePath(scan.RootPath, component.RootPath),
            kind = component.Kind.ToString(),
            component.FileCount,
            component.ProtectedFileCount,
            component.TotalBytes,
            component.IsRepository,
            component.GitBranch,
            component.GitRemote,
            component.Technologies,
            manifests = component.Manifests.Select(manifest => new
            {
                manifest.Name,
                kind = manifest.Kind.ToString(),
                manifest.Technology,
                relativePath = Path.GetRelativePath(scan.RootPath, manifest.Path)
            })
        }),
        proposalBreakdown = plan.Entries.GroupBy(entry => entry.FileType).ToDictionary(group => group.Key.ToString(), group => group.Count())
    };
    var options = new JsonSerializerOptions { WriteIndented = true, Converters = { new JsonStringEnumConverter() } };
    Console.WriteLine(JsonSerializer.Serialize(summary, options));
}

static void Assert(bool condition, string message)
{
    if (!condition) throw new InvalidOperationException(message);
}

sealed class TemporaryWorkspace : IDisposable
{
    public TemporaryWorkspace()
    {
        Root = Path.Combine(Path.GetTempPath(), "GTAngel.Organizer.Tests", Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(Root);
    }

    public string Root { get; }

    public void Write(string relativePath, string content)
    {
        var path = Path.Combine(Root, relativePath.Replace('/', Path.DirectorySeparatorChar));
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllText(path, content);
    }

    public void Dispose()
    {
        if (!Directory.Exists(Root)) return;
        try
        {
            Directory.Delete(Root, true);
        }
        catch
        {
            // Test failures report their state; cleanup is best-effort on Windows filesystem watcher delays.
        }
    }
}
