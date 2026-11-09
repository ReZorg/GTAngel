/**
 * Vocabulary — a first-class, persistable artefact.
 *
 * Generation needs index→token, not just a size. Until now the string
 * vocabulary existed only inside the training pipeline and the model kept only
 * `vocabSize`, which makes decoding impossible: you can produce an index but
 * you cannot say what word it was. This module makes the mapping explicit,
 * bidirectional, O(1) in both directions, and saved alongside the model.
 */

export const SPECIAL_TOKENS = {
  PAD: '<pad>',
  UNK: '<unk>',
  BOS: '<bos>',
  EOS: '<eos>',
};

/** Order matters: these occupy indices 0..3 and must stay stable across builds. */
export const SPECIAL_TOKEN_LIST = [
  SPECIAL_TOKENS.PAD,
  SPECIAL_TOKENS.UNK,
  SPECIAL_TOKENS.BOS,
  SPECIAL_TOKENS.EOS,
];

export const VOCABULARY_SCHEMA_VERSION = 1;

export class Vocabulary {
  /**
   * @param {string[]} [tokens] - Ordered tokens; specials are prepended if absent
   */
  constructor(tokens = []) {
    this.indexToToken = [];
    this.tokenToIndex = new Map();
    this.frequencies = new Map();

    for (const special of SPECIAL_TOKEN_LIST) this.add(special);
    for (const token of tokens) this.add(token);
  }

  get size() {
    return this.indexToToken.length;
  }

  get padIndex() {
    return this.tokenToIndex.get(SPECIAL_TOKENS.PAD);
  }

  get unkIndex() {
    return this.tokenToIndex.get(SPECIAL_TOKENS.UNK);
  }

  get bosIndex() {
    return this.tokenToIndex.get(SPECIAL_TOKENS.BOS);
  }

  get eosIndex() {
    return this.tokenToIndex.get(SPECIAL_TOKENS.EOS);
  }

  /**
   * Add a token, returning its index. Idempotent.
   * @param {string} token
   * @returns {number}
   */
  add(token) {
    const existing = this.tokenToIndex.get(token);
    if (existing !== undefined) return existing;

    const index = this.indexToToken.length;
    this.indexToToken.push(token);
    this.tokenToIndex.set(token, index);
    return index;
  }

  /**
   * @param {string} token
   * @returns {boolean}
   */
  has(token) {
    return this.tokenToIndex.has(token);
  }

  /**
   * Index of a token, falling back to `<unk>`.
   * @param {string} token
   * @returns {number}
   */
  indexOf(token) {
    const index = this.tokenToIndex.get(token);
    return index === undefined ? this.unkIndex : index;
  }

  /**
   * Token at an index, falling back to `<unk>`.
   * @param {number} index
   * @returns {string}
   */
  tokenAt(index) {
    return this.indexToToken[index] ?? SPECIAL_TOKENS.UNK;
  }

  /**
   * @param {string[]} tokens
   * @param {Object} [options]
   * @param {boolean} [options.addBos=false]
   * @param {boolean} [options.addEos=false]
   * @returns {number[]}
   */
  encode(tokens, { addBos = false, addEos = false } = {}) {
    const indices = tokens.map(token => this.indexOf(token));
    if (addBos) indices.unshift(this.bosIndex);
    if (addEos) indices.push(this.eosIndex);
    return indices;
  }

  /**
   * @param {number[]} indices
   * @param {Object} [options]
   * @param {boolean} [options.skipSpecial=true]
   * @returns {string[]}
   */
  decode(indices, { skipSpecial = true } = {}) {
    const tokens = indices.map(index => this.tokenAt(index));
    if (!skipSpecial) return tokens;
    return tokens.filter(token => !SPECIAL_TOKEN_LIST.includes(token));
  }

  /**
   * Build a vocabulary from token sequences.
   *
   * Uses a `Map` for counting rather than `Array.indexOf`, which was O(n) per
   * token and made corpus-scale vocabularies quadratic.
   *
   * @param {Iterable<string[]>} sequences
   * @param {Object} [options]
   * @param {number} [options.minFrequency=1]
   * @param {number} [options.maxSize=Infinity]
   * @returns {Vocabulary}
   */
  static build(sequences, { minFrequency = 1, maxSize = Infinity } = {}) {
    const counts = new Map();

    for (const sequence of sequences) {
      for (const token of sequence) {
        counts.set(token, (counts.get(token) || 0) + 1);
      }
    }

    const ranked = [...counts.entries()]
      .filter(([, count]) => count >= minFrequency)
      // Frequency first, then lexicographic, so builds are reproducible.
      .sort((a, b) => b[1] - a[1] || (a[0] < b[0] ? -1 : a[0] > b[0] ? 1 : 0));

    const budget = Number.isFinite(maxSize)
      ? Math.max(0, maxSize - SPECIAL_TOKEN_LIST.length)
      : ranked.length;

    const vocabulary = new Vocabulary(ranked.slice(0, budget).map(([token]) => token));
    for (const [token, count] of ranked) vocabulary.frequencies.set(token, count);

    return vocabulary;
  }

  /**
   * @returns {{schemaVersion: number, tokens: string[], frequencies: Array}}
   */
  toJSON() {
    return {
      schemaVersion: VOCABULARY_SCHEMA_VERSION,
      tokens: this.indexToToken,
      frequencies: [...this.frequencies.entries()],
    };
  }

  /**
   * @param {Object} data
   * @returns {Vocabulary}
   */
  static fromJSON(data) {
    if (!data) return new Vocabulary();

    // Tolerate the legacy plain-array vocabulary format produced before this
    // artefact existed, so old model files still load.
    const tokens = Array.isArray(data) ? data : data.tokens || [];

    const vocabulary = new Vocabulary();
    vocabulary.indexToToken = [];
    vocabulary.tokenToIndex = new Map();
    for (const token of tokens) vocabulary.add(token);

    // A legacy vocabulary has no specials; add them so decoding still works.
    for (const special of SPECIAL_TOKEN_LIST) vocabulary.add(special);

    if (!Array.isArray(data) && Array.isArray(data.frequencies)) {
      vocabulary.frequencies = new Map(data.frequencies);
    }

    return vocabulary;
  }
}
