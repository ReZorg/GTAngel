import express from 'express';
import { ModelPredictor } from '../inference/predictor.js';
import { PredictionCache } from '../inference/cache.js';
import { tokenize } from '../../../utils/tokenizer.js';
import logger from '../../../utils/logger.js';

export class ModelServer {
  constructor(config) {
    this.config = config;
    this.predictor = new ModelPredictor(config);
    this.cache = new PredictionCache(config);
    this.app = express();
    this.setupMiddleware();
    this.setupRoutes();
  }

  /**
   * Turn request text into the integer token indices the model expects.
   *
   * The routes previously passed `text.split(' ')` — raw **strings** — straight
   * into a model that indexes its embedding table by integer, so `/predict`
   * was broken for every real input.
   *
   * @param {string} text
   * @returns {number[]}
   */
  encodeText(text) {
    const tokens = tokenize(text);
    const vocabulary = this.model?.vocabulary;

    if (vocabulary) return vocabulary.encode(tokens);

    // Without a vocabulary there is no honest mapping; fall back to a stable
    // hash into the embedding table rather than emitting strings.
    const vocabSize = this.model?.config?.vocabSize || this.config.vocabSize || 1;
    return tokens.map(token => {
      let hash = 0;
      for (let i = 0; i < token.length; i += 1) {
        hash = (hash * 31 + token.charCodeAt(i)) % vocabSize;
      }
      return hash;
    });
  }

  setupMiddleware() {
    this.app.use(express.json());
    
    // CORS middleware
    this.app.use((req, res, next) => {
      const origin = req.headers.origin;
      if (this.config.corsOrigins.includes(origin)) {
        res.header('Access-Control-Allow-Origin', origin);
      }
      next();
    });

    // Request logging
    this.app.use((req, res, next) => {
      logger.info(`${req.method} ${req.path}`, {
        query: req.query,
        body: req.body
      });
      next();
    });
  }

  setupRoutes() {
    this.app.get('/status', (req, res) => {
      res.json({
        status: 'ready',
        config: {
          batchSize: this.config.batchSize,
          cacheSize: this.config.cacheSize
        },
        cache: this.cache.getStats()
      });
    });

    this.app.post('/predict', async (req, res) => {
      try {
        const { text } = req.body;
        if (!text) {
          throw new Error('Missing text input');
        }

        // Check cache
        const cacheKey = text.toLowerCase();
        const cached = this.cache.get(cacheKey);
        if (cached) {
          return res.json({
            ...cached,
            cached: true
          });
        }

        // Make prediction
        const result = await this.predictor.predict(
          this.model,
          { tokens: this.encodeText(text) }
        );

        // Cache result
        this.cache.set(cacheKey, result);

        res.json(result);
      } catch (error) {
        logger.error('Prediction error:', error);
        res.status(400).json({ error: error.message });
      }
    });

    this.app.post('/batch-predict', async (req, res) => {
      try {
        const { texts } = req.body;
        if (!Array.isArray(texts)) {
          throw new Error('Input must be an array of texts');
        }

        const inputs = texts.map(text => ({
          tokens: this.encodeText(text)
        }));

        const results = await this.predictor.batchPredict(
          this.model,
          inputs
        );

        res.json(results);
      } catch (error) {
        logger.error('Batch prediction error:', error);
        res.status(400).json({ error: error.message });
      }
    });
  }

  setModel(model) {
    this.model = model;
  }

  start() {
    const { port, host } = this.config.server;
    this.app.listen(port, host, () => {
      logger.info(`Model server running at http://${host}:${port}`);
    });
  }
}