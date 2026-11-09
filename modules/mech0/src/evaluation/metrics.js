import { validateModel } from '../utils/validation.js';
import logger from '../utils/logger.js';

export class ModelEvaluator {
  constructor(model) {
    validateModel(model);
    this.model = model;
  }

  calculateConfusionMatrix(predictions, actualLabels) {
    const labels = this.model.labels;
    const matrix = {};
    
    labels.forEach(actual => {
      matrix[actual] = {};
      labels.forEach(predicted => {
        matrix[actual][predicted] = 0;
      });
    });

    predictions.forEach((pred, i) => {
      const predicted = pred.label;
      const actual = actualLabels[i];
      matrix[actual][predicted]++;
    });

    return matrix;
  }

  calculatePerLabelMetrics(confusionMatrix) {
    const metrics = {};
    
    this.model.labels.forEach(label => {
      const tp = confusionMatrix[label][label];
      let fp = 0, fn = 0;
      
      this.model.labels.forEach(otherLabel => {
        if (otherLabel !== label) {
          fp += confusionMatrix[otherLabel][label];
          fn += confusionMatrix[label][otherLabel];
        }
      });

      const precision = tp / (tp + fp) || 0;
      const recall = tp / (tp + fn) || 0;
      const f1 = 2 * (precision * recall) / (precision + recall) || 0;

      metrics[label] = { precision, recall, f1 };
    });

    return metrics;
  }

  evaluateModel(testData) {
    const predictions = [];
    const actualLabels = [];
    let totalLoss = 0;

    testData.forEach(item => {
      const prediction = this.predictSingle(item);
      predictions.push(prediction);
      actualLabels.push(item.label);
      totalLoss += this.calculateLoss(prediction, item.label);
    });

    const confusionMatrix = this.calculateConfusionMatrix(predictions, actualLabels);
    const labelMetrics = this.calculatePerLabelMetrics(confusionMatrix);
    
    const accuracy = predictions.filter((p, i) => 
      p.label === actualLabels[i]
    ).length / predictions.length;

    const avgLoss = totalLoss / testData.length;

    const results = {
      accuracy,
      averageLoss: avgLoss,
      confusionMatrix,
      labelMetrics,
      sampleCount: testData.length
    };

    logger.info('Model evaluation completed', results);
    return results;
  }

  /**
   * Run the model. This previously returned a uniformly random label with a
   * random probability, which made every accuracy, loss and confusion-matrix
   * figure it produced meaningless.
   *
   * Where the model cannot actually be run, fall back to a deterministic
   * uniform distribution rather than to chance: an evaluation that cannot
   * measure anything should at least be reproducible and visibly uninformative.
   */
  predictSingle(item) {
    const predictions = this.runModel(item);

    if (!predictions || predictions.length === 0) {
      const uniform = 1 / this.model.labels.length;
      return {
        label: this.model.labels[0],
        probability: uniform,
        predictions: this.model.labels.map(label => ({ label, probability: uniform })),
        degraded: true
      };
    }

    const best = predictions.reduce((max, p) =>
      p.probability > max.probability ? p : max
    );

    return { label: best.label, probability: best.probability, predictions };
  }

  /**
   * Obtain label probabilities from whichever model shape was supplied.
   * @param {Object} item
   * @returns {Array<{label: string, probability: number}>|null}
   */
  runModel(item) {
    const tokens = item.encodedTokens || item.tokens || [];

    try {
      if (typeof this.model.forward !== 'function') return null;

      const output = this.model.forward(tokens, false);

      // `SimpleLanguageModel` splits forward and predict; `CompositeLanguageModel`
      // returns predictions directly from forward.
      if (output?.predictions) return output.predictions;
      if (typeof this.model.predict === 'function') return this.model.predict(output);

      return Array.isArray(output) ? output : null;
    } catch {
      return null;
    }
  }

  calculateLoss(prediction, actualLabel) {
    // Cross-entropy against the probability assigned to the *true* label, not
    // to whichever label happened to win.
    const match = (prediction.predictions || []).find(p => p.label === actualLabel);
    const probability = match
      ? match.probability
      : prediction.label === actualLabel
        ? prediction.probability
        : 1 - prediction.probability;

    return -Math.log(Math.max(probability, 0) + 1e-10);
  }
}