# GTAngel v22 · Deep Tree Echo 3D rig

This independent Win32/C++ GTAngel build replaces the v21 low-poly avatar *when a validated local asset is present*. It does not change the v19 Ariadne quest, v20 bodily/world reflexes, or v21 measured-motion authority. It keeps a real 768×768 colour/depth/object-ID framebuffer in the 1024×768 client; the 64×64/12×12 fields remain downstream cognitive derivatives.

## Open on Windows

The copied workspace is `C:\ddd\GTAngel\cpp_test\`. Run **`run_v22.bat`**, which starts `gtangel_agi_v22.exe`. Keep `assets\dte_v22_lod.bin` next to the executable under the `assets` folder. A missing or corrupt LOD falls back to the v21 procedural avatar, not to an unvalidated remote asset. The original editable rig can be opened separately in Blender as `assets\dte_v22_rig.blend`; a portable skinned/animated `assets\dte_v22_rig.glb` is also included. `build_v22.bat` can regenerate the native source and rebuild it when Node.js and `C:\msys64\mingw64\bin\g++.exe` are available.

Existing native controls remain as in v21: **Space** starts/pauses, **V** toggles third person, **W/A/S/D** move, **B** begins the 36-frame visual guard and later folds its wings, **I** toggles the inspect pose, **Q** boards/exits the cart, **E** interacts with Ariadne and the governed beacon, and **F5/F9** save/load. No web credentials or provider keys are needed to **run** the game; `generate_asset.py` is a separate, already completed optional source-generation pipeline that would need `MESHY_API` only if deliberately repeated.

## Assets and provenance

The supplied DTE pose/reference image informed a one-time Meshy image-to-3D A-pose and humanoid auto-rig. The body was generated rather than extracted from supplied GTA/re3-like source code or proprietary game assets. `build_blend.py` adds original Blender-authored shoulder harness, two independent wing roots, booms, eight weighted crystal plate bones, and a 36-frame visual guard action. The complete GLB contains **2 meshes, 1 skin, 2 animations, and 43 armature bones**. Normalisation uses the body’s actual 1.70 m world-space source height, not its 170-unit armature-local dimensions, and sets the edited body to **1.72 m**.

`export_native_lod.py` converts the actual GLB to a bounded CPU skin-LOD file: **8,000 body triangles + 280 authored wing triangles**, **15,452 vertices** and **34 distinct bind records**. At runtime the v22 adapter conservatively keeps the authored mesh and transfers skin-weighted **joint displacements** from the local visual pose. This is intentionally *not* full quaternion skinning: the provider's GLB has irregular helper-bone tails, and directly transferring those frames distorted the body. Blender/GLB retains the complete editable rig, while the native CPU silhouette remains a reduced, per-triangle UV-colour approximation. `DteSkinnedAsset` only emits `ObjectAvatar` triangles; it has no motor, quest, collision, save, or niche-commit API.

## Verification

| Check | Result |
|---|---|
| Blender front/rear full-body frame-36 preview | **29.9353% / 22.5856%** foreground, both passed >20% visibility gate; 720×900 each |
| Native 768×768 skinned render | **8,280 triangles** loaded; 4,141 idle and 4,739 guard semantic avatar pixels; ~**28.9 ms** average per measured guard frame in the local Linux C++ benchmark |
| Corrupt or absent native binary | Rejected by bounded parser and payload/header CRC; falls back to v21 visual avatar |
| v22 source/authority invariants | **22 passed**; v20 reflex/quest/save and v21 measured-pose implementation spans byte-identical |
| Inherited v21 motion / v20 CNS / v19 quest | **20/20**, **124/124**, **17/17** passed |
| Full-resolution v20/v22 world renderer | Passed, ~**25.3 ms** per frame in the local benchmark; beacon repair observation and vehicle/NPC/shard IDs retained |
| Static Win32 C++17 build | Succeeded, `gtangel_agi_v22.exe` SHA-256 `6204c2f1fe0a6417f69fc7019c468e02c5992484772199e119e96c3a1aadb1cc` |

The actual native character frame is available as `assets/dte_v22_native_guard_close.png` and should be assessed alongside the editable Blender front/rear previews. A 768×768 framebuffer does **not** make an 8k-triangle flat-shaded software LOD photorealistic. Source-quality PBR textures, full inverse-bind/quaternion GPU skinning, dynamic feathers, facial FACS/AU morphs, contact-shadows and cinematic lighting remain future renderer/animation work, not completed claims. The live Windows UI and interactive guard have **not** been visually accepted by a workstation screenshot in this handoff.

## Rebuild and acceptance

From the v22 bundle root on Linux with Blender, g++, Node.js and MinGW installed:

```bash
blender -b --python build_blend.py -- --timeout 90 --poll 2
blender -b --python export_native_lod.py -- --input assets/dte_v22_rig.glb --output assets/dte_v22_lod.bin
node generate_v22.mjs && node verify_v22_source.mjs
g++ -std=c++17 -O2 verify_v22_skin.cpp -o verify_v22_skin && ./verify_v22_skin
g++ -std=c++17 -O2 verify_v22_renderer.cpp -o verify_v22_renderer && ./verify_v22_renderer
g++ -std=c++17 -O2 verify_v21_motion.cpp -o verify_v21_motion && ./verify_v21_motion
g++ -std=c++17 -O2 verify_v20_cns.cpp -o verify_v20_cns && ./verify_v20_cns
g++ -std=c++17 -O2 verify_v19_gameplay.cpp -o verify_v19_gameplay && ./verify_v19_gameplay
x86_64-w64-mingw32-g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -mwindows gtangel_agi_v22_full.cpp -o gtangel_agi_v22.exe -lwinhttp -lgdi32 -luser32 -lshell32 -lole32 -lcomdlg32
```

The final human acceptance gate is to run `C:\ddd\GTAngel\cpp_test\run_v22.bat` on Windows, inspect its actual window and gameplay interaction, and compare the visible player to both Blender rig previews. If the `DTE V22 SKINNED / LOCAL 768` model label is absent, check that the native LOD binary is at the expected adjacent path. Do not infer GUI success merely from a compiled executable or a process/window-title smoke test.
