/**
 * EnneadRotor — Phase-Locked Luminescence
 * The hero instrument: SVG ring of 12 step-nodes, three thread cursors
 * locked 120° apart, live triangle drawn between them each tick,
 * T8 gold coherence highlight. Driven ONLY by the shared tick clock.
 * Rotor easing: cubic-bezier(0.77, 0, 0.175, 1) 400ms per step advance.
 */
import { useMemo, useState } from "react";
import {
  CANONICAL_WORD,
  MODE_MEANING,
  TERM_INCIDENCE,
  THREADS,
  threadStep,
} from "@/lib/engineData";
import { useTick } from "@/contexts/TickContext";

const SIZE = 560;
const CX = SIZE / 2;
const CY = SIZE / 2;
const R_NODES = 218;
const R_CURSOR = 174;
const R_TICKS = 250;

/** step index 0..11 → angle. Step 1 at top (12 o'clock), clockwise. */
const stepAngle = (i: number) => (i / 12) * 360 - 90;
const polar = (r: number, deg: number) => {
  const rad = (deg * Math.PI) / 180;
  return { x: CX + r * Math.cos(rad), y: CY + r * Math.sin(rad) };
};

export default function EnneadRotor() {
  const { tick } = useTick();
  const [hovered, setHovered] = useState<number | null>(null);

  // active step indices (0-based) for the three threads
  const active = THREADS.map((th) => threadStep(tick, th.offset / 4));

  // triangle points between the three cursors
  const triPts = useMemo(
    () =>
      active
        .map((s) => {
          const p = polar(R_CURSOR, stepAngle(s));
          return `${p.x},${p.y}`;
        })
        .join(" "),
    [active.join(",")]
  );

  const isOdd = (tick % 12) % 2 === 0; // steps {1,5,9} on tick 0 → odd steps
  const hoveredWord = hovered !== null ? CANONICAL_WORD[hovered] : null;
  const hoveredTerm = hoveredWord ? hoveredWord.slice(0, 2) : null;
  const hoveredMode = hoveredWord
    ? (hoveredWord.slice(2) as "E" | "R")
    : null;

  return (
    <div className="relative w-full max-w-[560px] mx-auto select-none">
      <svg
        viewBox={`0 0 ${SIZE} ${SIZE}`}
        className="w-full h-auto"
        role="img"
        aria-label="Triad phasor: 12-step ennead ring with three phase-locked thread cursors">
        <defs>
          <radialGradient id="ring-halo" cx="50%" cy="50%" r="50%">
            <stop offset="60%" stopColor="transparent" />
            <stop offset="100%" stopColor="oklch(0.82 0.13 210 / 5%)" />
          </radialGradient>
        </defs>

        {/* halo + outer fine ticks (instrument dial) */}
        <circle cx={CX} cy={CY} r={R_TICKS + 16} fill="url(#ring-halo)" />
        {Array.from({ length: 60 }).map((_, i) => {
          const a = (i / 60) * 360 - 90;
          const p1 = polar(R_TICKS, a);
          const p2 = polar(R_TICKS + (i % 5 === 0 ? 10 : 5), a);
          return (
            <line
              key={i}
              x1={p1.x}
              y1={p1.y}
              x2={p2.x}
              y2={p2.y}
              stroke="oklch(0.87 0.01 250 / 20%)"
              strokeWidth={1}
            />
          );
        })}

        {/* base ring */}
        <circle
          cx={CX}
          cy={CY}
          r={R_NODES}
          fill="none"
          stroke="oklch(0.87 0.01 250 / 14%)"
          strokeWidth={1}
        />
        <circle
          cx={CX}
          cy={CY}
          r={R_CURSOR}
          fill="none"
          stroke="oklch(0.87 0.01 250 / 8%)"
          strokeWidth={1}
          strokeDasharray="2 6"
        />

        {/* live triangle between the three cursors */}
        <polygon
          key={`tri-${tick}`}
          points={triPts}
          fill={
            isOdd
              ? "oklch(0.82 0.13 210 / 6%)"
              : "oklch(0.72 0.2 340 / 6%)"
          }
          stroke={
            isOdd
              ? "oklch(0.82 0.13 210 / 45%)"
              : "oklch(0.72 0.2 340 / 45%)"
          }
          strokeWidth={1.25}
          style={{
            transition:
              "all 400ms cubic-bezier(0.77, 0, 0.175, 1)",
          }}
        />

        {/* 12 step nodes */}
        {CANONICAL_WORD.map((w, i) => {
          const a = stepAngle(i);
          const p = polar(R_NODES, a);
          const lp = polar(R_NODES + 30, a);
          const term = w.slice(0, 2);
          const mode = w.slice(2) as "E" | "R";
          const isT8 = term === "T8";
          const isActive = active.includes(i);
          const activeThread = THREADS[active.indexOf(i)];
          const nodeColor = isActive
            ? activeThread.colorVar
            : isT8
              ? "var(--gold)"
              : mode === "E"
                ? "var(--mode-e)"
                : "var(--mode-r)";
          return (
            <g
              key={i}
              onMouseEnter={() => setHovered(i)}
              onMouseLeave={() => setHovered(null)}
              style={{ cursor: "crosshair" }}>
              {/* hover hit area */}
              <circle cx={p.x} cy={p.y} r={24} fill="transparent" />
              <circle
                cx={p.x}
                cy={p.y}
                r={isActive ? 9 : 5.5}
                fill={nodeColor}
                opacity={isActive ? 1 : isT8 ? 0.9 : 0.55}
                style={{
                  transition: "r 400ms cubic-bezier(0.77,0,0.175,1)",
                  filter: isActive
                    ? `drop-shadow(0 0 8px ${activeThread.colorVar})`
                    : isT8
                      ? "drop-shadow(0 0 5px oklch(0.83 0.12 90 / 60%))"
                      : "none",
                }}
              />
              {isT8 && (
                <circle
                  cx={p.x}
                  cy={p.y}
                  r={13}
                  fill="none"
                  stroke="var(--gold)"
                  strokeWidth={1}
                  opacity={0.5}
                />
              )}
              <text
                x={lp.x}
                y={lp.y}
                textAnchor="middle"
                dominantBaseline="middle"
                className="font-mono"
                fontSize={13}
                fontWeight={isActive ? 600 : 400}
                fill={
                  hovered === i
                    ? "oklch(0.95 0.02 90)"
                    : isActive
                      ? "oklch(0.9 0.01 250)"
                      : "oklch(0.62 0.015 250)"
                }>
                {w}
              </text>
              <text
                x={lp.x}
                y={lp.y + 15}
                textAnchor="middle"
                className="font-mono"
                fontSize={9}
                fill="oklch(0.62 0.015 250 / 65%)">
                {i + 1}
              </text>
            </g>
          );
        })}

        {/* thread cursors */}
        {THREADS.map((th, k) => {
          const s = active[k];
          const a = stepAngle(s);
          const p = polar(R_CURSOR, a);
          return (
            <g
              key={th.id}
              style={{
                transition:
                  "transform 400ms cubic-bezier(0.77,0,0.175,1)",
              }}>
              <line
                x1={CX}
                y1={CY}
                x2={p.x}
                y2={p.y}
                stroke={th.colorVar}
                strokeWidth={1.5}
                opacity={0.5}
                style={{
                  transition:
                    "all 400ms cubic-bezier(0.77,0,0.175,1)",
                }}
              />
              <circle
                cx={p.x}
                cy={p.y}
                r={7}
                fill="var(--background)"
                stroke={th.colorVar}
                strokeWidth={2}
                style={{
                  transition:
                    "all 400ms cubic-bezier(0.77,0,0.175,1)",
                  filter: `drop-shadow(0 0 6px ${th.colorVar})`,
                }}
              />
            </g>
          );
        })}

        {/* center: T8 coherence hub */}
        <circle
          cx={CX}
          cy={CY}
          r={5}
          fill="var(--gold)"
          style={{ filter: "drop-shadow(0 0 8px oklch(0.83 0.12 90))" }}
        />
        <circle
          cx={CX}
          cy={CY}
          r={16}
          fill="none"
          stroke="var(--gold)"
          strokeWidth={1}
          opacity={0.35}
        />
      </svg>

      {/* hover readout — Term incidence */}
      <div className="absolute left-1/2 -translate-x-1/2 bottom-[34%] pointer-events-none text-center min-h-[72px] w-[240px]">
        {hoveredWord && hoveredTerm && hoveredMode ? (
          <div className="font-mono text-xs leading-relaxed">
            <div className="text-foreground font-semibold text-sm">
              {hoveredWord}
              <span className="text-muted-foreground font-normal">
                {" "}
                · {MODE_MEANING[hoveredMode].verb}
              </span>
            </div>
            <div className="text-muted-foreground mt-1">
              {TERM_INCIDENCE[hoveredTerm]?.role}
            </div>
            <div className="mt-1" style={{ color: "var(--gold)" }}>
              ⟨{TERM_INCIDENCE[hoveredTerm]?.interfaces.join(" · ")}⟩
            </div>
          </div>
        ) : null}
      </div>
    </div>
  );
}
