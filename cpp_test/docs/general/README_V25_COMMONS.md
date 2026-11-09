# GTAngel v25 — authenticated local VRD ingress and Echo Commons

This is a **separate** Win32 C++ iteration from GTAngel v24. It incorporates the 253 Deep Tree Echo adaptations from [EchoCog/arc-angel-echo PR #1](https://github.com/EchoCog/arc-angel-echo/pull/1) as a bounded, read-only native rule catalogue. It does **not** import the model-authored English guards as if they were implemented, nor promote Agent Zero/DTE into body or world authority.

## Run on Windows

The files are already installed beside the earlier builds in `C:\ddd\GTAngel\cpp_test\`. Double-click `run_v25_commons.bat`, or launch `gtangel_agi_v25.exe`. Keep `assets\dte_v22_lod.bin` beside the EXE for its verified original DTE skinned LOD. The application falls back to its procedural rig if this asset is absent/corrupt. The optional v25 **pose-library GLB** from the previous task remains separate: its multi-animation playback is not claimed to be integrated into this renderer.

- **Space:** start or pause. **F6:** switch between 768×768 sensor-world viewport and the older dashboard. **F7:** open/close independently rasterized 1536×864 display-only view. A screenshot of the dashboard alone does not show the VRD viewport.
- **WASD:** move/turn; **E:** interact or confirm beacon repair; **1–3:** dialogue choices; **J:** quest journal; **R:** recover; **F5/F9:** save/load. **B:** cosmetic guard wings; **Q:** enter/exit local cart; **I:** inspect.
- Follow Ariadne’s original quest. After acquiring the Signal Shard and approaching the beacon on foot, the rule panel may show `DTE APL160: REPAIR IDEA / E CONFIRM`. Press **E** to request the existing WGR-governed repair. The quest still works if no DTE suggestion is present—advice is never a prerequisite.

## Real integration points

1. **VRD sensor (unchanged):** the 768×768 colour/depth/object-ID framebuffer is produced by the local `HighResRenderer`. Its existing 96×96, 64×64 and 12×12 derivations remain unchanged. The optional 1536×864 display never certifies or feeds cognition.
2. **Local ingress:** `LocalVisualIngress` is constructible only by the renderer-owning `GTAngelAGI`; it binds to that exact framebuffer object. A sample is tied to a full-buffer deterministic digest, renderer frame sequence, local body sequence, WGR world version, 150 ms freshness and measured object/depth counts. Replaying a frame, altering pixels, changing body/world state or waiting past TTL invalidates it. This is **in-process provenance**, not cryptographic remote attestation or protection against malicious code injected into the same process.
3. **Rule bank:** all 253 IDs and scalar thresholds compile from a SHA-256-pinned catalogue. Only actually measured channels are evaluated; absent signals are **unknown**, not zero. Matches are advisory counts, not proof of the model-authored multi-clause English guards. Seven APL190–196 Sequence 27/26 alias rules have extra typed predicates, but live production does not supply all required measurements, so these rule effects fail closed. The alias is not seven extra rules.
4. **Echo Commons:** APL160’s *Building Edge* has a **specific** locally measured beacon-affordance adapter: >20 real goal object-ID/depth pixels, current quest/shard/range check, stable on-foot body, unspent niche budget, expected WGR world version and 100 ms candidate expiry. The candidate is a read-only proposal: the separately generated human **E** request is still the only route through `Runtime::repair → WGR → NICHE_COMMIT → WED` and the post-effect actual renderer observation. An Agent Zero-source proposal is deliberately rejected by WGR; a failed consequence rolls back the snapshot. This is a specialization, not a generic APL160 edge-layer protocol.
5. **Agent Zero / DAEDALOS:** Agent Zero may reason about candidate salience and propose attention/niche ideas. DAEDALOS can register provenance/device diagnostics. Neither has access to the private VRD mint, motor commits, WGR commit token or direct world writes. A future out-of-process Agent Zero adapter must authenticate its process separately and remain advisory-only.

## Validation and scope

- Deterministic source check: v19 gameplay, v20 motor/world reflex and v24 raster namespace byte-identical; fixed `768×768` AI sensor and separate `1536×864` display.
- Native standalone **Echo Commons: 49/49 checks**: seven Sequence 27 default/positive/negative predicates, fresh/replay/tamper/stale VRD, DTE candidate TTL, Agent Zero rejection, local operator/WGR acceptance, real frame consequence and rollback.
- Preserved: v19 Ariadne **17/17**, v20 and v23 CNS regressions, 768×768 renderer benchmark (`valid: true`; 60 frames, ~13.4 ms mean on this sandbox), v24 dual-surface display benchmark (`sensor_unchanged: true`). The renderer benchmark timing is a sandbox measurement, **not** a Windows display-FPS promise.
- [Full 253-pattern and Sequence 27 review](review/REVIEW_253_AND_SEQ27.md), [machine-readable review](review/pattern_guard_audit_253.json), [simulation receipt](echo_commons_simulation.json). The review exhaustively checks IDs, titles, source summaries, typed effects and proposed guards; independent historical/semantic certification of each adaptation is **not** asserted. **245** English row-specific guards remain unimplemented; seven alias guards are structurally tested, and one beacon special case is implemented.
- Windows build and in-app F6/F7/E gameplay still require **user workstation visual acceptance**. Local Linux compilation/tests and cross-built Win32 EXE do not establish that an observed Windows playthrough succeeded. Network telemetry remains optional/off.

## Rebuild

From this directory, with Python 3, Node.js 22 and MinGW-w64:

```sh
python3 audit_and_compile.py
python3 render_review.py
node generate_v25_commons.mjs
node verify_v25_source.mjs
g++ -std=c++17 -O2 verify_v25_commons.cpp -o verify_v25_commons && ./verify_v25_commons
x86_64-w64-mingw32-g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -mwindows \
  gtangel_agi_v25_full.cpp -o gtangel_agi_v25.exe \
  -lwinhttp -lgdi32 -luser32 -lshell32 -lole32 -lcomdlg32
```

`v24_baseline.cpp`, `gtangel_agi_v24_renderer.hpp`, the inherited v19/v20/v23 test fixtures, JSON rule catalogue and generator are included in the source bundle. Treat the compiled EXE, catalogue and generated audit as **provenance-linked artifacts**. Never feed a raw network image, synthetic DTE frame, arbitrary JSON or optional high-resolution presentation image into `LocalVisualIngress`.
