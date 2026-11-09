# GTAngel v18 Peer-Relational Validation

**Author:** Manus AI  
**Date:** 12 September 2026  
**Status:** Compiled and validated; live two-instance acceptance remains pending

## Result

GTAngel v18 implements the **doom-loop split** as two independently testable rates. The local high-compute loop retains the authoritative 768×768 colour, depth, and object-ID framebuffer, the derived 64×64 perceptual field, the 12×12 cognitive field, the recurrent neural proposal, and GTAngel’s final safety shield. The non-local low-compute loop exchanges only expiring peer relations: sender, optional recipient, monotonic sequence, 16-sector relative bearing, coarse range band, confidence, intent, event class, trace, source-frame sequence, and evidence digest.[1] [2]

> A peer ping can direct attention, but it cannot become a pixel, depth value, world coordinate, policy input, NavTrainer state, or controller command.

| Boundary | Enforced behaviour | Evidence |
|---|---|---|
| Payload | Native emitter rejects messages above 512 bytes | v18 source invariant |
| Geometry | Protocol has no absolute coordinates; bearing is an integer sector from 0 to 15 | Zod schema and contract tests |
| Lifetime | Server accepts only 250–5,000 ms TTL; v18 emits 3,000 ms and prunes expired predictions | Contract/service tests and native source |
| Replay | Sequence must increase per authenticated source | Database query and service tests |
| Rate | Ingest rejects more than 20 messages per source per second | Service test |
| Authority | Only `gtangel` scoped sources may publish or poll peer relations | Service tests |
| Policy isolation | Peer data is absent from visual tensor, proprioception, GRU, NavTrainer, and shield arguments | Native source verifier |
| Actuation | Neural actuator and ethics approval remain false by default; expert fallback remains final | v17/v18 source invariants |
| Confirmation | Only first-person local `ObjectAvatar` pixels and matching local depth can confirm a predicted relation | Native v18 confirmation path |

## Native Build

The deterministic generator applied six structural replacements to the validated v17 monolith: member insertion, reset/configuration, post-render local confirmation, attention overlay, right-rail telemetry, and the `P` operator toggle. The generated Win32 source compiled statically with MinGW-w64.

| Artifact | SHA-256 |
|---|---|
| `gtangel_agi_v18.exe` | `1f49a3cae8c6372db9f4b17eaa3f59a410d2cdba35c391917b1a76cd0c7a6551` |
| `gtangel_agi_v18_full.cpp` | `196cc48528691bdcf7d83bb6345221a668a4cf02fabac846dae4da2ee84ed425` |

The native verifier passed **13 required** and **11 forbidden-coupling** checks. Peer networking is disabled by default and must be explicitly enabled with `P`. Active relations are aggregated into a dedicated sixteen-sector `AttentionContext` with state-weighted salience; this advisory state is displayed in the rail but is absent from policy, NavTrainer, shield, framebuffer mutation, and controller arguments. Missing credentials or network failure leave the local embodied loop operational.

## Arc Angel Echo Relay

Arc Angel Echo now persists bounded relations in `peerPings`, indexed for owner, recipient, replay sequence, trace, and expiry.[3] The authenticated gateway exposes:

| Method | Route | Function |
|---|---|---|
| `POST` | `/api/peer-pings/ingest` | Validate authority, schema, replay, rate, and TTL before persistence |
| `GET` | `/api/peer-pings/poll` | Return only active broadcast or recipient-matched relations for the authenticated source |

Both routes returned **401** without a bearer credential. The project passes **32 tests**, TypeScript checking, and a production build. Desktop and mobile full-page captures show the relational field without overlap; empty state explicitly states that local vision remains authoritative.

## Remaining Acceptance Gate

No fabricated peer traffic was inserted into the production database. Completion requires two authentic GTAngel instances with separate source credentials. Instance A must observe B locally, emit a bounded ping, and instance B must show the cue as **unconfirmed** until its own first-person framebuffer supplies matching avatar and depth evidence. Replay, expiry, contradiction, and network-loss behaviour must then be observed in the live windows.

## References

[1]: ../shared/echoAngel.ts "Shared coordinate-free peer protocol"
[2]: GTANGEL_DOOM_LOOP_ARCHITECTURE.md "GTAngel doom-loop architecture"
[3]: ../drizzle/schema.ts "Peer-ping persistence and indexes"
