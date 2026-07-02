# Sprint 0 — Architecture & Orchestration

> Sprint Goal: Set up AI team orchestration structure, project brief, and sprint workflow
> Branch: drzo-urban-meme
> Estimated effort: 1 session

## Prioritized Task List

| # | Task | Owner | Est | Description |
|---|------|-------|-----|-------------|
| 1 | Create PROJECT_BRIEF.md | Remy | 30m | Single source of truth for all teams |
| 2 | Create sprint directory structure | Remy | 15m | docs/sprint-N/, docs/brainstorm/, docs/qa/ |
| 3 | Create Sprint 0 plan + progress | Remy | 15m | This plan and progress tracker |
| 4 | Document chat architecture | Remy | 15m | How teams communicate via files |
| 5 | Document anti-patterns | Remy | 10m | Lessons for the team to follow |

## Work Schedule

### Phase 1: Project Brief (tasks 1-2)
- Create PROJECT_BRIEF.md with all 14 sections
- Create directory structure for sprints, brainstorms, QA
- Checkpoint commit after phase

### Phase 2: Sprint Infrastructure (tasks 3-5)
- Create sprint plan and progress tracker
- Document team workflows and anti-patterns
- Final commit

## Success Criteria

- [x] PROJECT_BRIEF.md exists with all 14 sections filled
- [x] docs/sprint-0/ contains plan.md and progress.md
- [x] docs/brainstorm/ directory exists
- [x] docs/qa/ directory exists
- [ ] All files committed to branch

## What's NOT in This Sprint

| Feature | Reason |
|---------|--------|
| Code changes | Sprint 0 is documentation/orchestration only |
| Brainstorm session | Needs Sprint 0 complete first |
| Feature development | Awaits Sprint 1 planning |

## Agent Prompt

> Read PROJECT_BRIEF.md, then read docs/sprint-0/plan.md. Execute Sprint 0.
>
> First: git pull origin main && git checkout -b feature/sprint-0
>
> Close GitHub Issues in commits: "fix: description (Fixes #NN)"
> Update docs/sprint-0/progress.md after each phase.
> When done, push and create PR: git push origin feature/sprint-0
> Follow Sections 12-14 of PROJECT_BRIEF.md.
