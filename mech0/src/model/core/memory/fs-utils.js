import fs from 'fs';
import path from 'path';
import logger from '../../../utils/logger.js';

/**
 * Small filesystem helpers shared by the durable memory providers.
 *
 * Writes go through a temporary file and a rename so that a crash mid-write
 * cannot leave a half-written identity on disk. An identity that fails to load
 * is worse than one that is slightly stale.
 */

/**
 * Write JSON atomically.
 * @param {string} filePath
 * @param {any} data
 */
export function writeJsonAtomic(filePath, data) {
  const directory = path.dirname(filePath);
  fs.mkdirSync(directory, { recursive: true });

  const tempPath = `${filePath}.${process.pid}.tmp`;
  fs.writeFileSync(tempPath, JSON.stringify(data, null, 2));
  fs.renameSync(tempPath, filePath);
}

/**
 * Read JSON, returning a fallback when the file is missing or corrupt.
 * A corrupt snapshot is logged and skipped rather than thrown, so a damaged
 * checkpoint degrades to a cold start instead of preventing startup entirely.
 * @param {string} filePath
 * @param {any} [fallback=null]
 * @returns {any}
 */
export function readJsonSafe(filePath, fallback = null) {
  if (!fs.existsSync(filePath)) return fallback;

  try {
    return JSON.parse(fs.readFileSync(filePath, 'utf-8'));
  } catch (error) {
    logger.error(`Corrupt JSON at ${filePath}, ignoring:`, error);
    return fallback;
  }
}
