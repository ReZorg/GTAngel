/**
 * EnterpriseAxes — Phase-Locked Luminescence
 * Orthogonal-axes instrument face: organizational scope (x) versus
 * compute scale (y). Nine enterprise waypoints plot the diagonal where
 * Conway's Law holds as a conservation law — org structure and system
 * structure must move together. CogHood (origin anchor) and CogCity
 * (GPU expansion) mark the built ends of the village bridge.
 */
import { useState } from "react";

interface Waypoint {
  id: string;
  label: string;
  sub: string;
  /** org scope 0..1 (individual → federation) */
  x: number;
  /** compute scale 0..1 (edge → datacenter) */
  y: number;
  tier: "anchor" | "built" | "planned";
}

const WAYPOINTS: Waypoint[] = [
  { id: "resident", label: "Resident", sub: "one persona · one process", x: 0.08, y: 0.10, tier: "built" },
  { id: "household", label: "Household", sub: "dove9 · shared mailboxes", x: 0.20, y: 0.18, tier: "built" },
  { id: "coghood", label: "CogHood", sub: "origin VM · heartbeat + mail", x: 0.32, y: 0.30, tier: "anchor" },
  { id: "cogcity", label: "CogCity", sub: "GPU city · RTX 4000 Ada", x: 0.45, y: 0.52, tier: "anchor" },
  { id: "district", label: "District", sub: "pipeline cluster · shared corpus", x: 0.56, y: 0.62, tier: "planned" },
  { id: "cogcities", label: "CogCities", sub: "multi-city mesh · event bus", x: 0.68, y: 0.72, tier: "planned" },
  { id: "region", label: "Region", sub: "federated training pools", x: 0.79, y: 0.82, tier: "planned" },
  { id: "biome", label: "Biome", sub: "cross-org invariant treaties", x: 0.88, y: 0.90, tier: "planned" },
  { id: "planetary", label: "Planetary", sub: "the 4.6-billion-year clock", x: 0.95, y: 0.96, tier: "planned" },
];

const W = 640;
const H = 480;
const PAD = 56;

const px = (x: number) => PAD + x * (W - PAD * 2);
const py = (y: number) => H - PAD - y * (H - PAD * 2);

const tierColor = (t: Waypoint["tier"]) =>
  t === "anchor"
    ? "var(--gold)"
    : t === "built"
      ? "var(--gate-green)"
      : "var(--thread-a)";

export default function EnterpriseAxes() {
  const [hover, setHover] = useState<string | null>(null);
  const hovered = WAYPOINTS.find((w) => w.id === hover);

  return (
    <div className="relative border border-border bg-card/60 backdrop-blur-sm">
      <div className="flex items-center justify-between px-4 py-2.5 border-b border-border font-mono text-[11px]">
        <span className="text-muted-foreground">
          org scope × compute scale · conway conservation
        </span>
        <span style={{ color: "var(--gold)" }}>9 waypoints</span>
      </div>

      <svg
        viewBox={`0 0 ${W} ${H}`}
        className="w-full h-auto block"
        role="img"
        aria-label="Orthogonal axes: organizational scope versus compute scale, nine enterprise waypoints on the Conway diagonal">
        {/* grid */}
        {Array.from({ length: 5 }).map((_, i) => {
          const t = (i + 1) / 6;
          return (
            <g key={i} stroke="oklch(0.87 0.01 250 / 7%)">
              <line x1={px(t)} y1={py(0)} x2={px(t)} y2={py(1)} />
              <line x1={px(0)} y1={py(t)} x2={px(1)} y2={py(t)} />
            </g>
          );
        })}

        {/* conway diagonal band */}
        <polygon
          points={`${px(0)},${py(0.14)} ${px(0.86)},${py(1)} ${px(1)},${py(1)} ${px(1)},${py(0.86)} ${px(0.14)},${py(0)} ${px(0)},${py(0)}`}
          fill="oklch(0.82 0.13 210 / 5%)"
        />
        <line
          x1={px(0)}
          y1={py(0)}
          x2={px(1)}
          y2={py(1)}
          stroke="oklch(0.82 0.13 210 / 30%)"
          strokeDasharray="6 5"
        />
        <text
          x={px(0.62)}
          y={py(0.56)}
          fontSize="10"
          fontFamily="var(--font-mono, monospace)"
          fill="oklch(0.82 0.13 210 / 60%)"
          transform={`rotate(-33 ${px(0.62)} ${py(0.56)})`}>
          CONWAY DIAGONAL — org structure ≡ system structure
        </text>

        {/* axes */}
        <g stroke="oklch(0.87 0.01 250 / 35%)" strokeWidth="1.5">
          <line x1={px(0)} y1={py(0)} x2={px(1) + 14} y2={py(0)} />
          <line x1={px(0)} y1={py(0)} x2={px(0)} y2={py(1) - 14} />
        </g>
        {/* arrowheads */}
        <path d={`M ${px(1) + 14} ${py(0)} l -8 -4 v 8 z`} fill="oklch(0.87 0.01 250 / 35%)" />
        <path d={`M ${px(0)} ${py(1) - 14} l -4 8 h 8 z`} fill="oklch(0.87 0.01 250 / 35%)" />

        {/* axis labels */}
        <text x={px(1) + 10} y={py(0) + 26} textAnchor="end" fontSize="11" fontFamily="var(--font-mono, monospace)" fill="oklch(0.62 0.015 250)">
          ORG SCOPE → individual · household · city · federation
        </text>
        <text x={px(0) - 40} y={py(1) - 22} fontSize="11" fontFamily="var(--font-mono, monospace)" fill="oklch(0.62 0.015 250)">
          COMPUTE SCALE ↑ edge · VM · GPU · mesh · datacenter
        </text>

        {/* off-diagonal hazard labels */}
        <text x={px(0.72)} y={py(0.16)} fontSize="9" fontFamily="var(--font-mono, monospace)" fill="oklch(0.65 0.16 25 / 55%)" textAnchor="middle">
          ▼ org outruns compute — promises without ticks
        </text>
        <text x={px(0.18)} y={py(0.78)} fontSize="9" fontFamily="var(--font-mono, monospace)" fill="oklch(0.65 0.16 25 / 55%)" textAnchor="middle">
          ▲ compute outruns org — ticks without witnesses
        </text>

        {/* path connecting waypoints */}
        <polyline
          points={WAYPOINTS.map((w) => `${px(w.x)},${py(w.y)}`).join(" ")}
          fill="none"
          stroke="oklch(0.82 0.13 210 / 45%)"
          strokeWidth="1.5"
        />

        {/* waypoints */}
        {WAYPOINTS.map((w) => {
          const c = tierColor(w.tier);
          const active = hover === w.id;
          return (
            <g
              key={w.id}
              onMouseEnter={() => setHover(w.id)}
              onMouseLeave={() => setHover(null)}
              style={{ cursor: "pointer" }}>
              <circle
                cx={px(w.x)}
                cy={py(w.y)}
                r={active ? 16 : 12}
                fill="transparent"
              />
              <circle
                cx={px(w.x)}
                cy={py(w.y)}
                r={active ? 7 : 5}
                fill={c}
                style={{
                  filter: `drop-shadow(0 0 ${active ? 10 : 5}px ${c})`,
                  transition: "all 200ms cubic-bezier(0.23,1,0.32,1)",
                }}
              />
              {w.tier === "anchor" && (
                <circle
                  cx={px(w.x)}
                  cy={py(w.y)}
                  r={11}
                  fill="none"
                  stroke={c}
                  strokeWidth="1"
                  opacity="0.5"
                />
              )}
              <text
                x={px(w.x) + 12}
                y={py(w.y) - 8}
                fontSize="11"
                fontWeight={w.tier === "anchor" ? 700 : 500}
                fontFamily="var(--font-display, sans-serif)"
                fill={active ? c : "oklch(0.87 0.01 250 / 85%)"}>
                {w.label}
              </text>
            </g>
          );
        })}
      </svg>

      {/* readout strip */}
      <div className="px-4 py-2.5 border-t border-border font-mono text-[11px] min-h-[42px] flex items-center justify-between gap-4 flex-wrap">
        {hovered ? (
          <span>
            <span style={{ color: tierColor(hovered.tier) }}>
              {hovered.label.toUpperCase()}
            </span>{" "}
            <span className="text-muted-foreground">· {hovered.sub}</span>
          </span>
        ) : (
          <span className="text-muted-foreground">
            hover a waypoint · anchors are live infrastructure
          </span>
        )}
        <span className="flex items-center gap-4">
          <span className="flex items-center gap-1.5">
            <span className="w-2 h-2 rounded-full" style={{ background: "var(--gold)" }} />
            <span className="text-muted-foreground">anchor</span>
          </span>
          <span className="flex items-center gap-1.5">
            <span className="w-2 h-2 rounded-full" style={{ background: "var(--gate-green)" }} />
            <span className="text-muted-foreground">built</span>
          </span>
          <span className="flex items-center gap-1.5">
            <span className="w-2 h-2 rounded-full" style={{ background: "var(--thread-a)" }} />
            <span className="text-muted-foreground">planned</span>
          </span>
        </span>
      </div>
    </div>
  );
}
