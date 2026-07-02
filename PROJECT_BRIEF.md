# PROJECT_BRIEF.md — GTAngel

> Last updated: 2026-07-02 | Sprint 0 | Status: In Progress

## 1. Project Overview

GTAngel is a cognitive AI training platform that integrates GTA3 Definitive Edition game assets with the Deep Tree Echo (DTE) cognitive training framework. The system implements the DTE-KSM-Evo-Autogenesis evolution loop for game-world-based cognitive architecture training, evolving through Alexander's 15 Properties of Living Structure. It bridges WPF desktop UI, Unreal Engine 5 cognitive plugins, and Echo State Network (ESN) reservoir computing.

## 2. Concept / Product Description

The platform consists of:
- **WPF Desktop Application** — Main control panel for training sessions, visualization, and configuration
- **Archecho UE5 Plugins** — Cognitive architecture modules running inside Unreal Engine 5 (Echo, MCog, VNPU, Skills, Gizmo)
- **DTE Training Loop** — Reinforcement learning loop with ECAN attention, MOSES meta-optimization, and Thompson sampling
- **ESN Pipeline** — 3-layer Echo State Network reservoir computing for temporal pattern processing
- **Avatar Embodiment** — MetaHuman avatar with FACS expressions, IK, and personality modeling
- **ML Vision** — DXGI-based 768×768 frame capture with ONNX CNN feature extraction
- **Game World Navigation** — A* pathfinding with POI discovery in Liberty City

User flow: Launch GTAngel → Configure training parameters → Launch UE5 → Run autogenesis loop → Monitor via dashboard → Analyze results.

## 3. Tech Stack

- **Frontend:** WPF (.NET 8, C#), MVVM pattern, Material Design
- **Backend/Core:** .NET 8, ONNX Runtime, Named Pipe IPC
- **UE5 Plugins:** C++23 (Archecho modules — echo, mcog, vnpu, skills, gizmo, aiext)
- **ML:** Echo State Networks, ONNX CNN, Experience Replay, Multi-Agent RL
- **Hosting:** Desktop application (local), UE5 Editor
- **Testing:** xUnit (.NET), GTAngel.Tests project
- **CI/CD:** GitHub Actions (.github/workflows/ci.yml)

## 4. Architecture

```
┌─────────────────────────────────────────────────────────────┐
│              GTAngel WPF Desktop (C# .NET 8)                │
│  App.xaml.cs → MainWindowViewModel → Services               │
│                                                             │
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────────┐   │
│  │DteCognitive │  │EsnReservoir  │  │MlVisionCapture   │   │
│  │CoreService  │  │Pipeline      │  │Service (768×768) │   │
│  └──────┬──────┘  └──────┬───────┘  └────────┬─────────┘   │
│         │                │                    │             │
│  ┌──────┴──────┐  ┌──────┴───────┐  ┌────────┴─────────┐   │
│  │DteTraining  │  │MultiAgent    │  │OnnxCnnFeature    │   │
│  │Loop (RL)    │  │Trainer       │  │Extractor         │   │
│  └─────────────┘  └──────────────┘  └──────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  UE5LaunchOrchestrator (4-Stage Pipeline)           │    │
│  │  Ue5PlayerAiBridgeService (Human↔AI Arbitration)    │    │
│  └───────────────────────┬─────────────────────────────┘    │
└──────────────────────────┼──────────────────────────────────┘
                           │ Named Pipe IPC (GTAngel_MLVision_IPC)
┌──────────────────────────┼──────────────────────────────────┐
│              Unreal Engine 5 (LibertyCity)                   │
│  ┌────────────┐  ┌────────────┐  ┌────────────────────┐    │
│  │unreal-echo │  │unreal-mcog │  │unreal-vnpu         │    │
│  │(Ontelecho) │  │(OpenCog    │  │(NPU LLM Coprocessor│    │
│  │            │  │ C++23)     │  │)                   │    │
│  └────────────┘  └────────────┘  └────────────────────┘    │
│  ┌────────────┐  ┌────────────┐  ┌────────────────────┐    │
│  │unreal-skills│  │unreal-gizmo│  │unreal-aiext        │    │
│  │(Skill Sys) │  │(Debug Viz) │  │(AI Extension)      │    │
│  └────────────┘  └────────────┘  └────────────────────┘    │
│  MetaHuman Avatar (FACS + IK + Personality)                 │
└─────────────────────────────────────────────────────────────┘
```

## 5. Key Files Map

| Area | Path | Contents |
|------|------|----------|
| Solution | `GTAngel.sln` | .NET solution file |
| Entry point | `GTAngel/App.xaml.cs` | DI container bootstrap |
| ViewModels | `GTAngel/ViewModels/` | MVVM view models |
| Views | `GTAngel/Views/` | WPF XAML views |
| Services | `GTAngel/Services/` | Core services (DTE, ESN, ML, UE5) |
| Models | `GTAngel/Models/` | Domain models |
| Interop | `GTAngel/Interop/` | UE5 IPC communication |
| Controls | `GTAngel/Controls/` | Custom WPF controls |
| Tests | `GTAngel.Tests/` | xUnit test project |
| Archecho | `Archecho/` | UE5 cognitive plugin modules |
| CI/CD | `.github/workflows/ci.yml` | GitHub Actions pipeline |
| Docs | `docs/` | Architecture & sprint docs |
| Config | `GTAngel/appsettings.json` | App configuration |

## 6. Team Roles

| Agent | Name | Role |
|-------|------|------|
| Producer | Remy | Sprint plans, coordination, merging |
| Frontend/WPF | Nova | WPF UI, MVVM, views, controls, client logic |
| Backend/ML | Sage | Services, ML pipeline, ESN, DTE training loop |
| UE5/C++ | Milo | Archecho plugins, UE5 integration, IPC bridge |
| QA | Ivy | Testing, bug filing, sign-off |
| Product/Cognitive | Kira | Cognitive architecture design, feature specs |
| DevOps | Dash | CI/CD, deployment, build pipeline |

## 7. Sprint Status

| Sprint | Name | Status | Scope |
|--------|------|--------|-------|
| 0 | Architecture & Orchestration | 🔨 In Progress | Team setup, project brief, sprint structure |

## 8. Current State (rewrite every sprint)

**What works:**
- WPF application with MVVM architecture
- Core services (DTE, ESN, ML Vision, Navigation, Avatar)
- UE5 launch orchestrator and IPC bridge
- xUnit test project with converters, interop, models, services, and VM tests
- CI pipeline via GitHub Actions
- Archecho UE5 plugin modules (echo, mcog, vnpu, skills, gizmo, aiext, msdk)

**What doesn't work yet:**
- Sprint orchestration (being set up now)
- Cross-team handoff protocols
- QA sign-off process

**What's next:**
- Complete Sprint 0 (orchestration setup)
- Sprint 1: Feature development TBD via brainstorm

## 9. Security Rules

1. Secrets live in environment variables only — never in code or git.
2. Named Pipe IPC uses local-only connections (no network exposure).
3. ONNX models are loaded from local paths only.
4. No external API keys committed to source.

## 10. How to Run Locally

```bash
# Build the .NET solution
dotnet restore GTAngel.sln
dotnet build GTAngel.sln

# Run the WPF application
dotnet run --project GTAngel

# Run tests
dotnet test GTAngel.Tests
```

## 11. How to Deploy

- CI/CD via GitHub Actions (`.github/workflows/ci.yml`)
- Desktop application — distributed as self-contained .NET publish
- UE5 plugins built separately via Unreal Build Tool

## 12. Cross-Chat Handoff Protocol

Every sprint chat must do these before finishing:

1. Write `docs/sprint-N/done.md` — what was built, what's not done, what needs manual setup, files changed/created
2. Update PROJECT_BRIEF.md: Section 7 (mark sprint done) + Section 8 (rewrite current state)
3. Commit all changes with descriptive message: `sprint-N: <summary>`

This is how context survives across chats. If skipped, the next chat starts blind and may overwrite or duplicate work. The repo is the shared memory — keep it accurate.

## 13. Bug & Fix Tracking

Bugs are tracked as GitHub Issues on the repo. Single source of truth for all teams.

**For QA:** File bugs as GitHub Issues with labels (`bug`, `severity:blocker/major/minor`). Include: component, steps to reproduce, expected vs actual. When no blockers found: write `docs/qa/sprint-N-signoff.md` with test count, pass rate, explicit "no blockers" statement.

**For Dev Team:** Check GitHub Issues before starting work. Fix blockers and majors before polish. Use GitHub closing keywords in commits: `fix: description (Fixes #42)`. For reference-only, use `Refs #42`.

**For DevOps:** File infrastructure issues with label `infra`.

**For feature ideas:** add to `docs/ideas-backlog.md`.

## 14. Multi-Repo Setup

Each team works in their own separate clone of the repo. No worktrees. Everyone works on their own branch, pushes to origin, creates PRs.

**Teams:**
- Producer on `main` (coordination hub)
- Dev Team on `feature/sprint-N`
- QA on `feature/qa-N`
- DevOps on `feature/devops-N` (only when needed)

**Setup:**
```bash
git clone https://github.com/ReZorg/GTAngel.git gtangel-dev    # Dev team
git clone https://github.com/ReZorg/GTAngel.git gtangel-qa     # QA
git clone https://github.com/ReZorg/GTAngel.git gtangel-devops # DevOps (only when needed)
```

**Branch strategy:** Feature branches → PR → regular merge to main. Never push directly to main. Never squash. Never rebase feature branches (causes commit loss).
