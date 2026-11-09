# GTAngel Repository Organizer Architecture

**Author:** Manus AI  
**Target:** Windows 10/11, .NET 8, WPF  
**Solution:** `GTAngel.sln`

## Purpose

The GTAngel Repository Organizer is a local desktop application that catalogs fragmented components, classifies files by **type** and **function**, proposes an explainable organization plan, and executes approved moves as reversible transactions. It is designed for the mixed .NET, C++, Unreal Engine, Node/TypeScript, Python, documentation, media, and archive content already present in the workspace.[1]

> **Primary invariant:** scanning is read-only, planning is non-destructive, and execution is journaled and reversible.

## Selected architecture

| Project | Target | Responsibility |
|---|---|---|
| `GTAngel.Organizer.Core` | `net8.0` | Repository discovery, manifest detection, classification, duplicate hashing, plan generation, conflict handling, transaction journaling, undo, configuration, and watch-event processing |
| `GTAngel.Organizer` | `net8.0-windows` with WPF | Native Windows dashboard, path selection, scan progress, filters, component registry, plan approval, execution, history, undo, and settings |
| `GTAngel.Organizer.Tests` | `net8.0` | Deterministic unit and integration tests using temporary directories |

This separation keeps the file engine independent from the desktop shell. The existing GTAngel project already uses .NET 8, WPF, MVVM, dependency injection, and Windows packaging, making a native sidecar project the most compatible implementation.[1] The registry, lifecycle, bounded-work, profiling, and approval concepts are adapted from the local AI Integration Hub rather than importing its Electron/Supabase dependencies.[2]

## Core domain model

| Model | Key fields | Purpose |
|---|---|---|
| `RepositoryComponent` | Name, root, kind, manifests, technology, repository status, file counts, health | Represents one nested repository, project, plugin, module, content collection, or loose-file group |
| `FileClassification` | Source path, file type, functional role, confidence, reasons, protected status | Records explainable classification evidence |
| `OrganizationRule` | Match conditions, target template, priority, enabled, safety level | Defines deterministic classification and destination behavior |
| `MovePlanEntry` | Source, destination, selected, conflict policy, status | Represents one reviewable proposed action |
| `OrganizationPlan` | Plan ID, root, entries, warnings, statistics | Immutable preview produced by a scan |
| `TransactionJournal` | Transaction ID, timestamps, plan hash, operation records | Durable audit and recovery record |
| `OperationRecord` | Source, destination, hashes, timestamps, status, error | Tracks one move through prepared, completed, failed, or undone states |

## Classification taxonomy

The classifier combines extension evidence, filename conventions, path tokens, and manifest context. Rules are deterministic and ordered. Every result includes confidence and human-readable reasons.

| File type | Typical evidence | Default target for loose files |
|---|---|---|
| Source | `.cs`, `.cpp`, `.h`, `.hpp`, `.py`, `.ts`, `.tsx`, `.js`, `.rs`, `.go`, `.java` | Catalog only unless the user selects a non-project intake folder |
| Tests | `tests`, `spec`, `__tests__`, or test-name suffixes | Catalog only inside projects |
| Documentation | `.md`, `.rst`, `.adoc`, documentation path | `docs/general` |
| Reference material | `.pdf`, `.docx`, `.xlsx`, `.pptx` | `docs/reference` |
| Images | `.png`, `.jpg`, `.jpeg`, `.gif`, `.svg`, `.webp` | `assets/images` |
| Audio/video | `.wav`, `.mp3`, `.mp4`, `.webm`, `.mov` | `assets/media` |
| 3D/game assets | `.fbx`, `.glb`, `.gltf`, `.uasset`, `.umap` | Catalog only near Unreal projects; otherwise `assets/3d` |
| Automation | `.ps1`, `.sh`, `.bat`, `.cmd` | `tools/scripts` |
| Configuration | `.json`, `.yaml`, `.yml`, `.toml`, `.ini`, `.xml`, `.props`, `.targets`, `.env*` | Catalog only when manifest-adjacent; otherwise `config` |
| Data | `.csv`, `.tsv`, `.parquet`, `.db`, `.sqlite` | `data` |
| Archives | `.zip`, `.7z`, `.rar`, `.tar`, `.gz` | `archives` |
| Unknown | No reliable rule | `other/review` only after approval |

Functional roles refine the type into **UI**, **service**, **domain model**, **integration/bridge**, **plugin/module**, **build**, **installer**, **automation**, **test**, **documentation**, **research**, **asset**, **configuration**, **data**, or **unknown**. The approach follows the purpose-oriented catalog pattern found in the user-owned Togai repository.[3]

## Boundary and protection rules

| Boundary | Detection | Default behavior |
|---|---|---|
| Nested Git repository | `.git` file or directory | Register the component; never move files across its root |
| .NET project or solution | `.sln`, `.csproj`, `.fsproj`, `.vbproj`, `.wixproj` | Preserve project-relative files and manifests |
| Node project | `package.json`, lockfiles | Preserve manifest, source tree, and dependency boundary |
| Python project | `pyproject.toml`, `requirements.txt`, virtual environment markers | Preserve project tree and ignore environments/caches |
| C/C++ project | `CMakeLists.txt`, `Makefile`, `.vcxproj` | Preserve source/include/build relationships |
| Unreal project or plugin | `.uproject`, `.uplugin` | Preserve Content, Config, Plugins, Source, and module relationships |
| Generated or dependency directory | `bin`, `obj`, `node_modules`, `build`, `dist`, `Binaries`, `Intermediate`, `Saved`, caches | Exclude from proposals and optionally from scans |
| Organizer state | `.gtangel-organizer` | Exclude from normal classification and protect from self-movement |

Nested workspaces are treated as dependency graphs rather than flattened folders, consistent with the boundary-preserving workspace pattern in LoopWorkspace.[4] Component relationships are exposed as filterable parent/child and manifest links, drawing on explorer-style filtering concepts.[5]

## Planning and conflict policy

The safe default scope is **loose root files only**. A scan may include the entire workspace for cataloging, but move proposals are generated only for files outside protected boundaries. Destinations are relative to the selected workspace and remain on the same volume by default.

The engine never overwrites an existing file. When a destination exists, it first compares size and SHA-256. Identical content is marked as a duplicate requiring review; different content is marked as a conflict. The user may skip, choose a deterministic numeric suffix, or change the target. Source metadata is revalidated immediately before execution to prevent stale-plan moves.

## Transaction protocol

| Stage | Required action |
|---|---|
| Prepare | Validate root, source existence, destination containment, protected boundaries, conflicts, and free space |
| Journal | Persist a transaction with all operations in `Prepared` state before any move |
| Execute | Create destination directories, move one file at a time, and persist each completed state immediately |
| Verify | Confirm destination existence and optional hash; retain failures without hiding partial completion |
| Commit | Mark the transaction completed only after all selected operations reach a terminal state |
| Undo | Replay completed operations in reverse order, refusing to overwrite changed or newly created files |

Journal files are stored under `.gtangel-organizer/journals`. Reports are stored under `.gtangel-organizer/reports`, and user rules under `.gtangel-organizer/config.json`. Writes use temporary files plus replace/rename to reduce the chance of torn JSON state.

## Watched-folder model

Watch mode uses a debounced filesystem event stream. New or renamed files are rescanned after they remain stable for a configurable interval. The default action is **create a pending recommendation**, not move the file. Optional auto-approval applies only to low-risk loose files with high-confidence rules, no conflict, no protected boundary, and no active Git repository relationship. The UI exposes start, pause, resume, stop, and queue counts, adapting explicit lifecycle states and bounded concurrency from the local integration hub.[2]

## User interface

The main window contains a workspace selector and lifecycle controls, followed by summary cards for components, files, protected items, proposals, conflicts, and duplicate groups. Tabs separate **Overview**, **Plan**, **Components**, **History**, and **Settings**. The plan grid shows selection, source, type, function, confidence, destination, reason, and status. Filters allow focused inspection of large mixed repositories, while progress and cancellation remain visible throughout long scans.

The visual design uses the existing GTAngel dark blue-violet language, but the organizer remains a standalone utility so a broken cognitive-runtime dependency cannot prevent repository recovery.

## Acceptance criteria

The implementation is accepted when it can scan the attached workspace without modifying it; recognize nested repositories and manifests; classify representative .NET, C++, Unreal, Node, Python, document, media, script, and archive files; produce a safe plan for loose root files; detect conflicts and duplicates; execute selected moves without overwrite; persist a complete journal; undo completed moves; cancel scans; export reports; and run deterministically under automated tests.

## References

[1]: ../README.md "GTAngel repository overview"
[2]: ../aihub/README.md "AI Agent Integration Hub"
[3]: https://github.com/o9nn/togai/blob/main/docs/FILE_FUNCTIONALITY_CATALOG.md "Togai File Functionality Catalog"
[4]: https://github.com/o9nn/LoopWorkspace "LoopWorkspace"
[5]: https://github.com/cogpy/atomspace-explorer "AtomSpace Explorer"
