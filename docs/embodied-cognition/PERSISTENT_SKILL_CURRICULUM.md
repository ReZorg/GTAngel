# Persistent learning and skill curriculum

## Objective

Retain genuine learning across process restarts and compose tested motor skills
into longer tasks. Training activity is not mastery. A persisted score is not a
motor capability: the local spinal authority still validates every action.

## Delivery sequence

1. **Checkpoint foundation.** Save an immutable generation containing the GNN
   weights, replay buffer, optimizer schedule and counters; Q/ESN/memory state;
   and skill records. Validate sizes, finite values, versions and checksums
   before applying any state. Publish a generation only after all files are
   written. Keep prior generations for recovery. Autosave on elapsed time,
   not an exact training-update multiple. Isolate all verification data.
2. **Skill registry and selector.** Track walking, running, driving, quest
   execution and strategy separately. Keep training observations separate from
   held-out evaluation results. Walking remains the conservative baseline.
   Selection considers physical occupancy, obstacles, proximity to the target
   and validated prerequisites. Missing capability or failed assessment means
   fallback, never a bypass.
3. **Evaluation and promotion.** Evaluate without training, on multiple distinct
   scenario seeds, using success, contact rate, stuck time and completion cost.
   Reject non-finite and incomplete results. Require demonstrated benefit over
   the incumbent and no safety regression before advancing a skill. Retain the
   best evaluated model independently from the latest learning state.
4. **Specialized control.** Introduce a running controller, then a bounded driving
   controller, each with its own demonstrations and regression suite. Train
   driving on an isolated track before traffic. Driving needs an explicit local
   motor authorization path; the existing on-foot DTE seal cannot authorize it.
5. **Hierarchical quests.** Compose travel, enter, drive, park, exit, approach and
   interact skills. The existing quest-state target planner is the initial
   baseline. A learned strategic selector must outperform it on held-out quests
   before replacing it. Preserve manual override and interrupted-task recovery.

## Checkpoint layout

Checkpoints live in the runtime's `learning/checkpoints/` directory, outside
generated sources. Each generation has a format/versioned manifest and bounded
payload files. A `.pending` generation is never considered recoverable. Valid
completed generations are immutable; loaders try newest first and fall back if
the newest generation is damaged. Legacy model/state files are import sources,
not destinations for ongoing learning. Test and diagnostic runs use a separate
per-process directory under `build/learning-tests/`.

The checkpoint covers the online GNN motor learner and the serialized
Q/ESN/episodic/social state. The GRU visuomotor model remains inference-only.
This is not a claim that every cognitive module's weights are checkpointed.
Model topology, feature definitions and curriculum version are compatibility
boundaries; migrations must be explicit.

## Skill lifecycle

`learning -> validated -> available`, with regression returning a skill to
learning. Availability additionally requires a compiled, governed controller.
Lifetime observations, travelled distance and rewards do not promote skills.
Evaluation records contain the skill, suite version, distinct scenario seeds,
successes, contacts, stuck fraction and normalized completion cost.

Walking is always a bounded baseline, not automatically labeled mastered.
Running requires validated walking and running evaluations plus a running
capability. Driving requires validated walking and driving evaluations plus
driving authorization. Quest execution requires validated navigation; strategy
requires validated quest execution. Close obstacles, contact or nearby
interactions force walking rather than running.

## Acceptance criteria

- A fresh process reloads exactly the saved GNN predictions, replay samples,
  gradient counts and optimizer schedule, then continues updating weights.
- Truncated, corrupt, incompatible or incomplete checkpoints cannot partially
  mutate a live learner. A prior valid generation remains usable.
- A failed save preserves the incumbent checkpoint and is visible in the HUD.
- Autosave occurs after elapsed time even when training takes extra batches.
- Verification never writes to the live model, memory or checkpoint directories.
- Training counters alone cannot unlock a skill. Duplicate seeds do not count
  as independent evaluation evidence.
- Validation can demote a regressed skill. Missing motor capability always wins
  over a persisted availability record.
- Live telemetry shows the checkpoint status, selected skill and why a higher
  mode is unavailable.

## Evaluation policy

Use at least eight distinct scenarios per skill and preserve a held-out suite.
Start with a 90% success target, no increase in contact rate, no more than 5%
stuck time, and improved normalized completion cost. These are explicit initial
engineering thresholds, not proof of general competence. Evaluate multiple
training seeds before promoting a motor model; keep raw evaluation records.
Training data must not include evaluation outcomes. Compare against the same
incumbent and suite version; new suite versions invalidate earlier promotion
evidence until reassessed.

## Operational notes

Normal exit requests a final checkpoint. Forced termination can lose changes
since the last completed autosave. Checkpoint retention bounds disk use.
Backups to another disk are a separate operational measure. Neither continued
training nor a rising reward counter guarantees improvement; only measured,
repeatable evaluation supports promotion.

## Implemented foundation

The first implementation provides:

- Immutable, checksummed generations, typed validation before publication,
  a writer lock, recovery to older valid generations, five recent recovery points
  and a separately retained `best.txt` champion.
- Full GNN learner continuation, including replay samples, adaptive learning-rate
  state, loss and lifetime gradient counters. Safe, bounded legacy import leaves
  the original `.net` and `.bin` files unchanged.
- Sixty-second autosaves and normal-exit checkpoints, with retry backoff and
  visible status. Verification uses `build/learning-tests/<process-id>/`.
- Persistent records for all five skill stages, a conservative selector and a
  running gait bounded to 0.8 forward demand through the existing local seal.
  Contact, nearby obstacles and interaction proximity force walking.
- An explicit walking/running assessment command. Eight held-out street seeds
  compare candidate control against the incumbent walking controller, using
  frozen motor weights and no replay writes or Q updates. Assessment world state
  is discarded. Only evidence paired with the original checkpoint is published.
- Separate learner and champion networks. Normal operation uses the DTE baseline
  until a walking candidate passes evaluation; afterwards it uses the retained
  champion while the candidate continues training.

Build from `native/GTAngel_v56` using `build_v56.bat`. Run normally to import
legacy learning and create the first checkpoint. Close the live app normally
before assessing its latest checkpoint:

```bat
gtangel_agi_v56.exe --evaluate-skills
```

Assessment takes several minutes. Its CSV is written to
`build/learning-tests/<process-id>/skill-evaluation.csv` in the repository.
Exit status 0 means assessment completed; the CSV's `walking_validated` and
`running_validated` fields indicate the result. Rejection is a valid outcome.
Concurrent learning invalidates publication instead of allowing an old evaluation
to overwrite newer progress. Restart normally to load a newly promoted champion.
F4 shows the selected skill, controller, checkpoint status and checkpoint ID.

The original suite-1 assessment completed all eight walking and all eight running
routes without contacts or stuck frames. Walking averaged about 0.432 normalized
completion cost against 0.399 for the baseline, so promotion was rejected.
Running averaged about 0.278, but remained unavailable because its walking
prerequisite was not validated. The thresholds were not relaxed to force an unlock.

This suite establishes short-route competence only. Turning, braking, crowded
routes and interrupted goals need broader suites before claiming general mastery.
Driving authorization, a specialized driving learner, learned quest execution and
strategic planning remain the subsequent implementation stages above. Their
registry entries do not make them available motor capabilities.

## Walking learning and assessment upgrades

Motor input contract 2 captures one feature vector before actuation and reuses it
for inference and the executed-action training label. It includes the intended
goal's continuous bearing and distance, while retaining the actual retinal
observations separately. Post-action speed is no longer substituted into a
pre-action demonstration.

The bounded 256-sample replay buffer retains examples of straight movement,
negative/positive turning, reverse and stop. Each 16-point training batch mixes
recent samples with rotating samples from those manoeuvres. A gradient step is
compared against the same batch before it ran. Worsening or non-finite steps are
rolled back and the learning rate is reduced. Attempted, accepted and reverted
updates are reported separately; this does not replace held-out evaluation.

Motor checkpoint format 2 reads the previous format. Updating the input contract
preserves network weights, learning rate and lifetime counters, but retires the
old replay labels because their body state/distance inputs are incompatible with
the corrected contract. Older checkpoints remain recovery points.

`--practice-walking` performs two passes over eight training scenarios through
the normal renderer and sealed local motor path. The baseline provides the
demonstrations; the candidate learns without controlling practice. Scenarios
include straight travel, left/right reorientation and an obstructed route.
Practice persists the motor learner and skill observation counts, preserving the
live Q table, ESN and memories. It never grants a skill or replaces the champion.

Suite 3 evaluates eight separate world seeds covering the same skill families,
including substantial heading changes and detours. A deliberate turn is not
classified as blocked movement. Each candidate trial resets body, animation,
proprioceptive history and the signals used by motor input. Reports include
reference and candidate rows, rather than only the reference's completion cost.
Suite-1 evidence and experimental suite-2 results are retained as history but
cannot authorize current skills. Suite 3 additionally requires fresh worlds and
compares the guarded candidate against the actual incumbent controller.

Close the live application normally, then run these commands in
`native/GTAngel_v56` from PowerShell:

```powershell
Start-Process .\gtangel_agi_v56.exe -ArgumentList '--practice-walking' -Wait -PassThru | Select-Object Id, ExitCode
Start-Process .\gtangel_agi_v56.exe -ArgumentList '--evaluate-skills' -Wait -PassThru | Select-Object Id, ExitCode
```

Each command writes its CSV under repository `build/learning-tests/<process-id>/`:
`walking-practice.csv` or `skill-evaluation.csv`. They take several minutes.
Publication is refused if another process advanced the source checkpoint.
Restart normally to load the resulting learner and any qualified champion.

F4 now shows the last walking assessment's failure reason and completed-scenario
count, replay composition, and reverted gradient steps. A failed assessment
identifies training work; it does not lower the promotion threshold. Driving
practice remains a later stage requiring its own bounded controller and seal.

The expanded suite exposed repeated contact while the reference controller
crept forward during large turns. The walking guard now reorients in place and
checks short prospective footpaths using `Runtime::canWalkTo`, a read-only query
of the same swept collision geometry used by the body. When a path is blocked,
it can propose a clear 1.35-metre detour or wait for clearance. A detour's
translation is capped at 0.45 walking demand. It checks the immediate step as
well as the longer lookahead, and finishes aligning with a clear waypoint
instead of repeatedly cancelling it. All returned demands still require the
normal local seal and spinal motor commit; driving authority is unchanged.

Detour state is transient and resets on manual/vehicle control, a changed goal,
world load/reset, and each assessment episode. The guard is staged with the
candidate: practice and candidate assessment use it; normal sessions retain
the incumbent controller until a guarded candidate passes promotion. A
previously validated champion is assessed with its guard, while the initial
DTE reference uses its existing controller. Improvements from the guard must
not be attributed solely to neural training.
Every trial constructs a fresh world before generation: traffic
clocks and protected quest positions must not carry across scenarios.
The guarded policy also suppresses third-person camera avoidance arcs when the
body's forward footpath is confirmed clear. This distinguishes obstacles near
the camera from those obstructing the avatar, without weakening collision
checks. This correction is staged with the candidate as well.

For a focused, non-publishing diagnosis, add `--scenario=0` through
`--scenario=7` to `--evaluate-skills`. This also writes `scenario-trace.csv`;
partial runs cannot update checkpoints or grant mastery.

## Verified upgrade snapshot

- Learning verification: 303/303; runtime verification: 119/119.
- Generation from another launch directory reproduced 54 identical files.
- Practice advanced gradient attempts from 2,562 to 2,820 and changed model
  weights. The persisted Q/ESN/memory payload remained byte-for-byte unchanged,
  apart from the separate neural-counter/clock tail.
- Suite 3: the DTE reference completed 7/8 routes; the guarded walking candidate
  completed 6/8 and running completed 5/8. Both candidates recorded zero contacts,
  but completion failures prevented promotion. The reference had contacts on
  several routes and is not labeled mastered.
- Desktop restart retained 2,851 gradient attempts and continued to 2,856.
  The restarted app travelled 3.4 metres without contact during that capture.

The next control priority is resolving the guarded candidate's route timeouts
without sacrificing its contact avoidance. Normal sessions retain the DTE
baseline until a candidate meets the complete assessment gate.
