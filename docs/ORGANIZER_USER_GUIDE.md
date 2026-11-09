# GTAngel Repository Organizer User Guide

**Application:** GTAngel Repository Organizer 1.0  
**Default workspace:** `C:\ddd\GTAngel`

![GTAngel Repository Organizer dashboard](../tools/organizer-ui.png)

## Launch

Double-click [`Launch-GTAngel-Organizer.cmd`](../Launch-GTAngel-Organizer.cmd) from the GTAngel folder. The launcher opens the self-contained Windows executable at:

```text
dist\GTAngel.RepositoryOrganizer\win-x64\GTAngel.RepositoryOrganizer.exe
```

The executable can also be started directly. Pass a repository path as the first argument to select a different workspace:

```powershell
.\dist\GTAngel.RepositoryOrganizer\win-x64\GTAngel.RepositoryOrganizer.exe "C:\path\to\workspace"
```

## Safe operating workflow

> **The application does not move anything during scanning.** A move occurs only after a plan exists, entries are selected, and the user confirms execution.

| Step | Action | Safety effect |
|---|---|---|
| 1 | Choose a workspace and click **Scan & Plan** | Performs read-only discovery and classification |
| 2 | Inspect **Overview** and **Components** | Confirms detected Git, .NET, Node, Python, C/C++, Unreal, installer, documentation, and asset boundaries |
| 3 | Open **Plan** | Reviews source, type, function, risk, confidence, destination, status, and reasons |
| 4 | Click **Select safe** or choose entries individually | Restricts execution to explicitly selected actions |
| 5 | Click **Export JSON + CSV** when a review artifact is needed | Writes a non-destructive report under `.gtangel-organizer\reports` |
| 6 | Click **Execute selected** and confirm | Writes a journal first, revalidates sources, and performs conflict-free moves |
| 7 | Review **History** | Displays durable transaction status and any errors |
| 8 | Select a completed transaction and click **Undo selected transaction** if required | Restores completed moves in reverse order without overwriting newer files |

## Tabs

The **Overview** tab presents the component registry, safety contract, and activity log. The **Plan** tab is the approval surface for proposed moves. The **Components** tab exposes nested component roots, types, file counts, protected counts, Git state, and remotes. The **History** tab lists transaction journals and provides undo. The **Watch** tab manages event-driven recommendations. The **Settings** tab controls planning scope and conflict behavior.

## Default classification destinations

| Type | Default destination |
|---|---|
| Documentation | `docs\general`, with `research` where function evidence supports it |
| PDFs and office references | `docs\reference\research` |
| Images | `assets\images` |
| Audio and video | `assets\media` |
| 3D assets | `assets\3d` |
| Scripts and automation | `tools\scripts` |
| Configuration | `config`, with functional subdirectories where useful |
| Data | `data` |
| Archives | `archives` |
| Binaries | `artifacts\binaries` |
| Unknown files | `other\review`, disabled by default |

Source files are excluded from move proposals by default. Existing Git roots, project manifests, Unreal projects and plugins, generated outputs, dependencies, caches, README/CHANGELOG files, and organizer state are protected.

## Conflicts and duplicates

Existing destinations are never overwritten. With the default **Review** policy, a destination collision becomes a blocked conflict. When **Resolve destination conflicts with numeric suffixes** is enabled, the plan generates deterministic names such as `logo (2).png`. Identical SHA-256 content becomes a duplicate review entry and is not preselected.

## Watched-folder mode

Click **Start watch** to monitor the selected workspace. New and renamed files are debounced until stable, classified, and added to the recommendation grid. Watch mode defaults to the loose workspace root and **never moves files automatically**. Use **Pause watch**, **Resume watch**, and **Stop watch** to control lifecycle state.

To act on watched files, stop or pause watch and run **Scan & Plan**. This ensures every proposed move enters the same preview, conflict, journal, and undo pipeline.

## State directory

The application creates `.gtangel-organizer` only when configuration, reports, or transactions need persistence.

```text
.gtangel-organizer\
├── config.json
├── journals\
│   └── YYYYMMDD-HHMMSS-<transaction-id>.json
└── reports\
    ├── plan-<timestamp>-<plan-id>.json
    └── plan-<timestamp>-<plan-id>.csv
```

Journal files are audit records. Do not delete a journal while its transaction may still need undo.

## Rebuild and verify

Run the reproducible build script from PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\Build-Organizer.ps1
```

The script repairs empty Windows program-folder environment variables that can break NuGet in remote shells, restores dependencies, builds the application, runs deterministic safety tests, and publishes the self-contained executable.

Run the tests independently with:

```powershell
dotnet run --project .\GTAngel.Organizer.Tests\GTAngel.Organizer.Tests.csproj
```

Run a read-only scan from the test harness with:

```powershell
dotnet run --project .\GTAngel.Organizer.Tests\GTAngel.Organizer.Tests.csproj -- --scan-only "C:\ddd\GTAngel"
```

## Troubleshooting

| Symptom | Resolution |
|---|---|
| **Scan & Plan** is disabled | Confirm that the workspace path exists and no scan or transaction is active |
| Scan reaches 100,000 files | The current safety cap was reached; the partial catalog remains valid and no files were changed |
| Entry shows `Conflict` | Remove or rename the destination, edit policy to use numeric suffixes, then rescan |
| Entry shows `Duplicate` | Review the identical files manually; duplicates are intentionally not preselected |
| Execution stops | Open **History** and inspect the journal error; the engine stops on stale sources or unexpected destinations |
| Undo stops | Confirm the original source path is unoccupied and the moved destination has not changed |
| Watch shows `Faulted` | Stop watch, verify access to the workspace, and restart it |
| Launcher cannot find the executable | Run `tools\Build-Organizer.ps1` to republish |

## References

[1]: ORGANIZER_ARCHITECTURE.md "Organizer architecture and safety model"
[2]: ../tools/ORGANIZER_TEST_REPORT.md "Organizer verification report"
[3]: ../tools/component-integration-analysis.md "Component integration analysis"
