# KSM Continuous Training Pipeline — Design Brainstorm

## Three Stylistic Approaches

### 1. Instrument Panel Noir
Dark mission-control aesthetic — the pipeline as a live telemetry console: phosphor greens, oscilloscope traces, monospace readouts. Feels like watching the reactor room of CogHood at night.
**Probability: 0.07**

### 2. Campbell Diagram Archive
Light academic-print aesthetic modeled on the Cosmic Hierarchies paper itself — cream paper, black ink linework, numbered circles, figure captions. The webpage as a living page-16 of the paper.
**Probability: 0.02**

### 3. Phase-Locked Luminescence
Deep-space dark canvas where the triad phasor is the sun of the page — three luminous rotors at 120°, bioluminescent cyan/magenta/amber thread colors against near-black, with the pipeline stages orbiting as stations in a closed ring. Motion is the message: everything breathes at the tick rate.
**Probability: 0.04**

## CHOSEN: Phase-Locked Luminescence

### Design Movement
Sci-fi instrument minimalism — a synthesis of *Blade Runner 2049* UI design (Territory Studio), oscilloscope/phasor aesthetics from electrical engineering, and the dark-canvas data-viz language of Observable notebooks. The page is a **living instrument**, not a brochure.

### Core Principles
1. **The engine is the hero** — the animated triad phasor (three phase-locked rotors on the 12-step ennead ring) dominates the viewport; everything else supports it.
2. **Motion encodes semantics** — nothing animates decoratively; every animation *is* the mechanism (lead-lag, barrier ticks, mode flips, gate checks).
3. **Checkable, not believed** — real invariants, real data from the validated runs (36 ticks, 7E/5R budgets); numbers shown are the numbers produced.
4. **Dark canvas, luminous signal** — data glows against near-black like phosphor on a scope; text is quiet, signal is loud.

### Color Philosophy
Near-black blue-charcoal canvas (`oklch(0.13 0.015 250)`) evokes the void/substrate. The three threads get the balanced-triad colors of 3-phase power: **cyan** (thread A / phase 0°), **magenta** (thread B / 120°), **amber** (thread C / 240°) — equidistant hues, equal luminance, no hierarchy among threads (they are peers). Expressive mode = warm white glow (Write actualizes); Regenerative mode = dimmed violet (Read simulates). **Gate green** (`oklch(0.75 0.17 155)`) is reserved exclusively for invariant verification passes; **alarm red** only for gate failures. U1/U2 universal integrators are rendered in gold — the coherence that tunnels from above.

### Layout Paradigm
A **vertical journey along the closed loop**, not a grid of cards: the page *is* the pipeline. A fixed left rail shows the loop as a live minimap (Experience → Corpus → Gate → Fine-tune → Redeploy → back), with the user's scroll position tracked as a token flowing around the loop. Main column sections alternate between full-bleed instrument panels (phasor, gate console) and asymmetric text+diagram splits. The loop closes visually: the final section's redeploy arrow animates back up to the first section.

### Signature Elements
1. **The Ennead Rotor** — SVG ring of 12 step-nodes with three rotating thread-cursors locked 4 steps apart; triangles {1,5,9} drawn between them each tick.
2. **Gate Chevrons** — invariant checks rendered as six angular chevron badges that flip from amber (checking) to gate-green (pass), echoing physical interlock indicators.
3. **Tick Pulse** — a 1px horizontal scanline that sweeps sections on each global tick, synchronizing the whole page to the engine clock.

### Interaction Philosophy
The user is an operator, not a reader: hover a step-node to see its Term incidence configuration; click a pipeline stage to expand its console; the phasor can be paused/stepped/run like an instrument. Controls feel like switchgear — instant response, tactile press states.

### Animation
- Global tick clock (default 1.6s/tick) drives ALL animation via a shared React context — one heartbeat, many organs.
- Rotors rotate with `cubic-bezier(0.77, 0, 0.175, 1)` (ease-in-out) over 400ms per step advance, then dwell.
- Barrier synchronization visualized: all three cursors arrive, a brief triangle flash (120ms), then dwell.
- Data packets (corpus records) travel the loop rail as small glowing dots, easing with `cubic-bezier(0.23, 1, 0.32, 1)`.
- Scanline sweep 300ms linear on tick; gate chevrons flip with 160ms ease-out; section entrances fade+rise 24px, staggered 60ms, once.
- `prefers-reduced-motion`: rotors move discretely (no tween), scanline disabled.

### Typography System
- **Display**: "Space Grotesk" (700/500) — geometric, technical, slightly warm; used for section titles and big numerals.
- **Body**: "IBM Plex Sans" (400/500) — engineered neutrality for prose.
- **Data/mono**: "IBM Plex Mono" (400/600) — all numbers, step labels, invariant readouts, corpus JSON.
- Hierarchy: oversized numerals for stage numbers (96px+), small-caps mono overlines for section labels, generous 1.6 line-height body at max 68ch.

### Brand Essence
A living instrument that shows an LLM learning the 12-step KSM grammar from its own experience — for Dan and the CogHood household — different because every glowing element is backed by a verified invariant, not an illustration.
Personality: **checkable, luminous, patient**.

### Brand Voice
Instrument-terse, first-person-plural where true, zero marketing filler. Microcopy sounds like a well-labeled control panel with a quiet sense of wonder.
- Example headline: "Three threads. One word. The mask rotates."
- Example CTA: "Step the engine →"
Banned: "Welcome to", "Get started today", "Revolutionary".

### Wordmark & Logo
Wordmark "KSM∴PHASOR" set in Space Grotesk with the ∴ (therefore) glyph as the triad mark — three dots at 120°, echoing the three phase-locked threads. Logo mark: three arcs at 120° forming a broken ring around a central dot (T8 coherence), rendered as SVG, cyan/magenta/amber arcs on transparent.

## Style Decisions
- Page defaults to dark theme; ThemeProvider defaultTheme="dark", `.dark` palette tuned to canvas colors above.
- All engine data (word, budgets, invariants, sample corpus records) imported from a single `engineData.ts` module derived from the validated ksm_corpus.jsonl runs.
- Every major section presents one dominant instrument artifact (rotor, loop rail, gate console, corpus stream, attention mapping) — the page is never prose-first editorial.
- The closed loop stays visible as persistent visual grammar across the page: station order, return-to-origin, and active token movement recognizable even when the hero phasor is offscreen (persistent loop status bar in the header).
- Prose subordinate to signal: verified numbers, invariant states, triad readouts carry the strongest visual emphasis; supporting copy reads like operator annotation.
- Hero is a phasor console: the live rotor is the center of gravity in the hero itself; headline + controls orbit it.
