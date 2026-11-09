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

The first held-out assessment completed all eight walking and all eight running
routes without contacts or stuck frames. Walking averaged about 0.432 normalized
completion cost against 0.399 for the baseline, so promotion was rejected.
Running averaged about 0.278, but remained unavailable because its walking
prerequisite was not validated. The thresholds were not relaxed to force an unlock.

This suite establishes short-route competence only. Turning, braking, crowded
routes and interrupted goals need broader suites before claiming general mastery.
Driving authorization, a specialized driving learner, learned quest execution and
strategic planning remain the subsequent implementation stages above. Their
registry entries do not make them available motor capabilities.
