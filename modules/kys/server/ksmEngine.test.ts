import { describe, expect, it } from "vitest";
import {
  CANONICAL_WORD,
  UNIVERSAL_PRIMARY,
  UNIVERSAL_SECONDARY,
  generateRun,
  runToJsonl,
  verifyRun,
} from "./ksmEngine";

describe("canonical word", () => {
  it("has 12 steps with 7E + 5R", () => {
    expect(CANONICAL_WORD).toHaveLength(12);
    const e = CANONICAL_WORD.filter((w) => w.endsWith("E")).length;
    const r = CANONICAL_WORD.filter((w) => w.endsWith("R")).length;
    expect(e).toBe(7);
    expect(r).toBe(5);
  });

  it("matches the final word revision (mode word EEEEEEERRRRR)", () => {
    expect(CANONICAL_WORD.join(" ")).toBe(
      "T8E T5E T7E T1E T4E T2E T8E T5R T7R T1R T4R T2R",
    );
  });

  it("universal sequences are period-4 as specified", () => {
    expect(UNIVERSAL_PRIMARY).toEqual(["UT9E", "UT9E", "UT8R", "UT8R"]);
    expect(UNIVERSAL_SECONDARY).toEqual(["UT3_", "UT6_", "UT6_", "UT2R"]);
  });
});

describe("generateRun", () => {
  it("is deterministic", () => {
    const a = generateRun(36);
    const b = generateRun(36);
    expect(JSON.stringify(a)).toBe(JSON.stringify(b));
  });

  it("produces 36 ticks over 3 cycles", () => {
    const run = generateRun(36);
    expect(run).toHaveLength(36);
    expect(run[0].cycle).toBe(0);
    expect(run[35].cycle).toBe(2);
  });

  it("step triple at tick g is {g+1, g+5, g+9} mod 12", () => {
    const run = generateRun(36);
    for (const rec of run) {
      const expected = [0, 4, 8].map((o) => ((rec.tick + o) % 12) + 1);
      expect(rec.steps).toEqual(expected);
    }
  });

  it("thread k executes word[(g+4k)%12]", () => {
    const run = generateRun(36);
    for (const rec of run) {
      for (let k = 0; k < 3; k++) {
        expect(rec.triad[k]).toBe(CANONICAL_WORD[(rec.tick + 4 * k) % 12]);
      }
    }
  });

  it("mode balance never saturates (no 3E or 3R)", () => {
    const run = generateRun(36);
    for (const rec of run) {
      expect(["1E2R", "2E1R"]).toContain(rec.modeBalance);
    }
  });
});

describe("verifyRun — the six invariant gates", () => {
  const run = generateRun(36);
  const results = verifyRun(run);

  it("returns all six invariants", () => {
    expect(results.map((r) => r.invariantId)).toEqual([
      "INV-1",
      "INV-2",
      "INV-3",
      "INV-4",
      "INV-5",
      "INV-6",
    ]);
  });

  it("all gates pass with zero faults on the canonical run", () => {
    for (const r of results) {
      expect(r.faults).toBe(0);
      expect(r.ticksPassed).toBe(r.ticksChecked);
    }
  });

  it("INV-5 checks 9 thread-cycles (3 cycles × 3 threads)", () => {
    const inv5 = results.find((r) => r.invariantId === "INV-5")!;
    expect(inv5.ticksChecked).toBe(9);
  });

  it("detects a broken word (fixed-column bug) via INV-5", () => {
    // simulate the classic bug: thread k stuck on steps [4k..4k+3] only —
    // build a fake run where each thread repeats its own 4-step block
    const bad = generateRun(36).map((rec) => ({
      ...rec,
      triad: [0, 1, 2].map(
        (k) => CANONICAL_WORD[(4 * k + (rec.tick % 4)) % 12],
      ),
    }));
    const badResults = verifyRun(bad);
    const totalFaults = badResults.reduce((s, r) => s + r.faults, 0);
    expect(totalFaults).toBeGreaterThan(0);
  });
});

describe("runToJsonl", () => {
  it("serializes one record per line, parseable JSON", () => {
    const run = generateRun(12);
    const jsonl = runToJsonl(run);
    const lines = jsonl.trim().split("\n");
    expect(lines).toHaveLength(12);
    const first = JSON.parse(lines[0]);
    expect(first.tick).toBe(0);
    expect(first.triad).toEqual(["T8E", "T4E", "T7R"]);
    expect(first.u).toEqual(["UT9E", "UT3_"]);
  });
});
