/**
 * ksmEngine.ts — server-side deterministic KSM engine.
 * Regenerates the validated 36-tick corpus run (3 cycles of 12) from the
 * canonical word, plus the six invariant verifications. This is the same
 * logic as the client's engineData derivations — kept dependency-free so it
 * can seed the database and generate JSONL exports.
 */

export const CANONICAL_WORD = [
  "T8E",
  "T5E",
  "T7E",
  "T1E",
  "T4E",
  "T2E",
  "T8E",
  "T5R",
  "T7R",
  "T1R",
  "T4R",
  "T2R",
] as const;

export const ENGINE_VERSION = "word-v3-5x4window";

export const UNIVERSAL_PRIMARY = ["UT9E", "UT9E", "UT8R", "UT8R"] as const;
export const UNIVERSAL_SECONDARY = ["UT3_", "UT6_", "UT6_", "UT2R"] as const;

const THREAD_NAMES = ["Daemon_A", "Artifice_B", "Angel_C"] as const;

/** Term → incident interfaces (get_incident_interfaces in the engine) */
const TERM_INCIDENCE: Record<string, string[]> = {
  T1: ["C1_Electronic"],
  T2: ["C1_Electronic", "C2_Cellular"],
  T4: ["C2_Cellular"],
  T5: ["C2_Cellular", "C3_Routine"],
  T7: ["C3_Routine", "C4_Environment"],
  T8: ["C1_Electronic", "C4_Environment"],
  T9: ["C1_Electronic", "C4_Environment"],
  T3: [],
  T6: [],
};

const INTERFACE_KEYS = [
  "C1_Electronic",
  "C2_Cellular",
  "C3_Routine",
  "C4_Environment",
] as const;

export interface TickRecord {
  tick: number;
  cycle: number;
  steps: number[];
  triad: string[];
  u: string[];
  interfaces: Record<string, string[]>;
  modeBalance: string;
}

/** thread k at global tick g executes word[(g + 4k) % 12] */
export function generateRun(ticks = 36): TickRecord[] {
  const records: TickRecord[] = [];
  for (let g = 0; g < ticks; g++) {
    const steps = [0, 4, 8].map((o) => ((g + o) % 12) + 1);
    const triad = [0, 1, 2].map((k) => CANONICAL_WORD[(g + 4 * k) % 12]);
    const u = [UNIVERSAL_PRIMARY[g % 4], UNIVERSAL_SECONDARY[g % 4]];

    const interfaces: Record<string, string[]> = {};
    for (const key of INTERFACE_KEYS) interfaces[key] = [];

    // triad terms in Expressive mode wire their incident interfaces
    triad.forEach((termMode, k) => {
      const term = termMode.slice(0, 2);
      const mode = termMode.slice(2);
      if (mode === "E") {
        for (const iface of TERM_INCIDENCE[term] ?? []) {
          interfaces[iface].push(`${THREAD_NAMES[k]}_${termMode}`);
        }
      }
    });
    // U1 policy injection (Expressive/Regenerative both witnessed at policy level)
    const u1 = u[0];
    const u1Term = u1.slice(1, 3); // "T9"/"T8"
    for (const iface of TERM_INCIDENCE[u1Term] ?? []) {
      interfaces[iface].push(`U1_Policy_${u1}`);
    }

    const e = triad.filter((t) => t.endsWith("E")).length;
    const r = 3 - e;
    records.push({
      tick: g,
      cycle: Math.floor(g / 12),
      steps,
      triad,
      u,
      interfaces,
      modeBalance: `${e}E${r}R`,
    });
  }
  return records;
}

export interface InvariantResult {
  invariantId: string;
  name: string;
  ticksChecked: number;
  ticksPassed: number;
  faults: number;
  summary: string;
}

/** verify_phase_lock.py — the six invariant gates, re-run server-side */
export function verifyRun(records: TickRecord[]): InvariantResult[] {
  const n = records.length;
  const results: InvariantResult[] = [];

  // INV-1 phase lock
  let pass1 = 0;
  for (const rec of records) {
    const expected = [0, 4, 8].map((o) => ((rec.tick + o) % 12) + 1);
    if (
      JSON.stringify([...rec.steps].sort((a, b) => a - b)) ===
      JSON.stringify([...expected].sort((a, b) => a - b))
    )
      pass1++;
  }
  results.push({
    invariantId: "INV-1",
    name: "Phase lock",
    ticksChecked: n,
    ticksPassed: pass1,
    faults: n - pass1,
    summary: `steps ≡ {g+1,g+5,g+9} mod 12 · ${pass1}/${n} pass`,
  });

  // INV-2 polar-pair coverage — one of each pair [1(8)],[4(5)],[7(2)]
  const pairOf: Record<string, string> = {
    T1: "P18",
    T8: "P18",
    T4: "P45",
    T5: "P45",
    T7: "P72",
    T2: "P72",
  };
  let pass2 = 0;
  for (const rec of records) {
    const pairs = new Set(rec.triad.map((t) => pairOf[t.slice(0, 2)]));
    if (pairs.size === 3) pass2++;
  }
  results.push({
    invariantId: "INV-2",
    name: "Polar-pair coverage",
    ticksChecked: n,
    ticksPassed: pass2,
    faults: n - pass2,
    summary: `one term from each polar pair every tick · ${pass2}/${n} pass`,
  });

  // INV-3 alternating triangles — odd steps {4,8,7}, even {1,2,5}
  let pass3 = 0;
  for (const rec of records) {
    const stepOdd = rec.steps[0] % 2 === 1;
    const terms = new Set(rec.triad.map((t) => parseInt(t.slice(1, 2), 10)));
    const expected = stepOdd ? new Set([4, 8, 7]) : new Set([1, 2, 5]);
    if (
      terms.size === expected.size &&
      Array.from(terms).every((t) => expected.has(t))
    )
      pass3++;
  }
  results.push({
    invariantId: "INV-3",
    name: "Alternating triangles",
    ticksChecked: n,
    ticksPassed: pass3,
    faults: n - pass3,
    summary: `odd:Structure{4,8,7} even:Process{1,2,5} · ${pass3}/${n} pass`,
  });

  // INV-4 mode balance — never 3E or 3R
  let pass4 = 0;
  for (const rec of records) {
    const e = rec.triad.filter((t) => t.endsWith("E")).length;
    if (e === 1 || e === 2) pass4++;
  }
  results.push({
    invariantId: "INV-4",
    name: "Mode balance",
    ticksChecked: n,
    ticksPassed: pass4,
    faults: n - pass4,
    summary: `never 3E/3R · ${pass4}/${n} pass`,
  });

  // INV-5 per-thread 7E/5R budget per full cycle
  const cycles = Math.floor(n / 12);
  let budgetChecks = 0;
  let budgetPass = 0;
  for (let c = 0; c < cycles; c++) {
    for (let k = 0; k < 3; k++) {
      budgetChecks++;
      let e = 0;
      for (let g = c * 12; g < (c + 1) * 12; g++) {
        // check the actual persisted record, not the theoretical word
        if (records[g].triad[k].endsWith("E")) e++;
      }
      if (e === 7) budgetPass++;
    }
  }
  results.push({
    invariantId: "INV-5",
    name: "7E/5R budget",
    ticksChecked: budgetChecks,
    ticksPassed: budgetPass,
    faults: budgetChecks - budgetPass,
    summary: `each thread 7E+5R per cycle · ${budgetPass}/${budgetChecks} thread-cycles pass`,
  });

  // INV-6 T8/T9 coherence present every tick (triad or U policy)
  let pass6 = 0;
  for (const rec of records) {
    const inTriad = rec.triad.some((t) => t.startsWith("T8"));
    const inU = rec.u.some((t) => t.includes("T8") || t.includes("T9"));
    if (inTriad || inU) pass6++;
  }
  results.push({
    invariantId: "INV-6",
    name: "T8 coherence",
    ticksChecked: n,
    ticksPassed: pass6,
    faults: n - pass6,
    summary: `coherence floor (triad T8 or U1/U2 T8/T9) · ${pass6}/${n} pass`,
  });

  return results;
}

/** Serialize a run to JSONL, one record per line (matches ksm_corpus.jsonl) */
export function runToJsonl(records: TickRecord[]): string {
  return (
    records
      .map((r) =>
        JSON.stringify({
          tick: r.tick,
          steps: r.steps,
          triad: r.triad,
          u: r.u,
          interfaces: r.interfaces,
        }),
      )
      .join("\n") + "\n"
  );
}
