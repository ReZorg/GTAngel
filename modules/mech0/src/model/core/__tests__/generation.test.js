import { describe, expect, test } from 'vitest';
import { CompositeLanguageModel } from '../composite.js';
import { Vocabulary, SPECIAL_TOKENS } from '../vocabulary.js';
import { GenerationHead, applyDecodingControls, sampleToken } from '../generation.js';

const CONFIG = { vocabSize: 32, embeddingDim: 8 };

/** Deterministic RNG so sampling assertions are not flaky. */
function seededRng(seed = 1) {
  let state = seed;
  return () => {
    state = (state * 1103515245 + 12345) % 2147483648;
    return state / 2147483648;
  };
}

describe('Vocabulary', () => {
  test('reserves special tokens at stable indices', () => {
    const vocabulary = new Vocabulary(['echo', 'reservoir']);

    expect(vocabulary.padIndex).toBe(0);
    expect(vocabulary.unkIndex).toBe(1);
    expect(vocabulary.bosIndex).toBe(2);
    expect(vocabulary.eosIndex).toBe(3);
    expect(vocabulary.size).toBe(6);
  });

  test('round-trips tokens through encode and decode', () => {
    const vocabulary = new Vocabulary(['echo', 'reservoir', 'resonance']);
    const encoded = vocabulary.encode(['echo', 'resonance'], { addBos: true, addEos: true });

    expect(encoded[0]).toBe(vocabulary.bosIndex);
    expect(encoded.at(-1)).toBe(vocabulary.eosIndex);
    expect(vocabulary.decode(encoded)).toEqual(['echo', 'resonance']);
  });

  test('maps unknown tokens to <unk> rather than dropping them', () => {
    const vocabulary = new Vocabulary(['echo']);
    const encoded = vocabulary.encode(['echo', 'unheard-of']);

    expect(encoded).toHaveLength(2);
    expect(encoded[1]).toBe(vocabulary.unkIndex);
    expect(vocabulary.decode(encoded, { skipSpecial: false })).toEqual([
      'echo',
      SPECIAL_TOKENS.UNK,
    ]);
  });

  test('builds deterministically by frequency then lexicographically', () => {
    const sequences = [
      ['echo', 'echo', 'reservoir'],
      ['echo', 'reservoir', 'gestalt'],
    ];

    const a = Vocabulary.build(sequences);
    const b = Vocabulary.build(sequences);

    expect(a.indexToToken).toEqual(b.indexToToken);
    expect(a.indexOf('echo')).toBeLessThan(a.indexOf('gestalt'));
  });

  test('honours minFrequency and maxSize', () => {
    const sequences = [['a', 'a', 'a', 'b', 'b', 'c']];

    expect(Vocabulary.build(sequences, { minFrequency: 2 }).has('c')).toBe(false);
    expect(Vocabulary.build(sequences, { maxSize: 5 }).size).toBe(5);
  });

  test('survives JSON round-trip', () => {
    const original = Vocabulary.build([['echo', 'echo', 'reservoir']]);
    const restored = Vocabulary.fromJSON(JSON.parse(JSON.stringify(original.toJSON())));

    expect(restored.indexToToken).toEqual(original.indexToToken);
    expect(restored.indexOf('echo')).toBe(original.indexOf('echo'));
  });

  test('tolerates the legacy plain-array vocabulary format', () => {
    const restored = Vocabulary.fromJSON(['echo', 'reservoir']);

    expect(restored.has('echo')).toBe(true);
    expect(restored.has(SPECIAL_TOKENS.UNK)).toBe(true);
  });
});

describe('decoding controls', () => {
  const logits = [1, 2, 3, 4, 5];

  test('top-k keeps only the k highest', () => {
    const { candidates } = applyDecodingControls(logits, { topK: 2 });
    expect(candidates.sort()).toEqual([3, 4]);
  });

  test('top-p truncates the tail', () => {
    const { candidates } = applyDecodingControls(logits, { topP: 0.5 });
    expect(candidates.length).toBeLessThan(logits.length);
    expect(candidates).toContain(4);
  });

  test('temperature sharpens and flattens the distribution', () => {
    const sharp = applyDecodingControls(logits, { temperature: 0.1 }).probabilities;
    const flat = applyDecodingControls(logits, { temperature: 5 }).probabilities;

    expect(sharp[4]).toBeGreaterThan(flat[4]);
  });

  test('repetition penalty demotes already-generated tokens', () => {
    const base = applyDecodingControls(logits, {}).probabilities;
    const penalised = applyDecodingControls(logits, {
      repetitionPenalty: 2,
      generated: [4],
    }).probabilities;

    expect(penalised[4]).toBeLessThan(base[4]);
  });

  test('logit bias shifts the distribution toward biased tokens', () => {
    const base = applyDecodingControls(logits, {}).probabilities;
    const biased = applyDecodingControls(logits, {
      logitBias: new Map([[0, 10]]),
    }).probabilities;

    expect(biased[0]).toBeGreaterThan(base[0]);
  });

  test('suppressed tokens are never selected', () => {
    const { candidates } = applyDecodingControls(logits, { suppress: [4] });
    expect(candidates).not.toContain(4);
  });

  test('falls back rather than producing NaN when everything is suppressed', () => {
    const { probabilities } = applyDecodingControls(logits, {
      suppress: [0, 1, 2, 3, 4],
    });
    expect(probabilities.every(Number.isFinite)).toBe(true);
  });

  test('greedy sampling is deterministic', () => {
    expect(sampleToken(logits, { temperature: 0 })).toBe(4);
    expect(sampleToken(logits, { temperature: 0 })).toBe(4);
  });

  test('stochastic sampling is reproducible for a given rng', () => {
    const a = sampleToken(logits, { temperature: 1, rng: seededRng(7) });
    const b = sampleToken(logits, { temperature: 1, rng: seededRng(7) });
    expect(a).toBe(b);
  });
});

describe('GenerationHead', () => {
  test('projects a hidden state onto vocabulary-sized logits', () => {
    const head = new GenerationHead(CONFIG);
    const logits = head.logits(new Array(CONFIG.embeddingDim).fill(0.5));

    expect(logits).toHaveLength(CONFIG.vocabSize);
    expect(logits.every(Number.isFinite)).toBe(true);
  });

  test('softmax sums to one', () => {
    const probs = GenerationHead.softmax([1, 2, 3]);
    expect(probs.reduce((a, b) => a + b, 0)).toBeCloseTo(1, 6);
  });

  test('learns: repeated updates reduce loss for the target token', () => {
    const head = new GenerationHead(CONFIG);
    const hidden = new Array(CONFIG.embeddingDim).fill(0.3);

    const first = head.update(hidden, 5, 0.5).loss;
    for (let i = 0; i < 40; i += 1) head.update(hidden, 5, 0.5);
    const last = head.update(hidden, 5, 0.5).loss;

    expect(last).toBeLessThan(first);
  });

  test('after training, greedy decoding picks the trained token', () => {
    const head = new GenerationHead(CONFIG);
    const hidden = new Array(CONFIG.embeddingDim).fill(0.3);

    for (let i = 0; i < 200; i += 1) head.update(hidden, 5, 0.5);

    expect(sampleToken(head.logits(hidden), { temperature: 0 })).toBe(5);
  });
});

describe('CompositeLanguageModel generation', () => {
  function buildModel() {
    const vocabulary = new Vocabulary(['the', 'reservoir', 'resonates', 'deeply']);
    return new CompositeLanguageModel({
      vocabSize: vocabulary.size,
      embeddingDim: 8,
      vocabulary,
    });
  }

  test('keeps the classification capability intact', () => {
    const model = buildModel();
    const output = model.forward([4, 5, 6], false);

    expect(output.predictions).toHaveLength(3);
    expect(output.contextVectors).toHaveLength(3);
    const total = output.predictions.reduce((sum, p) => sum + p.probability, 0);
    expect(total).toBeCloseTo(1, 6);
  });

  test('generates a bounded continuation and decodes it to text', () => {
    const model = buildModel();
    const result = model.generate([4, 5], { maxTokens: 6, temperature: 0 });

    expect(result.tokens.length).toBeGreaterThan(0);
    expect(result.tokens.length).toBeLessThanOrEqual(6);
    expect(typeof result.text).toBe('string');
  });

  test('generates from an empty prompt (the decisive persona test shape)', () => {
    const model = buildModel();
    const result = model.generate([], { maxTokens: 4, temperature: 0 });

    expect(result.tokens.length).toBeGreaterThan(0);
  });

  test('greedy generation is deterministic', () => {
    const model = buildModel();
    const a = model.generate([4], { maxTokens: 5, temperature: 0 });
    const b = model.generate([4], { maxTokens: 5, temperature: 0 });

    expect(a.tokens).toEqual(b.tokens);
  });

  test('stops at end-of-sequence', () => {
    const model = buildModel();
    const eos = model.vocabulary.eosIndex;

    // Force EOS to dominate every step.
    const bias = new Array(model.vocabulary.size).fill(0);
    bias[eos] = 1000;

    const result = model.generate([4], { maxTokens: 10, temperature: 0, logitBias: bias });

    expect(result.finishReason).toBe('eos');
    expect(result.tokens).toHaveLength(0);
  });

  test('learns a short sequence well enough to reproduce it greedily', () => {
    const model = buildModel();
    const sequence = model.vocabulary.encode(
      ['the', 'reservoir', 'resonates', 'deeply'],
      { addBos: true }
    );

    for (let epoch = 0; epoch < 300; epoch += 1) {
      model.trainSequence(sequence, 0.3);
    }

    const result = model.generate([model.vocabulary.bosIndex], {
      maxTokens: 4,
      temperature: 0,
    });

    expect(result.tokens[0]).toBe(model.vocabulary.indexOf('the'));
  });

  test('training reduces sequence loss', () => {
    const model = buildModel();
    const sequence = model.vocabulary.encode(['the', 'reservoir', 'resonates'], {
      addBos: true,
    });

    const first = model.trainSequence(sequence, 0.3).loss;
    for (let i = 0; i < 100; i += 1) model.trainSequence(sequence, 0.3);
    const last = model.trainSequence(sequence, 0.3).loss;

    expect(last).toBeLessThan(first);
  });

  test('carries generation weights and vocabulary through save and load', () => {
    const model = buildModel();
    const sequence = model.vocabulary.encode(['the', 'reservoir'], { addBos: true });
    for (let i = 0; i < 50; i += 1) model.trainSequence(sequence, 0.3);

    const restored = CompositeLanguageModel.load(
      JSON.parse(JSON.stringify(model.save()))
    );

    expect(restored.vocabulary.indexToToken).toEqual(model.vocabulary.indexToToken);
    expect(restored.generation.projection).toEqual(model.generation.projection);
    expect(restored.generate([4], { maxTokens: 3, temperature: 0 }).tokens).toEqual(
      model.generate([4], { maxTokens: 3, temperature: 0 }).tokens
    );
  });

  test('a persona prior changes generation without any prompt change', () => {
    const model = buildModel();
    const plain = model.generate([4], { maxTokens: 5, temperature: 0 });

    const bias = new Array(model.vocabulary.size).fill(0);
    bias[model.vocabulary.indexOf('resonates')] = 50;
    model.setPersonaPrior({ logitBias: bias });

    const conditioned = model.generate([4], { maxTokens: 5, temperature: 0 });

    expect(conditioned.tokens).not.toEqual(plain.tokens);
    expect(conditioned.tokens[0]).toBe(model.vocabulary.indexOf('resonates'));
  });

  test('causal attention prevents a position from seeing the future', () => {
    const model = buildModel();

    const causal = model.forward([4, 5, 6], false, { causal: true });
    const bidirectional = model.forward([4, 5, 6], false, { causal: false });

    // The first position can only attend to itself under causal masking, so its
    // context vector must differ from the bidirectional case.
    expect(causal.contextVectors[0]).not.toEqual(bidirectional.contextVectors[0]);
    expect(causal.contextVectors[0].every(Number.isFinite)).toBe(true);
  });
});
