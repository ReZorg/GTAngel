# Anti-Patterns for GTAngel Development

Lessons from multi-agent AI team orchestration. Follow these to avoid common pitfalls.

## Git & Branching

| Don't | Do Instead | Why |
|-------|------------|-----|
| Rebase feature branches | Regular merge | Rebase rewrites history and loses commits across chats |
| Squash merge PRs | Regular merge | Squash hides individual commits, can't revert single fix |
| Push directly to main | Feature branch → PR → merge | Direct pushes bypass review |
| Force push (`--force`) | Fix forward or revert | Force push destroys remote history |

## Team Roles

| Don't | Do Instead | Why |
|-------|------------|-----|
| Producer writes code | Producer only plans, merges, files issues | Coordinator coding causes conflicts |
| One agent does everything | Separate agents for dev, QA, coordination | Context isolation prevents cross-contamination |
| Skip the brainstorm | Run brainstorm → plan → execute | Jumping to code produces generic results |
| Vague brainstorm prompts | Name each agent with distinct perspective | Named agents produce real debate |

## Sprint Management

| Don't | Do Instead | Why |
|-------|------------|-----|
| Batch "fix everything" commits | One commit per fix with issue reference | Batch commits make tracking impossible |
| Keep bugs only in chat | File GitHub Issues | Chat context dies when conversation ends |
| Skip handoff docs (done.md) | Mandatory done.md + PROJECT_BRIEF update | Next chat starts blind without them |
| Skip progress tracker | Update progress.md after each phase | Context overflow recovery impossible without it |
| Rush the AI with time pressure | "Take your time, do it right" | Time pressure makes LLM skip edge cases |

## Testing & QA

| Don't | Do Instead | Why |
|-------|------------|-----|
| Merge before testing | Playtest → file issues → fix → merge | Untested code breaks main |
| QA modifies source code | QA only files issues, dev team fixes | QA fixes miss context |
| Close issues without verification | Dev fixes → QA verifies → close | Self-closing skips verification |

## GTAngel-Specific

| Don't | Do Instead | Why |
|-------|------------|-----|
| Mix WPF and UE5 changes in one commit | Separate commits per subsystem | Different build pipelines, easier rollback |
| Edit Archecho C++ without UE5 build check | Build UE5 plugins after changes | C++ errors are hard to debug after the fact |
| Commit .uexp binary changes casually | Discuss binary asset changes in PR | Large binaries bloat git history |
| Change IPC protocol without both sides | Update WPF and UE5 IPC together | Protocol mismatch causes runtime crashes |
