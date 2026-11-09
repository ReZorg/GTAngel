using System.Text;
using GTAngel.Organizer.Core.Models;
using GTAngel.Organizer.Core.Services;

namespace GTAngel.Organizer.Core;

public sealed class OrganizerEngine
{
    private readonly RepositoryScanner _scanner;
    private readonly OrganizationPlanner _planner;
    private readonly TransactionService _transactions;
    private readonly AtomicJsonStore _store;

    public OrganizerEngine(
        RepositoryScanner? scanner = null,
        OrganizationPlanner? planner = null,
        TransactionService? transactions = null,
        AtomicJsonStore? store = null)
    {
        _store = store ?? new AtomicJsonStore();
        _scanner = scanner ?? new RepositoryScanner();
        _planner = planner ?? new OrganizationPlanner();
        _transactions = transactions ?? new TransactionService(_store);
    }

    public async Task<(ScanResult Scan, OrganizationPlan Plan)> ScanAndPlanAsync(
        string rootPath,
        ScanOptions? scanOptions = null,
        OrganizationConfig? config = null,
        IProgress<ScanProgress>? progress = null,
        CancellationToken cancellationToken = default)
    {
        var scan = await _scanner.ScanAsync(rootPath, scanOptions, progress, cancellationToken).ConfigureAwait(false);
        var plan = _planner.CreatePlan(scan, config);
        return (scan, plan);
    }

    public Task<TransactionJournal> ExecuteAsync(
        OrganizationPlan plan,
        IProgress<ScanProgress>? progress = null,
        CancellationToken cancellationToken = default) =>
        _transactions.ExecuteAsync(plan, progress, cancellationToken);

    public Task<TransactionJournal> UndoAsync(
        string journalPath,
        IProgress<ScanProgress>? progress = null,
        CancellationToken cancellationToken = default) =>
        _transactions.UndoAsync(journalPath, progress, cancellationToken);

    public Task<IReadOnlyList<TransactionJournal>> LoadHistoryAsync(
        string rootPath,
        string stateDirectoryName = ".gtangel-organizer",
        CancellationToken cancellationToken = default) =>
        _transactions.LoadHistoryAsync(rootPath, stateDirectoryName, cancellationToken);

    public async Task<OrganizationConfig> LoadConfigAsync(string rootPath, CancellationToken cancellationToken = default)
    {
        var root = PathSafety.NormalizeRoot(rootPath);
        var defaultConfig = new OrganizationConfig();
        var path = Path.Combine(root, defaultConfig.StateDirectoryName, "config.json");
        return await _store.ReadAsync<OrganizationConfig>(path, cancellationToken).ConfigureAwait(false)
            ?? defaultConfig;
    }

    public Task SaveConfigAsync(string rootPath, OrganizationConfig config, CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(config);
        var root = PathSafety.NormalizeRoot(rootPath);
        var path = Path.Combine(root, config.StateDirectoryName, "config.json");
        return _store.WriteAsync(path, config, cancellationToken);
    }

    public async Task<(string JsonPath, string CsvPath)> ExportPlanAsync(
        OrganizationPlan plan,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(plan);
        var root = PathSafety.NormalizeRoot(plan.RootPath);
        var reportDirectory = Path.Combine(root, plan.Config.StateDirectoryName, "reports");
        Directory.CreateDirectory(reportDirectory);
        var prefix = $"plan-{plan.CreatedAtUtc:yyyyMMdd-HHmmss}-{plan.Id:N}";
        var jsonPath = Path.Combine(reportDirectory, prefix + ".json");
        var csvPath = Path.Combine(reportDirectory, prefix + ".csv");
        await _store.WriteAsync(jsonPath, plan, cancellationToken).ConfigureAwait(false);

        var builder = new StringBuilder();
        builder.AppendLine("Selected,Status,Risk,Type,Function,Confidence,Source,Destination,Reason,Conflict");
        foreach (var entry in plan.Entries)
        {
            builder.AppendLine(string.Join(',', new[]
            {
                Csv(entry.IsSelected.ToString()),
                Csv(entry.Status.ToString()),
                Csv(entry.Risk.ToString()),
                Csv(entry.FileType.ToString()),
                Csv(entry.Function.ToString()),
                Csv(entry.Confidence.ToString("0.00", System.Globalization.CultureInfo.InvariantCulture)),
                Csv(entry.RelativeSourcePath),
                Csv(entry.RelativeDestinationPath),
                Csv(entry.Reason),
                Csv(entry.ConflictReason ?? string.Empty)
            }));
        }

        await File.WriteAllTextAsync(csvPath, builder.ToString(), new UTF8Encoding(true), cancellationToken).ConfigureAwait(false);
        return (jsonPath, csvPath);
    }

    private static string Csv(string value) => $"\"{value.Replace("\"", "\"\"")}\"";
}
