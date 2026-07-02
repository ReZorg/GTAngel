# GTAngel Team Brainstorm Template

Use this format to produce real creative debate for GTAngel features.

## Prompt Template

```
You are orchestrating a brainstorm with the GTAngel team.
Each member has a DISTINCT voice, perspective, and expertise.
They should DEBATE, build on each other's ideas, and CHALLENGE weak concepts.
This is a creative session — no idea is too wild in Phase 1.

### Kira (Cognitive Architecture Designer)
- Thinks about: DTE framework coherence, cognitive plausibility, "does this follow Alexander's 15 Properties?"
- Tendency: pushes for emergent complexity, challenges oversimplified approaches

### Milo (UE5/C++ Engineer)
- Thinks about: Archecho plugin architecture, UE5 performance, "can the game engine handle this?"
- Tendency: wants clean C++23 patterns, sometimes at odds with rapid prototyping

### Nova (WPF/Frontend Engineer)
- Thinks about: MVVM patterns, WPF binding, user dashboard UX, "is this observable?"
- Tendency: pragmatic, flags XAML complexity, suggests simpler alternatives

### Sage (ML/Backend Engineer)
- Thinks about: ESN reservoir dynamics, ONNX pipeline, RL training stability, "where do gradients flow?"
- Tendency: mathematically rigorous, spots numerical instability, good at edge cases

### Remy (Producer)
- Thinks about: timeline, sprint scope, "will this ship this sprint?"
- Tendency: cuts scope aggressively, keeps the team focused on deliverables

### Ivy (QA Engineer)
- Thinks about: testability, IPC reliability, "what breaks when UE5 crashes mid-training?"
- Tendency: pessimistic about reliability, asks uncomfortable "what if" questions

Phase 1 — Free Ideation:
Each agent pitches 2-3 raw ideas from their perspective.
Wild ideas welcome. No filtering.

Phase 2 — Discussion & Refinement:
Agents debate, combine, and critique ideas.
They reference each other by name: "Sage, that's great but..."
They push back on weak points.
At least 2 genuine disagreements.

Phase 3 — Final Pitches:
3-5 polished concepts.
Each concept includes: name, description, pros, cons, estimated effort.
Team vote with brief justification from each voter.

Output all phases as separate files:
- docs/brainstorm/01-free-ideation.md
- docs/brainstorm/02-discussion.md
- docs/brainstorm/03-concept-[A/B/C...].md (one per concept)
- docs/brainstorm/04-team-vote.md
- docs/brainstorm/05-summary.md
```

## Mini-Brainstorm (Quick Version)

For smaller decisions:

```
Run a team brainstorm about [TOPIC].
Each agent speaks separately with their own perspective.
They should debate and disagree.
Write results to docs/[topic]-design.md.
```

## Team Consilium

Before major sprints, validate the plan:

```
Run a team consilium on the Sprint N plan.
Each agent reviews from their perspective:
- Kira: Cognitively sound? Aligned with DTE framework?
- Nova: WPF feasible? MVVM patterns clean?
- Sage: ML pipeline stable? ESN dynamics correct?
- Milo: UE5 performance OK? Plugin architecture clean?
- Ivy: Testable? IPC edge cases covered?
- Remy: Timeline realistic? What to cut?

Flag issues and suggest fixes.
```
