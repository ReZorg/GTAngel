/**
 * TetraPermute — the 4 triads as faces of one tetrahedron (K4), permuting
 * over its 4 vertices and 6 edges as the tick advances.
 *
 * The two gauge orbits (Gauge Theory of Cognitive Architectures §4):
 *   MP1: F123 → F124 → F134 → F234
 *   MP2: F134 → F234 → F123 → F124
 * advance one face per tick (a 4-step cycle — three cycles per 12-tick word).
 * At every step the two active faces share exactly one edge — the shared
 * fiber, rendered gold ("entanglement of order 2").
 *
 * Continuous slow 3D rotation is decorative (rAF); the face permutation is
 * strictly tick-driven from the shared clock.
 */
import { useEffect, useRef, useState } from "react";
import { useTick } from "@/contexts/TickContext";
import {
  MP1_ORBIT,
  MP2_ORBIT,
  TETRA_EDGES,
  TETRA_FACES,
  TETRA_VERTS,
  TETRA_VERT_LABELS,
  faceById,
  sharedEdgeAt,
} from "@/lib/geometryData";

const SIZE = 440;
const CX = SIZE / 2;
const CY = SIZE / 2 - 4;
const SCALE = 108;
const CAM_DIST = 5.2;

function rotateY([x, y, z]: [number, number, number], a: number): [number, number, number] {
  return [x * Math.cos(a) + z * Math.sin(a), y, -x * Math.sin(a) + z * Math.cos(a)];
}
function rotateX([x, y, z]: [number, number, number], a: number): [number, number, number] {
  return [x, y * Math.cos(a) - z * Math.sin(a), y * Math.sin(a) + z * Math.cos(a)];
}

function project(v: [number, number, number]): { x: number; y: number; z: number } {
  const persp = CAM_DIST / (CAM_DIST - v[2]);
  return { x: CX + v[0] * SCALE * persp, y: CY - v[1] * SCALE * persp, z: v[2] };
}

export default function TetraPermute() {
  const { tick } = useTick();
  const [angle, setAngle] = useState(0.6);
  const rafRef = useRef<number>(0);

  // slow continuous spin — decorative only
  useEffect(() => {
    let last = performance.now();
    const loop = (now: number) => {
      const dt = now - last;
      last = now;
      setAngle(a => a + dt * 0.00022);
      rafRef.current = requestAnimationFrame(loop);
    };
    rafRef.current = requestAnimationFrame(loop);
    return () => cancelAnimationFrame(rafRef.current);
  }, []);

  const phase = tick % 4; // 4-step gauge cycle
  const mp1Face = faceById(MP1_ORBIT[phase]);
  const mp2Face = faceById(MP2_ORBIT[phase]);
  const shared = sharedEdgeAt(phase);

  const TILT = 0.42;
  const rotated = TETRA_VERTS.map(v => rotateX(rotateY(v, angle), TILT));
  const proj = rotated.map(project);

  // painter's order for faces: farthest first
  const facesSorted = [...TETRA_FACES].sort((a, b) => {
    const za = a.verts.reduce((s, vi) => s + rotated[vi][2], 0);
    const zb = b.verts.reduce((s, vi) => s + rotated[vi][2], 0);
    return za - zb;
  });

  const isShared = (a: number, b: number) =>
    (a === shared[0] && b === shared[1]) || (a === shared[1] && b === shared[0]);

  return (
    <svg
      viewBox={`0 0 ${SIZE} ${SIZE}`}
      className="w-full max-w-[440px] mx-auto select-none"
      role="img"
      aria-label="K4 tetrahedron with the four triads permuting over vertices and edges">
      {/* faces */}
      {facesSorted.map(f => {
        const isMp1 = f.id === mp1Face.id;
        const isMp2 = f.id === mp2Face.id;
        const activeColor = isMp1
          ? "var(--thread-a)"
          : isMp2
            ? "var(--thread-b)"
            : null;
        const pts = f.verts.map(vi => `${proj[vi].x},${proj[vi].y}`).join(" ");
        return (
          <polygon
            key={f.id}
            points={pts}
            fill={activeColor ?? "oklch(0.3 0.02 250)"}
            opacity={activeColor ? 0.24 : 0.05}
            stroke="none"
            style={{ transition: "opacity 300ms, fill 300ms" }}
          />
        );
      })}

      {/* edges */}
      {TETRA_EDGES.map(([a, b]) => {
        const sharedEdge = isShared(a, b);
        return (
          <line
            key={`e-${a}-${b}`}
            x1={proj[a].x}
            y1={proj[a].y}
            x2={proj[b].x}
            y2={proj[b].y}
            stroke={sharedEdge ? "var(--gold)" : "oklch(0.55 0.03 250)"}
            strokeWidth={sharedEdge ? 3 : 1.4}
            strokeLinecap="round"
            opacity={sharedEdge ? 1 : 0.8}
            style={
              sharedEdge
                ? { filter: "drop-shadow(0 0 6px var(--gold))", transition: "all 300ms" }
                : { transition: "all 300ms" }
            }
          />
        );
      })}

      {/* vertices */}
      {proj.map((p, i) => (
        <g key={`v-${i}`}>
          <circle
            cx={p.x}
            cy={p.y}
            r={7}
            fill="oklch(0.18 0.01 250)"
            stroke="oklch(0.75 0.02 250)"
            strokeWidth={1.5}
          />
          <text
            x={p.x}
            y={p.y - 13}
            textAnchor="middle"
            fontSize={12}
            fontFamily="var(--font-mono, monospace)"
            fill="oklch(0.78 0.02 250)">
            {TETRA_VERT_LABELS[i]}
          </text>
        </g>
      ))}

      {/* orbit readout */}
      <g fontFamily="var(--font-mono, monospace)" fontSize={12}>
        <text x={14} y={SIZE - 44} fill="var(--thread-a)">
          MP1 · {mp1Face.id} · triad {"{"}
          {mp1Face.triad.join(",")}
          {"}"}
        </text>
        <text x={14} y={SIZE - 26} fill="var(--thread-b)">
          MP2 · {mp2Face.id} · triad {"{"}
          {mp2Face.triad.join(",")}
          {"}"}
        </text>
        <text x={14} y={SIZE - 8} fill="var(--gold)">
          shared fiber · edge {TETRA_VERT_LABELS[shared[0]]}–{TETRA_VERT_LABELS[shared[1]]}
        </text>
      </g>

      {/* phase indicator */}
      <g fontFamily="var(--font-mono, monospace)" fontSize={11}>
        {[0, 1, 2, 3].map(p => (
          <circle
            key={p}
            cx={SIZE - 70 + p * 16}
            cy={SIZE - 14}
            r={4.5}
            fill={p === phase ? "var(--gold)" : "oklch(0.35 0.02 250)"}
            style={{ transition: "fill 240ms" }}
          />
        ))}
        <text x={SIZE - 70 - 8} y={SIZE - 10} textAnchor="end" fill="oklch(0.65 0.02 250)">
          cycle phase
        </text>
      </g>
    </svg>
  );
}
