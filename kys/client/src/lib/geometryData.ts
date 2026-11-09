/**
 * geometryData.ts — data for the /geometry page's three synced diagrams.
 *
 * Panel 2: ng2p-3 (S3, the enneagram) s-gram configuration.
 *   Sigurd Andersen's Second-Power N-Gram #3: 9 points, base 10, divisor pair 3 & 7.
 *   - Resolving pattern (red): the 1/7 hexad 1→4→2→8→5→7 (0.142857…)
 *   - Informing points (black squares): 0/9, 3, 6 (the single repeating digits of 1/3, 2/3, 0.999…)
 *
 * Panel 3: K4 tetrahedron — 4 triads as 4 faces permuting over 4 vertices & 6 edges.
 *   Gauge orbits MP1 / MP2 traverse all four faces one step apart
 *   (Gauge Theory of Cognitive Architectures §4).
 */

/* ============ S3 / ng2p-3 ============ */

/** The 9 digits of S3 laid on a circle, 0/9 at top, clockwise. */
export function s3PointAngle(digit: number): number {
  // digit 0..8; 0 at top (-90°), 40° per step clockwise
  return (-90 + digit * 40) * (Math.PI / 180);
}

/** The 1/7 resolving hexad in traversal order. */
export const S3_HEXAD = [1, 4, 2, 8, 5, 7] as const;

/** Informing digits: zero point + the 1/3 pattern. */
export const S3_INFORMING = [0, 3, 6] as const;

export type SGramFamily = "informing" | "hexad1" | "hexad2" | "zero";

export interface SGramTickState {
  /** engine step 1..12 */
  step: number;
  /** which pattern family carries this tick */
  family: SGramFamily;
  /** the S3 digit lit at this tick (0..8; 0 = the 0/9 top point) */
  digit: number;
  /** short label for the readout */
  label: string;
}

/**
 * The 12-step cycle mapped onto S3 (per SGRAMS_ANALYSIS cognitive-loop mapping):
 *  steps {1,5,9}  → informing stream (division by 3): 3 → 6 → 0/9
 *  steps {2,6,10} → hexad pattern 1 (division by 7, odd hops): 1 → 2 → 5
 *  steps {3,7,11} → hexad pattern 2 (division by 7, counter-phase): 4 → 8 → 7
 *  steps {4,8,12} → integration: the zero point pulses
 * Patterns 1 & 2 together cover the full hexad each cycle.
 */
export const SGRAM_TICKS: SGramTickState[] = [
  { step: 1, family: "informing", digit: 3, label: "1/3 → ·3" },
  { step: 2, family: "hexad1", digit: 1, label: "1/7 p1 → 1" },
  { step: 3, family: "hexad2", digit: 4, label: "1/7 p2 → 4" },
  { step: 4, family: "zero", digit: 0, label: "∫ zero point" },
  { step: 5, family: "informing", digit: 6, label: "2/3 → ·6" },
  { step: 6, family: "hexad1", digit: 2, label: "1/7 p1 → 2" },
  { step: 7, family: "hexad2", digit: 8, label: "1/7 p2 → 8" },
  { step: 8, family: "zero", digit: 0, label: "∫ zero point" },
  { step: 9, family: "informing", digit: 0, label: "0.9̄ → 0/9" },
  { step: 10, family: "hexad1", digit: 5, label: "1/7 p1 → 5" },
  { step: 11, family: "hexad2", digit: 7, label: "1/7 p2 → 7" },
  { step: 12, family: "zero", digit: 0, label: "∫ zero point" },
];

export const SGRAM_FAMILY_COLOR: Record<SGramFamily, string> = {
  informing: "var(--gold)",
  hexad1: "var(--thread-a)",
  hexad2: "var(--thread-b)",
  zero: "var(--gate-green)",
};

/* ============ K4 tetrahedron ============ */

/** Regular tetrahedron vertices (unit-ish), indexed 1..4 → array 0..3. */
export const TETRA_VERTS: [number, number, number][] = [
  [1, 1, 1],
  [1, -1, -1],
  [-1, 1, -1],
  [-1, -1, 1],
];

export const TETRA_VERT_LABELS = ["C1", "C2", "C3", "C4"] as const;

/** All 6 edges as vertex-index pairs (0-based). */
export const TETRA_EDGES: [number, number][] = [
  [0, 1],
  [0, 2],
  [0, 3],
  [1, 2],
  [1, 3],
  [2, 3],
];

/** 4 faces as vertex-index triples (0-based), keyed by triad. */
export interface TetraFace {
  id: string;
  verts: [number, number, number];
  /** the step-triple this face embodies */
  triad: [number, number, number];
  colorVar: string;
}

export const TETRA_FACES: TetraFace[] = [
  { id: "F123", verts: [0, 1, 2], triad: [1, 5, 9], colorVar: "var(--thread-a)" },
  { id: "F124", verts: [0, 1, 3], triad: [2, 6, 10], colorVar: "var(--thread-b)" },
  { id: "F134", verts: [0, 2, 3], triad: [3, 7, 11], colorVar: "var(--thread-c)" },
  { id: "F234", verts: [1, 2, 3], triad: [4, 8, 12], colorVar: "var(--gold)" },
];

/**
 * The two gauge orbits over the 4 faces (advance one face per tick; 4-step cycle):
 *   MP1: F123 → F124 → F134 → F234
 *   MP2: F134 → F234 → F123 → F124
 * At every step the two active faces share exactly one edge — the shared fiber
 * ("entanglement of order 2"). It alternates between edge 1–3 and edge 2–4.
 */
export const MP1_ORBIT = ["F123", "F124", "F134", "F234"] as const;
export const MP2_ORBIT = ["F134", "F234", "F123", "F124"] as const;

/** For orbit phase p (0..3) return the shared edge between MP1[p] and MP2[p] as 0-based vertex pair. */
export function sharedEdgeAt(phase: number): [number, number] {
  // (F123,F134)→1-3 ; (F124,F234)→2-4 ; (F134,F123)→1-3 ; (F234,F124)→2-4
  return phase % 2 === 0 ? [0, 2] : [1, 3];
}

export function faceById(id: string): TetraFace {
  return TETRA_FACES.find(f => f.id === id)!;
}

/* ============ shared ============ */

/** Uploaded reference images */
export const REF_NG2P3 = "/manus-storage/ng2p-3_64a15895.gif";
export const REF_S4_ENNEAD = "/manus-storage/S4-Ennead-4-Steps_29ffa1af.jpg";
