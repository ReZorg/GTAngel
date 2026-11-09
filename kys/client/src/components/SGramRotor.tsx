/**
 * SGramRotor — the same 12-step tick state rendered in the ng2p-3 (S3)
 * s-gram configuration: Sigurd Andersen's Second-Power N-Gram #3, the
 * enneagram (9 points, base 10, divisor pair 3 & 7).
 *
 * Convention (per Andersen): 0/9 at top; informing points 0, 3, 6 drawn as
 * square markers; the 1/7 resolving hexad 1→4→2→8→5→7 drawn as the thick
 * path; dashed radial spokes to the centre.
 *
 * Tick mapping: step families {1,5,9} / {2,6,10} / {3,7,11} / {4,8,12}
 * become informing stream (÷3), hexad pattern 1 (÷7), hexad pattern 2 (÷7
 * counter-phase), and zero-point integration respectively.
 */
import { useTick } from "@/contexts/TickContext";
import {
  S3_HEXAD,
  S3_INFORMING,
  SGRAM_FAMILY_COLOR,
  SGRAM_TICKS,
  s3PointAngle,
} from "@/lib/geometryData";

const SIZE = 420;
const CX = SIZE / 2;
const CY = SIZE / 2 + 6;
const R = 158;

function pt(digit: number, r: number = R): { x: number; y: number } {
  const a = s3PointAngle(digit);
  return { x: CX + r * Math.cos(a), y: CY + r * Math.sin(a) };
}

export default function SGramRotor() {
  const { tick } = useTick();
  const step = (tick % 12) + 1; // engine step 1..12
  const state = SGRAM_TICKS[step - 1];
  const color = SGRAM_FAMILY_COLOR[state.family];

  // hexad segments as digit pairs, in traversal order
  const hexadSegs: [number, number][] = S3_HEXAD.map((d, i) => [
    d,
    S3_HEXAD[(i + 1) % S3_HEXAD.length],
  ]);

  // which hexad node is the current cursor (if a hexad family is active)
  const isHexadTick = state.family === "hexad1" || state.family === "hexad2";
  // segments already traversed this cycle by the two counter-running patterns:
  // light the segment leaving each pattern's current digit
  const litSegments = new Set<number>();
  if (isHexadTick) {
    const idx = S3_HEXAD.indexOf(state.digit as (typeof S3_HEXAD)[number]);
    if (idx >= 0) litSegments.add(idx);
  }

  const active = pt(state.digit);

  return (
    <svg
      viewBox={`0 0 ${SIZE} ${SIZE}`}
      className="w-full max-w-[420px] mx-auto select-none"
      role="img"
      aria-label="S3 s-gram (ng2p-3) enneagram view of the 12-step cycle">
      {/* outer circle */}
      <circle
        cx={CX}
        cy={CY}
        r={R}
        fill="none"
        stroke="oklch(0.35 0.02 250)"
        strokeWidth={1.5}
      />

      {/* dashed radial spokes to centre */}
      {Array.from({ length: 9 }, (_, d) => {
        const p = pt(d);
        return (
          <line
            key={`spoke-${d}`}
            x1={CX}
            y1={CY}
            x2={p.x}
            y2={p.y}
            stroke="oklch(0.4 0.02 250)"
            strokeWidth={0.8}
            strokeDasharray="3 5"
          />
        );
      })}

      {/* centre asterisk */}
      <text
        x={CX}
        y={CY + 4}
        textAnchor="middle"
        fontSize={14}
        fill="oklch(0.6 0.02 250)"
        fontFamily="var(--font-mono, monospace)">
        ✳
      </text>

      {/* the 1/7 resolving hexad path */}
      {hexadSegs.map(([a, b], i) => {
        const pa = pt(a);
        const pb = pt(b);
        const lit = litSegments.has(i);
        return (
          <line
            key={`hex-${i}`}
            x1={pa.x}
            y1={pa.y}
            x2={pb.x}
            y2={pb.y}
            stroke={lit ? color : "oklch(0.55 0.19 25)"}
            strokeWidth={lit ? 5 : 3.5}
            strokeLinecap="round"
            opacity={lit ? 1 : 0.75}
            style={
              lit
                ? { filter: `drop-shadow(0 0 6px ${color})`, transition: "all 240ms" }
                : { transition: "all 240ms" }
            }
          />
        );
      })}

      {/* 9 digit nodes */}
      {Array.from({ length: 9 }, (_, d) => {
        const p = pt(d);
        const isInforming = (S3_INFORMING as readonly number[]).includes(d);
        const isActive = state.digit === d;
        const lp = pt(d, R + 22);
        return (
          <g key={`node-${d}`}>
            {isInforming ? (
              <rect
                x={p.x - 6.5}
                y={p.y - 6.5}
                width={13}
                height={13}
                fill={isActive ? color : "oklch(0.2 0.01 250)"}
                stroke={isActive ? color : "oklch(0.6 0.02 250)"}
                strokeWidth={1.5}
                style={
                  isActive
                    ? { filter: `drop-shadow(0 0 8px ${color})`, transition: "all 240ms" }
                    : { transition: "all 240ms" }
                }
              />
            ) : (
              <circle
                cx={p.x}
                cy={p.y}
                r={6}
                fill={isActive ? color : "oklch(0.2 0.01 250)"}
                stroke={isActive ? color : "oklch(0.6 0.02 250)"}
                strokeWidth={1.5}
                style={
                  isActive
                    ? { filter: `drop-shadow(0 0 8px ${color})`, transition: "all 240ms" }
                    : { transition: "all 240ms" }
                }
              />
            )}
            <text
              x={lp.x}
              y={lp.y + 4}
              textAnchor="middle"
              fontSize={12}
              fontFamily="var(--font-mono, monospace)"
              fill={isActive ? color : "oklch(0.65 0.02 250)"}>
              {d === 0 ? "0/9" : d}
            </text>
          </g>
        );
      })}

      {/* active pulse ring */}
      <circle
        key={`pulse-${tick}`}
        cx={active.x}
        cy={active.y}
        r={12}
        fill="none"
        stroke={color}
        strokeWidth={1.5}
        opacity={0.8}>
        <animate attributeName="r" from="8" to="22" dur="0.9s" fill="freeze" />
        <animate attributeName="opacity" from="0.8" to="0" dur="0.9s" fill="freeze" />
      </circle>

      {/* readout */}
      <text
        x={CX}
        y={SIZE - 6}
        textAnchor="middle"
        fontSize={12}
        fontFamily="var(--font-mono, monospace)"
        fill={color}>
        step {step} · {state.label}
      </text>
    </svg>
  );
}
