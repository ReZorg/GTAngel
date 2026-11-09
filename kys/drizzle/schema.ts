import {
  int,
  json,
  mysqlEnum,
  mysqlTable,
  text,
  timestamp,
  varchar,
} from "drizzle-orm/mysql-core";

/**
 * Core user table backing auth flow.
 * Extend this file with additional tables as your product grows.
 * Columns use camelCase to match both database fields and generated types.
 */
export const users = mysqlTable("users", {
  /**
   * Surrogate primary key. Auto-incremented numeric value managed by the database.
   * Use this for relations between tables.
   */
  id: int("id").autoincrement().primaryKey(),
  /** Manus OAuth identifier (openId) returned from the OAuth callback. Unique per user. */
  openId: varchar("openId", { length: 64 }).notNull().unique(),
  name: text("name"),
  email: varchar("email", { length: 320 }),
  loginMethod: varchar("loginMethod", { length: 64 }),
  role: mysqlEnum("role", ["user", "admin"]).default("user").notNull(),
  createdAt: timestamp("createdAt").defaultNow().notNull(),
  updatedAt: timestamp("updatedAt").defaultNow().onUpdateNow().notNull(),
  lastSignedIn: timestamp("lastSignedIn").defaultNow().notNull(),
});

export type User = typeof users.$inferSelect;
export type InsertUser = typeof users.$inferInsert;

/* ------------------------------------------------------------------ */
/* KSM engine persistence                                              */
/* ------------------------------------------------------------------ */

/**
 * One row per engine tick of the validated corpus run.
 * The full 36-tick run (3 cycles of 12) is seeded from the canonical word.
 */
export const corpusTicks = mysqlTable("corpusTicks", {
  id: int("id").autoincrement().primaryKey(),
  /** global tick index 0..35 */
  tick: int("tick").notNull().unique(),
  /** cycle index 0..2 */
  cycle: int("cycle").notNull(),
  /** active step triple, e.g. [1,5,9] */
  steps: json("steps").$type<number[]>().notNull(),
  /** triad of Terms, e.g. ["T8E","T4E","T7R"] */
  triad: json("triad").$type<string[]>().notNull(),
  /** universal pair [U1, U2], e.g. ["UT9E","UT3_"] */
  u: json("u").$type<string[]>().notNull(),
  /** interface incidence state */
  interfaces: json("interfaces").$type<Record<string, string[]>>().notNull(),
  /** mode balance of the triad, e.g. "2E1R" */
  modeBalance: varchar("modeBalance", { length: 8 }).notNull(),
  createdAt: timestamp("createdAt").defaultNow().notNull(),
});

export type CorpusTick = typeof corpusTicks.$inferSelect;
export type InsertCorpusTick = typeof corpusTicks.$inferInsert;

/**
 * One row per invariant per verification run — the gate history.
 */
export const invariantChecks = mysqlTable("invariantChecks", {
  id: int("id").autoincrement().primaryKey(),
  /** e.g. "INV-1" */
  invariantId: varchar("invariantId", { length: 16 }).notNull(),
  name: varchar("name", { length: 128 }).notNull(),
  /** ticks checked in this run */
  ticksChecked: int("ticksChecked").notNull(),
  ticksPassed: int("ticksPassed").notNull(),
  faults: int("faults").notNull().default(0),
  /** human-readable result summary */
  summary: text("summary"),
  runLabel: varchar("runLabel", { length: 64 }).notNull().default("seed-36"),
  checkedAt: timestamp("checkedAt").defaultNow().notNull(),
});

export type InvariantCheck = typeof invariantChecks.$inferSelect;
export type InsertInvariantCheck = typeof invariantChecks.$inferInsert;

/**
 * Engine configuration — the canonical word and universal sequences,
 * versioned so word revisions are auditable.
 */
export const engineConfigs = mysqlTable("engineConfigs", {
  id: int("id").autoincrement().primaryKey(),
  version: varchar("version", { length: 32 }).notNull().unique(),
  /** the canonical 12-word */
  word: json("word").$type<string[]>().notNull(),
  u1: json("u1").$type<string[]>().notNull(),
  u2: json("u2").$type<string[]>().notNull(),
  active: int("active").notNull().default(0),
  note: text("note"),
  createdAt: timestamp("createdAt").defaultNow().notNull(),
});

export type EngineConfig = typeof engineConfigs.$inferSelect;
export type InsertEngineConfig = typeof engineConfigs.$inferInsert;

/**
 * Stored files — S3 references only (bytes live in storage).
 * Covers both generated exports (corpus JSONL) and user uploads.
 */
export const storedFiles = mysqlTable("storedFiles", {
  id: int("id").autoincrement().primaryKey(),
  fileKey: varchar("fileKey", { length: 512 }).notNull().unique(),
  url: text("url").notNull(),
  filename: varchar("filename", { length: 256 }).notNull(),
  mimeType: varchar("mimeType", { length: 128 }),
  sizeBytes: int("sizeBytes"),
  kind: mysqlEnum("kind", ["export", "upload"]).notNull().default("upload"),
  /** uploader user id (null for system-generated exports) */
  uploadedBy: int("uploadedBy"),
  createdAt: timestamp("createdAt").defaultNow().notNull(),
});

export type StoredFile = typeof storedFiles.$inferSelect;
export type InsertStoredFile = typeof storedFiles.$inferInsert;