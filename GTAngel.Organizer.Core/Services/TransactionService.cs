using System.Security.Cryptography;
using GTAngel.Organizer.Core.Models;

namespace GTAngel.Organizer.Core.Services;

public sealed class TransactionService
{
    private readonly AtomicJsonStore _store;

    public TransactionService(AtomicJsonStore? store = null)
    {
        _store = store ?? new AtomicJsonStore();
    }

    public async Task<TransactionJournal> ExecuteAsync(
        OrganizationPlan plan,
        IProgress<ScanProgress>? progress = null,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(plan);
        var root = PathSafety.NormalizeRoot(plan.RootPath);
        if (!Directory.Exists(root))
        {
            throw new DirectoryNotFoundException($"Workspace '{root}' no longer exists.");
        }

        var selected = plan.Entries
            .Where(entry => entry.IsSelected && entry.Status == PlanEntryStatus.Proposed && !entry.HasConflict && !entry.IsDuplicate)
            .ToArray();
        if (selected.Length == 0)
        {
            throw new InvalidOperationException("The plan has no executable selected entries.");
        }

        var journalDirectory = Path.Combine(root, plan.Config.StateDirectoryName, "journals");
        Directory.CreateDirectory(journalDirectory);
        var journal = new TransactionJournal
        {
            Id = Guid.NewGuid(),
            PlanId = plan.Id,
            RootPath = root,
            CreatedAtUtc = DateTime.UtcNow,
            Status = TransactionStatus.Prepared,
            Operations = selected.Select(entry => new OperationRecord
            {
                PlanEntryId = entry.Id,
                SourcePath = entry.SourcePath,
                DestinationPath = entry.DestinationPath,
                ExpectedLength = entry.ExpectedLength,
                ExpectedLastWriteTimeUtc = entry.ExpectedLastWriteTimeUtc,
                ExpectedSha256 = entry.ExpectedSha256,
                Status = PlanEntryStatus.Prepared
            }).ToList()
        };
        journal.JournalPath = Path.Combine(journalDirectory, $"{journal.CreatedAtUtc:yyyyMMdd-HHmmss}-{journal.Id:N}.json");

        await _store.WriteAsync(journal.JournalPath, journal, cancellationToken).ConfigureAwait(false);
        journal.Status = TransactionStatus.Running;
        await _store.WriteAsync(journal.JournalPath, journal, cancellationToken).ConfigureAwait(false);

        var completed = 0;
        for (var index = 0; index < journal.Operations.Count; index++)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var operation = journal.Operations[index];
            try
            {
                ValidateOperation(root, operation);
                var destinationDirectory = Path.GetDirectoryName(operation.DestinationPath)
                    ?? throw new InvalidOperationException("Destination has no parent directory.");
                Directory.CreateDirectory(destinationDirectory);
                File.Move(operation.SourcePath, operation.DestinationPath);

                operation.Status = PlanEntryStatus.Completed;
                operation.CompletedAtUtc = DateTime.UtcNow;
                completed++;
                var planEntry = plan.Entries.First(entry => entry.Id == operation.PlanEntryId);
                planEntry.Status = PlanEntryStatus.Completed;
            }
            catch (Exception exception)
            {
                operation.Status = PlanEntryStatus.Failed;
                operation.Error = exception.Message;
                journal.Errors.Add($"{operation.SourcePath}: {exception.Message}");
                var planEntry = plan.Entries.First(entry => entry.Id == operation.PlanEntryId);
                planEntry.Status = PlanEntryStatus.Failed;
                planEntry.Error = exception.Message;
                await _store.WriteAsync(journal.JournalPath, journal, CancellationToken.None).ConfigureAwait(false);
                break;
            }

            progress?.Report(new ScanProgress(
                "Executing",
                index + 1,
                journal.Operations.Count,
                operation.SourcePath,
                (index + 1) * 100.0 / journal.Operations.Count));
            await _store.WriteAsync(journal.JournalPath, journal, CancellationToken.None).ConfigureAwait(false);
        }

        journal.CompletedAtUtc = DateTime.UtcNow;
        journal.Status = completed == journal.Operations.Count
            ? TransactionStatus.Completed
            : completed > 0
                ? TransactionStatus.PartiallyCompleted
                : TransactionStatus.Failed;
        await _store.WriteAsync(journal.JournalPath, journal, CancellationToken.None).ConfigureAwait(false);
        return journal;
    }

    public async Task<TransactionJournal> UndoAsync(
        string journalPath,
        IProgress<ScanProgress>? progress = null,
        CancellationToken cancellationToken = default)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(journalPath);
        var journal = await _store.ReadAsync<TransactionJournal>(journalPath, cancellationToken).ConfigureAwait(false)
            ?? throw new InvalidDataException($"Journal '{journalPath}' could not be loaded.");
        journal.JournalPath = Path.GetFullPath(journalPath);
        var root = PathSafety.NormalizeRoot(journal.RootPath);
        if (!Directory.Exists(root))
        {
            throw new DirectoryNotFoundException($"Journal workspace '{root}' does not exist.");
        }

        journal.Status = TransactionStatus.Undoing;
        await _store.WriteAsync(journal.JournalPath, journal, cancellationToken).ConfigureAwait(false);

        var completedOperations = journal.Operations
            .Where(operation => operation.Status == PlanEntryStatus.Completed)
            .Reverse()
            .ToArray();
        var undone = 0;
        for (var index = 0; index < completedOperations.Length; index++)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var operation = completedOperations[index];
            try
            {
                ValidateUndoOperation(root, operation);
                var sourceDirectory = Path.GetDirectoryName(operation.SourcePath)
                    ?? throw new InvalidOperationException("Original source has no parent directory.");
                Directory.CreateDirectory(sourceDirectory);
                File.Move(operation.DestinationPath, operation.SourcePath);
                operation.Status = PlanEntryStatus.Undone;
                operation.UndoneAtUtc = DateTime.UtcNow;
                operation.Error = null;
                undone++;
            }
            catch (Exception exception)
            {
                operation.Error = $"Undo failed: {exception.Message}";
                journal.Errors.Add($"Undo {operation.DestinationPath}: {exception.Message}");
                await _store.WriteAsync(journal.JournalPath, journal, CancellationToken.None).ConfigureAwait(false);
                break;
            }

            progress?.Report(new ScanProgress(
                "Undoing",
                index + 1,
                completedOperations.Length,
                operation.DestinationPath,
                completedOperations.Length == 0 ? 100 : (index + 1) * 100.0 / completedOperations.Length));
            await _store.WriteAsync(journal.JournalPath, journal, CancellationToken.None).ConfigureAwait(false);
        }

        journal.UndoneAtUtc = DateTime.UtcNow;
        journal.Status = undone == completedOperations.Length
            ? TransactionStatus.Undone
            : undone > 0
                ? TransactionStatus.PartiallyUndone
                : TransactionStatus.Failed;
        await _store.WriteAsync(journal.JournalPath, journal, CancellationToken.None).ConfigureAwait(false);
        return journal;
    }

    public async Task<IReadOnlyList<TransactionJournal>> LoadHistoryAsync(
        string rootPath,
        string stateDirectoryName = ".gtangel-organizer",
        CancellationToken cancellationToken = default)
    {
        var directory = Path.Combine(PathSafety.NormalizeRoot(rootPath), stateDirectoryName, "journals");
        if (!Directory.Exists(directory)) return [];
        var result = new List<TransactionJournal>();
        foreach (var path in Directory.EnumerateFiles(directory, "*.json").OrderDescending())
        {
            cancellationToken.ThrowIfCancellationRequested();
            try
            {
                var journal = await _store.ReadAsync<TransactionJournal>(path, cancellationToken).ConfigureAwait(false);
                if (journal is not null)
                {
                    journal.JournalPath = path;
                    result.Add(journal);
                }
            }
            catch
            {
                // A malformed journal remains on disk for forensic inspection but does not block valid history.
            }
        }

        return result;
    }

    private static void ValidateOperation(string root, OperationRecord operation)
    {
        if (!PathSafety.IsContainedBy(operation.SourcePath, root) || !PathSafety.IsContainedBy(operation.DestinationPath, root))
        {
            throw new InvalidOperationException("Source or destination is outside the workspace.");
        }

        if (!File.Exists(operation.SourcePath))
        {
            throw new FileNotFoundException("Source no longer exists.", operation.SourcePath);
        }

        if (File.Exists(operation.DestinationPath) || Directory.Exists(operation.DestinationPath))
        {
            throw new IOException("Destination already exists; overwrite is forbidden.");
        }

        var source = new FileInfo(operation.SourcePath);
        if (source.Length != operation.ExpectedLength || source.LastWriteTimeUtc != operation.ExpectedLastWriteTimeUtc)
        {
            throw new IOException("Source changed after planning; execution stopped.");
        }

        if (!string.IsNullOrWhiteSpace(operation.ExpectedSha256)
            && !string.Equals(ComputeSha256(operation.SourcePath), operation.ExpectedSha256, StringComparison.OrdinalIgnoreCase))
        {
            throw new IOException("Source content hash changed after planning; execution stopped.");
        }
    }

    private static void ValidateUndoOperation(string root, OperationRecord operation)
    {
        if (!PathSafety.IsContainedBy(operation.SourcePath, root) || !PathSafety.IsContainedBy(operation.DestinationPath, root))
        {
            throw new InvalidOperationException("Journal path is outside the workspace.");
        }

        if (!File.Exists(operation.DestinationPath))
        {
            throw new FileNotFoundException("Moved destination no longer exists.", operation.DestinationPath);
        }

        if (File.Exists(operation.SourcePath) || Directory.Exists(operation.SourcePath))
        {
            throw new IOException("Original source path is occupied; undo will not overwrite it.");
        }

        var destination = new FileInfo(operation.DestinationPath);
        if (destination.Length != operation.ExpectedLength)
        {
            throw new IOException("Moved file length changed; undo stopped.");
        }

        if (!string.IsNullOrWhiteSpace(operation.ExpectedSha256)
            && !string.Equals(ComputeSha256(operation.DestinationPath), operation.ExpectedSha256, StringComparison.OrdinalIgnoreCase))
        {
            throw new IOException("Moved file content changed; undo stopped.");
        }
    }

    private static string ComputeSha256(string path)
    {
        using var stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read, 131_072, FileOptions.SequentialScan);
        return Convert.ToHexString(SHA256.HashData(stream));
    }
}
