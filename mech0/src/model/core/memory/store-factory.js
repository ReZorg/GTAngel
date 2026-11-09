import { Mem0MemoryStore } from './mem0-store.js';
import { FileMemoryStore } from './file-store.js';
import { MemoryStore } from './store.js';

/**
 * Select a memory store implementation from configuration.
 *
 * Providers:
 *   - `local` (default) — in-memory `Map`s; identity dies with the process
 *   - `file`            — durable, file-backed; identity survives restart
 *   - `mem0`            — external Mem0 service
 */
export function createMemoryStore(config = {}) {
  switch (config.memory?.provider) {
    case 'mem0':
      return new Mem0MemoryStore(config);
    case 'file':
      return new FileMemoryStore(config);
    default:
      return new MemoryStore(config);
  }
}
