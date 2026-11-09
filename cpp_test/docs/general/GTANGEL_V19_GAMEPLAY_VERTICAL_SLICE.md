# GTAngel v19 — Ariadne and the Broken Signal

**Author:** Manus AI  
**Date:** 12 September 2026  
**Status:** Implementation design

## Playable premise

The first complete GTAngel gameplay loop is **Ariadne and the Broken Signal**. Ariadne is the principal non-player character and keeper of a damaged civic beacon. She is rendered as a distinct articulated 3D character, follows a bounded autonomous routine around the sanctuary, remembers the player’s conduct, and offers a quest only when the player approaches and faces her.

> The vertical slice must be understandable without telemetry, external AI services, or a network connection: see Ariadne, approach, interact, accept a purpose, act in the world, witness the consequence, return, and receive closure.

## Quest arc

| Stage | Player goal | Grounded completion condition | Visible consequence |
|---|---|---|---|
| Dormant | Discover Ariadne | Enter interaction range while facing her | NPC highlight and `E — Speak` prompt |
| Offered | Understand the broken signal | Open dialogue and select **Accept** | Quest tracker appears; beacon changes to amber |
| Seek Shard | Recover the signal shard | Enter the shard trigger and press **E** | Shard disappears; inventory becomes `Signal Shard ×1` |
| Restore Beacon | Carry the shard to the beacon | Enter beacon radius with the shard | Beacon turns cyan; a vertical light column activates |
| Return | Report to Ariadne | Speak to Ariadne after restoration | Ariadne’s disposition and dialogue change |
| Complete | Receive recognition | Select the completion dialogue | Quest completes; sanctuary lighting warms; `Resonance Sigil` reward is granted |

Failure is recoverable. Leaving the arena, colliding repeatedly, or declining the quest never destroys progress. The player may ask Ariadne to repeat the current purpose, and an abandoned shard returns to its trigger after reset.

## Main NPC contract

Ariadne owns identity, position, facing, routine, interaction range, disposition, memory flags, dialogue state, and current quest relevance. Her autonomous routine alternates between **watch beacon**, **patrol sanctuary**, **notice player**, **face player**, **converse**, and **celebrate restoration**. Routine changes are driven by local player distance, facing, quest state, and recent interaction—not remote telemetry.

Dialogue is assembled from current state rather than emitted as a fixed fake agent response. The native dialogue composer combines greeting, observed world condition, quest intent, disposition, and available choices. External language generation can later enrich wording, but gameplay truth remains owned by the quest state machine.

## Quest architecture

`QuestManager` owns a small data-driven graph of `QuestDefinition`, `QuestObjective`, and `QuestRuntime` records. Objectives are advanced only by grounded gameplay events: `NpcSpoken`, `QuestAccepted`, `TriggerEntered`, `ItemCollected`, `BeaconRestored`, and `RewardGranted`. A completed objective activates its declared successor; no timer or arbitrary progress increment may complete a quest.

The first release stores quest and inventory state in `gtangel_v19_save.json`. Save writes use a temporary file followed by rename. The game remains playable if persistence fails and reports the failure in the native status rail.

## Graphics and UI

The 1024×768 client area remains fixed. The left 768×768 region is the authoritative 3D viewport. The right 256-pixel rail contains the quest tracker, inventory, Ariadne disposition, control legend, and compact system state. Dialogue uses a lower-third translucent panel over the viewport; objective markers and interaction highlights are drawn after the authoritative framebuffer blit and do not alter perception buffers.

| Visual element | Native implementation |
|---|---|
| Ariadne | RE3-informed frame/atomic hierarchy with a distinct violet-gold material and idle/patrol/converse poses |
| Signal shard | Rotating emissive octahedron with cyan pulse |
| Beacon | Existing goal geometry promoted to a three-state material: broken, charged, restored |
| Interaction | Outline/ring and world-space marker based on local range and facing |
| Quest tracker | Right-rail stage, current objective, and grounded progress |
| Dialogue | GDI lower-third with generated state-aware line and numbered choices |

## Controls

| Key | Action |
|---|---|
| `W/A/S/D` | Move and turn the player |
| `E` | Interact, collect, restore, or continue dialogue according to local context |
| `1/2/3` | Choose a dialogue option |
| `J` | Toggle quest journal |
| `F5` | Save gameplay state |
| `F9` | Load gameplay state |
| `R` | Recover the current quest objective without erasing completed progress |

Telemetry, the peer relay, and neural actuation remain optional and disabled by default. The safe expert/player controller remains authoritative for this gameplay release.

## Acceptance sequence

The build is accepted only when a human can launch the executable with no credentials, see Ariadne and the 3D sanctuary, accept the quest, collect the shard, restore the beacon, return to Ariadne, receive the sigil, save, reload, and observe the completed world state. Screenshots must prove the NPC interaction, active objective, restored beacon, and completed quest state.
