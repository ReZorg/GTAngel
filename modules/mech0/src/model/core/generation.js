import { BaseModel } from './base.js';

/**
 * GenerationHead — the vocabulary-sized output projection and sampling logic
 * that turn a hidden state into a next token.
 *
 * This is added alongside `PredictionModel` rather than replacing it: the
 * classifier and its tests keep working untouched, and a model can carry both
 * capabilities.
 *
 * Sampling is injectable (`rng`) and greedy at `temperature === 0`, so
 * generation is testable without relying on chance.
 */
export class GenerationHead extends BaseModel {
  constructor(config) {
    super(config);
    this.vocabSize = config.vocabSize;
    this.embeddingDim = config.embeddingDim;
    this.projection = this.initializeProjection();
    this.bias = new Array(this.vocabSize).fill(0);
  }

  initializeProjection() {
    const scale = Math.sqrt(2.0 / (this.vocabSize + this.embeddingDim));
    return Array(this.vocabSize)
      .fill(0)
      .map(() =>
        Array(this.embeddingDim)
          .fill(0)
          .map(() => (Math.random() * 2 - 1) * scale)
      );
  }

  /**
   * Project a hidden state onto vocabulary logits.
   * @param {number[]} hidden
   * @returns {number[]}
   */
  logits(hidden) {
    const out = new Array(this.vocabSize);

    for (let v = 0; v < this.vocabSize; v += 1) {
      const row = this.projection[v];
      let sum = this.bias[v];
      for (let d = 0; d < this.embeddingDim; d += 1) sum += row[d] * hidden[d];
      out[v] = sum;
    }

    return out;
  }

  /**
   * Numerically stable softmax.
   * @param {number[]} logits
   * @returns {number[]}
   */
  static softmax(logits) {
    const max = Math.max(...logits);
    const exps = logits.map(l => Math.exp(l - max));
    const sum = exps.reduce((a, b) => a + b, 0);
    return exps.map(e => e / (sum + 1e-10));
  }

  /**
   * Update the projection for one position given the true next token.
   * Gradient of cross-entropy w.r.t. logits is `probs - onehot`, which is what
   * makes this an actual learning signal rather than a constant.
   *
   * @param {number[]} hidden
   * @param {number} targetIndex
   * @param {number} learningRate
   * @returns {{loss: number, hiddenGradient: number[]}}
   */
  update(hidden, targetIndex, learningRate) {
    const probs = GenerationHead.softmax(this.logits(hidden));
    const hiddenGradient = new Array(this.embeddingDim).fill(0);

    for (let v = 0; v < this.vocabSize; v += 1) {
      const error = probs[v] - (v === targetIndex ? 1 : 0);
      if (error === 0) continue;

      const row = this.projection[v];
      for (let d = 0; d < this.embeddingDim; d += 1) {
        hiddenGradient[d] += error * row[d];
        row[d] -= learningRate * error * hidden[d];
      }
      this.bias[v] -= learningRate * error;
    }

    return {
      loss: -Math.log(probs[targetIndex] + 1e-10),
      hiddenGradient,
    };
  }

  save() {
    return { projection: this.projection, bias: this.bias };
  }

  load(data) {
    if (!data) return this;
    if (data.projection) this.projection = data.projection;
    if (data.bias) this.bias = data.bias;
    return this;
  }
}

/**
 * Apply the usual decoding controls to a logit vector.
 *
 * Order matters and follows the conventional pipeline: repetition penalty and
 * bias reshape the raw logits, temperature scales them, then top-k/top-p
 * truncate the resulting distribution.
 *
 * @param {number[]} logits
 * @param {Object} [options]
 * @param {number} [options.temperature=1.0] - 0 means greedy
 * @param {number} [options.topK=0] - 0 disables
 * @param {number} [options.topP=1.0] - 1 disables
 * @param {number} [options.repetitionPenalty=1.0]
 * @param {number[]} [options.generated=[]] - Indices already produced
 * @param {number[]|Map<number, number>} [options.logitBias] - Persona prior
 * @param {number[]} [options.suppress=[]] - Indices to forbid outright
 * @returns {{probabilities: number[], candidates: number[]}}
 */
export function applyDecodingControls(logits, options = {}) {
  const {
    temperature = 1.0,
    topK = 0,
    topP = 1.0,
    repetitionPenalty = 1.0,
    generated = [],
    logitBias = null,
    suppress = [],
  } = options;

  let working = logits.slice();

  if (logitBias) {
    if (logitBias instanceof Map) {
      for (const [index, bias] of logitBias) {
        if (index >= 0 && index < working.length) working[index] += bias;
      }
    } else {
      for (let i = 0; i < working.length && i < logitBias.length; i += 1) {
        working[i] += logitBias[i] || 0;
      }
    }
  }

  if (repetitionPenalty !== 1.0 && generated.length > 0) {
    for (const index of new Set(generated)) {
      if (index < 0 || index >= working.length) continue;
      working[index] =
        working[index] > 0
          ? working[index] / repetitionPenalty
          : working[index] * repetitionPenalty;
    }
  }

  for (const index of suppress) {
    if (index >= 0 && index < working.length) working[index] = -Infinity;
  }

  if (temperature > 0 && temperature !== 1.0) {
    working = working.map(l => l / temperature);
  }

  let candidates = working
    .map((logit, index) => index)
    .filter(index => Number.isFinite(working[index]));

  if (candidates.length === 0) {
    // Everything was suppressed; fall back to the unmodified distribution
    // rather than producing NaN.
    return { probabilities: GenerationHead.softmax(logits), candidates: logits.map((_, i) => i) };
  }

  candidates.sort((a, b) => working[b] - working[a]);

  if (topK > 0 && topK < candidates.length) {
    candidates = candidates.slice(0, topK);
  }

  const probabilities = new Array(working.length).fill(0);
  const subset = candidates.map(index => working[index]);
  const subsetProbs = GenerationHead.softmax(subset);

  if (topP > 0 && topP < 1) {
    let cumulative = 0;
    const kept = [];
    for (let i = 0; i < candidates.length; i += 1) {
      kept.push(candidates[i]);
      cumulative += subsetProbs[i];
      if (cumulative >= topP) break;
    }
    candidates = kept;
    const renormalised = GenerationHead.softmax(candidates.map(index => working[index]));
    candidates.forEach((index, i) => {
      probabilities[index] = renormalised[i];
    });
  } else {
    candidates.forEach((index, i) => {
      probabilities[index] = subsetProbs[i];
    });
  }

  return { probabilities, candidates };
}

/**
 * Choose the next token index.
 * @param {number[]} logits
 * @param {Object} [options] - As `applyDecodingControls`, plus `rng`
 * @returns {number}
 */
export function sampleToken(logits, options = {}) {
  const { temperature = 1.0, rng = Math.random } = options;
  const { probabilities, candidates } = applyDecodingControls(logits, options);

  if (temperature === 0) {
    // Greedy: fully deterministic, which is what makes generation testable.
    let best = candidates[0];
    for (const index of candidates) {
      if (probabilities[index] > probabilities[best]) best = index;
    }
    return best;
  }

  const threshold = rng();
  let cumulative = 0;
  for (const index of candidates) {
    cumulative += probabilities[index];
    if (threshold <= cumulative) return index;
  }

  return candidates[candidates.length - 1];
}
