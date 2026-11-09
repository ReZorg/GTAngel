/**
 * ksm.ts — feature router for the KSM engine persistence + file storage.
 * - seed: deterministic 36-tick run → corpusTicks + invariantChecks + engineConfigs
 * - ticks / invariants / config: read the persisted run
 * - exportCorpus: generate ksm_corpus.jsonl → S3 storage → storedFiles row
 * - files: list / record uploads / delete references
 */
import { z } from "zod";
import { nanoid } from "nanoid";
import { TRPCError } from "@trpc/server";
import { publicProcedure, protectedProcedure, router } from "../_core/trpc";
import * as db from "../db";
import {
  CANONICAL_WORD,
  ENGINE_VERSION,
  UNIVERSAL_PRIMARY,
  UNIVERSAL_SECONDARY,
  generateRun,
  runToJsonl,
  verifyRun,
} from "../ksmEngine";
import {
  generateClockTowerRun,
  verifySylvesterRun,
} from "../sylvesterEngine";
import { storagePut } from "../storage";

const RUN_LABEL = "seed-36";

/** Idempotent: (re)seed the database from the deterministic engine */
async function seedDatabase() {
  const records = generateRun(36);
  const results = verifyRun(records);

  await db.clearCorpusTicks();
  await db.insertCorpusTicks(
    records.map((r) => ({
      tick: r.tick,
      cycle: r.cycle,
      steps: r.steps,
      triad: r.triad,
      u: r.u,
      interfaces: r.interfaces,
      modeBalance: r.modeBalance,
    })),
  );

  await db.clearInvariantChecks(RUN_LABEL);
  await db.insertInvariantChecks(
    results.map((r) => ({ ...r, runLabel: RUN_LABEL })),
  );

  await db.upsertEngineConfig({
    version: ENGINE_VERSION,
    word: [...CANONICAL_WORD],
    u1: [...UNIVERSAL_PRIMARY],
    u2: [...UNIVERSAL_SECONDARY],
    note: "Canonical 12-word with 5×4 window encoding · seeded from deterministic engine",
  });

  return {
    ticks: records.length,
    invariants: results.length,
    allPass: results.every((r) => r.faults === 0),
  };
}

const SYLVESTER_RUN_CACHE = generateClockTowerRun(36);
const SYLVESTER_INVARIANTS_CACHE = verifySylvesterRun(SYLVESTER_RUN_CACHE);

export const ksmRouter = router({
  /** Persisted engine config (canonical word + universal sequences) */
  config: publicProcedure.query(async () => {
    const cfg = await db.getActiveEngineConfig();
    return cfg ?? null;
  }),

  /** The persisted 36-tick corpus run */
  ticks: publicProcedure
    .input(z.object({ limit: z.number().int().min(1).max(36).optional() }).optional())
    .query(async ({ input }) => {
      return db.listCorpusTicks(input?.limit);
    }),

  /** Latest invariant gate results */
  invariants: publicProcedure.query(async () => {
    return db.listInvariantChecks();
  }),

  /** Sylvester Nonion Frame-Shifting Clock-Tower 36-tick run */
  sylvesterRun: publicProcedure.query(async () => {
    return SYLVESTER_RUN_CACHE;
  }),

  /** Sylvester Nonion & Dual Ternary-Quinary Lie Algebra Invariant Checks */
  sylvesterInvariants: publicProcedure.query(async () => {
    return SYLVESTER_INVARIANTS_CACHE;
  }),

  /** Seed / re-seed the database from the deterministic engine (owner action) */
  seed: publicProcedure.mutation(async () => {
    return seedDatabase();
  }),

  /** Generate ksm_corpus.jsonl from the persisted run and store it in S3 */
  exportCorpus: publicProcedure.mutation(async () => {
    let rows = await db.listCorpusTicks();
    if (rows.length === 0) {
      await seedDatabase();
      rows = await db.listCorpusTicks();
    }
    const records = rows.map((r) => ({
      tick: r.tick,
      cycle: r.cycle,
      steps: r.steps,
      triad: r.triad,
      u: r.u,
      interfaces: r.interfaces,
      modeBalance: r.modeBalance,
    }));
    const jsonl =
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
        .join("\n") + "\n";

    const stamp = new Date().toISOString().replace(/[:.]/g, "-");
    const fileKey = `exports/ksm_corpus_${stamp}_${nanoid(6)}.jsonl`;
    const { key, url } = await storagePut(
      fileKey,
      jsonl,
      "application/jsonl",
    );

    await db.insertStoredFile({
      fileKey: key,
      url,
      filename: "ksm_corpus.jsonl",
      mimeType: "application/jsonl",
      sizeBytes: Buffer.byteLength(jsonl),
      kind: "export",
    });

    return { key, url, records: records.length, bytes: Buffer.byteLength(jsonl) };
  }),

  /** Files stored in S3 (exports + uploads) */
  files: router({
    list: publicProcedure.query(async () => {
      return db.listStoredFiles();
    }),

    /** Upload a small artifact file (base64 payload ≤ 2 MB) */
    upload: protectedProcedure
      .input(
        z.object({
          filename: z.string().min(1).max(200),
          mimeType: z.string().min(1).max(120),
          dataBase64: z.string().min(1),
        }),
      )
      .mutation(async ({ input, ctx }) => {
        const buf = Buffer.from(input.dataBase64, "base64");
        if (buf.length > 2 * 1024 * 1024) {
          throw new TRPCError({
            code: "PAYLOAD_TOO_LARGE",
            message: "File exceeds the 2 MB upload limit",
          });
        }
        const safeName = input.filename.replace(/[^\w.\-]+/g, "_");
        const fileKey = `uploads/${ctx.user.id}/${nanoid(8)}_${safeName}`;
        const { key, url } = await storagePut(fileKey, buf, input.mimeType);
        await db.insertStoredFile({
          fileKey: key,
          url,
          filename: input.filename,
          mimeType: input.mimeType,
          sizeBytes: buf.length,
          kind: "upload",
          uploadedBy: ctx.user.id,
        });
        return { key, url };
      }),

    /** Remove a file reference (bytes become unreachable, effectively deleted) */
    remove: protectedProcedure
      .input(z.object({ id: z.number().int() }))
      .mutation(async ({ input, ctx }) => {
        const all = await db.listStoredFiles();
        const target = all.find((f) => f.id === input.id);
        if (!target) {
          throw new TRPCError({ code: "NOT_FOUND", message: "File not found" });
        }
        const isOwner = target.uploadedBy === ctx.user.id;
        const isAdmin = ctx.user.role === "admin";
        if (!isOwner && !isAdmin) {
          throw new TRPCError({ code: "FORBIDDEN", message: "Not your file" });
        }
        await db.deleteStoredFile(input.id);
        return { success: true } as const;
      }),
  }),
});
