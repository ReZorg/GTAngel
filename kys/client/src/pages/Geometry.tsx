/**
 * Geometry — three projections of one engine state.
 * 1. The 12-step ternary mask rotor (phasor view)
 * 2. The identical state in the ng2p-3 / S3 s-gram configuration (enneagram view)
 * 3. The 4 triads as faces of one tetrahedron, permuting over 4 vertices & 6 edges
 * All three are driven by the SAME shared tick clock — one heartbeat, three geometries.
 */
import { motion } from "framer-motion";
import { Link } from "wouter";
import EnneadRotor from "@/components/EnneadRotor";
import SGramRotor from "@/components/SGramRotor";
import TetraPermute from "@/components/TetraPermute";
import SylvesterNonionPanel from "@/components/SylvesterNonionPanel";
import TickControls from "@/components/TickControls";
import { TickProvider, TickScanline } from "@/contexts/TickContext";
import { REF_NG2P3, REF_S4_ENNEAD } from "@/lib/geometryData";

const LOGO = "/manus-storage/ksm-logo-mark_9e52438b.png";

const rise = {
  initial: { opacity: 0, y: 24 },
  whileInView: { opacity: 1, y: 0 },
  viewport: { once: true, margin: "-60px" },
  transition: { duration: 0.5, ease: "easeOut" as const },
};

function PanelFrame({
  station,
  title,
  children,
  accentVar,
}: {
  station: string;
  title: string;
  children: React.ReactNode;
  accentVar: string;
}) {
  return (
    <div className="relative border border-border bg-card/60 p-5 h-full">
      {/* console corner frames */}
      <div className="absolute -inset-px pointer-events-none" aria-hidden>
        {[
          "top-0 left-0 border-t-2 border-l-2",
          "top-0 right-0 border-t-2 border-r-2",
          "bottom-0 left-0 border-b-2 border-l-2",
          "bottom-0 right-0 border-b-2 border-r-2",
        ].map(pos => (
          <span
            key={pos}
            className={`absolute w-5 h-5 ${pos}`}
            style={{ borderColor: accentVar }}
          />
        ))}
      </div>
      <div className="font-mono text-[10px] tracking-[0.25em] text-muted-foreground mb-1">
        {station}
      </div>
      <h3 className="font-display font-bold text-lg mb-3" style={{ color: accentVar }}>
        {title}
      </h3>
      {children}
    </div>
  );
}

export default function Geometry() {
  return (
    <TickProvider>
      <div className="min-h-screen bg-background text-foreground overflow-x-clip">
        <TickScanline />

        {/* header */}
        <header className="fixed top-0 inset-x-0 z-40 border-b border-border bg-background/75 backdrop-blur-md">
          <div className="container flex items-center justify-between py-3">
            <Link href="/" className="flex items-center gap-3">
              <img src={LOGO} alt="KSM∴PHASOR mark" className="w-9 h-9" />
              <span className="font-display font-bold tracking-wide text-lg">
                KSM<span style={{ color: "var(--gold)" }}>∴</span>PHASOR
              </span>
            </Link>
            <nav className="hidden md:flex items-center gap-5 font-mono text-xs text-muted-foreground">
              <a href="#rotor" className="hover:text-foreground transition-colors">
                ROTOR
              </a>
              <a href="#sgram" className="hover:text-foreground transition-colors">
                S-GRAM
              </a>
              <a href="#tetra" className="hover:text-foreground transition-colors">
                TETRAHEDRON
              </a>
              <a href="#sylvester" className="hover:text-foreground transition-colors">
                SYLVESTER NONION
              </a>
              <Link href="/" className="hover:text-foreground transition-colors">
                ← PIPELINE
              </Link>
            </nav>
          </div>
        </header>

        <main className="pt-28 pb-20">
          <div className="container">
            {/* intro */}
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, ease: "easeOut" }}
              className="max-w-3xl mb-10">
              <div className="overline-label mb-4">
                GEOMETRY · THREE PROJECTIONS · ONE CLOCK
              </div>
              <h1 className="font-display font-bold text-4xl md:text-5xl leading-tight">
                Same state,
                <br />
                three geometries.
              </h1>
              <p className="mt-5 text-secondary-foreground leading-relaxed">
                The panels below are driven by one shared tick. The phasor
                rotor shows the 12-step ternary mask; the s-gram shows the
                identical state in Andersen's{" "}
                <span className="font-mono text-sm">ng2p-3</span> (S3)
                configuration — 9 points, base 10, divisor pair 3 & 7; the
                tetrahedron shows the four triads as faces of one K4,
                permuting over its 4 vertices and 6 edges. Pause, step, or
                scrub — all three move together.
              </p>
              <div className="mt-6">
                <TickControls />
              </div>
            </motion.div>

            {/* three panels */}
            <div className="grid lg:grid-cols-3 gap-6 items-stretch">
              <motion.div {...rise} id="rotor">
                <PanelFrame
                  station="G1 · PHASOR VIEW"
                  title="12-step ternary mask rotor"
                  accentVar="var(--thread-a)">
                  <EnneadRotor />
                  <p className="mt-3 font-mono text-[11px] text-muted-foreground leading-relaxed">
                    Three thread cursors locked 120° apart on the 12-ring;
                    thread k executes word[(g+4k)%12]. The step-triple
                    {" {g+1, g+5, g+9}"} draws the Structure / Process
                    triangles.
                  </p>
                </PanelFrame>
              </motion.div>

              <motion.div {...rise} id="sgram">
                <PanelFrame
                  station="G2 · S-GRAM VIEW (ng2p-3)"
                  title="S3 enneagram configuration"
                  accentVar="var(--thread-b)">
                  <SGramRotor />
                  <p className="mt-3 font-mono text-[11px] text-muted-foreground leading-relaxed">
                    Base 10, divisors 3 & 7. Steps {"{1,5,9}"} walk the
                    informing points 3→6→0/9 (÷3); steps {"{2,6,10}"} and
                    {" {3,7,11}"} run the two counter-phase 1/7 hexad patterns
                    (1→2→5 and 4→8→7) that jointly cover 1→4→2→8→5→7; steps
                    {" {4,8,12}"} pulse the zero point — integration.
                  </p>
                </PanelFrame>
              </motion.div>

              <motion.div {...rise} id="tetra">
                <PanelFrame
                  station="G3 · TETRAHEDRAL VIEW (K4)"
                  title="4 triads on 4 faces"
                  accentVar="var(--thread-c)">
                  <TetraPermute />
                  <p className="mt-3 font-mono text-[11px] text-muted-foreground leading-relaxed">
                    Vertices C1–C4 are the four threads/interfaces; the 6
                    edges are all pairwise connections P(i,j); the 4 faces are
                    the triads {"{1,5,9}"}, {"{2,6,10}"}, {"{3,7,11}"},
                    {" {4,8,12}"}. Gauge orbits MP1/MP2 advance one face per
                    tick; the gold edge is the shared fiber — two orbits
                    touching the same connection.
                  </p>
                </PanelFrame>
              </motion.div>
            </div>

            {/* Sylvester Nonion Clock-Tower Panel */}
            <motion.div {...rise} id="sylvester" className="mt-8">
              <PanelFrame
                station="G4 · SYLVESTER NONION & DUAL LIE ALGEBRA VIEW"
                title="Frame-Shifting Clock-Tower & 3-5 Lie Algebra Duality"
                accentVar="var(--gold)">
                <div className="grid lg:grid-cols-2 gap-6 items-center">
                  <SylvesterNonionPanel />
                  <div className="space-y-3 font-mono text-xs text-secondary-foreground leading-relaxed">
                    <p>
                      <strong className="text-gold">Sylvester Shift + Clock Matrix:</strong>{" "}
                      Grounded in Sylvester's (1882) S₃ clock and T₃ shift matrices.
                      Satisfies the commutation relation S<sub>N</sub>T<sub>N</sub> = e<sup>2πi/N</sup> T<sub>N</sub>S<sub>N</sub>.
                    </p>
                    <p>
                      <strong className="text-cyan-400">Nonion Hypercomplex Algebra:</strong>{" "}
                      9-dimensional associative algebra over C (M₃(C)), generated by E<sub>j,k</sub> = S₃<sup>j</sup>T₃<sup>k</sup>.
                      Models KSM niche-construction frame shifting as unitary frame rotations F(g) = S₃<sup>⌊g/4⌋</sup>T₃<sup>g mod 3</sup>.
                    </p>
                    <p>
                      <strong className="text-magenta-400" style={{ color: "var(--thread-b)" }}>Dual Ternary-Quinary Lie Algebra:</strong>{" "}
                      Bilateral 3-5 correspondence relating commutative-associative relations
                      (like DNA triplet codons / icosidodecahedron 20×3 = 12×5 = 60 edge duality) on the joint g₃,₅ tensor space.
                    </p>
                  </div>
                </div>
              </PanelFrame>
            </motion.div>

            {/* correspondence table */}
            <motion.div {...rise} className="mt-14 max-w-4xl">
              <div className="flex items-center gap-4 mb-5">
                <span className="overline-label">THE CORRESPONDENCE</span>
                <div className="hairline flex-1" />
              </div>
              <div className="overflow-x-auto border border-border">
                <table className="w-full font-mono text-xs">
                  <thead>
                    <tr className="border-b border-border bg-card/70 text-muted-foreground">
                      <th className="text-left p-3 font-normal">step family</th>
                      <th className="text-left p-3 font-normal">rotor (12-ring)</th>
                      <th className="text-left p-3 font-normal">s-gram (S3, base 10)</th>
                      <th className="text-left p-3 font-normal">tetrahedron (K4)</th>
                    </tr>
                  </thead>
                  <tbody className="text-secondary-foreground">
                    <tr className="border-b border-border">
                      <td className="p-3" style={{ color: "var(--gold)" }}>
                        {"{1, 5, 9}"}
                      </td>
                      <td className="p-3">thread-A anchor steps</td>
                      <td className="p-3">informing points 3 → 6 → 0/9 (÷3)</td>
                      <td className="p-3">face F123</td>
                    </tr>
                    <tr className="border-b border-border">
                      <td className="p-3" style={{ color: "var(--thread-a)" }}>
                        {"{2, 6, 10}"}
                      </td>
                      <td className="p-3">thread-B anchor steps</td>
                      <td className="p-3">hexad pattern 1 · 1 → 2 → 5 (÷7)</td>
                      <td className="p-3">face F124</td>
                    </tr>
                    <tr className="border-b border-border">
                      <td className="p-3" style={{ color: "var(--thread-b)" }}>
                        {"{3, 7, 11}"}
                      </td>
                      <td className="p-3">thread-C anchor steps</td>
                      <td className="p-3">hexad pattern 2 · 4 → 8 → 7 (÷7)</td>
                      <td className="p-3">face F134</td>
                    </tr>
                    <tr>
                      <td className="p-3" style={{ color: "var(--gate-green)" }}>
                        {"{4, 8, 12}"}
                      </td>
                      <td className="p-3">integration steps</td>
                      <td className="p-3">zero point 0/9 pulse</td>
                      <td className="p-3">face F234</td>
                    </tr>
                  </tbody>
                </table>
              </div>
              <p className="mt-4 text-secondary-foreground leading-relaxed text-sm max-w-[70ch]">
                The two hexad patterns together traverse the full 1/7 cycle
                1→4→2→8→5→7 once per 12 ticks, while the informing stream
                completes the ÷3 triple and the zero point fires three times —
                the S-gram's resolving/informing duality is the rotor's
                Expressive/Regenerative mode budget seen in base 10. On the
                tetrahedron the same 12 ticks are three passes of the 4-step
                gauge cycle: each triad-face is visited by both orbits, and
                every one of the 6 edges carries the shared fiber twice per
                word — the complete K4 connection structure is sampled
                exactly.
              </p>
            </motion.div>

            {/* reference strip */}
            <motion.div {...rise} className="mt-14">
              <div className="flex items-center gap-4 mb-5">
                <span className="overline-label">SOURCE FIGURES</span>
                <div className="hairline flex-1" />
              </div>
              <div className="grid md:grid-cols-[auto_1fr] gap-6 items-start">
                <div className="border border-border bg-white p-3 w-fit">
                  <img
                    src={REF_NG2P3}
                    alt="Sigurd Andersen's ng2p-3 S-gram: the S3 enneagram with red 1/7 hexad and informing squares at 0, 3, 6"
                    className="w-[180px]"
                  />
                </div>
                <div className="border border-border bg-white p-3">
                  <img
                    src={REF_S4_ENNEAD}
                    alt="Four steps in each cycle: three Particular Sets transforming through the six-term sequence one step apart, regulated by the Primary and Secondary Universal Sets"
                    className="w-full max-w-[760px]"
                  />
                </div>
              </div>
              <p className="mt-4 font-mono text-[11px] text-muted-foreground leading-relaxed max-w-[80ch]">
                left: ng2p-3 — Second-Power N-Gram #3 (S3), Sigurd Andersen,
                solbakkn.com/math/n-grams · right: "Four Steps in Each Cycle"
                — the three Particular Sets one step apart with Primary /
                Secondary Universal regulation, the source of the 4-step gauge
                cycle shown on the tetrahedron.
              </p>
            </motion.div>
          </div>
        </main>

        {/* footer */}
        <footer className="border-t border-border py-10">
          <div className="container flex flex-col md:flex-row items-start md:items-center justify-between gap-6">
            <p className="font-mono text-xs text-muted-foreground max-w-[52ch] leading-relaxed">
              One clock, three geometries: phasor ring · S3 s-gram · K4
              tetrahedron. The projection changes; the state does not.
            </p>
            <Link
              href="/"
              className="inline-flex items-center gap-3 font-mono text-sm px-5 py-3 border transition-all hover:-translate-x-1"
              style={{ borderColor: "var(--thread-a)", color: "var(--thread-a)" }}>
              ← back to the pipeline
            </Link>
          </div>
        </footer>
      </div>
    </TickProvider>
  );
}
