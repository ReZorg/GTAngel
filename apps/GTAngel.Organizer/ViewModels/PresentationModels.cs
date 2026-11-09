using GTAngel.Organizer.Core.Models;
using GTAngel.Organizer.Mvvm;

namespace GTAngel.Organizer.ViewModels;

public sealed class PlanEntryViewModel : ObservableObject
{
    private readonly MovePlanEntry _entry;

    public PlanEntryViewModel(MovePlanEntry entry)
    {
        _entry = entry;
    }

    public Guid Id => _entry.Id;
    public string Source => _entry.RelativeSourcePath;
    public string Destination => _entry.RelativeDestinationPath;
    public string Type => _entry.FileType.ToString();
    public string Function => _entry.Function.ToString();
    public string Risk => _entry.Risk.ToString();
    public string Confidence => $"{_entry.Confidence:P0}";
    public string Reason => _entry.Reason;
    public string Status => _entry.Status.ToString();
    public bool HasConflict => _entry.HasConflict;
    public bool IsDuplicate => _entry.IsDuplicate;
    public string Conflict => _entry.ConflictReason ?? string.Empty;

    public bool IsSelected
    {
        get => _entry.IsSelected;
        set
        {
            if (_entry.IsSelected == value) return;
            _entry.IsSelected = value;
            OnPropertyChanged();
        }
    }

    public void Refresh()
    {
        OnPropertyChanged(nameof(IsSelected));
        OnPropertyChanged(nameof(Status));
        OnPropertyChanged(nameof(Conflict));
    }
}

public sealed class JournalViewModel
{
    public JournalViewModel(TransactionJournal journal)
    {
        Journal = journal;
    }

    public TransactionJournal Journal { get; }
    public string Id => Journal.Id.ToString("N")[..8];
    public string Created => Journal.CreatedAtUtc.ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss");
    public string Status => Journal.Status.ToString();
    public int Operations => Journal.Operations.Count;
    public int Completed => Journal.Operations.Count(operation => operation.Status == PlanEntryStatus.Completed);
    public int Undone => Journal.Operations.Count(operation => operation.Status == PlanEntryStatus.Undone);
    public string Errors => string.Join(" | ", Journal.Errors);
    public string? JournalPath => Journal.JournalPath;
}
