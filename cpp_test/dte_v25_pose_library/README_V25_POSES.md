# Deep Tree Echo — multi-pose video and skinned 3D rig

## Deliverables

- `dte_multi_pose_reel.mp4` — **12.04-second, 720 × 1280, H.264** reference-driven cinematic pose study. It shows a consistent DTE-inspired character through an attentive stance, walking, inspection turn, and winged guard. This video is **not motion capture of the rig** and its pixels are not a texture/animation source for the GLB.
- `assets/dte_v25_pose_rig.blend` — editable Blender 4.0 character project, with skin, original source skeleton and six independently named authored actions. It is a cleaned variant of the existing GTAngel DTE v22 Blender rig, not a reconstructed photogrammetric copy of a single photograph.
- `assets/dte_v25_pose_rig.glb` — portable glTF 2.0 character/armature export with **one 43-joint skin, two mesh definitions, and seven named animation clips** (six new pose actions and the preserved, optional 36-frame wing-deploy action). Blender re-import independently found one armature and seven actions.
- `dte_v25_motion_binding.json` — versioned, *advisory only* GTAngel clip-selection contract. This is a **configuration proposal, not a deployed v24/v25 game adapter**.
- `preview/dte_v25_idle_explore.png`, `preview/dte_v25_reach.png`, `preview/dte_v25_guard_ready.png` — actual Blender renders of the exported rig's three representative poses. `preview/dte_video_contact.png` is a five-frame inspection sheet of the separate generated video.
- `assets/pose_validation.json` and `assets/structural_validation.json` — machine-readable rig tests and glTF structure checks.

## Rig pose inventory

| Clip | Authored motion | Scope |
|---|---|---|
| `DTE_Pose_Idle_Explore` | 49-frame attentive idle loop | Visual only |
| `DTE_Pose_Walk24_Loop` | 24-frame in-place alternating walk, contact frames 1 and 13 | Visual only; no root motion |
| `DTE_Pose_Turn_Inspect` | 32-frame head/torso inspection | Visual only |
| `DTE_Pose_Reach` | 32-frame forward right-hand reach | Visual only |
| `DTE_Pose_Guard_Ready` | 32-frame two-hand guard with restrained wing articulation | Visual only |
| `DTE_Pose_Return_Recovery` | 32-frame guard-to-neutral recovery | Visual only |
| `ACT_DTE_V22_WingDeploy_Guard_36F` | Unaltered historical wing-action fixture | Optional; disabled by default in the Blender preview |

The supplied references drive the **character identity and intended pose language**. A single 2D image cannot establish watertight geometry, unseen anatomy, exact occluded garments, accurate skin weights, or facial performance; the already authored DTE body is the editable base. The video and GLB are *parallel* interpretations of the references. Neither proves one-to-one identity match or real-time animation in GTAngel.

## Validated and intentionally changed geometry

`build_pose_library.py` reopens the included `assets/dte_v22_rig_source.blend` **read-only**. `rig_mesh_cleanup.py` deletes exactly 2,387 disconnected source organic-feather components (14,226 vertices) and two clearly isolated lateral strips (105 vertices), preserving the central belt/skirt straps. It compacts the separate, rig-weighted mechanical wing vertices to 42% relative to each original side's shoulder mount. Total body: 93,606 → 79,275 vertices and 61,811 → 51,979 faces; hardware wing topology remains 188 vertices / 148 faces; both wing-root bones and original 41-curve action remain present. This is a deliberately modified **new variant**, not a claim of zero mesh changes or a modification to the original v22 file.

Real rendered previews reveal **remaining organic feather/curved-ribbon fragments, pale source texture and stylized facial proportions**: these are visible limitations, not hidden by structural checks. In particular the GLB still needs a dedicated artist-led retopology/material pass before it should replace the game's existing validated `dte_v22_lod.bin`. The short AI video appears closer to the photographic reference than this editable source mesh. The original wing deploy action has **not** been signed off against the cleaned mesh for runtime playback.

## Reproduce and verify

From this directory with Blender 4.0.2, Python 3.11+ and FFmpeg/ffprobe installed:

```bash
blender -b --python build_pose_library.py
python3 validate_glb.py assets/dte_v25_pose_rig.glb --json assets/structural_validation.json
ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1 dte_multi_pose_reel.mp4
```

Open `assets/dte_v25_pose_rig.blend` in Blender's Action Editor; select a `DTE_Pose_*` action to inspect or modify the rig. Import the GLB into Blender or any glTF skin-and-animation viewer to inspect the seven clips. Blender 4.0.2 re-import verified one armature, 43 bones and seven actions. The original source and supplied references are retained in `assets/` and `references/` for provenance. `build_pose_library.py`, `rig_mesh_cleanup.py`, `validate_glb.py` and the binding JSON form a reusable retargeting **starting point**, not a full game-engine importer.

## GTAngel / DAEDALOS integration boundary

The current GTAngel native v24 code and its 768×768 AI colour/depth/object-ID sensor frame are **unchanged**. The optional higher-resolution display is independent. Future animation playback must consume *measured locally accepted* body speed, inspection/reach interactions and held guard intent; foot IK and terrain adaptation remain future work. The rig's animation never generates collision, pose translation, `MOTOR_COMMIT`, `NICHE_COMMIT`, or world/quest state. Agent Zero, network pings, scripts and replay remain advisory. Native GLB importing, runtime pose sampling, mapped materials, FPS benchmarking and Windows visual acceptance of this new rig are **not yet implemented or claimed**. Do not overwrite the shipping `assets/dte_v22_lod.bin` with this GLB.
