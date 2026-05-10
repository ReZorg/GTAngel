# UE5 Spec for Embodied Cognition

> **Audience.** UE5 plugin authors and avatar configurators wiring the UE5 side
> of GTAngel to the .NET embodied-cognition module added in PR #12.
>
> **Source of truth.** The .NET implementation in `GTAngel/Services/EmbodiedCognition/`
> and `GTAngel/Models/EmbodiedCognition/` is canonical; the rules below are
> *derived* from what that code actually requires. If the UE5 side wants a
> different convention, change this doc + the code together — don't silently
> diverge.

This document captures the constraints, contracts, and metrics the UE5 side
must satisfy so the embodied-cognition pipeline behaves correctly. Each rule is
linked to the specific .NET code that depends on it.

---

## 1. Data contracts

The cognition module sees the engine *only* through three IPC types defined in
`GTAngel/Interop/UE5ProcessManager.cs`:

- `AvatarObservation` — the per-tick ground-truth snapshot UE5 emits.
- `PerceivedObject` — what the UE5 AI Perception system reports as nearby.
- `NeurochemicalSnapshot` — DTE NeurochemicalSystem state.

The cognition module then emits `AvatarAction` back to UE5.

### 1.1 `AvatarObservation` — required fields

| Field | Required? | Cognition use | Failure mode if violated |
|---|---|---|---|
| `Timestamp` (double, **monotonic seconds**) | **Yes** | Drives `SpatialMemory.Decay()` time math; copied through to `PerceptualField.Timestamp`. | Non-monotonic / wall-clock-jumpy timestamps will break decay (entries can decay forwards or refresh out of order). |
| `Position` (`float[3]`, world UU, X, Y, Z) | **Yes** | Origin for FOV cone math, distance math, and elevation. | Null/short array is tolerated (filled with zeros) but yields a zero-position avatar — every percept's bearing is wrong. |
| `Rotation` (`float[3]`, **degrees**, [Pitch, **Yaw**, Roll]) | **Yes** (Yaw) | `Rotation[1]` is read as yaw and used for body-frame bearing. Pitch and Roll are stored but currently unused by perception/motor math. | Wrong index ⇒ avatar acts like it's always facing world-+X. |
| `Velocity` (`float[3]`, world UU/s) | Optional | Planar speed = √(vx²+vy²) → `EmbodiedSelfState.Speed`. Not used for gating. | Null is tolerated. |
| `PerceivedObjects` (`PerceivedObject[]`) | **Yes** | The only world view fed to the policy. | Null/empty ⇒ avatar wanders blindly. |
| `NeurochemicalState` (`NeurochemicalSnapshot?`) | Optional | Drives `Self.Fatigue = 1 - Homeostasis` and `Self.Arousal = 0.5·ChaosIntensity + 0.5·Curiosity`. | Null is fine; both default to 0. |

**Other `AvatarObservation` fields (`FrameBase64`, `ActiveInputActions`,
`PlayerMode`, `ArbitrationScore`) are NOT read by the cognition module** —
they're consumed by adjacent subsystems (ESN vision, KSM Cycle 6 arbitration).
The cognition module does not depend on them.

### 1.2 `PerceivedObject` — required fields

This is the **single most important contract** in the system: it is the only
channel through which the cognitive policy learns what exists in the world.

| Field | Required? | Cognition use | Notes |
|---|---|---|---|
| `Tag` (string) | **Yes** | (a) Identity for `SpatialMemory` (grouped by `(tag, gridX, gridY)`); (b) drives `ReactivePerceptionPolicy.InterestingVisualTags`; (c) `SilentTags` (`Scenery`, `Prop`, `Landmark` by default) suppress auditory percepts. | **Must be stable across ticks** — a moving NPC with a changing tag will create new memory entries every tick. Recommended: `"NPC:<id>"` or `"Vehicle:<id>"` with a stable suffix. Empty tag is allowed but defeats memory and policy filtering. |
| `Location` (`float[3]`, world UU) | **Yes** | Origin for distance, bearing, and elevation math. Must use the same world frame as `Position`. | Length < 2 ⇒ object is silently dropped. |
| `Distance` (float, world UU) | Optional but recommended | If `> 0` the engine value is taken as **3D Euclidean distance** for sight-range and hearing-range gates. Otherwise we compute **planar (XY) distance** ourselves and use that for both. | The cognition module always computes its own planar distance internally for elevation math, regardless. |
| `IsVisible` (bool) | **Yes for sight** | When `PerceptionConfig.RequireVisibility` is true (default), only objects with `IsVisible == true` enter the visual percept set. **Hearing ignores this flag entirely** — sounds pass through walls. | This flag must encode UE5's actual line-of-sight check (e.g. `UAIPerceptionComponent`'s `Sight` sense). If UE5 reports stale or always-true visibility, occlusion is broken. |

**The cognition module does not read any other `PerceivedObject` field.** If the
UE5 plugin extends the type (affiliation, hostility, last-seen-time, etc.),
those fields will be ignored. To use them, surface them through `Tag` (e.g.
`"NPC:hostile"`) or extend the contract here and in the cognition policy.

### 1.3 `NeurochemicalSnapshot`

Only three fields are read:
- `Homeostasis` ∈ [0, 1] → `Fatigue = 1 - Homeostasis`.
- `ChaosIntensity` ∈ [0, 1] → contributes 50% to `Arousal`.
- `Curiosity` ∈ [0, 1] → contributes 50% to `Arousal`.

These are exposed on `EmbodiedSelfState` for future policies; the included
`ReactivePerceptionPolicy` does not consume them. UE5 can leave the snapshot
null without breaking anything.

### 1.4 `AvatarAction` — what cognition writes back

| Field | Cognition output | UE5 must handle |
|---|---|---|
| `InputAction` (string) | One of: `IA_Move`, `IA_Look`, `IA_Sprint`, `IA_Crouch`, `IA_Interact`, `IA_StrafeR`, `IA_StrafeL`, `IA_Jump`. | All eight must be wired as Enhanced Input Action assets. (These are the same names already emitted by `AvatarExplorationPolicy`; this PR introduces no new action names.) |
| `AxisX` (float ∈ [-1, +1]) | `IA_Move`: body-frame **right** axis. `IA_Look`: yaw delta (negative = look left, positive = look right). | Bind `IA_Move` X to right-axis movement, `IA_Look` X to yaw input. |
| `AxisY` (float ∈ [-1, +1]) | `IA_Move`: body-frame **forward** axis. (`AxisY = cos(bearing)`, `AxisX = sin(bearing)`.) `IA_Look` always emits 0 here. | Bind `IA_Move` Y to forward-axis movement. |
| `Magnitude` (float ∈ [0, 1]) | Joystick magnitude for analog actions; 1 for digital (Jump, Interact). Always ≥ `MotorConfig.DeadzoneMagnitude` (0.05) when non-zero. | Use directly as analog scalar. |
| `HoldDuration` (float, seconds) | Default 0.25 s for analog moves; 0 for instantaneous events. | Hold the input active for this many seconds, or release at next tick — both are acceptable since the cognition loop re-issues each tick. |
| `Source` (string) | Telemetry only (e.g. `"Reactive:Approach:Pickup"`, `"Reactive:OrientTo:NPC:1"`). | Log/forward; no semantic meaning to UE5. |

**Crouch & sprint are stateful:**
- `IA_Crouch` arrives as a *toggle* (alternating Magnitude=1 / Magnitude=0).
  UE5 should treat it as toggle-on-rising-edge, not as a hold.
- `IA_Sprint` will not be emitted while the avatar is logically crouched —
  the motor controller suppresses it. UE5 doesn't need a separate
  sprint-while-crouched lockout (cognition already enforces it).
- `IA_Jump` will not be emitted while the avatar is logically crouched.

---

## 2. Coordinate & unit conventions

These are the conventions the math in `SensoryPerceptionService` and
`MotorController` is hard-coded to expect. UE5's defaults match — but if
the GTA3DE port changes any of these, tests will fail in unobvious ways.

### 2.1 World frame

- **UE5 default actor frame** (left-handed, Z-up).
- `Position[0]` = X (avatar **forward** when yaw=0), `Position[1]` = Y
  (avatar **right** when yaw=0), `Position[2]` = Z (up).
- `Location[]` on `PerceivedObject` uses the same world frame.

### 2.2 Yaw convention

- `Rotation[1]` is the **yaw in degrees**, with **0° pointing along world +X**
  (the avatar's forward direction).
- Yaw rotates in the `atan2(dy, dx)` sense: yaw = +90° turns the avatar from
  facing +X toward facing +Y.
- This matches UE5's `FRotator(Pitch, Yaw, Roll)` Yaw and the existing
  `obs.Rotation[1]` usage in `DTE4EAvatarService` (line 465).

The cognition module's bearing math (`SignedYawDelta`) reduces to
`atan2(target.Y - self.Y, target.X - self.X) - yawDeg`, normalised to
`(-180, 180]`. **Positive bearing = target is to the right of forward;
negative = to the left.** This is internally consistent with
`MotorController.BuildMoveToward`, which emits
`AxisX = sin(bearing)` (positive ⇒ strafe right) and `AxisY = cos(bearing)`
(positive ⇒ walk forward), so a positive-bearing target produces a
forward-and-right-strafe `IA_Move`.

> **Verification:** with avatar yaw = 0° and target at `(self.X + 100, self.Y + 0, self.Z + 0)`,
> `SignedYawDelta` returns **0°** ⇒ target dead ahead.
> With target at `(self.X + 0, self.Y + 100, self.Z + 0)`, it returns
> **+90°** ⇒ target to the right of forward (matches UE5's left-handed
> actor frame where +Y is the avatar's right when yaw = 0).

### 2.3 Distance units

- All distance fields are **Unreal Units (UU)** ≈ centimetres in standard UE5.
- The defaults in `PerceptionConfig` (sight 2500 UU, hearing 4000 UU,
  full-strength sight 300 UU, full-loudness 200 UU) assume the UE5 default
  scale. If the GTA3DE port has scaled the world, multiply these defaults
  by the same factor or override `PerceptionConfig` at construction.

### 2.4 Body-frame movement axes

`MotorController.BuildMoveToward` emits:
- `AxisX = sin(bearing)` ⇒ **+X means strafe right relative to body**.
- `AxisY = cos(bearing)` ⇒ **+Y means walk forward relative to body**.

So when bearing is 0° (target straight ahead), `(AxisX, AxisY) = (0, 1)`. UE5
must bind `IA_Move`'s X to right-strafe and Y to forward-walk for this to
produce the right motion. If UE5's `IA_Move` is configured with X as forward
(some templates do this), swap the axes in the Enhanced Input mapping or
adjust `BuildMoveToward` — don't fix it on the UE5 side silently.

---

## 3. Sampling rate & latency budget

| Property | Value | Source |
|---|---|---|
| Cognition tick rate | **4 Hz (250 ms)** | `DTE4EAvatarService.ExplorationStepIntervalMs = 250f` |
| Maximum tolerable observation staleness | ≤ 250 ms | Same — at 4 Hz, an observation older than one tick is already stale. |
| Memory half-life (default) | ≈ 13 s | `SpatialMemory.DecayPerSecond = 0.05` ⇒ `t½ = ln 2 / 0.05`. |

**UE5 should emit `AvatarObservation` at ≥ 4 Hz** (≥ once every 250 ms). The
cognition loop runs on the .NET side independent of the UE5 game thread; if
observations arrive less frequently, the cognition loop will see the same
observation multiple times and `SpatialMemory.Decay()` will run on stale data.

**It is fine — and expected — for UE5 to emit observations more frequently
than 4 Hz**: the cognition loop polls the most recent observation each tick.
A typical setup:
- UE5 emits observations from the AI Controller at the game-thread tick rate
  (60–120 Hz).
- The .NET side picks up the latest one every 250 ms.

---

## 4. UE5 side: avatar configuration

### 4.1 AI Perception component

The avatar controller in UE5 should be configured roughly as follows. These
settings produce a stream the cognition module can act on without further
filtering at the .NET level.

```
UAIPerceptionComponent
├─ Sight Sense
│   ├─ Sight Radius                   ≥ 2500 UU         (matches PerceptionConfig.SightRangeUu)
│   ├─ Lose Sight Radius              ≥ 3000 UU         (some hysteresis)
│   ├─ Peripheral Vision Half-Angle   ≥ 55°             (matches FieldOfViewDeg / 2 = 55°)
│   ├─ Auto Success Range             0                 (don't pre-grant visibility)
│   └─ Affiliation Filter             permissive        (the cognition module decides interest, not the engine)
└─ Hearing Sense
    ├─ Hearing Range                  ≥ 4000 UU         (matches HearingRangeUu)
    └─ LoSHearing                     OFF               (cognition does NOT want occluded-hearing semantics — see §4.2)
```

The cognition module is **stricter** than the engine on FOV (it re-checks the
cone in software) and **more lenient** on hearing (it does its own 1/r²
falloff). It is OK if UE5 emits a slightly larger candidate set; it is NOT OK
if UE5 pre-filters to a smaller set than the cognition module wants — the
cognition layer cannot recover percepts UE5 dropped.

> **Recommendation:** set the UE5 sense radii to **1.2× the values in
> `PerceptionConfig`** so the cognition module has a small buffer when ranges
> are tuned at runtime.

### 4.2 `PerceivedObject.IsVisible` semantics

The cognition module relies on `IsVisible` being **the engine's actual
line-of-sight test result for this exact tick**. Specifically:

- **`true` ⇒ the avatar's sight sensor has a clear line to this object's
  observable point.** Walls, vehicles, and other geometry must occlude.
- **`false` ⇒ the object is in range but occluded.** The cognition module
  will refuse to add it to the visual percept set (when
  `PerceptionConfig.RequireVisibility = true`, the default), but **will still
  add it to the auditory percept set** if the tag is non-silent.

The plugin **must not** set `IsVisible = true` simply because the object is in
the AI perception list — that would defeat occlusion. If UE5's perception API
gives "perceived" without LoS detail, do an additional `LineTraceSingle`
against `ECC_Visibility` between the avatar's eye socket and the object's
center and use the result.

### 4.3 Tag conventions

| Tag pattern | Meaning to cognition |
|---|---|
| `Pickup`, `POI`, `Landmark`, `Vehicle`, `NPC`, `Doorway` | Treated as "interesting" by the default `ReactivePerceptionPolicy.InterestingVisualTags`. |
| `Scenery`, `Prop`, `Landmark` | Treated as **silent** by default — added to visuals but never to sounds. |
| `NPC:<stable-id>`, `Vehicle:<stable-id>` | Recommended for moving objects so `SpatialMemory` keys them consistently across ticks. |

> **`Landmark` is in both lists by default** — visible AND silent. That is
> intentional: landmarks are interesting to look at but should not contribute
> to the auditory field.

---

## 5. Metrics UE5 should expose for verification

To verify the perceptual gating is working in-engine (and not silently broken
by a UE5-side filter), the plugin should surface these counters. They are
deliberately small and cheap — none requires per-frame work; once-per-second
sampling is enough.

### 5.1 Per-tick metrics (the cognition module already computes these)

These come back through `EmbodiedDecisionLoop.LastField`:

| Metric | .NET source | Use |
|---|---|---|
| `RawCandidateCount` | `PerceptualField.RawCandidateCount` | UE5-emitted `PerceivedObjects.Length`. Should track UE5's perception sense radii. |
| `Visuals.Length` | `PerceptualField.Visuals.Length` | After FOV/range/visibility filtering. |
| `Sounds.Length` | `PerceptualField.Sounds.Length` | After silent-tag filtering and 1/r² range filter. |
| `Self.Speed` | `PerceptualField.Self.Speed` | Planar avatar speed; sanity-check against UE5's `GetVelocity().Size2D()`. |

Wire these to the UE5 HUD (or an OnScreen.AddDebugMessage) so a designer can
watch them while playing.

### 5.2 UE5 plugin counters to add

| Counter | Why |
|---|---|
| `AvatarObs.Emitted/sec` | Confirms ≥ 4 Hz emission rate. Below 4 ⇒ cognition is starved. |
| `AvatarObs.PerceivedObjectCount` (mean/p95) | Should approximately match `RawCandidateCount` from .NET. Mismatch ⇒ IPC drop / serialisation bug. |
| `AvatarObs.IsVisibleTrue / Total` | The fraction of perceived objects that pass LoS. Stable ratio ⇒ occlusion is being computed; suspiciously close to 1.0 every frame ⇒ probably broken (always-visible). |
| `AvatarObs.UniqueTagCount` | Helps catch tag instability — if it climbs unboundedly while the avatar isn't moving, tags are not stable. |
| `AvatarAct.Received/sec` | Should approximately match cognition tick rate (4/sec). |
| `AvatarAct.InputAction.Histogram` | Fraction of each `IA_*` issued. Useful for catching e.g. constant-jump bugs. |
| `AvatarAct.UnknownInputAction` | Count of `IA_*` strings that don't map to a configured Enhanced Input asset. **Must be 0**. |

### 5.3 Behavioural smoke tests on the UE5 side

Once the avatar is wired up, these manual checks confirm the contract holds.
They mirror the .NET integration tests in `EmbodiedDecisionLoopTests`:

1. **FOV cone respected.** Place an interesting `Pickup` at exactly behind the
   avatar (180°). The avatar must not approach. Rotate it 60° into the FOV
   cone — the avatar must approach.
2. **Occlusion respected.** Put the same `Pickup` behind a wall. With UE5's
   line-of-sight working, the avatar must not approach. Remove the wall —
   the avatar must approach.
3. **Hearing ignores FOV & occlusion.** Tag a moving NPC with a non-silent
   tag and place it behind the avatar **and** behind a wall. The avatar must
   issue an `IA_Look` toward it (within the hearing radius).
4. **Memory decay.** Have the avatar perceive a `POI` for one tick, then
   teleport it away. After ~13 s the entry should drop out of
   `loop.Memory.Snapshot()` (verify via the .NET telemetry feed or a
   debug overlay).

---

## 6. Open knobs (what's safe to tune vs. what isn't)

### Safe to tune via `PerceptionConfig` / `MotorConfig` at runtime

- All ranges (`SightRangeUu`, `HearingRangeUu`, `FullLoudnessRangeUu`,
  `SightFullStrengthRangeUu`, `FieldOfViewDeg`).
- `RequireVisibility` (turning off occlusion gives the avatar X-ray vision —
  useful for debugging cognition without UE5 LoS noise).
- `SilentTags`, `MaxVisualPercepts`, `MaxAuditoryPercepts`.
- `MotorConfig.DeadzoneMagnitude`, `MaxAxis`, `DefaultHoldSeconds`,
  `TurnSaturationDeg`, `SuppressSprintWhileCrouched`.
- `ReactivePerceptionPolicy.Settings` (thresholds, interesting tags).
- `SpatialMemory.CellSize`, `DecayPerSecond`, `MinConfidence`.

### Requires a code change

- World coordinate frame (Z-up, X-forward yaw zero) — hard-coded in
  `SignedYawDelta`, `Distance2D`, `ElevationDeg`.
- The `IA_*` action name set — hard-coded in `MotorController` builders.
- The body-frame movement axis convention (`AxisX = sin(bearing)`,
  `AxisY = cos(bearing)`).
- The shape of the percept records (`VisualPercept`, `AuditoryPercept`,
  `EmbodiedSelfState`) — extending these is a contract change for any
  custom `IPerceptionPolicy` implementations.
- The 4 Hz tick rate — set in `DTE4EAvatarService.ExplorationStepIntervalMs`.

---

## 7. Cross-references

- Implementation: `GTAngel/Services/EmbodiedCognition/`
- Models: `GTAngel/Models/EmbodiedCognition/`
- Wiring: `GTAngel/Services/DTE4EAvatarService.cs` (search for `_embodiedLoop`)
- IPC types: `GTAngel/Interop/UE5ProcessManager.cs`
- Tests pinning these contracts:
  - `GTAngel.Tests/Services/EmbodiedCognition/SensoryPerceptionServiceTests.cs`
  - `GTAngel.Tests/Services/EmbodiedCognition/MotorControllerTests.cs`
  - `GTAngel.Tests/Services/EmbodiedCognition/EmbodiedDecisionLoopTests.cs`
  - `GTAngel.Tests/Services/EmbodiedCognition/SpatialMemoryTests.cs`
