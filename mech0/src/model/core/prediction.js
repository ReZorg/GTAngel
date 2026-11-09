import { BaseModel } from './base.js';

export class PredictionModel extends BaseModel {
  constructor(config) {
    super(config);
    this.labels = config.labels || ['greeting', 'farewell', 'question'];
    this.weights = this.initializeWeights();
  }

  /**
   * One weight vector per label. Previously scoring was `Math.random()`, which
   * meant the classifier never used its input at all and every evaluation
   * number derived from it was noise.
   */
  initializeWeights() {
    const scale = Math.sqrt(2.0 / (this.labels.length + this.config.embeddingDim));
    return this.labels.map(() =>
      Array(this.config.embeddingDim)
        .fill(0)
        .map(() => (Math.random() * 2 - 1) * scale)
    );
  }

  predict(embeddings) {
    if (!embeddings || embeddings.length === 0) {
      const uniform = 1 / this.labels.length;
      return this.labels.map(label => ({ label, probability: uniform }));
    }

    const avgEmbedding = embeddings.reduce((sum, emb) =>
      sum.map((val, i) => val + emb[i]),
      new Array(this.config.embeddingDim).fill(0)
    ).map(val => val / embeddings.length);

    const scores = this.labels.map((label, index) => ({
      label,
      score: this.weights[index].reduce(
        (sum, weight, i) => sum + weight * avgEmbedding[i],
        0
      )
    }));

    const maxScore = Math.max(...scores.map(s => s.score));
    const expScores = scores.map(s => ({
      ...s,
      score: Math.exp(s.score - maxScore)
    }));

    const sumExp = expScores.reduce((sum, s) => sum + s.score, 0);
    
    return expScores.map(s => ({
      label: s.label,
      probability: s.score / sumExp
    }));
  }

  /**
   * Cross-entropy update. Returns the gradient w.r.t. the pooled hidden state so
   * the rest of the model can learn from classification too.
   * @param {number[]} avgEmbedding
   * @param {string} actualLabel
   * @param {number} learningRate
   * @returns {{loss: number, hiddenGradient: number[]}}
   */
  update(avgEmbedding, actualLabel, learningRate = 0.01) {
    const probabilities = this.predict([avgEmbedding]);
    const targetIndex = this.labels.indexOf(actualLabel);
    const hiddenGradient = new Array(this.config.embeddingDim).fill(0);

    this.labels.forEach((label, index) => {
      const error = probabilities[index].probability - (index === targetIndex ? 1 : 0);
      const weights = this.weights[index];
      for (let i = 0; i < weights.length; i += 1) {
        hiddenGradient[i] += error * weights[i];
        weights[i] -= learningRate * error * avgEmbedding[i];
      }
    });

    const targetProbability =
      targetIndex >= 0 ? probabilities[targetIndex].probability : 1 / this.labels.length;

    return { loss: -Math.log(targetProbability + 1e-10), hiddenGradient };
  }
}