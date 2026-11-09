import { asc, desc, eq } from "drizzle-orm";
import { drizzle } from "drizzle-orm/mysql2";
import {
  corpusTicks,
  engineConfigs,
  InsertCorpusTick,
  InsertEngineConfig,
  InsertInvariantCheck,
  InsertStoredFile,
  InsertUser,
  invariantChecks,
  storedFiles,
  users,
} from "../drizzle/schema";
import { ENV } from './_core/env';

let _db: ReturnType<typeof drizzle> | null = null;

// Lazily create the drizzle instance so local tooling can run without a DB.
export async function getDb() {
  if (!_db && process.env.DATABASE_URL) {
    try {
      _db = drizzle(process.env.DATABASE_URL);
    } catch (error) {
      console.warn("[Database] Failed to connect:", error);
      _db = null;
    }
  }
  return _db;
}

export async function upsertUser(user: InsertUser): Promise<void> {
  if (!user.openId) {
    throw new Error("User openId is required for upsert");
  }

  const db = await getDb();
  if (!db) {
    console.warn("[Database] Cannot upsert user: database not available");
    return;
  }

  try {
    const values: InsertUser = {
      openId: user.openId,
    };
    const updateSet: Record<string, unknown> = {};

    const textFields = ["name", "email", "loginMethod"] as const;
    type TextField = (typeof textFields)[number];

    const assignNullable = (field: TextField) => {
      const value = user[field];
      if (value === undefined) return;
      const normalized = value ?? null;
      values[field] = normalized;
      updateSet[field] = normalized;
    };

    textFields.forEach(assignNullable);

    if (user.lastSignedIn !== undefined) {
      values.lastSignedIn = user.lastSignedIn;
      updateSet.lastSignedIn = user.lastSignedIn;
    }
    if (user.role !== undefined) {
      values.role = user.role;
      updateSet.role = user.role;
    } else if (user.openId === ENV.ownerOpenId) {
      values.role = 'admin';
      updateSet.role = 'admin';
    }

    if (!values.lastSignedIn) {
      values.lastSignedIn = new Date();
    }

    if (Object.keys(updateSet).length === 0) {
      updateSet.lastSignedIn = new Date();
    }

    await db.insert(users).values(values).onDuplicateKeyUpdate({
      set: updateSet,
    });
  } catch (error) {
    console.error("[Database] Failed to upsert user:", error);
    throw error;
  }
}

export async function getUserByOpenId(openId: string) {
  const db = await getDb();
  if (!db) {
    console.warn("[Database] Cannot get user: database not available");
    return undefined;
  }

  const result = await db.select().from(users).where(eq(users.openId, openId)).limit(1);

  return result.length > 0 ? result[0] : undefined;
}

/* ------------------------------------------------------------------ */
/* KSM engine queries                                                  */
/* ------------------------------------------------------------------ */

export async function countCorpusTicks(): Promise<number> {
  const db = await getDb();
  if (!db) return 0;
  const rows = await db.select({ tick: corpusTicks.tick }).from(corpusTicks);
  return rows.length;
}

export async function insertCorpusTicks(rows: InsertCorpusTick[]) {
  const db = await getDb();
  if (!db) throw new Error("database not available");
  if (rows.length === 0) return;
  await db.insert(corpusTicks).values(rows);
}

export async function clearCorpusTicks() {
  const db = await getDb();
  if (!db) throw new Error("database not available");
  await db.delete(corpusTicks);
}

export async function listCorpusTicks(limit?: number) {
  const db = await getDb();
  if (!db) return [];
  const q = db.select().from(corpusTicks).orderBy(asc(corpusTicks.tick));
  return limit ? q.limit(limit) : q;
}

export async function clearInvariantChecks(runLabel: string) {
  const db = await getDb();
  if (!db) throw new Error("database not available");
  await db.delete(invariantChecks).where(eq(invariantChecks.runLabel, runLabel));
}

export async function insertInvariantChecks(rows: InsertInvariantCheck[]) {
  const db = await getDb();
  if (!db) throw new Error("database not available");
  if (rows.length === 0) return;
  await db.insert(invariantChecks).values(rows);
}

export async function listInvariantChecks() {
  const db = await getDb();
  if (!db) return [];
  return db
    .select()
    .from(invariantChecks)
    .orderBy(asc(invariantChecks.invariantId), desc(invariantChecks.checkedAt));
}

export async function upsertEngineConfig(cfg: InsertEngineConfig) {
  const db = await getDb();
  if (!db) throw new Error("database not available");
  const existing = await db
    .select({ id: engineConfigs.id })
    .from(engineConfigs)
    .where(eq(engineConfigs.version, cfg.version))
    .limit(1);
  if (existing.length === 0) {
    await db.insert(engineConfigs).values(cfg);
  }
  // mark this version active, all others inactive
  await db.update(engineConfigs).set({ active: 0 });
  await db
    .update(engineConfigs)
    .set({ active: 1 })
    .where(eq(engineConfigs.version, cfg.version));
}

export async function getActiveEngineConfig() {
  const db = await getDb();
  if (!db) return undefined;
  const rows = await db
    .select()
    .from(engineConfigs)
    .where(eq(engineConfigs.active, 1))
    .limit(1);
  return rows[0];
}

/* ------------------------------------------------------------------ */
/* Stored file queries                                                 */
/* ------------------------------------------------------------------ */

export async function insertStoredFile(row: InsertStoredFile) {
  const db = await getDb();
  if (!db) throw new Error("database not available");
  await db.insert(storedFiles).values(row);
}

export async function getStoredFileByKey(fileKey: string) {
  const db = await getDb();
  if (!db) return undefined;
  const rows = await db
    .select()
    .from(storedFiles)
    .where(eq(storedFiles.fileKey, fileKey))
    .limit(1);
  return rows[0];
}

export async function listStoredFiles() {
  const db = await getDb();
  if (!db) return [];
  return db.select().from(storedFiles).orderBy(desc(storedFiles.createdAt));
}

export async function deleteStoredFile(id: number) {
  const db = await getDb();
  if (!db) throw new Error("database not available");
  await db.delete(storedFiles).where(eq(storedFiles.id, id));
}
