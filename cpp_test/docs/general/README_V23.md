# GTAngel v23 — navigation and DTE wing-stability correction

An **isolated native Win32 iteration** of the v22 skinned DTE prototype. The original v22 executable and authored Blender/GLB sources remain unchanged. This release uses the existing bounded `assets/dte_v22_lod.bin`; it does **not** assert that its stylized low-detail native render matches the newly confirmed target-avatar reference photographs. Those photographs are visual targets, not screenshots or assets to import into the game.

## Run on the attached Windows computer

Run `C:\ddd\GTAngel\cpp_test\run_v23.bat` or start `gtangel_agi_v23.exe` from the same directory as `assets\dte_v22_lod.bin`. Use **Space** to start/pause. **W/S** move forward/backward; **A/D** turn. **F6** switches between dashboard and the native viewport, so **D no longer toggles the view**. **V** changes the 768×768 display channel. **B** enters/leaves visual guard; **Q** enters/exits the cart, **E** interacts, **1–3** select dialogue, **J** shows the journal, **R** recovers, and **F5/F9** save/load. Telemetry remains optional and off.

This build was cross-compiled and tested in the sandbox. The copied v23 executable was launched on the attached Windows computer and, after five seconds, its process remained **responding** with a nonzero window handle and title `GTAngel v23 - DTE navigation / governed world`. **Live Windows gameplay/visual acceptance is pending**. A responsive window handle is not proof that the requested movement and B image fixes feel correct on the workstation.

## What changed

- **World:** bounded playable plane widened from 20×20 m to **128×128 m** (6.4× per side, 40.96× area); 48 sparsely placed, non-overlapping pillars instead of 14 clustered near the quest. Spawn, Ariadne, beacon, shard and cart locations retain clear placement. Pillar generation has a 4,096-attempt cap. Quest locations were not moved.
- **Performance:** the true 768×768 colour/depth/object-ID renderer draws only a 40 m local ground window inside the 128 m world; the older 64×64 visual sampler uses a separately bounded local window. This is local culling, **not** a resized or stretched low-res authoritative image.
- **Locomotion:** after a full-body swept collision rejects a pedestrian move, independently swept X and Z tangential candidates allow wall sliding without passing through the obstacle. The cart remains on its original stricter full-sweep behavior. Human proposals are no longer vetoed solely by coarse *advisory* visual collision risk; SRD still requires local valid evidence and local authority, and the body effector still enforces actual world geometry and bounds. Neural/route candidates retain visual-risk gating.
- **Wing guard:** the GLB wing is **already open in its neutral native mesh**. Applying the v21 three-joint metre-scale sweep to eight separately weighted wing plates tore it visually. v23 retains the authored feather topology and translates the whole native fan with the upper body during B guard; body and facial pose continue normally. This is a **stability correction, not articulated feather deployment**. True per-feather wing retargeting needs explicit authored pivot/action tracks and a compatible full skinning runtime. The low-poly native frame still has detached-looking adornments inherited from the v22 asset; it is not a photoreal likeness of the supplied target images.
- **UI:** controls on the right rail no longer overlap at the B/quest/save rows and now show F6 and 128 m map status.

## Architecture and reproducibility

`generate_v23.mjs` deterministically patches immutable `v22_baseline.cpp` plus local copies of the original and bounded-skin header; it emits the complete `gtangel_agi_v23_full.cpp` and exact test-fixture slices. It refuses missing or ambiguous anchors. `node generate_v23.mjs && node verify_v23_source.mjs` regenerates and checks the source. `build_v23.bat` regenerates and compiles with an installed Windows MinGW `g++`; or use Linux `x86_64-w64-mingw32-g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -mwindows gtangel_agi_v23_full.cpp -o gtangel_agi_v23.exe -lwinhttp -lgdi32 -luser32 -lshell32 -lole32 -lcomdlg32`.

No attached reference image, GTA/re3 engine source, RenderWare binary, provider credential or raw framebuffer is imported into the executable. The v19 Ariadne quest implementation, v20 save contract and spinal/world governance, and v21 visual pose are byte-identical to v22, checked in `verify_v23_source.mjs`. The generated v23 motor effector remains the sole local body-write route; the specialized beacon world governance remains the only path to its protected niche commit.

## Native regression evidence

| Gate | Observed result |
| --- | --- |
| Ariadne authored quest | 17/17, `COMPLETE` |
| v20 dual-reflex suite against original fixture | 124/124 |
| v23 dual-reflex suite against *modified* v23 CNS | 124/124 |
| v21 motion suite | 20/20 |
| New v23 boundary/slide/source test | 208/208; swept tangential escape stays outside obstacle clearance, remote proposal denied, 20+ m open travel, map edge blocks |
| New native wing topology test | 8,280 triangles; B guard maximum wing vertex translation 0.065 m; **zero** wing-triangle edge stretch |
| Distant full-resolution scene | at (50,50) m, 311,830 ground + 5,270 avatar ID pixels in actual 768×768 buffer |
| v23 renderer semantic/quest proof | valid; NPC 332, cart 3,674, shard 260, restored beacon 3,736 pixels; sample average 14.32 ms/frame |
| v23 native skinned render | valid; idle 4,141, guard 4,654 avatar ID pixels; sample average 23.56 ms/frame |
| Source guard / cross build | pass / statically linked Win32 executable |

Renderer times are sandbox test samples, not promised workstation frame rates. No live keyboard, visual or quest run on the user's Windows display is claimed. The genuine generated close guard frame is `assets/dte_v23_native_guard_close.png`; it exposes remaining stylized fidelity limits rather than substituting for the photographic target.

**Remaining work:** witness live Windows WASD, B, Ariadne and cart acceptance; author a reference-faithful face/hair/outfit + wing skin and individually rigged feathers with a proper runtime skinning/animation pipeline. The supplied reference photographs are targets for that later asset iteration, not evidence of what v23 already looks like.
