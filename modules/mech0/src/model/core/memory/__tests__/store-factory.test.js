import { expect, test } from 'vitest';
import { Mem0MemoryStore } from '../mem0-store.js';
import { MemoryStore } from '../store.js';
import { createMemoryStore } from '../store-factory.js';

test('createMemoryStore should default to the in-memory store', () => {
  const store = createMemoryStore({});
  expect(store).toBeInstanceOf(MemoryStore);
});

test('createMemoryStore should create a mem0 store when configured', () => {
  const store = createMemoryStore({
    memory: {
      provider: 'mem0',
      mem0: {
        apiKey: 'test-key',
        fetch: async () => ({ ok: true, json: async () => ({}) })
      }
    }
  });

  expect(store).toBeInstanceOf(Mem0MemoryStore);
});
