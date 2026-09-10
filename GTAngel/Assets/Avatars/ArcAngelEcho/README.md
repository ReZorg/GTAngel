# Arc Angel Echo Avatar

**Arc Angel Echo** is the default 3D embodiment profile for GTAngel's Deep Tree Echo cognitive agent. The package keeps the supplied Meshy AI source assets intact and wraps them in GTAngel's reusable, versioned avatar-manifest contract.

## Asset profile

| Property | Value |
|---|---:|
| Height | 170 cm |
| Meshes | 1 skinned mesh (`char1`) |
| Vertices | 453,319 |
| Triangles | 578,861 |
| Skeleton | 22-bone biped |
| Animations | Walk and run, 60 fps |
| Textures | 4096×4096 base colour, normal, metallic, roughness |
| Facial rig | No facial bones or morph targets detected |
| Expression mode | Skeletal head/gaze + emissive aura fallback |

The authoritative runtime definition is `v1/arc-angel-echo.avatar.json`. Every binary entry includes an expected byte length and SHA-256 digest; `AvatarAssetProfileService` validates those values before the profile is activated.

## UE5 import and launch

`UE5LaunchOrchestrator` checks for `SK_ArcAngelEcho`, `A_ArcAngelEcho_Walk`, and `A_ArcAngelEcho_Run` before every launch. When they are absent it runs the committed `GTAngelRuntime/Content/Python/import_arc_angel_echo.py` commandlet, which imports the skeletal mesh and animations, creates the PBR/aura material, assigns textures with correct colour-space settings, saves generated `.uasset` files, and verifies the three required assets before startup continues. Generated Unreal assets remain local build products; the LFS source files and signed manifest remain the repository source of truth.

The default launcher uses `/Engine/Maps/Entry` with `GameModeBase` because the archived GTA content in this repository contains export sidecars rather than complete `.uasset`/`.umap` packages. The runtime auto-spawns and possesses `AArcAngelEchoCharacter` with a third-person camera. A complete licensed GTA map can replace the fallback world without changing the avatar profile or IPC contracts.

Before shipping in dense scenes, generate at least three production LODs. The supplied archival mesh is approximately 579k triangles.

The enabled `GTAngelRuntime` plugin performs this wiring in native code. When GTAngel is launched with `-DTECognitive` or `-GTAngel_DTE_Avatar=1`, it spawns `AArcAngelEchoCharacter`, connects as a duplex client to the .NET-owned `GTAngel_UE5_IPC` pipe, consumes `AvatarAction` messages, and emits compatible `MLVisionFrame`/`AvatarObservation` messages at 4 Hz. A second local pipe, `GTAngel_Embodiment_IPC`, receives the material, locomotion, and expression activation plan.

## Expression strategy

The uploaded mesh has a body/head biped but no face deformation channels. GTAngel therefore preserves the cognitive expression pipeline while adapting its renderer:

- FACS and endocrine state continue to be computed for telemetry and future rigs.
- Head pitch/yaw and `headfront` gaze provide skeletal expression.
- Emotional valence and arousal drive the neon material/aura intensity.
- No Live2D or facial-deformation module is claimed by this profile; a future overlay can be added explicitly.
- A later MetaHuman or blendshape transfer can set `supportsFacs` and `hasMorphTargets` to `true` without changing the cognitive core.

The initial native perception payload deliberately contains an empty `PerceivedObjects` array. World-specific AI Perception and line-of-sight enrichment should be added only where the target map provides stable actor tags; this preserves the existing perception-limited cognition contract rather than emitting fabricated visibility data.

## Adding another angel

Create `Assets/Avatars/<Name>/<version>/`, copy the manifest, update asset keys/hashes and rig capabilities, and point `AvatarAssetProfileService` to the new manifest. The WPF, integrity-validation, UE5 command, embodiment, and cognitive layers remain unchanged.
