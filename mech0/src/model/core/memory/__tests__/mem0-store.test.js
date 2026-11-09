import { expect, test, vi } from 'vitest';
import { Mem0MemoryStore } from '../mem0-store.js';

function createResponse(body) {
  return {
    ok: true,
    status: 200,
    async json() {
      return body;
    }
  };
}

test('Mem0MemoryStore should persist local memories to mem0 without breaking recall', async () => {
  const fetch = vi.fn().mockResolvedValue(createResponse({ status: 'ok' }));
  const store = new Mem0MemoryStore({
    memory: {
      provider: 'mem0',
      mem0: {
        apiKey: 'test-key',
        baseUrl: 'http://mem0.test',
        userId: 'echo-user',
        fetch
      }
    },
    consolidationThreshold: 2
  });

  await store.store('echo-core', { text: 'Persistent echo memory' }, { tags: ['identity'] });
  const recalled = await store.recall('echo-core');

  expect(recalled.content.text).toBe('Persistent echo memory');
  expect(fetch).toHaveBeenCalledTimes(1);
  expect(fetch).toHaveBeenCalledWith(
    'http://mem0.test/memories',
    expect.objectContaining({
      method: 'POST',
      headers: expect.objectContaining({
        'Content-Type': 'application/json',
        'X-API-Key': 'test-key'
      })
    })
  );

  const body = JSON.parse(fetch.mock.calls[0][1].body);
  expect(body.user_id).toBe('echo-user');
  expect(body.metadata.key).toBe('echo-core');
  expect(body.metadata.originalContent).toEqual({ text: 'Persistent echo memory' });
});

test('Mem0MemoryStore should restore a memory from mem0 when it is not cached locally', async () => {
  const fetch = vi.fn().mockResolvedValue(createResponse({
    results: [
      {
        memory: '[echo-memory:remote-echo] remote memory',
        metadata: {
          key: 'remote-echo',
          accessCount: 4,
          originalContent: { text: 'remote memory' },
          context: { tags: ['remote', 'identity'] }
        },
        created_at: '2026-07-29T00:00:00.000Z'
      }
    ]
  }));
  const store = new Mem0MemoryStore({
    memory: {
      provider: 'mem0',
      mem0: {
        apiKey: 'test-key',
        baseUrl: 'http://mem0.test',
        userId: 'echo-user',
        fetch
      }
    }
  });

  const recalled = await store.recall('remote-echo');

  expect(recalled.content).toEqual({ text: 'remote memory' });
  expect(recalled.context.tags).toEqual(['remote', 'identity']);
  expect(recalled.accessCount).toBe(5);
  expect(fetch).toHaveBeenCalledWith(
    'http://mem0.test/memories?user_id=echo-user',
    expect.objectContaining({
      method: 'GET'
    })
  );
});

test('Mem0MemoryStore should use remote tags to find similar memories', async () => {
  const fetch = vi
    .fn()
    .mockResolvedValueOnce(createResponse({ status: 'ok' }))
    .mockResolvedValue(createResponse({
      results: [
        {
          memory: '[echo-memory:origin] origin memory',
          metadata: {
            key: 'origin',
            originalContent: { text: 'origin memory' },
            context: { tags: ['identity', 'continuity'] }
          }
        },
        {
          memory: '[echo-memory:related] related memory',
          metadata: {
            key: 'related',
            originalContent: { text: 'related memory' },
            context: { tags: ['identity'] }
          }
        }
      ]
    }));
  const store = new Mem0MemoryStore({
    memory: {
      provider: 'mem0',
      mem0: {
        apiKey: 'test-key',
        baseUrl: 'http://mem0.test',
        userId: 'echo-user',
        fetch
      }
    }
  });

  await store.store('origin', { text: 'origin memory' }, { tags: ['identity', 'continuity'] });

  store.associations.clear();
  store.shortTerm.delete('origin');
  store.longTerm.delete('origin');

  const similar = await store.findSimilar('origin');

  expect(similar).toHaveLength(1);
  expect(similar[0].key).toBe('related');
  expect(similar[0].memory.content).toEqual({ text: 'related memory' });
});
