import path from 'path';
import { MemoryStore } from './store.js';
import { restoreStore, serializeStore } from './serialization.js';
import { readJsonSafe, writeJsonAtomic } from './fs-utils.js';
import logger from '../../../utils/logger.js';

/** Default location for the durable memory store. */
export const DEFAULT_IDENTITY_DIR = 'data/identity';

/**
 * File-backed memory store.
 *
 * Behaviourally identical to {@link MemoryStore} — it inherits every method —
 * but its contents survive process exit. It loads on construction and marks
 * itself dirty on mutation; flushing is driven by the memory system lifecycle
 * rather than happening on every write, so the hot path stays in memory.
 */
export class FileMemoryStore extends MemoryStore {
  constructor(config = {}) {
    super(config);

    const directory = config.memory?.file?.directory || DEFAULT_IDENTITY_DIR;
    this.filePath =
      config.memory?.file?.storePath || path.join(directory, 'memory-store.json');
    this.dirty = false;

    this.load();
  }

  /**
   * Load the persisted store, if one exists.
   * @returns {boolean} true when state was restored
   */
  load() {
    const snapshot = readJsonSafe(this.filePath);
    if (!snapshot) return false;

    restoreStore(this, snapshot);
    logger.info(
      `Restored memory store from ${this.filePath}`,
      { shortTerm: this.shortTerm.size, longTerm: this.longTerm.size }
    );
    return true;
  }

  /**
   * Persist the store. No-op when nothing has changed since the last flush.
   * @param {Object} [options]
   * @param {boolean} [options.force=false]
   * @returns {boolean} true when a write happened
   */
  flush({ force = false } = {}) {
    if (!this.dirty && !force) return false;

    writeJsonAtomic(this.filePath, serializeStore(this));
    this.dirty = false;
    return true;
  }

  async store(key, memory, context = {}) {
    const entry = await super.store(key, memory, context);
    this.dirty = true;
    return entry;
  }

  async recall(key, context = {}) {
    const entry = await super.recall(key, context);
    // Recall mutates access counts and can consolidate to long-term storage.
    if (entry) this.dirty = true;
    return entry;
  }

  async associate(key, tags) {
    const result = await super.associate(key, tags);
    if (result) this.dirty = true;
    return result;
  }

  async prune(maxAge) {
    await super.prune(maxAge);
    this.dirty = true;
  }
}
