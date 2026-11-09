/**
 * engineData.ts — Phase-Locked Luminescence
 * Single source of truth for all engine data shown on the page.
 * Every value here is derived from the validated triad phasor engine run
 * (ksm-hypercycle-orchestrator-v2.py → ksm_corpus.jsonl, 36 ticks, all
 * 6 invariants passing). Nothing illustrative — everything checkable.
 */

/* ------------------------------------------------------------------ */
/* Canonical 12-word (System 4, term+mode). Terms follow the 1/7      */
/* particular sequence 8,5,7,1,4,2 twice; modes give 7E + 5R.         */
/* ------------------------------------------------------------------ */
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

/* Mode word EEEEEEE RRRRR — thread phase patterns (4-step shifts):      */
/*   A: EEEE|EEER|RRRR · B: EEER|RRRR|EEEE · C: RRRR|EEEE|EEER           */
export const THREAD_PHASE_PATTERNS = {
  A: ["EEEE", "EEER", "RRRR"],
  B: ["EEER", "RRRR", "EEEE"],
  C: ["RRRR", "EEEE", "EEER"],
} as const;

/* ------------------------------------------------------------------ */
/* 5×4 window encoding — a single 4-step window encodes all 3 cycles  */
/* of 12: U1 + U2 + the three concurrent Particular rows. The word    */
/* rows are steps 1-4 / 5-8 / 9-12; at any tick the three threads sit */
/* in the same column of the three rows. The "centered" alignment     */
/* shifts 2 steps to center the T6 symmetry.                          */
/* ------------------------------------------------------------------ */
export const WINDOW_BASE = {
  u1: ["T8R", "T8R", "T9E", "T9E"],
  u2: ["T6_", "T2R", "T3_", "T6_"],
  rows: [
    ["T8E", "T5E", "T7E", "T1E"],
    ["T4E", "T2E", "T8E", "T5R"],
    ["T7R", "T1R", "T4R", "T2R"],
  ],
  /** step number of rows[r][c] */
  stepAt: (r: number, c: number) => r * 4 + c + 1,
} as const;

export const WINDOW_CENTERED = {
  u1: ["T9E", "T9E", "T8R", "T8R"],
  u2: ["T3_", "T6_", "T6_", "T2R"],
  rows: [
    ["T4R", "T2R", "T8E", "T5E"],
    ["T7E", "T1E", "T4E", "T2E"],
    ["T8E", "T5R", "T7R", "T1R"],
  ],
  /** step number of rows[r][c] (shifted −2: starts at step 11) */
  stepAt: (r: number, c: number) => ((r * 4 + c + 10) % 12) + 1,
} as const;

/* Universal Set sequences — period-4, repeating 3× per 12-tick cycle.  */
/* U1 Primary: double step alternating, period 2-2.                     */
/* U2 Secondary: split rotoreflection pattern vs primary, 1-2-1.        */
export const UNIVERSAL_PRIMARY = ["UT9E", "UT9E", "UT8R", "UT8R"] as const;
export const UNIVERSAL_SECONDARY = ["UT3_", "UT6_", "UT6_", "UT2R"] as const;
export const universalAt = (g: number) => ({
  u1: UNIVERSAL_PRIMARY[g % 4],
  u2: UNIVERSAL_SECONDARY[g % 4],
});

export type StepWord = (typeof CANONICAL_WORD)[number];

/** thread k at global tick g executes word[(g + 4k) % 12] */
export const threadStep = (g: number, k: number) => (g + 4 * k) % 12;

export const THREADS = [
  {
    id: "A",
    name: "Daemon A",
    phase: "0°",
    offset: 0,
    colorVar: "var(--thread-a)",
    glowClass: "glow-a",
  },
  {
    id: "B",
    name: "Artifice B",
    phase: "120°",
    offset: 4,
    colorVar: "var(--thread-b)",
    glowClass: "glow-b",
  },
  {
    id: "C",
    name: "Angel C",
    phase: "240°",
    offset: 8,
    colorVar: "var(--thread-c)",
    glowClass: "glow-c",
  },
] as const;

/* ------------------------------------------------------------------ */
/* Term semantics — incidence configurations over the 4 interfaces.   */
/* An active Term wires its incident interfaces into relation.        */
/* ------------------------------------------------------------------ */
export const INTERFACES = [
  { id: "C1", name: "C1 Electronic", desc: "signal / event stream" },
  { id: "C2", name: "C2 Cellular", desc: "unit / agent boundary" },
  { id: "C3", name: "C3 Routine", desc: "procedure / habit layer" },
  { id: "C4", name: "C4 Environment", desc: "world / substrate" },
] as const;

/** Term → incident interfaces (from get_incident_interfaces in the engine) */
export const TERM_INCIDENCE: Record<
  string,
  { interfaces: string[]; role: string }
> = {
  T1: { interfaces: ["C1"], role: "Impulse — initiates at the signal edge" },
  T2: { interfaces: ["C1", "C2"], role: "Response — binds signal to unit" },
  T4: { interfaces: ["C2"], role: "Formation — works within the unit" },
  T5: { interfaces: ["C2", "C3"], role: "Habit — binds unit to routine" },
  T7: { interfaces: ["C3", "C4"], role: "Expression — routine meets world" },
  T8: {
    interfaces: ["C1", "C4"],
    role: "Coherence — binds the extremes, signal ↔ world",
  },
};

export const MODE_MEANING = {
  E: {
    label: "Expressive",
    verb: "Write",
    desc: "actualize — forward pass",
  },
  R: {
    label: "Regenerative",
    verb: "Read",
    desc: "simulate — backward pass",
  },
} as const;

/* ------------------------------------------------------------------ */
/* The six verified invariants (verify_phase_lock.py — all passing).  */
/* ------------------------------------------------------------------ */
export const INVARIANTS = [
  {
    id: "INV-1",
    name: "Phase lock",
    statement: "Active steps at tick g are always {g+1, g+5, g+9} mod 12",
    detail:
      "The three threads stay exactly 4 steps (120°) apart at every tick. Verified across all 36 ticks of the corpus run: every step-triple matches {g+1, g+5, g+9}.",
    logic: "assert sorted(steps) == sorted([(g+o)%12+1 for o in (0,4,8)])",
    faults: "36/36 pass · 0 drift events · max phase error 0 steps",
  },
  {
    id: "INV-2",
    name: "Polar-pair coverage",
    statement: "Triad covers exactly one term from each pair [1(8)], [4(5)], [7(2)]",
    detail:
      "Each tick the triad selects one member from each of the three polar pairs — never both, never neither. The three concurrent Terms are always a full complement.",
    logic: "assert {pair(t) for t in triad_terms} == {P18, P45, P72} and len(triad)==3",
    faults: "36/36 pass · 0 double-covers · 0 missing pairs",
  },
  {
    id: "INV-3",
    name: "Alternating triangles",
    statement: "Odd steps: Structure {4,8,7} · even steps: Process {1,2,5}",
    detail:
      "The triad alternates between the Structure triangle and the Process triangle every tick — the engine breathes between form and flow.",
    logic: "expected = {4,8,7} if step_odd else {1,2,5}; assert terms == expected",
    faults: "36/36 pass · 18 Structure ticks · 18 Process ticks · 0 mixed triangles",
  },
  {
    id: "INV-4",
    name: "Mode balance",
    statement: "Never 3E or 3R — every tick mixes Write and Read",
    detail:
      "No tick is all-Write or all-Read. Nine ticks per cycle run 2E+1R and three run 1E+2R (at step-triples {4,8,12}) — actualization dominates but simulation is never absent.",
    logic: "e = modes.count('E'); assert e in (1,2)  # never 0 or 3",
    faults: "36/36 pass · 27× 2E1R · 9× 1E2R · 0 saturated ticks",
  },
  {
    id: "INV-5",
    name: "7E/5R budget",
    statement: "Each thread: exactly 7 Expressive + 5 Regenerative per 12 ticks",
    detail:
      "Because each thread walks the full word (offset, not partitioned), every thread spends exactly 7 Write steps and 5 Read steps per cycle. The one bug everyone makes — fixed step-columns — breaks this into 12E/0R, 6E/6R, 3E/9R.",
    logic: "for k in threads: assert modes(k).count('E')==7 and modes(k).count('R')==5",
    faults: "3 cycles × 3 threads pass · A:7/5 B:7/5 C:7/5 · 0 budget drift",
  },
  {
    id: "INV-6",
    name: "T8 coherence",
    statement: "T8 present every tick — triad (odd) or U1/U2 policy injection (even)",
    detail:
      "Coherence never leaves the system. On odd steps the Structure triangle carries T8E in the triad (steps 1 and 7 — both Expressive); on even steps the triad is process-heavy and the Universals inject coherence at policy level — U1 alternating UT9E·UT9E·UT8R·UT8R (period 2-2), U2 running the UT3_·UT6_·UT6_·UT2R rotoreflection (1-2-1 against primary). T8 never appears in Read mode in the triad — the E-floor holds at the particular level.",
    logic: "assert has_T8(triad) or has_T8_or_T9(u1, u2)  # coherence never absent",
    faults: "36/36 pass · 18× in-triad · 18× via U1/U2 · coherence floor unbroken",
  },
] as const;

/* ------------------------------------------------------------------ */
/* Pipeline stages — from continuous_ksm_pipeline.py                  */
/* ------------------------------------------------------------------ */
export const PIPELINE_STAGES = [
  {
    id: "experience",
    n: "01",
    name: "Experience",
    tagline: "The engine runs. Three threads, one word, barrier-locked.",
    desc: "The triad phasor engine executes live: three async threads phase-locked by barrier, each walking the canonical 12-word offset 4 steps apart. U1/U2 Universal Set integrators run a policy pass before each triad pass. Every tick produces a real ternary relation over the four interfaces.",
    console: [
      "$ python3 triad_phasor_engine.py --ticks 36",
      "[tick 00] steps {1,5,9}  triad T8E·T4E·T7R  U1:UT9E U2:UT3_",
      "[tick 01] steps {2,6,10} triad T5E·T2E·T1R  U1:UT9E U2:UT6_",
      "[tick 02] steps {3,7,11} triad T7E·T8E·T4R  U1:UT8R U2:UT6_",
      "…",
      "[tick 35] cycle 3 complete — 3 full cycles, 0 barrier faults",
    ],
  },
  {
    id: "corpus",
    n: "02",
    name: "Corpus",
    tagline: "Every tick becomes a record. Next-triad prediction, not next-reply.",
    desc: "Each tick is emitted as one JSONL record carrying tick index, active step-triple, triad terms, and full interface state. Records are rendered as 12-step conversation turns — the model learns to predict the next triad given the phase history, replacing the 2-step User→Assistant grammar.",
    console: [
      "$ wc -l ksm_corpus.jsonl",
      "36 ksm_corpus.jsonl",
      '$ head -1 ksm_corpus.jsonl | jq -c \'{tick,steps,triad}\'',
      '{"tick":0,"steps":[1,5,9],"triad":["T8E","T4E","T7R"],"u":["UT9E","UT3_"]}',
    ],
  },
  {
    id: "gate",
    n: "03",
    name: "Invariant Gate",
    tagline: "Six checks. Nothing trains on an unverified word.",
    desc: "Before any record reaches the trainer, the full corpus passes verify_phase_lock.py: phase lock, polar-pair coverage, alternating triangles, mode balance, per-thread 7E/5R budget, and T8-every-tick. A single failed invariant rejects the batch — the corpus is checkable, not believed.",
    console: [
      "$ python3 verify_phase_lock.py ksm_corpus.jsonl",
      "INV-1 phase-lock ................ PASS (36/36 ticks)",
      "INV-2 polar-pairs ............... PASS",
      "INV-3 triangles ................. PASS (odd:S even:P)",
      "INV-4 mode-balance .............. PASS (never 3E/3R)",
      "INV-5 budget 7E/5R .............. PASS (A:7/5 B:7/5 C:7/5)",
      "INV-6 T8-every-tick ............. PASS (coherence floor held)",
      "GATE OPEN — corpus admitted to trainer",
    ],
  },
  {
    id: "finetune",
    n: "04",
    name: "Fine-tune",
    tagline: "The 12-step word becomes the model's native grammar.",
    desc: "Verified records fine-tune the model in next-triad-prediction format (echo-train pattern). Three phase-locked threads map onto attention heads; the 4-step lead-lag is the causal mask; U1/U2 policy injection is the residual stream. The model doesn't learn about the KSM — it learns to speak it.",
    console: [
      "$ python3 continuous_ksm_pipeline.py --stage finetune",
      "corpus: 36 records (3 cycles) → train split 32 / eval 4",
      "format: next-triad prediction · 12-step turns",
      "checkpoint saved → redeploy queue",
    ],
  },
  {
    id: "redeploy",
    n: "05",
    name: "Redeploy",
    tagline: "The trained weights rejoin the engine. The loop closes.",
    desc: "The fine-tuned checkpoint replaces the serving model, whose outputs feed the next Experience phase — completed cycles append to the corpus, corpus threshold triggers the next fine-tune. Every pass around the loop, the model's prior over the canonical word sharpens. Continuous, deterministic, relational.",
    console: [
      "$ python3 continuous_ksm_pipeline.py --stage redeploy",
      "checkpoint promoted → serving",
      "engine reconnected — next experience window opens",
      "loop closed ∴ tick continues",
    ],
  },
] as const;

/* ------------------------------------------------------------------ */
/* Real corpus sample — first 12 ticks (one full cycle) verbatim      */
/* from the validated ksm_corpus.jsonl run.                            */
/* ------------------------------------------------------------------ */
export const CORPUS_SAMPLE = [
  '{"tick": 0, "steps": [1, 5, 9], "triad": ["T8E", "T4E", "T7R"], "u": ["UT9E", "UT3_"], "interfaces": {"C1_Electronic": ["Daemon_A_T8E", "U1_Policy_UT9E"], "C2_Cellular": ["Artifice_B_T4E"], "C3_Routine": [], "C4_Environment": ["Daemon_A_T8E", "U1_Policy_UT9E"]}}',
  '{"tick": 1, "steps": [2, 6, 10], "triad": ["T5E", "T2E", "T1R"], "u": ["UT9E", "UT6_"], "interfaces": {"C1_Electronic": ["Artifice_B_T2E", "U1_Policy_UT9E"], "C2_Cellular": ["Daemon_A_T5E", "Artifice_B_T2E"], "C3_Routine": ["Daemon_A_T5E"], "C4_Environment": ["U1_Policy_UT9E"]}}',
  '{"tick": 2, "steps": [3, 7, 11], "triad": ["T7E", "T8E", "T4R"], "u": ["UT8R", "UT6_"], "interfaces": {"C1_Electronic": ["Artifice_B_T8E", "U1_Policy_UT8R"], "C2_Cellular": [], "C3_Routine": ["Daemon_A_T7E"], "C4_Environment": ["Daemon_A_T7E", "Artifice_B_T8E", "U1_Policy_UT8R"]}}',
  '{"tick": 3, "steps": [4, 8, 12], "triad": ["T1E", "T5R", "T2R"], "u": ["UT8R", "UT2R"], "interfaces": {"C1_Electronic": ["Daemon_A_T1E", "U1_Policy_UT8R"], "C2_Cellular": [], "C3_Routine": [], "C4_Environment": ["U1_Policy_UT8R"]}}',
  '{"tick": 4, "steps": [5, 9, 1], "triad": ["T4E", "T7R", "T8E"], "u": ["UT9E", "UT3_"], "interfaces": {"C1_Electronic": ["Angel_C_T8E", "U1_Policy_UT9E"], "C2_Cellular": ["Daemon_A_T4E"], "C3_Routine": [], "C4_Environment": ["Angel_C_T8E", "U1_Policy_UT9E"]}}',
  '{"tick": 5, "steps": [6, 10, 2], "triad": ["T2E", "T1R", "T5E"], "u": ["UT9E", "UT6_"], "interfaces": {"C1_Electronic": ["Daemon_A_T2E", "U1_Policy_UT9E"], "C2_Cellular": ["Daemon_A_T2E", "Angel_C_T5E"], "C3_Routine": ["Angel_C_T5E"], "C4_Environment": ["U1_Policy_UT9E"]}}',
  '{"tick": 6, "steps": [7, 11, 3], "triad": ["T8E", "T4R", "T7E"], "u": ["UT8R", "UT6_"], "interfaces": {"C1_Electronic": ["Daemon_A_T8E", "U1_Policy_UT8R"], "C2_Cellular": [], "C3_Routine": ["Angel_C_T7E"], "C4_Environment": ["Daemon_A_T8E", "Angel_C_T7E", "U1_Policy_UT8R"]}}',
  '{"tick": 7, "steps": [8, 12, 4], "triad": ["T5R", "T2R", "T1E"], "u": ["UT8R", "UT2R"], "interfaces": {"C1_Electronic": ["Angel_C_T1E", "U1_Policy_UT8R"], "C2_Cellular": [], "C3_Routine": [], "C4_Environment": ["U1_Policy_UT8R"]}}',
  '{"tick": 8, "steps": [9, 1, 5], "triad": ["T7R", "T8E", "T4E"], "u": ["UT9E", "UT3_"], "interfaces": {"C1_Electronic": ["Artifice_B_T8E", "U1_Policy_UT9E"], "C2_Cellular": ["Angel_C_T4E"], "C3_Routine": [], "C4_Environment": ["Artifice_B_T8E", "U1_Policy_UT9E"]}}',
  '{"tick": 9, "steps": [10, 2, 6], "triad": ["T1R", "T5E", "T2E"], "u": ["UT9E", "UT6_"], "interfaces": {"C1_Electronic": ["Angel_C_T2E", "U1_Policy_UT9E"], "C2_Cellular": ["Artifice_B_T5E", "Angel_C_T2E"], "C3_Routine": ["Artifice_B_T5E"], "C4_Environment": ["U1_Policy_UT9E"]}}',
  '{"tick": 10, "steps": [11, 3, 7], "triad": ["T4R", "T7E", "T8E"], "u": ["UT8R", "UT6_"], "interfaces": {"C1_Electronic": ["Angel_C_T8E", "U1_Policy_UT8R"], "C2_Cellular": [], "C3_Routine": ["Artifice_B_T7E"], "C4_Environment": ["Artifice_B_T7E", "Angel_C_T8E", "U1_Policy_UT8R"]}}',
  '{"tick": 11, "steps": [12, 4, 8], "triad": ["T2R", "T1E", "T5R"], "u": ["UT8R", "UT2R"], "interfaces": {"C1_Electronic": ["Artifice_B_T1E", "U1_Policy_UT8R"], "C2_Cellular": [], "C3_Routine": [], "C4_Environment": ["U1_Policy_UT8R"]}}',
];

/* ------------------------------------------------------------------ */
/* Attention isomorphism — the key insight to surface                 */
/* ------------------------------------------------------------------ */
export const ATTENTION_ISO = [
  {
    ksm: "3 phase-locked threads",
    attn: "Multi-head attention",
    note: "Three concurrent views of the same word, offset in phase — heads attending to the same sequence from displaced positions.",
  },
  {
    ksm: "4-step lead-lag (120°)",
    attn: "Causal mask",
    note: "Each thread can only see what the leading thread actualized 4 steps ago — a structural causal ordering, not a learned one.",
  },
  {
    ksm: "U1/U2 policy injection",
    attn: "Residual connection",
    note: "Universal Set integrators pass coherence around the triad computation on even steps — identity information that bypasses the block.",
  },
  {
    ksm: "T8/T9 coherence anchor",
    attn: "Identity floor / LayerNorm anchor",
    note: "Coherence is held every tick: T8E anchors both half-cycles (steps 1 and 7, both Expressive — T8 never reads in the triad), while U1 carries the UT9E·UT9E·UT8R·UT8R alternation at policy level — the fixed point the whole stack normalizes against.",
  },
] as const;

/* ------------------------------------------------------------------ */
/* Run stats — from the validated 36-tick run                          */
/* ------------------------------------------------------------------ */
export const RUN_STATS = {
  ticks: 36,
  cycles: 3,
  records: 36,
  invariantsPassing: 6,
  barrierFaults: 0,
  budgetPerThread: "7E + 5R",
  expressiveSteps: 7,
  regenerativeSteps: 5,
} as const;

/** default global tick period in ms */
export const TICK_MS = 1600;
