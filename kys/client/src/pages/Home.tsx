/**
 * Home — KSM∴PHASOR Continuous Training Pipeline
 * Design: Phase-Locked Luminescence (see /ideas.md)
 * - Near-black blue-charcoal canvas; luminous cyan/magenta/amber threads
 * - Vertical journey along the closed loop; fixed left loop-rail minimap
 * - ONE shared tick clock drives all animation (TickProvider)
 * - Everything shown is real data from the validated 36-tick corpus run
 */
import { motion } from "framer-motion";
import AttentionIso from "@/components/AttentionIso";
import LoopStatusBar from "@/components/LoopStatusBar";
import CorpusStream from "@/components/CorpusStream";
import EnneadRotor from "@/components/EnneadRotor";
import EnterpriseAxes from "@/components/EnterpriseAxes";
import InvariantGates from "@/components/InvariantGates";
import DataVault from "@/components/DataVault";
import LoopRail from "@/components/LoopRail";
import PipelineLoop from "@/components/PipelineLoop";
import SteppingRulePanel from "@/components/SteppingRulePanel";
import WindowEncoding from "@/components/WindowEncoding";
import TickControls from "@/components/TickControls";
import { TickProvider, TickScanline } from "@/contexts/TickContext";
import {
  CANONICAL_WORD,
  RUN_STATS,
  THREADS,
} from "@/lib/engineData";

const HERO_BG = "/manus-storage/ksm-hero-bg_38ba9929.png";
const LOGO = "/manus-storage/ksm-logo-mark_9e52438b.png";
const LOOP_TEXTURE = "/manus-storage/ksm-loop-texture_300c8323.png";
const GATE_PANEL = "/manus-storage/ksm-gate-panel_84c0cfa3.png";

const rise = {
  initial: { opacity: 0, y: 24 },
  whileInView: { opacity: 1, y: 0 },
  viewport: { once: true, margin: "-80px" },
  transition: { duration: 0.5, ease: "easeOut" as const },
};

function SectionLabel({ children }: { children: string }) {
  return (
    <div className="flex items-center gap-4 mb-6">
      <span className="overline-label">{children}</span>
      <div className="hairline flex-1" />
    </div>
  );
}

export default function Home() {
  return (
    <TickProvider>
      <div className="min-h-screen bg-background text-foreground overflow-x-clip">
        <TickScanline />
        <LoopRail />

        {/* ============ HEADER ============ */}
        <header className="fixed top-0 inset-x-0 z-40 border-b border-border bg-background/75 backdrop-blur-md">
          <div className="container flex items-center justify-between py-3">
            <a href="#hero" className="flex items-center gap-3">
              <img src={LOGO} alt="KSM∴PHASOR mark" className="w-9 h-9" />
              <span className="font-display font-bold tracking-wide text-lg">
                KSM<span style={{ color: "var(--gold)" }}>∴</span>PHASOR
              </span>
            </a>
            <LoopStatusBar />
            <nav className="hidden md:flex items-center gap-5 font-mono text-xs text-muted-foreground">
              <a href="#pipeline" className="hover:text-foreground transition-colors">
                PIPELINE
              </a>
              <a href="#gates" className="hover:text-foreground transition-colors">
                GATES
              </a>
              <a href="#isomorphism" className="hover:text-foreground transition-colors">
                ≅ ATTENTION
              </a>
              <a href="#topology" className="hover:text-foreground transition-colors">
                TOPOLOGY
              </a>
              <a href="#vault" className="hover:text-foreground transition-colors">
                VAULT
              </a>
              <a
                href="/geometry"
                className="transition-colors hover:brightness-125"
                style={{ color: "var(--thread-b)" }}>
                GEOMETRY ↗
              </a>
            </nav>
          </div>
        </header>

        {/* ============ HERO — PHASOR CONSOLE ============ */}
        <section
          id="hero"
          className="relative min-h-screen flex items-center pt-24 pb-12"
          style={{
            backgroundImage: `linear-gradient(180deg, oklch(0.13 0.015 250 / 88%) 0%, oklch(0.13 0.015 250 / 96%) 100%), url(${HERO_BG})`,
            backgroundSize: "cover",
            backgroundPosition: "center",
          }}>
          <div className="container relative z-10">
            <div className="grid lg:grid-cols-[1fr_1.05fr] gap-10 items-center">
              {/* left: headline + word strip, orbiting the rotor */}
              <motion.div
                initial={{ opacity: 0, y: 24 }}
                animate={{ opacity: 1, y: 0 }}
                transition={{ duration: 0.6, ease: "easeOut" }}>
                <div className="overline-label mb-5">
                  CONTINUOUS LLM TRAINING · TRIAD PHASOR ENGINE · LIVE
                </div>
                <h1 className="font-display font-bold text-5xl md:text-6xl xl:text-7xl leading-[1.02] tracking-tight">
                  Three threads.
                  <br />
                  One word.
                  <br />
                  <span
                    style={{
                      background:
                        "linear-gradient(90deg, var(--thread-a), var(--thread-b), var(--thread-c))",
                      WebkitBackgroundClip: "text",
                      backgroundClip: "text",
                      color: "transparent",
                    }}>
                    The mask rotates.
                  </span>
                </h1>
                <p className="mt-6 text-base text-secondary-foreground leading-relaxed max-w-[52ch]">
                  The 12-step KSM cycle replaces the 2-step User→Assistant
                  dialogue as the model's native grammar. Experience runs the
                  engine, the corpus records it, six invariants gate it,
                  fine-tuning absorbs it, redeploy closes the loop. Every
                  number on this page comes from the validated{" "}
                  <span className="font-mono" style={{ color: "var(--gate-green)" }}>
                    {RUN_STATS.ticks}-tick
                  </span>{" "}
                  run — checkable, not believed.
                </p>

                {/* canonical word strip */}
                <div className="mt-8 flex flex-wrap gap-1 font-mono text-xs">
                  {CANONICAL_WORD.map((w, i) => (
                    <span
                      key={i}
                      className="px-2 py-1 border border-border bg-card/70"
                      style={{
                        color: w.startsWith("T8")
                          ? "var(--gold)"
                          : w.endsWith("E")
                            ? "var(--mode-e)"
                            : "var(--mode-r)",
                      }}>
                      {w}
                    </span>
                  ))}
                </div>
                <div className="mt-2 font-mono text-[11px] text-muted-foreground">
                  canonical 12-word · 7 Expressive + 5 Regenerative · T8
                  anchors both half-cycles
                </div>

                {/* operator switchgear lives IN the hero console */}
                <div className="mt-8">
                  <TickControls />
                </div>

                <div className="mt-8">
                  <a
                    href="#engine"
                    className="inline-flex items-center gap-2 px-6 py-3 font-mono text-sm font-semibold border transition-all hover:translate-x-1"
                    style={{
                      borderColor: "var(--thread-a)",
                      color: "var(--thread-a)",
                      boxShadow: "0 0 16px oklch(0.82 0.13 210 / 20%)",
                    }}>
                    Read the stepping rule →
                  </a>
                </div>
              </motion.div>

              {/* right: the LIVE rotor — the center of gravity */}
              <motion.div
                initial={{ opacity: 0, scale: 0.96 }}
                animate={{ opacity: 1, scale: 1 }}
                transition={{ duration: 0.8, ease: "easeOut" }}
                className="relative">
                {/* console frame corners */}
                <div className="absolute -inset-3 pointer-events-none" aria-hidden>
                  {[
                    "top-0 left-0 border-t border-l",
                    "top-0 right-0 border-t border-r",
                    "bottom-0 left-0 border-b border-l",
                    "bottom-0 right-0 border-b border-r",
                  ].map((pos) => (
                    <span
                      key={pos}
                      className={`absolute w-6 h-6 ${pos}`}
                      style={{ borderColor: "oklch(0.82 0.13 210 / 40%)" }}
                    />
                  ))}
                </div>
                <div className="absolute -top-2 left-1/2 -translate-x-1/2 font-mono text-[10px] tracking-[0.3em] text-muted-foreground bg-background/70 px-3">
                  PHASOR · 12-STEP ENNEAD · 3×120°
                </div>
                <EnneadRotor />
              </motion.div>
            </div>
          </div>
        </section>

        {/* ============ ENGINE / ROTOR ============ */}
        <section id="engine" className="relative py-24 instrument-grid">
          <div className="container">
            <SectionLabel>01 · THE ENGINE — PHASE-LOCKED TRIAD</SectionLabel>
            <div className="grid lg:grid-cols-[1fr_1.1fr] gap-12 items-center">
              <motion.div {...rise}>
                <h2 className="font-display font-bold text-3xl md:text-4xl leading-tight">
                  Twelve steps on the ring.
                  <br />
                  Three cursors, 120° apart.
                </h2>
                <p className="mt-5 text-secondary-foreground leading-relaxed max-w-[58ch]">
                  Thread <em className="not-italic" style={{ color: "var(--thread-a)" }}>k</em>{" "}
                  at global tick{" "}
                  <em className="not-italic font-mono">g</em> executes{" "}
                  <code className="font-mono text-sm px-1.5 py-0.5 bg-card border border-border">
                    word[(g + 4k) % 12]
                  </code>
                  . Each thread walks the <strong>full</strong> word offset by
                  four steps — never assigned to fixed columns, which is the one
                  bug everyone makes (it breaks the per-thread 7E/5R budget into
                  12E/0R, 6E/6R, 3E/9R).
                </p>
                <p className="mt-4 text-secondary-foreground leading-relaxed max-w-[58ch]">
                  The step-triple is always{" "}
                  <code className="font-mono text-sm px-1.5 py-0.5 bg-card border border-border">
                    {"{g+1, g+5, g+9}"}
                  </code>{" "}
                  mod 12. Odd steps draw the Structure triangle {"{4,8,7}"};
                  even steps draw the Process triangle {"{1,2,5}"}. Hover any
                  step-node to read its Term's incidence configuration over the
                  four interfaces.
                </p>

                <div className="mt-6 flex flex-wrap gap-4">
                  {THREADS.map((th) => (
                    <div key={th.id} className="flex items-center gap-2 font-mono text-xs">
                      <span
                        className="w-3 h-3 rounded-full"
                        style={{
                          background: th.colorVar,
                          boxShadow: `0 0 6px ${th.colorVar}`,
                        }}
                      />
                      <span className="text-secondary-foreground">
                        {th.name} · {th.phase}
                      </span>
                    </div>
                  ))}
                  <div className="flex items-center gap-2 font-mono text-xs">
                    <span
                      className="w-3 h-3 rounded-full"
                      style={{
                        background: "var(--gold)",
                        boxShadow: "0 0 6px var(--gold)",
                      }}
                    />
                    <span className="text-secondary-foreground">
                      U1/U2 · policy level
                    </span>
                  </div>
                </div>

              </motion.div>

              <motion.div {...rise}>
                <SteppingRulePanel />
              </motion.div>
            </div>

            {/* 5×4 window — the whole system in one 4-step window */}
            <div className="mt-14 grid lg:grid-cols-[0.9fr_1.4fr] gap-12 items-start">
              <motion.div {...rise}>
                <h3 className="font-display font-bold text-2xl md:text-3xl leading-tight">
                  One window.
                  <br />
                  The whole system.
                </h3>
                <p className="mt-4 text-secondary-foreground leading-relaxed max-w-[52ch]">
                  Because the three Particular sequences are the same word one
                  row apart, a single 4-step window encodes all 3 cycles of 12:
                  five rows — U1, U2, and the three concurrent word rows. At
                  every tick the triad reads one <strong>column</strong>; the
                  live column sweeps left to right and wraps.
                </p>
                <p className="mt-4 text-secondary-foreground leading-relaxed max-w-[52ch]">
                  Shift the alignment two steps and the{" "}
                  <span style={{ color: "var(--gold)" }} className="font-mono text-sm">
                    T6 symmetry
                  </span>{" "}
                  centers: U2's twin T6 cells sit in the middle columns while
                  U1's 2-2 alternation frames them — the rotoreflection made
                  visible.
                </p>
              </motion.div>
              <motion.div {...rise}>
                <WindowEncoding />
              </motion.div>
            </div>
          </div>
        </section>

        {/* ============ PIPELINE LOOP ============ */}
        <section
          id="pipeline"
          className="relative py-24"
          style={{
            backgroundImage: `linear-gradient(180deg, oklch(0.13 0.015 250 / 92%), oklch(0.13 0.015 250 / 96%)), url(${LOOP_TEXTURE})`,
            backgroundSize: "cover",
            backgroundPosition: "center",
          }}>
          <div className="container">
            <SectionLabel>02 · THE LOOP — EXPERIENCE TO REDEPLOY</SectionLabel>
            <motion.div {...rise} className="max-w-3xl mb-14">
              <h2 className="font-display font-bold text-3xl md:text-4xl leading-tight">
                The loop is closed.
                <br />
                Nothing trains unverified.
              </h2>
              <p className="mt-5 text-secondary-foreground leading-relaxed">
                Five stations, one circulation: the engine's lived ticks become
                corpus records, the gate re-verifies every invariant, the
                trainer absorbs the word, and the trained weights rejoin the
                engine. Click any station to open its operator console — the
                output shown is from the real pipeline run.
              </p>
            </motion.div>

            <div className="max-w-3xl">
              {/* anchor targets per stage for the LoopRail */}
              <div id="stage-experience" className="absolute -mt-24" />
              <PipelineLoop />
            </div>
          </div>
        </section>

        {/* ============ INVARIANT GATES ============ */}
        <section
          id="gates"
          className="relative py-24 border-y border-border"
          style={{
            backgroundImage: `linear-gradient(180deg, oklch(0.13 0.015 250 / 94%), oklch(0.13 0.015 250 / 97%)), url(${GATE_PANEL})`,
            backgroundSize: "cover",
            backgroundPosition: "center",
          }}>
          <div className="container">
            <SectionLabel>03 · THE GATE — SIX INTERLOCKS</SectionLabel>
            <div className="grid lg:grid-cols-[0.9fr_1.4fr] gap-12 items-start">
              <motion.div {...rise}>
                <h2 className="font-display font-bold text-3xl md:text-4xl leading-tight">
                  Amber while checking.
                  <br />
                  <span style={{ color: "var(--gate-green)" }}>
                    Green only on proof.
                  </span>
                </h2>
                <p className="mt-5 text-secondary-foreground leading-relaxed">
                  Gate-green is reserved on this entire page for one thing:
                  verified invariant passes. All six held across every tick of
                  the {RUN_STATS.ticks}-tick corpus run — {RUN_STATS.cycles}{" "}
                  full cycles, {RUN_STATS.barrierFaults} barrier faults. A
                  single failure rejects the whole batch before it ever reaches
                  the trainer.
                </p>
                <div className="mt-6 grid grid-cols-3 gap-px border border-border bg-border">
                  {[
                    { v: RUN_STATS.ticks, l: "ticks verified" },
                    { v: RUN_STATS.invariantsPassing, l: "invariants pass" },
                    { v: RUN_STATS.barrierFaults, l: "barrier faults" },
                  ].map((s) => (
                    <div key={s.l} className="bg-card p-4 text-center">
                      <div
                        className="font-display font-bold text-3xl"
                        style={{ color: "var(--gate-green)" }}>
                        {s.v}
                      </div>
                      <div className="font-mono text-[10px] text-muted-foreground mt-1">
                        {s.l}
                      </div>
                    </div>
                  ))}
                </div>
              </motion.div>

              <motion.div {...rise}>
                <InvariantGates />
              </motion.div>
            </div>
          </div>
        </section>

        {/* ============ CORPUS STREAM ============ */}
        <section id="corpus" className="relative py-24">
          <div className="container">
            <SectionLabel>04 · THE CORPUS — NEXT-TRIAD PREDICTION</SectionLabel>
            <div className="grid lg:grid-cols-[1fr_1.2fr] gap-12 items-center">
              <motion.div {...rise}>
                <h2 className="font-display font-bold text-3xl md:text-4xl leading-tight">
                  Every tick,
                  <br />
                  one record.
                </h2>
                <p className="mt-5 text-secondary-foreground leading-relaxed max-w-[56ch]">
                  The stream on the right is the first full cycle of{" "}
                  <code className="font-mono text-sm px-1.5 py-0.5 bg-card border border-border">
                    ksm_corpus.jsonl
                  </code>{" "}
                  — verbatim. Each record carries the tick, the active
                  step-triple, the triad of Terms, and the full interface
                  state. Rendered as 12-step conversation turns, the model
                  learns to predict the <strong>next triad</strong> given the
                  phase history — not the next reply given a prompt.
                </p>
                <p className="mt-4 text-secondary-foreground leading-relaxed max-w-[56ch]">
                  Note the{" "}
                  <span style={{ color: "var(--gold)" }} className="font-mono text-sm">
                    U1_Policy / U2_Policy
                  </span>{" "}
                  entries: the Primary Universal walks its 2-2 alternation
                  UT9E·UT9E·UT8R·UT8R while the Secondary runs the 1-2-1
                  rotoreflection UT3_·UT6_·UT6_·UT2R against it. When the
                  triad is process-heavy and holds no T8, the Universals carry
                  coherence at policy level — it is present every tick.
                </p>
              </motion.div>
              <motion.div {...rise}>
                <CorpusStream />
              </motion.div>
            </div>
          </div>
        </section>

        {/* ============ ATTENTION ISOMORPHISM ============ */}
        <section id="isomorphism" className="relative py-24 instrument-grid border-t border-border">
          <div className="container">
            <SectionLabel>05 · THE ISOMORPHISM — KSM ≅ ATTENTION</SectionLabel>
            <motion.div {...rise} className="max-w-3xl mb-10">
              <h2 className="font-display font-bold text-3xl md:text-4xl leading-tight">
                The engine already speaks
                <br />
                the transformer's language.
              </h2>
              <p className="mt-5 text-secondary-foreground leading-relaxed">
                The triad phasor is not an arbitrary curriculum — its mechanics
                map structurally onto the attention architecture that will
                learn it. Four correspondences:
              </p>
            </motion.div>
            <motion.div {...rise}>
              <AttentionIso />
            </motion.div>
          </div>
        </section>

        {/* ============ ENTERPRISE CONTEXT — ORTHOGONAL AXES ============ */}
        <section
          id="topology"
          className="relative py-24 border-t border-border"
          style={{
            backgroundImage:
              "radial-gradient(ellipse 80% 60% at 70% 30%, oklch(0.82 0.13 210 / 4%), transparent)",
          }}>
          <div className="container">
            <SectionLabel>06 · THE TOPOLOGY — ORG SCOPE × COMPUTE SCALE</SectionLabel>
            <div className="grid lg:grid-cols-[0.85fr_1.3fr] gap-12 items-start">
              <motion.div {...rise}>
                <h2 className="font-display font-bold text-3xl md:text-4xl leading-tight">
                  The loop scales
                  <br />
                  along the diagonal.
                </h2>
                <p className="mt-5 text-secondary-foreground leading-relaxed max-w-[52ch]">
                  The same closed loop that trains one model governs how the
                  village grows. Two orthogonal axes: organizational scope
                  (who shares the word) and compute scale (what carries the
                  ticks). Conway's Law holds here as a conservation law —
                  the communication structure and the system structure must
                  move together, or the invariants break at the boundary.
                </p>
                <p className="mt-4 text-secondary-foreground leading-relaxed max-w-[52ch]">
                  Off-diagonal drift is the failure mode: scope without
                  compute makes promises no tick can keep; compute without
                  scope makes ticks no witness can verify. The two{" "}
                  <span style={{ color: "var(--gold)" }} className="font-mono text-sm">
                    anchors
                  </span>{" "}
                  — CogHood (origin, heartbeat + mail) and CogCity (GPU
                  expansion) — are live infrastructure; the bridge grammar is
                  origin → expansion → return, the same shape as the loop
                  above.
                </p>
                <div className="mt-6 font-mono text-[11px] text-muted-foreground leading-relaxed">
                  bridge grammar: origin → expansion → return
                  <br />
                  every waypoint keeps the six invariants · scale never
                  outruns verification
                </div>
              </motion.div>
              <motion.div {...rise}>
                <EnterpriseAxes />
              </motion.div>
            </div>
          </div>
        </section>

        {/* ============ DATA VAULT — PERSISTENCE + STORAGE ============ */}
        <section
          id="vault"
          className="relative py-24 border-t border-border instrument-grid">
          <div className="container">
            <SectionLabel>07 · THE VAULT — PERSISTED RUN + OBJECT STORAGE</SectionLabel>
            <motion.div {...rise} className="max-w-3xl mb-10">
              <h2 className="font-display font-bold text-3xl md:text-4xl leading-tight">
                The run is persisted.
                <br />
                The corpus is downloadable.
              </h2>
              <p className="mt-5 text-secondary-foreground leading-relaxed">
                The full 36-tick run now lives in a real database — every
                tick, every gate result, every word revision. The
                deterministic engine re-runs server-side on demand, so the
                persisted rows are always re-derivable and re-checkable.
                Export the corpus as JSONL to object storage, or sign in and
                upload your own run artifacts — bytes go to S3, the database
                keeps only keys.
              </p>
            </motion.div>
            <motion.div {...rise}>
              <DataVault />
            </motion.div>
          </div>
        </section>

        {/* ============ FOOTER / LOOP CLOSE ============ */}
        <footer className="relative border-t border-border py-16">
          <div className="container">
            <div className="flex flex-col md:flex-row items-start md:items-center justify-between gap-8">
              <div>
                <div className="flex items-center gap-3">
                  <img src={LOGO} alt="KSM∴PHASOR logo mark" className="w-8 h-8" />
                  <span className="font-display font-bold tracking-wide">
                    KSM<span style={{ color: "var(--gold)" }}>∴</span>PHASOR
                  </span>
                </div>
                <p className="font-mono text-xs text-muted-foreground mt-3 max-w-[52ch] leading-relaxed">
                  Triad phase-locked phasor engine · continuous training
                  pipeline · built in the CogHood village. Every event a
                  conscious event: heartbeat → lockstep → reactor → cogloop.
                </p>
              </div>
              <a
                href="#hero"
                className="group inline-flex items-center gap-3 font-mono text-sm px-5 py-3 border transition-all"
                style={{
                  borderColor: "var(--gate-green)",
                  color: "var(--gate-green)",
                }}>
                <span className="inline-block transition-transform group-hover:-translate-y-1">
                  ↺
                </span>
                loop closed — return to Experience
              </a>
            </div>
            <div className="hairline mt-10" />
            <div className="mt-4 font-mono text-[11px] text-muted-foreground flex flex-wrap gap-x-6 gap-y-1">
              <span>word: 8E 5E 7E 1E 4E 2E | 8E 5R 7R 1R 4R 2R</span>
              <span>budget: 7E + 5R per thread per cycle</span>
              <span>U1: 9E 9E 8R 8R · U2: 3_ 6_ 6_ 2R ∴ coherence every tick</span>
            </div>
          </div>
        </footer>
      </div>
    </TickProvider>
  );
}
