# Full-Stack Upgrade State (2026-07-21)

## Done
- Upgraded to web-db-user (tRPC + MySQL + auth + S3 storage). Auto-publish ON.
- Schema (drizzle/schema.ts): corpusTicks, invariantChecks, engineConfigs, storedFiles (+ users). Migration 0000_mighty_lady_mastermind.sql applied; tables verified via SHOW TABLES.
- server/ksmEngine.ts: deterministic engine — generateRun(36), verifyRun (6 invariants), runToJsonl. Word: T8E T5E T7E T1E T4E T2E | T8E T5R T7R T1R T4R T2R. U1: UT9E UT9E UT8R UT8R; U2: UT3_ UT6_ UT6_ UT2R. ENGINE_VERSION="word-v3-5x4window".
- server/db.ts: KSM query helpers (count/insert/clear/list ticks, invariant checks, engine config upsert/get, storedFiles CRUD).
- server/routers/ksm.ts: ksm router registered in routers.ts — config, ticks, invariants, seed, exportCorpus, files.list/upload(protected, ≤2MB base64)/remove(owner or admin).
- Seed tested via curl: 36 ticks, 6 invariants, allPass=true. INV results: INV-1..4,6 = 36/36, INV-5 = 9/9 thread-cycles. Export tested: 36 records, 8657 bytes, downloadable via /manus-storage/... (verified curl -L returns 36 lines).
- client/src/components/DataVault.tsx: section 07 UI — DB status panel (ticks persisted, invariant rows, gate status, word version, re-seed button) + vault panel (export JSONL, upload artifact w/ login, file table with get/del).
- Home.tsx: DataVault imported, section 07 "THE VAULT" inserted before footer, VAULT nav link added.
- TS clean (0 errors).

## Remaining (phase 4)
- Write vitest tests (server/ksm.test.ts) — engine determinism + invariants + router with mock ctx.
- Screenshot verify vault section renders.
- Update todo.md remaining items ([ ] Wire CorpusStream/gates to DB — decided: keep static engineData for animation, DataVault shows DB truth; mark as done via DataVault approach or leave noted).
- Checkpoint + deliver (auto-publish).

## Notes
- dotenv error in devserver.log is stale (pre-pnpm-install, 05:19:52). Server running fine.
- storagePut appends hash suffix to keys automatically.
- Corpus seed rows ids start at 37 due to earlier insert then clear+reinsert — fine.
