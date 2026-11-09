import { MetricsTracker } from '../metrics/tracker.js';
import { TrainingMonitor } from '../metrics/monitor.js';
import { ModelValidator } from '../evaluation/validator.js';
import { ModelEvaluator } from '../evaluation/evaluator.js';
import { Optimizer } from '../optimization/optimizer.js';
import { LearningRateScheduler } from '../optimization/scheduler.js';
import logger from '../../../utils/logger.js';

export class ModelTrainer {
  constructor(config) {
    this.config = config;
    this.metricsTracker = new MetricsTracker();
    this.monitor = new TrainingMonitor(config);
    this.validator = new ModelValidator(config);
    this.evaluator = new ModelEvaluator(config);
    this.optimizer = new Optimizer(config);
    this.scheduler = new LearningRateScheduler(config);
  }

  async train(model, trainData, validationData) {
    try {
      // Validate inputs
      this.validator.validateArchitecture(model);
      this.validator.validateTrainingData(trainData);

      logger.info('Starting training process');

      for (let epoch = 0; epoch < this.config.epochs; epoch++) {
        // Reset metrics for new epoch
        this.metricsTracker.reset();

        // Train on batches
        await this.trainEpoch(model, trainData);

        // Get epoch metrics
        const epochMetrics = this.metricsTracker.getEpochMetrics();
        logger.info(`Epoch ${epoch + 1}, Metrics:`, epochMetrics);

        // Evaluate on validation set
        if (validationData) {
          const validationResults = this.evaluator.evaluate(model, validationData);
          this.monitor.check(validationResults);

          // Update learning rate
          const newLr = this.scheduler.step(validationResults.averageLoss);
          this.optimizer.learningRate = newLr;
        }

        // Check early stopping
        if (this.monitor.shouldStop) {
          logger.info('Early stopping triggered');
          break;
        }
      }

      return {
        finalMetrics: this.metricsTracker.getBestMetrics(),
        history: this.metricsTracker.history
      };

    } catch (error) {
      logger.error('Training failed:', error);
      throw error;
    }
  }

  async trainEpoch(model, trainData) {
    // Shuffle training data
    const shuffled = [...trainData].sort(() => Math.random() - 0.5);

    for (let i = 0; i < shuffled.length; i += this.config.batchSize) {
      const batch = shuffled.slice(i, i + this.config.batchSize);
      await this.trainBatch(model, batch);
    }
  }

  async trainBatch(model, batch) {
    // Forward pass
    const batchResults = batch.map(item => {
      const output = model.forward(item.tokens, true);
      return {
        item,
        output,
        loss: this.calculateLoss(output.predictions, item.label)
      };
    });

    // Calculate gradients
    const gradients = this.calculateGradients(batchResults);

    // Update model parameters
    this.updateModel(model, gradients);

    // Update metrics
    batchResults.forEach(result => {
      this.metricsTracker.update(
        result.loss,
        result.output.predictions,
        result.item.label
      );
    });
  }

  calculateLoss(predictions, actualLabel) {
    const match = predictions.find(p => p.label === actualLabel);
    const actualProb = match ? match.probability : (predictions.length > 0 ? predictions[0].probability : 0.5);
    return -Math.log((actualProb ?? 0.5) + 1e-10);
  }

  /**
   * Cross-entropy gradients derived from the actual prediction error.
   *
   * This previously returned a constant `0.1` in every dimension, which meant
   * every parameter moved by the same amount in the same direction regardless
   * of the data: the model could not learn, and any training curve it produced
   * was an artefact of the optimizer, not of learning.
   */
  calculateGradients(batchResults) {
    const dim = this.config.embeddingDim;
    const zeros = () => new Array(dim).fill(0);

    const embeddings = [];
    const attention = { query: zeros(), key: zeros(), value: zeros() };
    const normalization = { gamma: zeros(), beta: zeros() };

    for (const result of batchResults) {
      const { output, item } = result;
      const contextVectors = output.contextVectors || [];

      // Pooled hidden state for this example.
      const pooled = zeros();
      for (const vector of contextVectors) {
        for (let i = 0; i < dim; i += 1) pooled[i] += (vector[i] || 0) / contextVectors.length;
      }

      // dL/dlogit = p - y, projected back onto the hidden state.
      const error = zeros();
      for (const prediction of output.predictions || []) {
        const signal = prediction.probability - (prediction.label === item.label ? 1 : 0);
        for (let i = 0; i < dim; i += 1) error[i] += signal * pooled[i];
      }

      embeddings.push(error);

      for (let i = 0; i < dim; i += 1) {
        attention.query[i] += error[i] / batchResults.length;
        attention.key[i] += error[i] / batchResults.length;
        attention.value[i] += error[i] / batchResults.length;
        normalization.gamma[i] += (error[i] * pooled[i]) / batchResults.length;
        normalization.beta[i] += error[i] / batchResults.length;
      }
    }

    if (embeddings.length === 0) embeddings.push(zeros());

    return { embeddings, attention, normalization };
  }

  async updateModelFromBatch(model, batchResults) {
    const gradients = this.calculateGradients(batchResults);
    this.updateModel(model, gradients);
  }

  updateModel(model, gradients) {
    // Update embeddings
    model.embedding.embeddings = model.embedding.embeddings.map(
      (embedding, i) => this.optimizer.step(
        embedding,
        gradients.embeddings[i] || gradients.embeddings[0],
        `embedding_${i}`
      )
    );

    // Update attention weights (queryWeight is 2D: embeddingDim x embeddingDim)
    // Update each row independently to maintain correct matrix shape
    model.attention.queryWeight = model.attention.queryWeight.map(
      (row, i) => this.optimizer.step(row, gradients.attention.query, `attention_query_${i}`)
    );
    model.attention.keyWeight = model.attention.keyWeight.map(
      (row, i) => this.optimizer.step(row, gradients.attention.key, `attention_key_${i}`)
    );
    model.attention.valueWeight = model.attention.valueWeight.map(
      (row, i) => this.optimizer.step(row, gradients.attention.value, `attention_value_${i}`)
    );

    // Update normalization parameters
    model.normalization.updateNormParameters(
      gradients.normalization,
      this.optimizer.learningRate
    );
  }
}