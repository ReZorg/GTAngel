import { BaseModel } from './base.js';
import { multiply, dot } from 'mathjs';

export class AttentionModel extends BaseModel {
  constructor(config) {
    super(config);
    this.queryWeight = this.initializeWeight();
    this.keyWeight = this.initializeWeight();
    this.valueWeight = this.initializeWeight();
  }

  initializeWeight() {
    const scale = Math.sqrt(2.0 / (this.config.embeddingDim + this.config.embeddingDim));
    return Array(this.config.embeddingDim).fill(0).map(() =>
      Array(this.config.embeddingDim).fill(0).map(() =>
        (Math.random() * 2 - 1) * scale
      )
    );
  }

  forward(embeddings, options = {}) {
    const { causal = false } = options;

    const queries = embeddings.map(emb => multiply(emb, this.queryWeight));
    const keys = embeddings.map(emb => multiply(emb, this.keyWeight));
    const values = embeddings.map(emb => multiply(emb, this.valueWeight));

    let scores = queries.map(q =>
      keys.map(k => dot(q, k) / Math.sqrt(this.config.embeddingDim))
    );

    // Autoregressive training requires that position i cannot see position j > i;
    // without this the model learns from tokens it will not have at decode time.
    if (causal) {
      scores = scores.map((row, i) =>
        row.map((score, j) => (j > i ? -Infinity : score))
      );
    }

    const weights = this.softmax(scores);
    return this.computeContext(weights, values);
  }

  softmax(scores) {
    const finite = scores.flat().filter(Number.isFinite);
    const maxScore = finite.length > 0 ? Math.max(...finite) : 0;
    const expScores = scores.map(row =>
      row.map(score => (Number.isFinite(score) ? Math.exp(score - maxScore) : 0))
    );

    const sumExp = expScores.map(row =>
      row.reduce((sum, exp) => sum + exp, 0)
    );

    return expScores.map((row, i) =>
      row.map(exp => exp / (sumExp[i] + 1e-10))
    );
  }

  computeContext(weights, values) {
    return weights.map((weightRow, i) =>
      values.reduce((context, value, j) =>
        context.map((c, k) => c + value[k] * weightRow[j]),
        new Array(this.config.embeddingDim).fill(0)
      )
    );
  }
}