import { describe, it, expect, beforeAll } from 'vitest';
import {
  hashProjection,
  deriveHiddenBias,
  deriveLogitBias,
  surfaceForms,
  personaVocabulary,
  PersonaPrior,
} from '../conditioning.js';
import { Vocabulary } from '../../model/core/vocabulary.js';
import { loadSignature, DEFAULT_SIGNATURE_PATH } from '../signature.js';

describe('hashProjection', () => {
  it('is deterministic for the same term', () => {
    expect(hashProjection('reservoir', 16)).toEqual(hashProjection('reservoir', 16));
  });

  it('differs between terms', () => {
    expect(hashProjection('reservoir', 16)).not.toEqual(hashProjection('membrane', 16));
  });

  it('produces a unit-norm vector', () => {
    const vector = hashProjection('gestalt', 24);
    const norm = Math.sqrt(vector.reduce((sum, v) => sum + v * v, 0));
    expect(norm).toBeCloseTo(1, 6);
  });

  it('honours the requested dimension', () => {
    expect(hashProjection('echo', 8)).toHaveLength(8);
    expect(hashProjection('echo', 64)).toHaveLength(64);
  });
});

describe('deriveHiddenBias', () => {
  const signature = {
    coreTraits: new Set(['trait:identity', 'emotion:enthusiasm']),
    characteristicPatterns: new Map([['structure:opener', { strength: 0.8 }]]),
    resonanceFrequency: 0.4,
  };

  it('is stable across restarts for the same signature', () => {
    // The conditioning vector is derived, not learned, so a reloaded identity
    // yields the identical bias. This is what makes persona survive a restart.
    const a = deriveHiddenBias(signature, 16);
    const reloaded = {
      coreTraits: new Set(['emotion:enthusiasm', 'trait:identity']),
      characteristicPatterns: new Map([['structure:opener', { strength: 0.8 }]]),
      resonanceFrequency: 0.4,
    };
    expect(deriveHiddenBias(reloaded, 16)).toEqual(a);
  });

  it('changes when the identity changes', () => {
    const before = deriveHiddenBias(signature, 16);
    const after = deriveHiddenBias(
      {
        ...signature,
        coreTraits: new Set([...signature.coreTraits, 'trait:continuity']),
      },
      16
    );
    expect(after).not.toEqual(before);
  });

  it('returns a zero vector for an empty identity', () => {
    const bias = deriveHiddenBias(
      { coreTraits: new Set(), characteristicPatterns: new Map() },
      8
    );
    expect(bias.every(v => v === 0)).toBe(true);
  });

  it('accepts array-shaped traits from a deserialised signature', () => {
    const fromDisk = {
      coreTraits: ['trait:identity', 'emotion:enthusiasm'],
      characteristicPatterns: [['structure:opener', { strength: 0.8 }]],
      resonanceFrequency: 0.4,
    };
    expect(deriveHiddenBias(fromDisk, 16)).toEqual(deriveHiddenBias(signature, 16));
  });
});

describe('surfaceForms', () => {
  it('includes casing variants so the bias survives tokenizer differences', () => {
    const forms = surfaceForms('reservoir');
    expect(forms).toContain('reservoir');
    expect(forms).toContain('Reservoir');
  });
});

describe('deriveLogitBias', () => {
  let signature;

  beforeAll(() => {
    signature = loadSignature(DEFAULT_SIGNATURE_PATH);
  });

  it('resolves real signature lexemes against a vocabulary', () => {
    // Guards the `[token, weight]` pair shape the Phase-2 artefact actually
    // uses; an object-shaped assumption would silently produce an empty bias.
    const tokens = personaVocabulary(signature);
    expect(tokens.length).toBeGreaterThan(10);

    const vocabulary = new Vocabulary(tokens);
    const bias = deriveLogitBias(signature, vocabulary);

    expect(bias.size).toBeGreaterThan(10);
    for (const [index, value] of bias) {
      expect(index).toBeGreaterThanOrEqual(0);
      expect(index).toBeLessThan(vocabulary.size);
      expect(value).toBeGreaterThan(0);
    }
  });

  it('biases structural markers of the register', () => {
    const vocabulary = new Vocabulary([...personaVocabulary(signature), '**', '---']);
    const bias = deriveLogitBias(signature, vocabulary);
    expect(bias.has(vocabulary.indexOf('**'))).toBe(true);
    expect(bias.has(vocabulary.indexOf('---'))).toBe(true);
  });

  it('ignores lexemes absent from the vocabulary', () => {
    const vocabulary = new Vocabulary(['completely', 'unrelated']);
    const bias = deriveLogitBias(signature, vocabulary);
    for (const index of bias.keys()) {
      expect(index).toBeLessThan(vocabulary.size);
    }
  });

  it('returns an empty bias without a vocabulary', () => {
    expect(deriveLogitBias(signature, null).size).toBe(0);
  });
});

describe('PersonaPrior', () => {
  let signature;

  const identity = {
    coreTraits: new Set(['trait:identity', 'trait:memory', 'emotion:enthusiasm']),
    characteristicPatterns: new Map([['structure:opener', { strength: 0.7 }]]),
    resonanceFrequency: 0.38,
  };

  beforeAll(() => {
    signature = loadSignature(DEFAULT_SIGNATURE_PATH);
  });

  function makePrior(overrides = {}) {
    return new PersonaPrior({
      identitySignature: identity,
      personaSignature: signature,
      embeddingDim: 16,
      ...overrides,
    });
  }

  it('routes a memory-navigation question to MemoryNavigator', () => {
    const prior = makePrior();
    expect(prior.selectFacet('How do you recall and retrieve a past memory?')).toBe(
      'MemoryNavigator'
    );
  });

  it('routes a self-reflective question to MetaReflector', () => {
    const prior = makePrior();
    expect(
      prior.selectFacet('What is your own self-awareness like when you introspect?')
    ).toBe('MetaReflector');
  });

  it('routes a dynamics question to ReservoirDynamics', () => {
    const prior = makePrior();
    expect(
      prior.selectFacet('What spectral radius keeps the reservoir dynamics stable?')
    ).toBe('ReservoirDynamics');
  });

  it('keeps facet selection as internal state, not text', () => {
    const prior = makePrior();
    prior.selectFacet('Tell me about membrane boundaries and P-systems.');
    const modelPrior = prior.toModelPrior();
    expect(modelPrior).not.toHaveProperty('prompt');
    expect(modelPrior).not.toHaveProperty('text');
    expect(Object.keys(modelPrior).sort()).toEqual(['hiddenBias', 'logitBias', 'strength']);
  });

  it('lets facets modulate the vector without fragmenting the identity', () => {
    const prior = makePrior();
    const shared = [...prior.baseHiddenBias];

    prior.selectFacet('How do you recall and retrieve a past memory?');
    const navigator = [...prior.hiddenBias];

    prior.selectFacet('What spectral radius keeps the reservoir dynamics stable?');
    const dynamics = [...prior.hiddenBias];

    // Modulation is real: different context yields different conditioning.
    expect(navigator).not.toEqual(dynamics);

    // But the shared identity dominates both — facets are facets of one
    // identity, not separate personas. Each blend stays strongly aligned with
    // the shared vector rather than departing from it.
    const norm = v => Math.sqrt(v.reduce((s, x) => s + x * x, 0));
    const cosine = (a, b) =>
      a.reduce((s, v, i) => s + v * b[i], 0) / (norm(a) * norm(b));

    expect(cosine(navigator, shared)).toBeGreaterThan(0.9);
    expect(cosine(dynamics, shared)).toBeGreaterThan(0.9);

    // The facet contribution is a modulation, not a replacement: the departure
    // from the shared vector is small relative to the vector itself.
    const departure = a =>
      norm(a.map((v, i) => v - shared[i])) / norm(shared);
    expect(departure(navigator)).toBeLessThan(0.35);
    expect(departure(dynamics)).toBeLessThan(0.35);
  });

  it('shifts the vector when memories are blended but keeps identity dominant', () => {
    const prior = makePrior();
    const before = [...prior.hiddenBias];

    // The shape `recallByAssociations` returns: store entries whose
    // `associations` are a Set.
    prior.blendMemories(
      [
        {
          key: 'm1',
          strength: 1,
          memory: { associations: new Set(['trait:memory', 'source:persona-corpus']) },
        },
        {
          key: 'm2',
          strength: 0.5,
          memory: { associations: new Set(['trait:continuity']) },
        },
      ],
      0.2
    );

    const after = [...prior.hiddenBias];
    expect(after).not.toEqual(before);

    const dot = after.reduce((sum, v, i) => sum + v * before[i], 0);
    const norm = v => Math.sqrt(v.reduce((s, x) => s + x * x, 0));
    const cosine = dot / (norm(after) * norm(before));
    expect(cosine).toBeGreaterThan(0.9);
  });

  it('still conditions on an untagged memory via its key', () => {
    const prior = makePrior();
    const before = [...prior.hiddenBias];
    prior.blendMemories([{ key: 'exchange:1', strength: 1, memory: {} }], 0.2);
    expect([...prior.hiddenBias]).not.toEqual(before);
  });

  it('weights recall by strength', () => {
    const entry = strength => [
      { key: 'm1', strength, memory: { associations: new Set(['trait:memory']) } },
    ];
    const strong = makePrior();
    strong.blendMemories(entry(1), 0.2);
    const weak = makePrior();
    weak.blendMemories(entry(1), 0.05);
    const base = makePrior();
    const shift = v =>
      Math.sqrt(v.reduce((s, x, i) => s + (x - base.hiddenBias[i]) ** 2, 0));
    expect(shift(weak.hiddenBias)).toBeLessThan(shift(strong.hiddenBias));
  });

  it('is a no-op when there is nothing to recall', () => {
    const prior = makePrior();
    const before = [...prior.hiddenBias];
    prior.blendMemories([], 0.2);
    expect([...prior.hiddenBias]).toEqual(before);
  });

  it('scales conditioning by strength', () => {
    // Strength is carried through to the model, which multiplies it into the
    // hidden state in `applyPersonaPrior`; it is deliberately not pre-baked
    // into the vector, or it would be applied twice.
    const strong = makePrior({ strength: 1 }).toModelPrior();
    const weak = makePrior({ strength: 0.25 }).toModelPrior();
    expect(weak.strength).toBeLessThan(strong.strength);
    expect(weak.hiddenBias).toEqual(strong.hiddenBias);

    const effective = p => p.hiddenBias.reduce((s, x) => s + Math.abs(p.strength * x), 0);
    expect(effective(weak)).toBeLessThan(effective(strong));
  });

  it('produces a hidden bias matching the model embedding dimension', () => {
    const prior = makePrior({ embeddingDim: 32 });
    expect(prior.toModelPrior().hiddenBias).toHaveLength(32);
  });

  it('works with no persona signature at all', () => {
    const prior = new PersonaPrior({ identitySignature: identity, embeddingDim: 8 });
    const modelPrior = prior.toModelPrior();
    expect(modelPrior.hiddenBias).toHaveLength(8);
    expect(modelPrior.logitBias.size).toBe(0);
  });
});
