import { afterEach, beforeEach, describe, expect, test } from 'vitest';
import fs from 'fs';
import os from 'os';
import path from 'path';
import { MemoryStore } from '../store.js';
import { MemoryNetwork } from '../network.js';
import { HolographicIdentity } from '../holographic.js';
import { FileMemoryStore } from '../file-store.js';
import { IdentityPersistence } from '../persistence.js';
import { MemoryOrchestrator } from '../orchestrator.js';
import { createMemoryStore } from '../store-factory.js';
import {
  deserializeHolographic,
  restoreNetwork,
  restoreStore,
  serializeHolographic,
  serializeNetwork,
  serializeStore,
} from '../serialization.js';
import { seedIdentity, signatureFingerprint } from '../seed.js';

let workDir;

function makeConfig() {
  return {
    consolidationThreshold: 5,
    memory: { provider: 'file', file: { directory: workDir } },
  };
}

beforeEach(() => {
  workDir = fs.mkdtempSync(path.join(os.tmpdir(), 'echo-identity-'));
});

afterEach(() => {
  fs.rmSync(workDir, { recursive: true, force: true });
});

describe('memory serialization', () => {
  test('round-trips Sets that JSON.stringify would silently destroy', async () => {
    const store = new MemoryStore({ consolidationThreshold: 5 });
    await store.store('a', { text: 'one' }, { tags: ['alpha', 'beta'] });
    await store.store('b', { text: 'two' }, { tags: ['beta'] });

    const snapshot = JSON.parse(JSON.stringify(serializeStore(store)));
    const restored = new MemoryStore({ consolidationThreshold: 5 });
    restoreStore(restored, snapshot);

    const entry = await restored.recall('a');
    expect(entry.associations).toBeInstanceOf(Set);
    expect(entry.associations.has('alpha')).toBe(true);
    expect(restored.associations.get('beta')).toBeInstanceOf(Set);
    expect(restored.associations.get('beta').has('b')).toBe(true);
  });

  test('naive JSON of a raw store loses associations (regression guard)', async () => {
    const store = new MemoryStore({ consolidationThreshold: 5 });
    await store.store('a', { text: 'one' }, { tags: ['alpha'] });

    const naive = JSON.parse(
      JSON.stringify({ shortTerm: [...store.shortTerm.entries()] })
    );

    expect(naive.shortTerm[0][1].associations).toEqual({});
  });

  test('round-trips network nodes, edges and clusters', () => {
    const network = new MemoryNetwork({});
    network.addNode('a', { content: 1, associations: new Set(['x']) });
    network.addNode('b', { content: 2, associations: new Set(['x']) });
    network.addEdge('a', 'b', 'relates');
    network.detectClusters();

    const snapshot = JSON.parse(JSON.stringify(serializeNetwork(network)));
    const restored = new MemoryNetwork({});
    restoreNetwork(restored, snapshot);

    expect(restored.nodes.size).toBe(2);
    expect(restored.edges.size).toBe(1);
    expect(restored.nodes.get('a').connections).toBeInstanceOf(Set);
    expect(restored.nodes.get('a').connections.size).toBe(1);
  });

  test('round-trips holographic identity data', () => {
    const identity = new HolographicIdentity();
    identity.ingestExperience({
      key: 'm1',
      content: { text: 'hello' },
      associations: new Set(['trait:curious', 'intent:learn']),
    });

    const snapshot = JSON.parse(
      JSON.stringify(serializeHolographic(identity.exportHolographicData()))
    );
    const restoredIdentity = new HolographicIdentity();
    restoredIdentity.importHolographicData(deserializeHolographic(snapshot));

    expect(restoredIdentity.identitySignature.coreTraits).toBeInstanceOf(Set);
    expect(restoredIdentity.identitySignature.coreTraits.has('trait:curious')).toBe(true);
    expect(
      restoredIdentity.identitySignature.characteristicPatterns
    ).toBeInstanceOf(Map);
    expect(restoredIdentity.experienceMeshwork.size).toBe(1);
    const [pattern] = [...restoredIdentity.experienceMeshwork.values()];
    expect(pattern.associations).toBeInstanceOf(Set);
  });
});

describe('file-backed memory store', () => {
  test('is selected by the store factory', () => {
    expect(createMemoryStore(makeConfig())).toBeInstanceOf(FileMemoryStore);
    expect(createMemoryStore({})).not.toBeInstanceOf(FileMemoryStore);
  });

  test('survives a restart', async () => {
    const store = new FileMemoryStore(makeConfig());
    await store.store('persisted', { text: 'still here' }, { tags: ['identity'] });
    expect(store.flush()).toBe(true);

    const reloaded = new FileMemoryStore(makeConfig());
    const entry = await reloaded.recall('persisted');

    expect(entry).not.toBeNull();
    expect(entry.content.text).toBe('still here');
    expect(entry.associations.has('identity')).toBe(true);
  });

  test('does not rewrite when nothing changed', async () => {
    const store = new FileMemoryStore(makeConfig());
    await store.store('a', { text: 'x' });
    expect(store.flush()).toBe(true);
    expect(store.flush()).toBe(false);
  });
});

describe('identity persistence', () => {
  test('versions identity snapshots like model versions', () => {
    const persistence = new IdentityPersistence(makeConfig());
    const identity = new HolographicIdentity();

    identity.ingestExperience({
      key: 'm1',
      content: { text: 'first' },
      associations: new Set(['trait:first']),
    });
    const v1 = persistence.saveIdentity(identity.exportHolographicData(), {
      note: 'first',
    });

    identity.ingestExperience({
      key: 'm2',
      content: { text: 'second' },
      associations: new Set(['trait:second']),
    });
    const v2 = persistence.saveIdentity(identity.exportHolographicData());

    expect(v1.version).toBe(1);
    expect(v2.version).toBe(2);
    expect(persistence.loadVersions().currentVersion).toBe(2);

    const rolledBack = persistence.rollbackTo(1);
    expect(rolledBack.identitySignature.coreTraits.has('trait:first')).toBe(true);
    expect(rolledBack.identitySignature.coreTraits.has('trait:second')).toBe(false);
    expect(persistence.loadVersions().currentVersion).toBe(1);
  });

  test('does not burn a version slot when nothing changed', () => {
    const persistence = new IdentityPersistence(makeConfig());
    const identity = new HolographicIdentity();
    identity.ingestExperience({
      key: 'm1',
      content: { text: 'first' },
      associations: new Set(['trait:first']),
    });

    expect(persistence.saveIdentity(identity.exportHolographicData()).version).toBe(1);
    expect(persistence.saveIdentity(identity.exportHolographicData())).toBeNull();
    expect(persistence.loadVersions().versions).toHaveLength(1);

    // A restart must not rewrite an identical snapshot either.
    const restarted = new IdentityPersistence(makeConfig());
    expect(restarted.saveIdentity(identity.exportHolographicData())).toBeNull();
  });

  test('ignores a corrupt snapshot instead of failing startup', () => {
    const persistence = new IdentityPersistence(makeConfig());
    fs.mkdirSync(workDir, { recursive: true });
    fs.writeFileSync(persistence.identityPath, '{ not json');

    expect(persistence.loadIdentity()).toBeNull();
  });
});

describe('orchestrator persistence lifecycle', () => {
  test('identity and signature survive a process restart', async () => {
    const orchestrator = new MemoryOrchestrator(makeConfig());

    await orchestrator.processHolographicMemory(
      'exchange:1',
      { text: 'The reservoir resonates.' },
      { tags: ['trait:reservoir', 'intent:explain'] }
    );

    const before = orchestrator.getIdentitySnapshot();
    expect(orchestrator.checkpoint({ force: true })).toBe(true);

    // A brand new orchestrator stands in for a brand new process.
    const restarted = new MemoryOrchestrator(makeConfig());
    const after = restarted.getIdentitySnapshot();

    expect(after.meshworkSize).toBe(before.meshworkSize);
    expect([...after.signature.coreTraits].sort()).toEqual(
      [...before.signature.coreTraits].sort()
    );
    expect(after.continuityMetrics.identityPreservation).toBe(
      before.continuityMetrics.identityPreservation
    );

    const recalled = await restarted.store.recall('exchange:1');
    expect(recalled).not.toBeNull();
    expect(restarted.network.nodes.has('exchange:1')).toBe(true);
  });

  test('local provider keeps its in-memory behaviour', () => {
    const orchestrator = new MemoryOrchestrator({ consolidationThreshold: 5 });

    expect(orchestrator.persistence).toBeNull();
    expect(orchestrator.checkpoint()).toBe(false);
  });
});

describe('identity seeding', () => {
  const signature = {
    schemaVersion: 1,
    corpus: { curationRulesetVersion: 1, exemplarCount: 3 },
    identitySignature: {
      coreTraits: ['trait:reservoir', 'intent:invite-continuation'],
      characteristicPatterns: [['quirk:emDashRate', { strength: 0.6, examples: [] }]],
      resonanceFrequency: 0.42,
    },
    seedExemplars: [
      {
        id: 'x#1',
        source: 'training_dataset_dtesnn.jsonl',
        subVoice: null,
        authenticity: 'authentic',
        text: 'The reservoir resonates — shall we explore further?',
      },
      {
        id: 'x#2',
        source: 'narratives/themes',
        subVoice: 'MetaReflector',
        authenticity: 'synthetic',
        text: 'I observe myself observing.',
      },
    ],
  };

  test('boots the identity with a past', async () => {
    const orchestrator = new MemoryOrchestrator(makeConfig());
    const result = await seedIdentity(orchestrator, signature);

    expect(result.seeded).toBe(true);
    expect(result.exemplars).toBe(2);

    const snapshot = orchestrator.getIdentitySnapshot();
    expect(snapshot.signature.coreTraits.has('trait:reservoir')).toBe(true);
    expect(snapshot.signature.characteristicPatterns.has('quirk:emDashRate')).toBe(true);
    expect(snapshot.meshworkSize).toBeGreaterThanOrEqual(2);
  });

  test('is idempotent for an unchanged signature', async () => {
    const orchestrator = new MemoryOrchestrator(makeConfig());

    await seedIdentity(orchestrator, signature);
    const second = await seedIdentity(orchestrator, signature);

    expect(second.seeded).toBe(false);
    expect(second.fingerprint).toBe(signatureFingerprint(signature));
  });

  test('re-seeds when the signature fingerprint changes', async () => {
    const orchestrator = new MemoryOrchestrator(makeConfig());
    await seedIdentity(orchestrator, signature);

    const evolved = {
      ...signature,
      corpus: { ...signature.corpus, exemplarCount: 4 },
    };

    expect((await seedIdentity(orchestrator, evolved)).seeded).toBe(true);
  });

  test('never erases lived experience when applying a signature', async () => {
    const orchestrator = new MemoryOrchestrator(makeConfig());

    await orchestrator.processHolographicMemory(
      'lived:1',
      { text: 'something that happened' },
      { tags: ['trait:lived'] }
    );

    await seedIdentity(orchestrator, signature);

    const snapshot = orchestrator.getIdentitySnapshot();
    expect(snapshot.signature.coreTraits.has('trait:lived')).toBe(true);
    expect(snapshot.signature.coreTraits.has('trait:reservoir')).toBe(true);
  });
});
