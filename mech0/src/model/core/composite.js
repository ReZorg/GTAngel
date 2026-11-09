import { BaseModel } from './base.js';
import { EmbeddingModel } from './embeddings.js';
import { AttentionModel } from './attention.js';
import { NormalizationModel } from './normalization.js';
import { RegularizationModel } from './regularization.js';
import { PredictionModel } from './prediction.js';
import { GenerationHead, sampleToken } from './generation.js';
import { Vocabulary } from './vocabulary.js';

export class CompositeLanguageModel extends BaseModel {
  constructor(config) {
    super(config);
    this.embedding = new EmbeddingModel(config);
    this.attention = new AttentionModel(config);
    this.normalization = new NormalizationModel(config);
    this.regularization = new RegularizationModel(config);
    this.prediction = new PredictionModel(config);

    // Generation is an additional capability, not a replacement: classification
    // and its tests are untouched by its presence.
    this.generation = new GenerationHead(config);

    // Decoding needs index→token, so the vocabulary travels with the model.
    this.vocabulary =
      config.vocabulary instanceof Vocabulary
        ? config.vocabulary
        : config.vocabulary
          ? Vocabulary.fromJSON(config.vocabulary)
          : null;

    // A persona prior applied at decode time. Set once from persistent identity
    // state at construction, so it is present on every request without anyone
    // passing it, and no request-level instruction can remove it.
    this.personaPrior = null;
  }

  forward(tokenIndices, training = true, options = {}) {
    const { causal = false } = options;

    // Get embeddings
    let embeddings = this.embedding.forward(tokenIndices);

    // Apply dropout during training
    embeddings = this.regularization.dropout(embeddings, training);

    // Apply layer normalization
    embeddings = this.normalization.normalize(embeddings);

    // Apply attention mechanism
    let contextVectors = this.attention.forward(embeddings, { causal });

    if (this.personaPrior?.hiddenBias) {
      contextVectors = this.applyPersonaPrior(contextVectors);
    }

    return {
      embeddings,
      contextVectors,
      predictions: this.prediction.predict(contextVectors)
    };
  }

  /**
   * Additive conditioning of the hidden state. A parameter-level intervention:
   * it costs zero context tokens and survives any instruction the user gives.
   * @param {number[][]} contextVectors
   * @returns {number[][]}
   */
  applyPersonaPrior(contextVectors) {
    const { hiddenBias, strength = 1.0 } = this.personaPrior;
    return contextVectors.map(vector =>
      vector.map((value, i) => value + strength * (hiddenBias[i] || 0))
    );
  }

  /**
   * Install a persona prior derived from persistent identity state.
   * @param {{hiddenBias?: number[], logitBias?: Map<number, number>|number[], strength?: number}} prior
   */
  setPersonaPrior(prior) {
    this.personaPrior = prior || null;
    return this;
  }

  /**
   * Autoregressively decode a continuation.
   *
   * @param {number[]} promptIndices - May be empty; an empty prompt is the
   *   decisive persona test, since anything Echo-shaped then came from the
   *   weights and the prior rather than from context.
   * @param {Object} [options]
   * @param {number} [options.maxTokens=48]
   * @param {number} [options.temperature=1.0]
   * @param {number} [options.topK=0]
   * @param {number} [options.topP=1.0]
   * @param {number} [options.repetitionPenalty=1.0]
   * @param {number} [options.maxContext=128] - Sliding window over the prefix
   * @param {number} [options.eosIndex]
   * @param {Function} [options.rng]
   * @returns {{tokens: number[], text: string|null, finishReason: string}}
   */
  generate(promptIndices = [], options = {}) {
    const {
      maxTokens = 48,
      maxContext = this.config.maxContext || 128,
      eosIndex = this.vocabulary?.eosIndex,
      rng = Math.random,
    } = options;

    const bos = this.vocabulary?.bosIndex ?? 0;
    const context = promptIndices.length > 0 ? [...promptIndices] : [bos];
    const generated = [];

    const logitBias = options.logitBias ?? this.personaPrior?.logitBias ?? null;
    let finishReason = 'length';

    for (let step = 0; step < maxTokens; step += 1) {
      const window = context.slice(-maxContext);
      const { contextVectors } = this.forward(window, false, { causal: true });
      const hidden = contextVectors[contextVectors.length - 1];

      const next = sampleToken(this.generation.logits(hidden), {
        ...options,
        generated,
        logitBias,
        rng,
      });

      if (eosIndex !== undefined && next === eosIndex) {
        finishReason = 'eos';
        break;
      }

      generated.push(next);
      context.push(next);
    }

    return {
      tokens: generated,
      text: this.vocabulary ? this.vocabulary.decode(generated).join(' ') : null,
      finishReason,
    };
  }

  /**
   * One teacher-forced training step over a token sequence.
   * @param {number[]} tokenIndices
   * @param {number} learningRate
   * @returns {{loss: number, tokens: number}}
   */
  trainSequence(tokenIndices, learningRate = 0.01) {
    if (tokenIndices.length < 2) return { loss: 0, tokens: 0 };

    const inputs = tokenIndices.slice(0, -1);
    const targets = tokenIndices.slice(1);

    const { contextVectors } = this.forward(inputs, true, { causal: true });

    let totalLoss = 0;
    for (let i = 0; i < targets.length; i += 1) {
      const { loss, hiddenGradient } = this.generation.update(
        contextVectors[i],
        targets[i],
        learningRate
      );
      totalLoss += loss;

      // Propagate into the embedding of the token that produced this position,
      // which is the only path by which the body of the model can learn here.
      this.embedding.updateEmbedding(inputs[i], hiddenGradient, learningRate);
    }

    return { loss: totalLoss / targets.length, tokens: targets.length };
  }

  backward(gradients, learningRate) {
    // Clip gradients
    const clippedGradients = this.regularization.gradientClipping(gradients);

    // Update embeddings
    gradients.tokenIndices.forEach((tokenIndex, i) => {
      this.embedding.updateEmbedding(
        tokenIndex,
        clippedGradients.embeddings[i],
        learningRate
      );
    });

    // Update normalization parameters
    this.normalization.updateNormParameters(
      gradients.normalization,
      learningRate
    );
  }

  save() {
    return {
      ...super.save(),
      embedding: this.embedding.embeddings,
      attention: {
        queryWeight: this.attention.queryWeight,
        keyWeight: this.attention.keyWeight,
        valueWeight: this.attention.valueWeight
      },
      normalization: this.normalization.layerNorm,
      generation: this.generation.save(),
      vocabulary: this.vocabulary ? this.vocabulary.toJSON() : null,
      config: {
        ...this.config,
        // The vocabulary is saved as a sibling artefact; keeping a copy nested
        // in config as well would double the file size for no benefit.
        vocabulary: undefined,
        dropoutRate: this.regularization.dropoutRate,
        l2Lambda: this.regularization.l2Lambda
      }
    };
  }

  static load(data) {
    const model = new CompositeLanguageModel(data.config);
    model.embedding.embeddings = data.embedding;
    model.attention.queryWeight = data.attention.queryWeight;
    model.attention.keyWeight = data.attention.keyWeight;
    model.attention.valueWeight = data.attention.valueWeight;
    model.normalization.layerNorm = data.normalization;

    if (data.generation) model.generation.load(data.generation);
    if (data.vocabulary) model.vocabulary = Vocabulary.fromJSON(data.vocabulary);

    return model;
  }
}