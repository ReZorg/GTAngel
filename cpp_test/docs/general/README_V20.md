# GTAngel v20 — dual-reflex sensorimotor / civic repair slice

**Scope:** An independent, clean-room native Win32 C++17 vertical slice. It adds an original, procedural utility cart and a specialized, reversible beacon-repair transaction to the validated Ariadne v19 quest. It does **not** import, compile, execute, link, or redistribute the uploaded GTA/re3-like engine code, model files, SCM scripts, RenderWare objects, or other third-party art. The original v19 generated source remains byte-for-byte unchanged in `v19_baseline.cpp` (SHA-256 `73d9d99ab6d9400fd93656651663c715fda23a2d538709d0e7167ba3b88ff68d`).

## The playable demonstration

An operator explores the original 3D test world in a true **768×768 colour/depth/object-ID viewport** within a 1024×768 client. Ariadne offers the Broken Signal quest. A procedural local utility cart starts beside the operator: approach it and press **Q** to board, use **W/A/S/D** to drive, stop and press **Q** to exit. Collect the signal shard on foot, approach the broken civic beacon, and press **E** to repair it. That event must pass the independent World Governance Reflex, mutate through its local effector, and be **re-observed in actual rendered object-ID/depth/colour evidence** before it counts as accepted. Return to Ariadne for the Resonance Sigil. Save with **F5**, load with **F9**, inspect the journal with **J**, use **Home** for the existing reset, and keep telemetry dormant unless intentionally enabled by **F10/F11**.

The compelling next public demonstration is a *self-healing city that cannot simply hallucinate or decree its repair*: an embodied Agent Zero executive may propose where to go or what to restore, but only local body/world reflexes can commit changes, and the system must see the consequences from its own visual perspective. **This version does not connect a live Agent Zero executive or implement all 17 virtual devices**; it establishes a bounded exemplar for those later integrations.

## Implemented authority boundaries

- **MAG** explicitly tags human, route and learned candidates; human-local input has priority. Learned/route/script/replay/Agent Zero/remote proposals remain **non-authoritative in v20**, including if an upstream shield claims acceptance.
- **SRD** owns creation of `MotorCommit`; only its friend `BodyEffector` can consume the one-shot nonce. Freshness, frame sequence, finite ranges, confidence, uncertainty, ethics, collision risk, emergency stop, replay order, and source are checked first. A rejected untrusted sequence cannot poison the local replay cursor. Motion uses signed throttle/brake transition, speed-dependent steering, explicit dt, grounded pose, and local swept footprint checks against pillars, Ariadne, shard/beacon, and world bounds.
- **NPD/WGR/WED** cover one specialized niche transform: restore Ariadne's beacon when the quest and local geometry support it. WGR checks a versioned proposal's freshness, locally permitted source, inverse/snapshot, one-unit budget, protected-centre/quest overlap, proximity, and stage. Only WGR can mint `NicheCommit`; only WED invokes the private quest mutation. Quest/NPC/world/version snapshots restore on failed **or throwing** observation. A successful change has world version 2 and a recorded observation receipt.
- **Measured evidence:** body pose, speed and contact are taken from the local effector. The 64×64 retina and 12×12 map are *derived from* the 768×768 renderer; `dte::SensorimotorLoop::captureFrame()` and random `selectAction()` are no longer invoked by the native v20 runtime. Peer attention remains advisory, and telemetry defaults off. No web credentials or external producer are required for native play.
- **Save v2:** single atomic JSON (`gtangel_v20_save.json`, temp write + replace); validates finite/ranged vehicle, parked cart, quest, and niche state and cannot accept a restored beacon without freshly rendering its semantic/depth consequence. F9 may migrate the prior v19 v1 file to safe cart/version defaults. A bad file leaves live state unchanged.

## Reproduce

In this directory on Linux with Node.js, g++ and MinGW-w64:

```bash
node generate_v20.mjs
node verify_v20_source.mjs gtangel_agi_v20_full.cpp
g++ -std=c++17 -O2 verify_v20_cns.cpp -o verify_v20_cns && ./verify_v20_cns
g++ -std=c++17 -O2 verify_v20_renderer.cpp -o verify_v20_renderer && ./verify_v20_renderer
g++ -std=c++17 -O2 -I /home/ubuntu/gtangel-v19-work verify_v19_gameplay.cpp -o verify_v19_gameplay && ./verify_v19_gameplay
x86_64-w64-mingw32-g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -mwindows gtangel_agi_v20_full.cpp -o gtangel_agi_v20.exe -lwinhttp -lgdi32 -luser32 -lshell32 -lole32 -lcomdlg32
```

On Windows, run `run_v20.bat` next to the executable. `build_v20.bat` optionally regenerates and compiles it if Node.js and MinGW-w64 are on `PATH`. The generator **reads an isolated copy** of the independent v19 generated source (`v19_baseline.cpp`) and performs exact, uniquely checked transformations; it never edits v19's original generated source. The standalone verifier fixtures are extracted from the generated v20 source.

## Verified evidence in this release

| Gate | Result |
|---|---|
| v19 quest verifier | **17/17** preserved, including Ariadne interaction and reward |
| v19 baseline source invariants | **24 required / 6 forbidden** passed |
| v20 native adversarial verifier | **124 checks / 0 failures**: proposal pre-emption, starvation resistance, vehicle enter/drive/collision, world-source/version/rollback, save/migration, complete Ariadne reward |
| v20 generated-source invariants | **39/39** passed |
| Native Win32 build | Static x86-64 PE32+ GUI target cross-compiled and linked |
| Windows launch smoke test | Copied executable launched on the attached Windows computer; after five seconds it remained running with window title `GTAngel v20 - Ariadne / local cart and governed beacon` (process ID 10852 at the time of test) |
| 768×768 renderer | Valid; **3,666 cart ObjectVehicle pixels**, Ariadne **443**, shard **263**, restored beacon **3,736** in the most recent run; separate native harness confirmed a WGR repair through real colour/depth/object-ID re-observation |

`v20_render_benchmark.json` and `v20_source_verification.json` are reproducible machine-readable records. PNGs in this package are **unchanged re-encodings of actual renderer output**, not Windows GUI screenshots or imported art.

Release SHA-256: executable `12a58daa8bdc41365955c6c2120ebd3124936ff7f35c8312d2faffaa3603703f`; generated source `3c2dd4d8fdf555f9a11b23e1d51b71750e6a386dca7c449ef9eff93663ebb777`.

## Not claimed / next acceptance

- **Windows visual/gameplay acceptance is still pending.** The launch smoke test verifies an alive process and window title, not visible pixels, actual keyboard handling, or successful quest play on the user's workstation. Inspect the already-running v20 window (or run `C:\ddd\GTAngel\cpp_test\run_v20.bat`), verify **Q entry/exit**, E/shard/beacon progression, F5/F9, and the 768×768 viewport; report a screenshot or concise observed result.
- The safety proofs cover the **new v20 governed motor path and specialized beacon mutation**. Existing v19 quest events such as shard pickup and authored NPC routine are maintained but are not yet all routed through a generalized WGR; do not interpret this slice as a universal world-editor proof.
- The new cart is original procedural geometry with simple kinematics and swept point-disc collision; not GTA assets, full traffic/road graph, rigid-body physics, vehicle damage, navigation autonomy, or a production-grade simulator.
- Learned, route, script, replay, Agent Zero and remote source tags are represented and challenged by native tests; they are not connected as live controllers. DAEDALOS device registry/permission integration and NMD consolidation remain future work.
- No live external producer, peer gameplay, web runtime, or two-machine acceptance was performed. Previous workstation bridge failures are not converted into a claim of success.
