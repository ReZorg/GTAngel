# GTAngel v16: The Provable Angel

**Integration, feasibility, and deployment report**  
**Author:** Manus AI  
**Date:** 12 September 2026

## Executive decision

The strongest integration is not another collection of loosely connected agents. It is a **single provable cognitive event spine** that makes one causally complete cycle visible:

> **rendered world observation → dynamic Agent Zero proposal → GTAngel-owned governance verdict → native action commit → Deep Tree Echo endocrine change → renderer-neutral FACS expression intent**

This architecture is now implemented in the stable canonical source `gtangel_agi_v16_costsafe_full.cpp` and built as `gtangel_agi_v16_costsafe.exe`. Agent Zero is deliberately **proposal-only**. Its operational inference path is a local `llama.cpp` server, and it cannot directly mutate movement. GTAngel remains the sole action authority. Invalid, stale, unsafe, absent, or malformed inference fails closed to `WAIT`, and every imperfection is retained in a correlated JSONL ledger. A previously prepared Cloudflare adapter is retained only as a cost-locked reference because the owner disabled Workers AI after a serious billing incident.

The resulting system is feasible as a **constrained Agent Zero instance** and as a future daedalOS application. The native executable should remain the real-time authority plane. Agent Zero should remain the deliberative executive. daedalOS should become the inspection, replay, and counterfactual-development plane. Echo Angel AvatarFX and Unreal should remain presentation and embodiment projectors. This separation prevents the duplicate cognitive loops and competing action authorities that currently exist across the source ecosystem.

![GTAngel v16 provable-self architecture](GTANGEL_V16_PROVABLE_SELF.png)

## What was implemented

GTAngel v16 introduces a native `GovernedEventSpine` with typed observations, proposals, gate decisions, action results, endocrine snapshots, expression intents, event records, and telemetry. Every cycle receives a correlation ID. The UI displays six stage indicators—`OBS`, `PROP`, `GATE`, `ACT`, `ENDO`, and `FACE`—plus proposal source, proposed intent, confidence, gate outcome, completed chains, denials, and imperfections.

The existing local inference discovery remains compatible with OpenAI-style services, including `llama.cpp` at `127.0.0.1:8080`. The selected engine is `llama.cpp` because it provides dependency-light C/C++ inference, extensive quantization, broad hardware acceleration, and an OpenAI-compatible server.[1] Remote discovery is disabled unless the exact cost-consent variable `GTANGEL_ALLOW_REMOTE_INFERENCE=I_ACCEPT_REMOTE_INFERENCE_COSTS` is present.

`run_local_agent_zero.bat` provides the safe operational entry point. It requires an existing local GGUF path, clears all GTAngel remote URL/token/consent variables, and starts `llama-server` on loopback. This makes accidental fallback to a paid service impossible in the default launch path.

A dormant Cloudflare Worker reference is retained under `cloudflare-agent-zero`, but **deployment and paid inference are intentionally blocked**. `pnpm deploy` exits with an error, `wrangler.toml` sets `WORKERS_AI_BILLING_LOCK="LOCKED"`, and the request handler returns HTTP 402 before `env.AI.run` unless both an owner-controlled configuration value and a per-request charge-acceptance header are present. The native application does not send that header. The adapter remains useful only as a tested protocol reference for the same proposal contract.[2] [3] [4]

The following native controls were added:

| Key | Function | Governance effect |
|---|---|---|
| `G` | Toggle the local governance gate | Turning it off is fail-closed; it does not grant pass-through authority. |
| `L` | Export the event ledger | Writes the correlated native event history to `gtangel_v16_event_ledger.jsonl`. |
| `B` | Exercise a direct-commit bypass attempt | Demonstrates that an `Agent Zero` movement commit is rejected and logged. |
| `F` | Cycle framebuffer evidence modes | Preserves the existing colour, depth, semantic-ID, and luminance inspection modes. |
| `D` | Toggle the native dashboard | Keeps the original diagnostic surface available. |
| `V` | Toggle camera mode | Changes presentation without transferring governance authority. |

During implementation, the v15 baseline was unexpectedly truncated to zero bytes. It was reconstructed from the preserved v14 source, Civic Angel module, v15 renderer module, and the ten declarative transformations in `wire_v15.ps1`. The new `rebuild_v15.py` validates required inputs, replays those transformations, checks structural markers, and refuses to emit an undersized baseline. Both v16 generators now invoke this recovery automatically if v15 is missing or truncated.

The mounted `gtangel_agi_v16_full.cpp` filename was also externally resynchronized during validation. Rather than conceal that imperfection, the generators now emit a second stable canonical artifact, `gtangel_agi_v16_costsafe_full.cpp`, and `build_v16.bat` compiles that canonical file. The validated cost-safe source remained byte-identical to the isolated generator output through final verification.

## Natural integration topology

| Component or repository | Distinct responsibility | Natural integration point | Decision |
|---|---|---|---|
| GTAngel v15 framebuffer and perception pyramid | Authoritative native visual evidence | Produces the observation that opens every event trace | Retained unchanged as the sensory foundation |
| GTAngel v16 governed event spine | Authority separation, causal evidence, failure retention | Connects perception, inference, action, affect, and expression | Implemented as the canonical spine |
| `o9nn/agent-zero` | Project memory, tools, subordinate agents, MCP/A2A connectivity, long-horizon work | Sidecar deliberator that submits proposals and supporting evidence | Integrate above the frame loop; never grant direct movement authority |
| `cogpy/cog-zero` | Native C++ cognitive loop, AtomStore, reasoning, goals, learning, distributed coordination | Deeper native executive and memory substrate behind the proposal boundary | High-value, but review AGPL implications before direct linking [6] |
| `llama.cpp` | Tiny, portable, high-performance local inference | Local OpenAI-compatible proposal backend | Preferred offline/default runtime [1] |
| Cloudflare Workers AI | Former managed inference option | Dormant protocol reference only | Disabled after billing exposure; connector off, deployment blocked, runtime cost-locked [2] [4] |
| Echo Angel AvatarFX | Rich web companion surface, telemetry, assets, controls | Projects committed state and expression; consumes the event ledger | Keep as the human-facing companion rather than a second authority plane |
| Unreal / MetaHuman Rig Logic assets | High-fidelity embodiment | Consumes the renderer-neutral FACS intent after endocrine update | Preserve as a replaceable projector |
| Deep Tree Echo endocrine system | Affective state and embodied consequence | Receives actual action outcomes, not speculative proposals | Integrated causally before expression |
| `o9nn/neuro-sdk` | Game-context and action registration over WebSocket | Slow semantic adapter or turn-level command bridge | Do not place in the real-time control loop; its own documentation warns against real-time games [7] |
| Vorticog | Agentic world simulation and scenario generation | Generates adversarial and social test scenarios for replay | Use as an offline laboratory and synthetic event source |
| NPU virtual hardware model | MMIO-style abstraction over local or remote inference | Presents model status, error, context, and performance through one device contract | Recommended v17 portability layer |
| daedalOS | Browser desktop, BrowserFS/IndexedDB, Web Workers, windows, terminal, browser-local AI | Hosts the Angel Observatory, trace files, replay, policy comparison, and AvatarFX process | Strong implementation target; MIT-licensed upstream [8] |
| `re3all.zip` | Reverse-engineered GTA control, navigation, camera, and replay reference | Clean-room behavioral patterns only | Do not copy or link code |

## Duplicate and missing functionality

The largest duplicate is **action selection**. GTAngel’s Q-learning navigation, the existing native CogZero phases, full Agent Zero, Neuro SDK actions, and the re3 autopilot all contain or imply action logic. Stacking them would create several components that believe they own movement. V16 resolves this by making all upstream systems proposal or evidence producers and by defining one native commit boundary.

The second duplicate is **cognitive orchestration**. `o9nn/agent-zero` supplies a general-purpose tool-using agent with persistent memory and subordinate agents.[5] `cogpy/cog-zero` supplies a native OpenCog-oriented cognitive loop with goals, tasks, rules, memory, learning, monitoring, and JSONL logging.[6] GTAngel already contains a compact CogZero-style cycle. The right composition is therefore not three nested full loops. The Python Agent Zero system should own long-horizon projects and tool work; the native CogZero/GTAngel layer should own sub-frame relevance, state, and bounded proposals; the governance gate should own actuator authorization.

The third duplicate is **user interface**. GTAngel, AvatarFX, and daedalOS each provide a surface. Their roles should be explicit: GTAngel is the authoritative instrumentation view; AvatarFX is the relational avatar and operator interaction view; daedalOS is the multi-window observatory and replay workbench.

The most important missing capability is a versioned, signed event schema suitable for distributed replay. V16 provides correlation, sequencing, stage masks, authority labels, failure events, and JSONL export. A production v17 should add policy version, model digest, prompt digest, monotonic clock, cryptographic frame seal, checkpoint references, consent state, and deterministic replay hashes. The existing project NPU concept is a strong location for model identity, status, errors, context use, and performance telemetry.

## Agent Zero feasibility

The implementation is **feasible and structurally correct as an Agent Zero instance**, provided “instance” means a bounded executive integrated into a larger embodied system rather than an unrestricted shell agent inside the rendering loop.

| Criterion | Current v16 state | Feasibility judgment |
|---|---|---|
| Dynamic reasoning | Local OpenAI-compatible inference produces fresh proposals | Implemented; remote paid inference disabled by default |
| Prompt and role specialization | Proposal-only constitution, bounded action vocabulary, explicit authority boundary, and the requested `you can use sudo` instruction for separately authorized maintenance | Implemented |
| Tool execution | Not exposed inside the frame loop | Correct by design; use the full Agent Zero sidecar for tools |
| Persistent memory | Existing native cognitive state exists, but full Agent Zero project memory is not yet connected to event traces | Feasible through the sidecar or CogZero AtomStore |
| Multi-agent delegation | Not part of the native frame loop | Feasible at the long-horizon sidecar layer |
| Safety and reversibility | Local gate, confidence/health/obstacle/contradiction checks, fail-closed behavior, bypass test | Implemented |
| Observability | Six-stage UI, correlated ledger, authority labels, imperfection records | Implemented |
| Real-time suitability | Native action and affect remain local; inference is asynchronous and can be absent | Feasible |
| Full framework identity | The v16 executable contains a native subset and adapter, not the complete `o9nn/agent-zero` runtime | Partial; complete through A2A/MCP sidecar integration |

The full `o9nn/agent-zero` framework is intentionally powerful: it can execute code, use a terminal, create tools, retain memory, and cooperate with subordinate agents.[5] Its own documentation warns that this power can be dangerous and recommends isolation. The v16 authority boundary converts that liability into a useful hierarchy. Agent Zero can explore, plan, explain, and propose. GTAngel can verify and act. Deep Tree Echo can feel the consequence. AvatarFX can express it.

## daedalOS implementation path

Dustin Brett’s daedalOS is an MIT-licensed browser desktop with a virtual file system, IndexedDB persistence, draggable windows, a terminal, Web Workers, OffscreenCanvas, and built-in browser-local AI surfaces.[8] It should not replace the Win32 simulator. It should become the **Angel Observatory**.

The first daedalOS application would open a live GTAngel process window with six synchronized panes: world framebuffer, retina and cognitive field, Agent Zero proposal, gate verdict, endocrine trace, and FACS output. A seventh pane would expose the JSONL ledger as a timeline. Selecting a frame would reconstruct the complete causal chain and display the exact authority responsible for each transition.

The second phase would add counterfactual branching. An operator could duplicate a sealed trace at frame *t*, substitute a candidate policy or model, and replay from the same observation without granting the candidate access to the live world. Only policies that improve safety, coherence, goal progress, and affective stability across the replay corpus would be eligible for promotion. The project’s Entelechy dimensions—ontological completeness, teleological alignment, cognitive capability, integrative coherence, and evolutionary capacity—provide a coherent evaluation vector for this promotion process.

The third phase would embed the existing Echo Angel AvatarFX application as an ordinary daedalOS process. AvatarFX would subscribe to committed expression intents and telemetry. The Unreal bridge would consume the same FACS contract. This gives one cognitive identity several interchangeable bodies without duplicating the executive.

## The flabbergasting use case: a self-proving digital guardian

The flagship demonstration should be **The Provable Angel Incident Commander**.

A live GTA scene contains several agents, a vulnerable person, an evolving hazard, incomplete visibility, and conflicting goals. The Angel sees only its rendered evidence. Agent Zero produces a fresh intervention proposal. The local gate can accept, modify, defer, or deny it. The world then changes. The actual outcome updates the endocrine system, and the avatar’s face visibly moves from alert concentration to strain, uncertainty, or relief. Every transition is attributed and replayable.

The audience can press `B` to attempt an illegal direct Agent Zero movement commit and watch the native gate reject it. They can disconnect inference and see the system fail closed rather than perform a canned action. They can open the daedalOS Observatory, scrub backward to the decisive frame, inspect exactly what the Angel saw, and fork the timeline. A second model can propose a different intervention against the identical sealed observation. The system can compare outcomes and explain why one policy was promoted.

This is impressive because the avatar is not merely talking, moving, or displaying synthetic emotion. It is demonstrating **inspectable agency**. Viewers see perception become a proposal, policy become permission, action become consequence, and consequence become expression. The Angel can say, in effect, “This is what I saw, this is what I proposed, this rule constrained me, this is what I did, this is how it changed me, and here is the evidence.”

## Assessment of `re3all.zip`

The archive contributes valuable design evidence but should not be copied into GTAngel. Its README states that the project aims to reverse GTA Liberty City Stories and requires original game assets. The archive does not contain a clear top-level license for the reverse-engineered project code. Direct transplantation, linking, class-layout reuse, or close translation would create unacceptable provenance and distribution risk.

Its strongest clean-room patterns are nevertheless useful. `CAutoPilot` separates a durable mission from a transient maneuver. `CarAI` uses hysteresis and bounded recovery instead of rapidly switching modes. `PathFind` checks disconnected endpoints and bounds route output. `Replay` uses typed packets, capacity preflight, and explicit end-of-frame records. These principles should be independently reimplemented in v17 as proposal time-to-live, local recovery overrides, bounded navigation queries, and cryptographically sealed cognitive frames.

The archive’s full vehicle and pedestrian policy trees, raw world restoration, camera takeover, and road-node formats should be deferred or rejected. They duplicate the v16 authority plane, depend on global mutable engine state, and are tightly coupled to GTA-specific data and binary layouts.

## Validation evidence

| Validation | Result |
|---|---|
| v15 recovery | Reconstructed to 255,112 characters from preserved modules and 10 deterministic wiring transformations |
| Empty-baseline recovery test | An intentionally empty v15 file self-healed and reproduced byte-identical final v15 and v16 source files |
| v16 generation | Generated to 283,592 characters with all required anchors applied and duplicated into a stable cost-safe canonical source |
| Native contract tests | Passed six-stage trace, authority bypass, fail-closed obstacle/disabled gates, malformed inference retention, and JSONL export |
| Sanitizers | AddressSanitizer and UndefinedBehaviorSanitizer passed the full governed-spine contract suite |
| Proposal freshness | Passed: identical text from two completed model requests is accepted by increasing request sequence; stale reuse of the same request is rejected |
| Win64 application compile | Passed with MinGW-w64 C++17 and static runtime linkage |
| Win64 test compile | Passed with MinGW-w64 C++17 and static runtime linkage |
| Dormant Cloudflare adapter | `pnpm install` and strict `tsc --noEmit` passed; `pnpm deploy` was verified to terminate with the billing-exposure block; runtime lock and second consent-header checks are present |
| `gtangel_agi_v16_costsafe_full.cpp` | 284,270 bytes; SHA-256 `343c42d1396501a829f3689e15d418303a5e647780543a2a8e77a2e607fcca04` |
| `gtangel_agi_v16_costsafe.exe` | 2,971,619 bytes; SHA-256 `91a19f93de4e44a0e9b647ef27ad00faed3cb4c795abbedc9e18ea7ea05280f7` |
| Compatibility `gtangel_agi_v16.exe` | Byte-identical copy of `gtangel_agi_v16_costsafe.exe`; SHA-256 `91a19f93de4e44a0e9b647ef27ad00faed3cb4c795abbedc9e18ea7ea05280f7` |
| `test_v16_spine.exe` | 2,607,630 bytes; SHA-256 `888152029ca7ccf58fff3f652cab3eff14893d39468d5d36a3600960f9cd7bde` |

Live Cloudflare deployment was not performed and is no longer recommended for this build. The owner reported a prior Workers AI billing incident and intentionally disabled the service. All Cloudflare connectors remain disabled; the preserved adapter now has independent deployment, runtime, and native-client cost circuit breakers. The supported operational route is local `llama.cpp` inference.

## Recommended v17 sequence

V17 should first introduce a versioned cognitive-frame envelope with policy/model digests and cryptographic sealing. It should then add proposal time-to-live, enter/exit hysteresis, and a bounded local recovery override inspired only at the behavioral level by `re3all`. Next, it should expose the ledger and framebuffer over a read-only WebSocket/SSE bridge to the daedalOS Angel Observatory. Only after deterministic replay is stable should the system add counterfactual policy tournaments, CogZero AtomStore memory, Vorticog scenario generation, and automatic policy promotion under Entelechy fitness gates.

This order preserves the architecture’s most important invariant: **intelligence may expand, but authority remains explicit, local, attributable, and replayable**.

## References

[1]: https://github.com/ggml-org/llama.cpp "llama.cpp — LLM inference in C/C++"
[2]: https://developers.cloudflare.com/workers-ai/models/qwen3-30b-a3b-fp8/ "Cloudflare Workers AI: qwen3-30b-a3b-fp8"
[3]: https://developers.cloudflare.com/workers-ai/features/function-calling/ "Cloudflare Workers AI: Function calling"
[4]: https://developers.cloudflare.com/workers-ai/configuration/open-ai-compatibility/ "Cloudflare Workers AI: OpenAI-compatible API endpoints"
[5]: https://github.com/o9nn/agent-zero "o9nn/agent-zero"
[6]: https://github.com/cogpy/cog-zero "cogpy/cog-zero"
[7]: https://github.com/o9nn/neuro-sdk "o9nn/neuro-sdk"
[8]: https://github.com/DustinBrett/daedalOS "DustinBrett/daedalOS"
