# GTAngel — Copilot Instructions

## Project Overview

GTAngel is a cognitive AI training platform that integrates GTA3 Definitive Edition game assets with the Deep Tree Echo (DTE) cognitive training framework. Read `PROJECT_BRIEF.md` for complete project context.

## Quick Start

```bash
# Build the .NET solution
dotnet restore GTAngel.sln
dotnet build GTAngel.sln

# Run tests
dotnet test GTAngel.Tests

# Run the WPF application
dotnet run --project GTAngel
```

## Tech Stack

- **Frontend:** WPF (.NET 8, C#), MVVM pattern, Material Design
- **Backend/Core:** .NET 8, ONNX Runtime, Named Pipe IPC
- **UE5 Plugins:** C++23 (Archecho modules)
- **Testing:** xUnit (.NET)
- **CI/CD:** GitHub Actions

---

# Orchestration Guide

## Team Structure

GTAngel uses a multi-agent AI team orchestration model. Each team member has a specialized role defined in `.github/agents/`:

| Agent | File | Role |
|-------|------|------|
| Remy | `remy.md` | Producer — Sprint plans, coordination, merging |
| Nova | `nova.md` | Frontend/WPF — Views, ViewModels, controls |
| Sage | `sage.md` | Backend/ML — Services, ESN, DTE training |
| Milo | `milo.md` | UE5/C++ — Archecho plugins, IPC bridge |
| Ivy | `ivy.md` | QA — Testing, bug filing, sign-off |
| Kira | `kira.md` | Cognitive — Architecture design, feature specs |
| Dash | `dash.md` | DevOps — CI/CD, deployment, builds |

## Agent Invocation

To work as a specific team member, reference their agent file:

```
Read .github/agents/[agent].md, then read PROJECT_BRIEF.md.
Execute the following task: [TASK DESCRIPTION]
```

## Sprint Workflow

### 1. Planning Phase (Remy)
```
You are Remy. Read PROJECT_BRIEF.md and create docs/sprint-N/plan.md.
Define prioritized task list with owners and estimates.
```

### 2. Development Phase (Nova/Sage/Milo)
```
You are [Agent]. Read your agent file and PROJECT_BRIEF.md.
Implement task N from docs/sprint-N/plan.md.
Build and test your changes.
```

### 3. Testing Phase (Ivy)
```
You are Ivy. Read your agent file and pull the feature branch.
Run all tests. File bugs as GitHub Issues.
Write sign-off: docs/qa/sprint-N-signoff.md
```

### 4. Merge Phase (Remy)
```
You are Remy. Review the PR and QA sign-off.
If clear, merge to main. Update PROJECT_BRIEF.md Section 7 and 8.
```

## Cross-Team Handoff Protocol

Every sprint must complete these handoff steps:

1. **Development Done:**
   - Push all changes to feature branch
   - Create PR with description
   - Update `docs/sprint-N/progress.md`

2. **QA Done:**
   - All tests pass
   - No blockers in GitHub Issues
   - Write `docs/qa/sprint-N-signoff.md`

3. **Sprint Done:**
   - Write `docs/sprint-N/done.md`
   - Update PROJECT_BRIEF.md Section 7 (mark sprint done)
   - Update PROJECT_BRIEF.md Section 8 (rewrite current state)
   - Commit: `sprint-N: <summary>`

## Brainstorm Sessions

For feature ideation, use the brainstorm template in `docs/brainstorm/README.md`:

```
Run a brainstorm with the GTAngel team about [TOPIC].
Each agent (Kira, Milo, Nova, Sage, Remy, Ivy) speaks from their perspective.
They should debate and disagree.
Output to docs/brainstorm/[topic]/
```

## Key Directories

| Directory | Purpose |
|-----------|---------|
| `GTAngel/` | WPF application source |
| `GTAngel.Tests/` | xUnit test project |
| `Archecho/` | UE5 cognitive plugins |
| `docs/sprint-N/` | Sprint documentation |
| `docs/brainstorm/` | Feature ideation |
| `docs/qa/` | QA sign-offs |
| `.github/agents/` | Team agent definitions |
| `.github/workflows/` | CI/CD pipelines |

## Branch Strategy

| Team | Branch Pattern |
|------|----------------|
| Producer | `main` (coordination) |
| Development | `feature/sprint-N` |
| QA | `feature/qa-N` |
| DevOps | `feature/devops-N` |

**Rules:**
- Never push directly to main
- Use regular merge (never squash, never rebase)
- Feature branches → PR → merge

## Bug Tracking

- File bugs as GitHub Issues
- Labels: `bug`, `severity:blocker|major|minor`
- Use closing keywords: `Fixes #42`
- Infrastructure issues: `infra` label

## Security Rules

1. Secrets in environment variables only — never in code
2. Named Pipe IPC uses local-only connections
3. ONNX models loaded from local paths only
4. No external API keys in source

## Anti-Patterns to Avoid

| Don't | Do Instead |
|-------|------------|
| Rebase feature branches | Regular merge |
| Squash merge PRs | Regular merge |
| Push directly to main | Feature branch → PR → merge |
| Skip handoff docs | Mandatory done.md + PROJECT_BRIEF update |
| Batch commits | One commit per fix with issue reference |
| Mix WPF and UE5 changes | Separate commits per subsystem |
| Change IPC without both sides | Update WPF and UE5 IPC together |

## Getting Help

1. Read `PROJECT_BRIEF.md` for project context
2. Check `.github/agents/[agent].md` for role-specific guidance
3. Review `docs/sprint-N/` for current sprint status
4. Check GitHub Issues for known bugs
