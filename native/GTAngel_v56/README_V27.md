# GTAngel v27 — DTE-owned local navigation and learning

## Purpose

**Deep Tree Echo is the embodied operator. A person is not required to drive or teach it.** On launch, the local cognitive loop runs automatically. DTE produces bounded on-foot controller proposals from the native local visual field and its online Q-table. The local SRD evaluates each proposal and remains the only issuer of `MOTOR_COMMIT`.

This is an isolated repair release beside v26, not a restoration or overwrite. The DTE visual LOD and exact System 7→8 topology remain available.

## Regressions found and repaired

1. Normal `WM_CREATE` startup never called the NN weight loader. Loading was confined to `HOME` reset. The shared model still existed and matched its original verified SHA-256.
2. `N` changed a flag but the navigation loop no longer called `NavTrainer.update()`. It requested a rejected `Route` source with a zero command instead.
3. NN inference and learning metrics were absent from the default 3D view.
4. Negative local reward values could become negative token indices in the older token-ML training path. A full Windows application-loop test exposed an access violation. The proprioceptive token encoder now maps finite bounded values into the model's valid vocabulary range.
5. Controller recording paired the post-action frame with the preceding action. v27 retains the actual pre-action visual tensor/proprioception and records the subsequent local outcome and accepted action label.
6. The body and camera already agreed on horizontal forward: `(sin(yaw), 0, cos(yaw))`. The avatar pose used the opposite yaw sign. Its cosmetic orientation and debug left/right yaw signs are now aligned.

## What is active

| Component | Actual behavior |
|---|---|
| Native CNN/GRU visuomotor NN | Loads adjacent original weights at boot; runs learning inference each active cycle alongside the GNN motor actuator. |
| GNN motor actuator (GNU GNeuralNet) | 20->16->2 tanh MLP trained **online** with gradient descent on accepted local demonstrations (turn/forward). Trains every 16 frames once >=8 samples are buffered; saves to `models/gtangel_motor_gnn.net` on exit and reloads on boot. Shield-governed actuation: accepted proposals steer through the sealed DteLocal path. |
| Existing navigation Q-table | Real online Bellman updates from locally accepted DTE actions and re-observed visual transitions. No privileged target coordinates or remote pings. |
| DTE local controller | Uses local visual goal/obstacle evidence, bounded exploration and visited Q entries. Proposals are bounded to at most 0.5 forward and 0.6 turn. |
| SRD/body effector | Independently validates a fresh renderer-owned seal and performs local swept collision checks before movement. |
| Existing reservoir / token ML | Continue running. A cumulative token-ML update count is displayed; this small illustrative classifier is **not** the navigation CNN. |
| System 7 / System 8 | Exact structural lens remains advisory. It cannot submit controller or niche commands. |
| World governance | Unchanged. Navigation cannot repair a beacon or mutate the niche. |

### Sealed DTE source

`DteLocalMotorSeal` is non-defaultable and privately issued only by the renderer-owning `GTAngelAGI` after the current v25 local ingress sample has been validated. It binds frame sequence, full colour/depth/object-ID digest, issue time, body sequence, world version and the exact forward/turn demand.

SRD rejects missing seals, expired seals (>100 ms), altered commands, mismatched digests, changed bodies/world versions and replayed evidence. DTE's sealed source is on-foot only. The generic public `motor(Source::DteLocal, ...)` call without a seal is rejected. `Neural`, `Route`, `Script`, `Replay`, `AgentZero` and `Remote` remain untrusted motor sources.

This is an in-process capability boundary, **not** cryptographic protection against hostile code injected into the process. The original GRU's autonomous-actuation flags remain false because its old corpus had no collision-positive or goal-positive training records. The authorized local controller is not a disguised promotion of that model.

## Run locally

Run `run_v27.bat` in:

```text
C:\ddd\GTAngel\cpp_test\GTAngel_v27_Learning
```

DTE starts moving and learning automatically; no WASD demonstration is required.

| Key | Purpose |
|---|---|
| `F4` | Learning monitor: run state, NN frames/latency, Q updates, reward/Q delta, capture count, actual token-ML updates, and the magenta Echo persona line (live emotion / FACS AUs / spice / confidence). |
| `O` | Pause/resume DTE-owned locomotion; NN/reservoir may continue observing. |
| `SPACE` | Pause/resume the entire cognitive loop. |
| `N` | Toggle online Q updates; does not request a `Route` motor source. |
| `M` | Toggle NN learning inference (GRU + GNN motor actuator). |
| `C` | Explicitly start/stop local trajectory recording. Not started automatically, to avoid unbounded disk use. |
| `W/S`, `A/D` | Optional local debugging override. W follows camera horizontal yaw; A turns left, D right. Debug input pre-empts DTE but still passes through SRD. |
| `F8` | System 7→8 advisory inspector. |
| `F7` | Separate 1536×864 presentation window. |
| `HOME` | Reset the local episode/Q session and reinitialize learning. |

The default view shows advancing NN-frame and Q-update counters. F4 and F8 are GDI-only diagnostics, excluded from the 768×768 AI receptor. The optional high-resolution presentation is not NN input.

## Rebuild and test

`build_v27.bat` requires Node and the existing MinGW compiler at `C:\msys64\mingw64\bin`. It regenerates v27 from the included immutable v26 baseline, emits component headers (`emit_v27_components.mjs` - emission-only, no locks), builds the app with the GNU GNeuralNet objects (`gnn_obj\*.o`, headers from `..\gneural-net\include`), runs numerical checks (250 checks including GNN convergence + save/reload round-trip) and runs the full no-keyboard Windows loop/soak harness.

For Linux source checks:

```bash
node generate_v27.mjs
node emit_v27_components.mjs
g++ -std=c++17 -O2 verify_v27_learning.cpp -o verify_v27_learning
./verify_v27_learning
g++ -std=c++17 -O2 verify_v27_authority.cpp -o verify_v27_authority
./verify_v27_authority
```

The runtime self-test runs **in-process** (`gtangel_agi_v27.exe --selftest`): it drives the same control path as the player through a plain member variable and reports via the game's own logging plus `V27_RUNTIME_VERIFICATION.json` / `V27_SELFTEST.txt`. There is no external harness, no window capture, and no input injection, so nothing trips antivirus heuristics.

## Honest limits

- This is a bounded local visual/Q navigation loop, not evidence of general autonomy, completed quest execution or superior learned performance.
- Learning state (Q weights, visitation counts, episodic memory, semantic concepts, ESN readout, social relationships, counters, GNN training counters/loss) is persisted as a binary snapshot at `models/v27_learning_state.bin`: saved on graceful exit, loaded on boot, and round-trip verified by the selftest. The GNN motor network itself is saved separately at `models/gtangel_motor_gnn.net` (GNeuralNet native format) and reloaded on boot so learned motor skill survives restarts. `HOME` still performs a live reset; a hard kill skips the save.
- The CNN/GRU visuomotor policy observes and predicts with fixed weights; online Q updates and GNN motor training are separate learning systems. The GNN motor actuator (GNU GNeuralNet) trains online from accepted local demonstrations and persists across sessions.
- Trajectory capture uses accepted DTE actions; it is not an independently optimal expert corpus. Captured data needs qualification before offline NN retraining or promotion.
- The controller sees the visually encoded beacon, not privileged quest target coordinates. Automatic dialogue, shard acquisition and beacon repair are not implemented by this repair.
- DTE cart autonomy is not enabled. Debug cart driving remains separately governed.
- v25 GLB action clips are still not sampled by the native runtime. The model remains the validated cosmetic LOD plus existing procedural deformation.
- Neither Agent Zero nor daedalOS is connected as a live actuator. Agent Zero is a candidate executive/advisory host; daedalOS can host presentation/receipts without acquiring movement rights.
- Headless Windows loop tests and process/window responsiveness do not establish live visual/gameplay acceptance. F4/F7/F8 appearance and avatar likeness remain separate visual checks.
- Sensorimotor/proprioceptive loop repair (v27 learning): the post-action body proprioception is now stored (efference-copy closed) and blended into the reservoir input channels; dte::SensorimotorLoop reward/step accounting now accumulates from the real local navigation outcome (the synthetic captureFrame/selectAction tick path remains intentionally detached); echo_ksm coherence reads the live TRIZ arena aesthetic field and contradiction reads the real mean visual contradiction instead of hardcoded constants; the GTAngel-Cls env suggestion is observed as reservoir context only and is never actuated (SRD source whitelist unchanged).
- Reservoir vision + episodic memory (v27 learning): the ESN input is widened from 16 to 32 channels - [0-7] real body proprioception, [8-15] endocrine hormones, [16-18] frame-buffer retina statistics (mean luminance/motion/edge re-derived from the live perception pyramid), [19] TRIZ mean contradiction, [20] TRIZ temporal derivative (spatio-temporal reasoning), [21-24] visual goal/obstacle evidence with depth buckets, [25-29] social game dynamics (group count, cohesion, peer attention confidence/relations/salience), [30] episodic recall similarity, [31] last outcome reward. A dte::EpisodicMemory (64-episode ring) stores reservoir state snapshots on significant events (collisions, goals) and recalls the nearest state by cosine similarity as feedback input. The readout W_out is trained online by RLS (recursive least squares, forgetting factor 0.999, P initialized to 1e6 on the diagonal) to predict the outcome reward from reservoir state - observational only, never actuated (the SRD source whitelist and neuralActuationUnlocked semantics are unchanged). The HUD ESN panel reports episode count, readout L1, the TRIZ derivative, RLS update count, semantic concept count and intrinsic plasticity updates.
- u9n reservoir/episodic port (v27 learning): the episodic memory gains u9n EpisodicMemorySystem mechanics - emotional encoding (valence, flashbulb markers for |reward| >= 1.0), scored retrieval (Score = Similarity x Strength x (1 + Activation), similarity threshold 0.3, retrieval touches the best episode), decay (strength fades with time since last access, activation decays 1% per tick), 4-phase consolidation every 64 ticks (strengthen x1.1, replay strongest x3, abstract the strongest episode into a semantic concept via EMA prototype merging at cosine > 0.7 with variance tracking, prune weak episodes below strength 0.1 with fewer than 3 accesses), and intrinsic plasticity (homeostatic mean-shift of reservoir activations toward zero mean, clamped [-1,1]). Social agents gain u9n SocialCognitionSystem relationship typing (ComputeRelationshipType threshold cascade: Adversary/Rival/Stranger/Acquaintance/Friend/CloseFriend/Ally) with familiarity accumulated by recorded group interactions (RecordInteraction: +0.05 familiarity, +0.01 affinity per grouped tick). The selftest verifies all of these online (45/45 checks). The reservoir runs at priority 8 before navigation (priority 6), so vision/social channels reflect the previous tick's render - a one-tick delay that is correct for a temporal memory system.
- Live learning telemetry (v27): every 5 ticks the full DTE learning state is appended as a CSV row to `datasets/V27_LIVE_TELEMETRY.csv` next to the executable (tick, cognitive mode, echobeats phase, TRIZ coherence, accumulated reward, group count, ESN energy, episodic episode count, recall score, TRIZ temporal derivative, RLS readout updates, semantic concept count, intrinsic plasticity updates, consolidations, online Q updates, learned DTE proposals, LLM training updates/accuracy/loss, sensorimotor reward/steps, average agent familiarity, ally and friend counts, GNN training updates/loss/accepted count). This is plain ofstream appends - no capture APIs - so autonomous learning is externally observable while the world runs. Note the u9n recall score is `similarity x strength x (1 + activation)`; episode strength is clamped to [0, 2] and activation to [0, 2] (matching the u9n reference practice of bounding memory strength), so the score is bounded by 6.0 - it is a retrieval strength score, not a normalized probability. An earlier unbounded variant showed runaway consolidation (recall growing without limit as strength compounded x1.1 per consolidation cycle); the clamp keeps long-run memory scores stable.
## Provenance

Original frozen NN weights SHA-256:

```text
8162a9deaebc2187556996ceca86434718419ed317d30e7ac373213a38d1fd61
```

The clean-room v26 DTE LOD is unchanged. No supplied engine-bound/proprietary code, GTA assets, opaque scripts, external credentials or user screenshots are introduced.

