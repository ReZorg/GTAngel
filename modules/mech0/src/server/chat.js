import express from 'express';
import { validateInput } from '../utils/validation.js';
import logger from '../utils/logger.js';

/**
 * The `/chat` surface.
 *
 * The API accepts user text and sampling controls, and nothing else. There is
 * deliberately no `system` field, no `persona` parameter and no style options:
 * **if the API cannot express a persona override, the persona cannot be
 * overridden.** Persona lives in the weights and in persisted identity state,
 * so there is nothing to pass.
 *
 * Any request key that looks like an attempt to steer identity is rejected
 * rather than ignored, so the contract fails loudly instead of appearing to
 * accept an instruction it silently drops.
 */

/**
 * Request keys that would amount to a persona override. Rejected explicitly.
 */
export const FORBIDDEN_FIELDS = [
  'system',
  'systemPrompt',
  'system_prompt',
  'persona',
  'personality',
  'style',
  'instructions',
  'instruction',
  'role',
  'context',
  'preamble',
  'prefix',
  'messages',
];

/** Sampling controls a caller may legitimately set. */
const DECODING_FIELDS = [
  'maxTokens',
  'temperature',
  'topK',
  'topP',
  'repetitionPenalty',
];

/**
 * Extract decoding controls, ignoring anything else in the body.
 * @param {Object} body
 * @returns {Object}
 */
export function extractDecodingOptions(body = {}) {
  const options = {};
  for (const field of DECODING_FIELDS) {
    const value = body[field];
    if (typeof value === 'number' && Number.isFinite(value)) {
      options[field] = value;
    }
  }
  return options;
}

/**
 * Build the chat router.
 *
 * @param {Object} options
 * @param {Function} options.getEngine - Returns the `EchoEngine` (lazily, so
 *   the identity is not loaded until first use)
 * @returns {import('express').Router}
 */
export function createChatRouter({ getEngine }) {
  const router = express.Router();

  router.post('/chat', async (req, res) => {
    const body = req.body || {};

    const offending = FORBIDDEN_FIELDS.filter(field => field in body);
    if (offending.length > 0) {
      logger.warn('Rejected persona override attempt', { fields: offending });
      return res.status(400).json({
        error:
          'This endpoint accepts user text only. Persona is part of the model ' +
          'and its persistent identity, and cannot be set per request.',
        rejectedFields: offending,
      });
    }

    try {
      const text = validateInput(body.text);
      const engine = await getEngine();
      const result = await engine.respond(text, extractDecodingOptions(body));

      return res.json({
        text: result.text,
        finishReason: result.finishReason,
        // Facet and coherence are reported for observability. They are outputs
        // of internal state, never inputs.
        facet: result.facet,
        recalled: result.recalled,
        coherence: result.coherence,
      });
    } catch (error) {
      logger.error('Chat error:', error.message);
      return res.status(400).json({ error: error.message });
    }
  });

  router.get('/identity', async (req, res) => {
    try {
      const engine = await getEngine();
      return res.json(engine.getIdentityStatus());
    } catch (error) {
      logger.error('Identity status error:', error.message);
      return res.status(503).json({ error: error.message });
    }
  });

  return router;
}
