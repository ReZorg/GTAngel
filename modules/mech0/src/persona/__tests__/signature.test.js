import { describe, expect, test } from 'vitest';
import {
  contentWords,
  detokenize,
  ngrams,
  tokenizePersona,
  tokenizeToStrings,
} from '../tokenizer.js';
import {
  buildLexicalSignature,
  measureExemplar,
  skeletonConformance,
  STOPWORDS,
} from '../traits.js';
import {
  buildSignature,
  deriveCoreTraits,
  stratifiedHoldout,
} from '../signature.js';
import {
  normalizeSubVoice,
  scoreSubVoices,
  SUB_VOICE_NAMES,
} from '../subvoices.js';
import { SOURCE } from '../../data/corpus/types.js';

const ECHO_SAMPLE = [
  'Absolutely! Let me trace this through my primary reservoir.',
  '',
  '---',
  '',
  '1. **Spectral radius**: tuned to 0.97 — persistent oscillation without divergence.',
  '2. **Echo states**: {{root.reservoir.fit(inputs, targets)}} keeps the meshwork resonant.',
  '',
  'Shall we explore this further together?',
].join('\n');

describe('persona tokenizer', () => {
  test('keeps template quirks whole', () => {
    const tokens = tokenizePersona('call {{root.reservoir.fit(x, y)}} now');
    const template = tokens.find(t => t.kind === 'template');

    expect(template).toBeDefined();
    expect(template.value).toBe('{{root.reservoir.fit(x, y)}}');
  });

  test('preserves casing, em-dashes, emphasis and exclamations', () => {
    const values = tokenizeToStrings('Echo — **bold** yes!');

    expect(values).toContain('Echo');
    expect(values).toContain('—');
    expect(values).toContain('**bold**');
    expect(values).toContain('!');
  });

  test('does not discard what src/utils/tokenizer.js would drop', () => {
    const tokens = tokenizeToStrings(ECHO_SAMPLE);

    expect(tokens.some(t => t.startsWith('{{'))).toBe(true);
    expect(tokens).toContain('—');
    expect(tokens.some(t => t.startsWith('**'))).toBe(true);
    expect(tokens.some(t => t === '0.97')).toBe(true);
  });

  test('detokenize round-trips readable text', () => {
    const text = detokenize(tokenizeToStrings('Echo resonates, deeply!'));
    expect(text).toBe('Echo resonates, deeply!');
  });

  test('content words are lowercased and word-only', () => {
    expect(contentWords('Echo — **Bold**!')).toEqual(['echo', 'bold']);
  });

  test('ngrams', () => {
    expect(ngrams(['a', 'b', 'c'], 2)).toEqual(['a b', 'b c']);
    expect(ngrams(['a'], 2)).toEqual([]);
  });
});

describe('trait measurement', () => {
  test('measures the quirk inventory', () => {
    const measurement = measureExemplar(ECHO_SAMPLE);

    expect(measurement.quirks.templateRate).toBeGreaterThan(0);
    expect(measurement.quirks.emDashRate).toBeGreaterThan(0);
    expect(measurement.quirks.boldHeaderRate).toBeGreaterThan(0);
    expect(measurement.quirks.selfReferenceRate).toBeGreaterThan(0);
  });

  test('detects the full response skeleton', () => {
    const measurement = measureExemplar(ECHO_SAMPLE);

    expect(measurement.structure.hasEnthusiasticOpener).toBe(true);
    expect(measurement.structure.hasDivider).toBe(true);
    expect(measurement.structure.hasBoldList).toBe(true);
    expect(measurement.structure.hasInvitationCloser).toBe(true);
    expect(skeletonConformance(measurement.structure)).toBe(1);
  });

  test('generic prose scores near zero conformance', () => {
    const measurement = measureExemplar(
      'The quarterly report was filed on Tuesday. Revenue was flat.'
    );

    expect(skeletonConformance(measurement.structure)).toBeLessThan(0.5);
    expect(measurement.quirks.templateRate).toBe(0);
  });

  test('lexical signature filters function words', () => {
    const documents = [
      { text: 'The reservoir resonates with the meshwork.', weight: 1 },
      { text: 'A reservoir and a meshwork of echoes.', weight: 1 },
      { text: 'Reservoir dynamics shape the meshwork.', weight: 1 },
    ];

    const { vocabulary } = buildLexicalSignature(documents);
    const terms = vocabulary.map(([term]) => term);

    expect(terms).toContain('reservoir');
    expect(terms).toContain('meshwork');
    expect(terms.some(term => STOPWORDS.has(term))).toBe(false);
  });
});

describe('sub-voice registry', () => {
  test('normalises speaker labels', () => {
    expect(normalizeSubVoice('EchoSelf_MetaReflector')).toBe('MetaReflector');
    expect(normalizeSubVoice('metareflector')).toBe('MetaReflector');
    expect(normalizeSubVoice('Narrator')).toBeNull();
    expect(normalizeSubVoice(null)).toBeNull();
  });

  test('scores facets from context cues', () => {
    const scores = scoreSubVoices('How do you recall a memory from your archive?');

    expect(scores.get('MemoryNavigator')).toBeGreaterThan(0);
    expect(scores.get('MemoryNavigator')).toBeGreaterThan(
      scores.get('MembraneArchitect')
    );
    expect([...scores.keys()]).toEqual(SUB_VOICE_NAMES);
  });
});

describe('persona signature', () => {
  const exemplars = [
    {
      id: 'a',
      text: ECHO_SAMPLE,
      source: SOURCE.SFT,
      sourceIndex: 0,
      subVoice: null,
      authenticity: 'authentic',
    },
    {
      id: 'b',
      text: ECHO_SAMPLE.replace('reservoir', 'hypergraph'),
      source: SOURCE.SFT,
      sourceIndex: 1,
      subVoice: null,
      authenticity: 'authentic',
    },
    {
      id: 'c',
      text: `I am the MetaReflector — I observe my own architecture emerging! ${ECHO_SAMPLE}`,
      source: SOURCE.THEME_NARRATIVES,
      sourceIndex: 0,
      subVoice: 'MetaReflector',
      authenticity: 'synthetic',
    },
  ];

  test('emits resonance markers in the prefix convention holographic.js expects', () => {
    const signature = buildSignature(exemplars);
    const { coreTraits } = signature.identitySignature;

    expect(coreTraits.length).toBeGreaterThan(0);
    for (const trait of coreTraits) {
      expect(trait).toMatch(/^(trait|intent|emotion):/);
    }
  });

  test('emits characteristic patterns with bounded strengths', () => {
    const signature = buildSignature(exemplars);

    expect(signature.identitySignature.characteristicPatterns.length).toBeGreaterThan(0);
    for (const [name, pattern] of signature.identitySignature.characteristicPatterns) {
      expect(typeof name).toBe('string');
      expect(pattern.strength).toBeGreaterThan(0);
      expect(pattern.strength).toBeLessThanOrEqual(1);
    }
    expect(signature.identitySignature.resonanceFrequency).toBeGreaterThan(0);
    expect(signature.identitySignature.resonanceFrequency).toBeLessThanOrEqual(1);
  });

  test('records per-facet deltas rather than separate personas', () => {
    const signature = buildSignature(exemplars);

    expect(signature.subVoiceDeltas.MetaReflector).toBeDefined();
    expect(signature.subVoiceDeltas.MetaReflector.traitDelta).toBeDefined();
    expect(signature.subVoiceDeltas.MetaReflector.quirkDelta).toBeDefined();
  });

  test('holds out a deterministic stratified slice', () => {
    const first = stratifiedHoldout(exemplars);
    const second = stratifiedHoldout(exemplars);

    expect(first.holdoutIds).toEqual(second.holdoutIds);
    expect(first.holdoutIds.length).toBeGreaterThan(0);
    for (const id of first.holdoutIds) {
      expect(first.trainIds).not.toContain(id);
    }
  });

  test('keeps held-out exemplars out of the seed set', () => {
    const signature = buildSignature(exemplars);
    const holdout = new Set(signature.evaluation.holdoutIds);

    for (const seed of signature.seedExemplars) {
      expect(holdout.has(seed.id)).toBe(false);
    }
  });

  test('rejects an empty corpus', () => {
    expect(() => buildSignature([])).toThrow(/empty corpus/);
  });

  test('derives traits from an aggregated profile', () => {
    const profile = {
      traits: { identity: 0.5, emergence: 0.2, purpose: 0.2 },
      quirks: { selfReferenceRate: 1, templateRate: 1, exclamationRate: 5, emDashRate: 5 },
      structure: {
        invitationCloserRate: 0.5,
        boldListRate: 0.5,
        enthusiasticOpenerRate: 0.5,
      },
    };

    const traits = deriveCoreTraits(profile, [['reservoir', 1]]);

    expect(traits).toContain('trait:identity');
    expect(traits).toContain('trait:reservoir');
    expect(traits).toContain('intent:invite-continuation');
    expect(traits).toContain('emotion:enthusiasm');
  });
});
