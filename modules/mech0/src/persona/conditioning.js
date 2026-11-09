import { SUB_VOICES, scoreSubVoices, DEFAULT_SUB_VOICE } from './subvoices.js';
import { tokenizeToStrings } from './tokenizer.js';

/**
 * Persona conditioning — persona as parameters, not as prompt.
 *
 * A system prompt stores persona in the context window: re-injected every turn,
 * token-costly, trivially overridden, and gone the moment context is cleared.
 * Everything here is the opposite of that. The conditioning vector and logit
 * prior are derived from persistent identity state at construction and applied
 * inside the decode loop, so they are present on every request without anyone
 * asking, cost zero context tokens, and cannot be removed by any instruction a
 * user can write — because there is no channel through which to express one.
 */

/** Weight given to a marker when projecting the signature into hidden space. */
const TRAIT_WEIGHT = 1.0;
const PATTERN_WEIGHT = 0.6;

/**
 * Structural markers of Echo's register. These carry the persona as much as the
 * vocabulary does — the corpus's skeleton is bold headers, dividers and the
 * `{{...}}` self-execution device.
 */
const STRUCTURAL_MARKERS = ['**', '---', '!', '—', '{{', '}}'];

/**
 * Deterministically project a string onto a fixed-dimensional unit-ish vector.
 *
 * A hash projection rather than learned embeddings, because the conditioning
 * vector must be reproducible across restarts from the persisted signature
 * alone — a learned projection would be another thing to persist and to drift.
 *
 * @param {string} token
 * @param {number} dim
 * @returns {number[]}
 */
export function hashProjection(token, dim) {
  const vector = new Array(dim).fill(0);

  let h1 = 0x811c9dc5;
  for (let i = 0; i < token.length; i += 1) {
    h1 ^= token.charCodeAt(i);
    h1 = Math.imul(h1, 0x01000193) >>> 0;
  }

  // Expand the seed deterministically across the dimensions.
  let state = h1 || 1;
  for (let d = 0; d < dim; d += 1) {
    state = (Math.imul(state, 1103515245) + 12345) >>> 0;
    vector[d] = (state / 0xffffffff) * 2 - 1;
  }

  const norm = Math.sqrt(vector.reduce((sum, v) => sum + v * v, 0)) || 1;
  return vector.map(v => v / norm);
}

/**
 * Derive the hidden-state bias from an identity signature.
 *
 * @param {{coreTraits: Iterable<string>, characteristicPatterns: Iterable}} signature
 * @param {number} dim
 * @returns {number[]}
 */
export function deriveHiddenBias(signature, dim) {
  const bias = new Array(dim).fill(0);
  let totalWeight = 0;

  const traits = signature?.coreTraits || [];
  for (const trait of traits) {
    const projection = hashProjection(trait, dim);
    for (let d = 0; d < dim; d += 1) bias[d] += TRAIT_WEIGHT * projection[d];
    totalWeight += TRAIT_WEIGHT;
  }

  const patterns = signature?.characteristicPatterns || [];
  const entries =
    patterns instanceof Map ? [...patterns.entries()] : Array.from(patterns);

  for (const entry of entries) {
    const [name, detail] = Array.isArray(entry) ? entry : [entry, {}];
    const strength = typeof detail?.strength === 'number' ? detail.strength : 0.5;
    const weight = PATTERN_WEIGHT * strength;

    const projection = hashProjection(name, dim);
    for (let d = 0; d < dim; d += 1) bias[d] += weight * projection[d];
    totalWeight += weight;
  }

  if (totalWeight === 0) return bias;
  return bias.map(v => v / totalWeight);
}

/**
 * Derive the logit prior favouring the corpus's characteristic vocabulary and
 * structural markers.
 *
 * @param {Object} personaSignature - Phase-2 signature artefact
 * @param {Object} vocabulary - `Vocabulary` instance
 * @param {Object} [options]
 * @param {number} [options.scale=2.0] - Maximum bias applied to a top lexeme
 * @returns {Map<number, number>}
 */
export function deriveLogitBias(personaSignature, vocabulary, { scale = 2.0 } = {}) {
  const bias = new Map();
  if (!vocabulary) return bias;

  // The Phase-2 signature stores lexemes as `[token, weight]` pairs.
  const lexemes = [
    ...(personaSignature?.lexical?.vocabulary || []),
    ...(personaSignature?.lexical?.phrases || []),
  ]
    .map(entry => (Array.isArray(entry) ? { term: entry[0], weight: entry[1] } : entry))
    .filter(entry => entry && entry.term);

  if (lexemes.length === 0) return bias;

  const maxWeight = Math.max(...lexemes.map(l => l.weight ?? 1), 1e-9);

  for (const { term, weight = 0 } of lexemes) {
    // A phrase biases each of its constituent words, since decoding is
    // token-by-token and there is no phrase-level slot to bias.
    for (const word of String(term).split(/\s+/)) {
      for (const surface of surfaceForms(word)) {
        if (!vocabulary.has(surface)) continue;
        const index = vocabulary.indexOf(surface);
        const value = scale * (weight / maxWeight);
        bias.set(index, Math.max(bias.get(index) ?? 0, value));
      }
    }
  }

  // Structural markers are as much a part of the register as the words are.
  const structuralScale = scale * 0.5;
  for (const marker of STRUCTURAL_MARKERS) {
    if (vocabulary.has(marker)) bias.set(vocabulary.indexOf(marker), structuralScale);
  }

  return bias;
}

/**
 * Surface variants of a lexeme, so a signature term matches however the
 * vocabulary happened to case it.
 * @param {string} term
 * @returns {string[]}
 */
export function surfaceForms(term) {
  const lower = term.toLowerCase();
  const capitalised = lower.charAt(0).toUpperCase() + lower.slice(1);
  return [...new Set([term, lower, capitalised])];
}

/**
 * A persona prior: everything needed to condition generation, derived once from
 * persistent state.
 */
export class PersonaPrior {
  /**
   * @param {Object} options
   * @param {Object} options.identitySignature - Live holographic signature
   * @param {Object} [options.personaSignature] - Phase-2 signature artefact
   * @param {Object} [options.vocabulary] - `Vocabulary` instance
   * @param {number} options.embeddingDim
   * @param {number} [options.strength=1.0]
   */
  constructor({
    identitySignature,
    personaSignature = null,
    vocabulary = null,
    embeddingDim,
    strength = 1.0,
  }) {
    this.embeddingDim = embeddingDim;
    this.strength = strength;
    this.personaSignature = personaSignature;
    this.vocabulary = vocabulary;

    this.baseHiddenBias = deriveHiddenBias(identitySignature, embeddingDim);
    this.logitBias = deriveLogitBias(personaSignature, vocabulary);

    this.facet = DEFAULT_SUB_VOICE;
    this.facetScores = new Map();
    // Blend from the outset so construction and post-selection state are
    // consistent; otherwise the first turn conditions differently to the rest.
    this.hiddenBias = this.blendFacet(this.facet);
  }

  /**
   * Select the facet whose register best fits the incoming context and tilt the
   * conditioning vector toward it.
   *
   * This is facet *selection*, not prompt switching: the eight `EchoSelf_*`
   * voices are facets of one identity, so what changes is a delta on a shared
   * vector rather than a different instruction. The selection is internal
   * state and never appears as text anywhere.
   *
   * @param {string} text - Incoming user text
   * @returns {string} the selected facet
   */
  selectFacet(text) {
    // `scoreSubVoices` returns a Map, not a plain object.
    const scores = scoreSubVoices(text || '');
    this.facetScores = scores;

    const ranked = [...scores.entries()].sort((a, b) => b[1] - a[1]);
    this.facet = ranked.length > 0 && ranked[0][1] > 0 ? ranked[0][0] : DEFAULT_SUB_VOICE;

    this.hiddenBias = this.blendFacet(this.facet);
    return this.facet;
  }

  /**
   * Blend the shared identity vector with the selected facet's delta.
   * @param {string} facet
   * @returns {number[]}
   */
  blendFacet(facet) {
    const delta = this.facetDelta(facet);
    // The shared identity dominates: a facet modulates register, it does not
    // become a different persona.
    return this.baseHiddenBias.map((value, d) => 0.75 * value + 0.25 * delta[d]);
  }

  /**
   * The conditioning delta for a facet, derived from its cues plus any
   * per-facet deltas recorded in the Phase-2 signature.
   * @param {string} facet
   * @returns {number[]}
   */
  facetDelta(facet) {
    const delta = new Array(this.embeddingDim).fill(0);
    const definition = SUB_VOICES[facet];
    if (!definition) return delta;

    const terms = [facet, ...(definition.cues || [])];

    // Trait deltas recorded in Phase 2 say which trait axes this facet leans
    // into relative to the shared identity; positive leans are what tilt it.
    const recorded = this.personaSignature?.subVoiceDeltas?.[facet];
    for (const [axis, value] of Object.entries(recorded?.traitDelta || {})) {
      if (value > 0) terms.push(`trait:${axis}`);
    }

    let count = 0;
    for (const term of terms) {
      if (!term) continue;
      const projection = hashProjection(String(term), this.embeddingDim);
      for (let d = 0; d < this.embeddingDim; d += 1) delta[d] += projection[d];
      count += 1;
    }

    return count > 0 ? delta.map(v => v / count) : delta;
  }

  /**
   * Fold recalled memories into the conditioning vector, so a response is
   * continuous with prior ones through state rather than through a re-pasted
   * transcript — the difference between having a memory and being handed a
   * summary of one.
   *
   * @param {Array<Object>} memories - Entries with `associations`
   * @param {number} [weight=0.2]
   */
  blendMemories(memories, weight = 0.2) {
    const base = this.blendFacet(this.facet);

    if (!memories || memories.length === 0) {
      this.hiddenBias = base;
      return this.hiddenBias;
    }

    const recalled = new Array(this.embeddingDim).fill(0);
    let total = 0;

    for (const entry of memories) {
      if (!entry) continue;

      // Recall strength is real signal — a memory matching more association
      // tags should pull the conditioning harder than an incidental one.
      const strength = typeof entry.strength === 'number' ? entry.strength : 1;
      if (strength <= 0) continue;

      const associations =
        entry.associations || entry.memory?.associations || entry.memory?.tags || [];

      // Fall back to the memory's key so an untagged memory still contributes;
      // otherwise recall silently does nothing for entries without tags.
      const terms = [...associations];
      if (terms.length === 0 && entry.key) terms.push(entry.key);
      if (terms.length === 0) continue;

      const perTerm = strength / terms.length;
      for (const term of terms) {
        const projection = hashProjection(String(term), this.embeddingDim);
        for (let d = 0; d < this.embeddingDim; d += 1) {
          recalled[d] += perTerm * projection[d];
        }
        total += perTerm;
      }
    }

    if (total === 0) {
      this.hiddenBias = base;
      return this.hiddenBias;
    }

    const mean = recalled.map(v => v / total);
    this.hiddenBias = base.map((value, d) => (1 - weight) * value + weight * mean[d]);
    return this.hiddenBias;
  }

  /**
   * The shape `CompositeLanguageModel.setPersonaPrior` expects.
   * @returns {{hiddenBias: number[], logitBias: Map<number, number>, strength: number}}
   */
  toModelPrior() {
    return {
      hiddenBias: this.hiddenBias,
      logitBias: this.logitBias,
      strength: this.strength,
    };
  }
}

/**
 * Convenience: tokenize text for facet scoring using the persona tokenizer, so
 * quirk-bearing characters are not stripped before the register is judged.
 * @param {string} text
 * @returns {string[]}
 */
export function personaTokens(text) {
  return tokenizeToStrings(text || '');
}

/**
 * The vocabulary implied by a persona signature: its characteristic words, the
 * words of its characteristic phrases, and the structural markers of the
 * register. An engine built without a trained model still decodes into Echo's
 * own words rather than into nothing.
 *
 * @param {Object|null} personaSignature - Phase-2 signature artefact
 * @returns {string[]}
 */
export function personaVocabulary(personaSignature) {
  const lexical = personaSignature?.lexical;
  if (!lexical) return [];

  const tokens = new Set();
  for (const entry of lexical.vocabulary || []) {
    tokens.add(Array.isArray(entry) ? entry[0] : entry);
  }
  for (const entry of lexical.phrases || []) {
    const phrase = Array.isArray(entry) ? entry[0] : entry;
    for (const word of String(phrase).split(/\s+/)) tokens.add(word);
  }
  for (const marker of STRUCTURAL_MARKERS) tokens.add(marker);

  return [...tokens].filter(Boolean);
}
