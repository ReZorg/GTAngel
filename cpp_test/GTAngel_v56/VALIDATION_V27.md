# GTAngel v27 validation

## Completed checks — 8 October 2026

| Gate | Result |
|---|---|
| New numerical Q/NN tests | **232/232**; real Bellman weight changes and actual original CNN/GRU inference from the 768 render. |
| New sealed DTE SRD adversarial tests | **23/23**; fake source/no seal, changed digest/body/world, TTL, altered request, replay, bounds, stop, policy gate, actual geometry collision. |
| Complete application-loop test on real Windows | **32/32**, exit 0; no key input for DTE-owned navigation; debug-only injected keys confined to the separately compiled test executable. |
| Sustained DTE soak | **500 cycles**, 500 NN inferences, **499 Q updates**, 499 locally accepted SRD submissions, **205 Q-backed proposals**, 25 actual token-ML training calls. |
| Ariadne inherited gameplay | **17/17**. |
| Inherited v20 CNS | **124 checks**, zero failures, against the v27 CNS header. |
| Inherited v23 CNS | **124 checks**, zero failures, against the v27 CNS header. |
| v24 independent display | PASS, 589,824 sensor pixels and 1,327,104 display pixels; sensor unchanged. |
| v24 and v25 renderer regressions | PASS. |
| Echo Commons | **49 checks**, zero failures; APL160, actual 768 VRD and WGR re-observation preserved. |
| System 7→8 bridge | **1,258/1,258**, 727 structural edges, complete 286 rooted / 47 free System 8 coverage. |
| Source gates | PASS: renderer, frozen NN, body effector, WGR, persistence, v25 ingress and v26 tree implementation remain unchanged. Explicit changes: sealed DTE source, debug yaw signs and cosmetic avatar yaw. |
| Native Windows build | Static PE32+ GUI executable, x86-64. |

`V27_RUNTIME_VERIFICATION.json` is the actual receipt emitted by the Windows test executable. The numerical tests do not substitute invented logits or zero weights for real inference.

## Camera-forward checks

The full Windows loop test submits optional debug W at yaw +90°, −90° and 0.7 radians. Actual SRD body displacement equals `0.1 × (sin(yaw), cos(yaw))` within 1e−4 in XZ. The avatar facial-forward offset agrees with that direction. The renderer's view-yaw transform is unchanged. This validates the current shared horizontal camera/body yaw, not an unimplemented free-look camera.

## Failures discovered during repair

The first full-loop repair test crashed after grounded negative rewards were reconnected to the old token classifier. Inspection found direct unchecked embedding indexing with a negative reward-derived token. The final encoder maps bounded finite proprioceptive values into the valid vocabulary. Subsequent Windows tests run beyond tick 20 and through 500 cycles successfully.

An intermediate test wrongly expected the old `llmMetrics.batches` value to accumulate across invocations; it is per-call batch size. The final runtime exposes and tests a separate actual `llmTrainingUpdates` counter. Neither failed attempt is represented as validation success.

## Preserved boundaries and limitations

- DTE runs by default; no human driving demonstrations are required.
- Sealed local DTE proposals are the newly authorized bounded motor source. Raw Neural/Route/Script/Replay/Agent Zero/Remote sources remain rejected.
- Q learning is online. The original CNN/GRU weights remain frozen shadow inference and are not promoted to motor authority.
- The no-key soak leaves niche world version 1 and beacon unmodified. It does not prove automatic quest completion or better navigation performance.
- Native model asset is still cosmetic; the v25 GLB animation clips are not sampled.
- The Windows harness proves execution on the workstation without operating its GUI. Live F4/F7/F8 appearance, avatar likeness and full gameplay acceptance remain separate checks.
- System 7 and external executives retain advisory-only placement.

## Hashes

| Artifact | SHA-256 |
|---|---|
| Win32 v27 executable | `cab62c2dff82b333a5dfbc5de71b2d3f7c6a4e06f6c91ecf5e6d9c6dcf069049` |
| Generated v27 source | `0f27c1641efbc8050d393ba3e88e51e2ca36705281371f7380955c3bfe485153` |
| Original local NN weights | `8162a9deaebc2187556996ceca86434718419ed317d30e7ac373213a38d1fd61` |
