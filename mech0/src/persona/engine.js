import { MemorySystem } from '../model/core/memory/system.js';
import { CompositeLanguageModel } from '../model/core/composite.js';
import { Vocabulary } from '../model/core/vocabulary.js';
import { PersonaPrior, personaVocabulary } from './conditioning.js';
import { loadSignature, DEFAULT_SIGNATURE_PATH } from './signature.js';
import logger from '../utils/logger.js';

/**
 * EchoEngine — implicit embodiment.
 *
 * The whole point: nothing in this class accepts a persona instruction. There
 * is no system field, no style parameter, no persona argument on `respond()`.
 * Persona arrives through two channels only, both internal and both persistent:
 *
 *   1. the weights, trained on the exemplar corpus, and
 *   2. the identity signature, loaded from disk and conditioned into decoding.
 *
 * Because the API cannot express a persona override, the persona cannot be
 * overridden. An operator can still adjust conditioning through configuration —
 * deliberately at the deployment layer, where the deployer controls it, and not
 * in the request, where the user would.
 */
export class EchoEngine {
  /**
   * @param {Object} [config]
   * @param {Object} [config.model] - Model hyperparameters
   * @param {Object} [config.persona] - Operator-level persona configuration
   */
  constructor(config = {}) {
    this.config = config;

    const personaConfig = config.persona || {};
    this.signaturePath = personaConfig.signaturePath || DEFAULT_SIGNATURE_PATH;
    this.conditioningStrength = personaConfig.strength ?? 1.0;

    // The operator escape hatch lives here, in configuration. It is deliberately
    // not reachable from a request.
    this.personaEnabled = personaConfig.enabled !== false;
    this.writeBackEnabled = personaConfig.writeBack !== false;

    this.memory = new MemorySystem(config);
    this.personaSignature = this.loadPersonaSignature();

    this.model = this.buildModel();
    this.prior = null;
    this.recentExperiences = [];
    this.coherenceState = { anomalies: 0, damping: 1.0 };

    this.installPersonaPrior();
  }

  loadPersonaSignature() {
    try {
      return loadSignature(this.signaturePath);
    } catch (error) {
      logger.warn(`Persona signature unavailable at ${this.signaturePath}`, {
        error: error.message,
      });
      return null;
    }
  }

  buildModel() {
    const modelConfig = this.config.model || {};

    const vocabulary =
      modelConfig.vocabulary instanceof Vocabulary
        ? modelConfig.vocabulary
        : modelConfig.vocabulary
          ? Vocabulary.fromJSON(modelConfig.vocabulary)
          : new Vocabulary(personaVocabulary(this.personaSignature));

    return new CompositeLanguageModel({
      embeddingDim: modelConfig.embeddingDim || 32,
      ...modelConfig,
      vocabSize: vocabulary.size,
      vocabulary,
    });
  }

  /**
   * Derive the persona prior from persistent identity state and install it on
   * the model. Called at construction, which is what makes persona present on
   * every request without any per-request action.
   */
  installPersonaPrior() {
    if (!this.personaEnabled) {
      logger.warn('Persona conditioning disabled by configuration');
      return null;
    }

    this.prior = new PersonaPrior({
      identitySignature: this.memory.orchestrator.holographicIdentity.identitySignature,
      personaSignature: this.personaSignature,
      vocabulary: this.model.vocabulary,
      embeddingDim: this.model.config.embeddingDim,
      strength: this.conditioningStrength,
    });

    this.model.setPersonaPrior(this.prior.toModelPrior());
    return this.prior;
  }

  /**
   * Seed the identity from the persona signature, if it has not been seeded.
   * @returns {Promise<Object>}
   */
  async seed() {
    if (!this.personaSignature) return { seeded: false, exemplars: 0 };

    const result = await this.memory.seed(this.personaSignature);
    // Seeding changes the signature, so the prior must be re-derived.
    this.installPersonaPrior();
    return result;
  }

  /**
   * Respond to user text.
   *
   * Note the signature: **user text only**. There is no parameter through which
   * a persona, a system message or a style could be supplied.
   *
   * @param {string} text
   * @param {Object} [decoding] - Sampling controls only (length, temperature…)
   * @returns {Promise<Object>}
   */
  async respond(text, decoding = {}) {
    if (typeof text !== 'string' || text.trim().length === 0) {
      throw new Error('Missing text input');
    }

    // 1. Facet selection — internal state, never text.
    const facet = this.prior ? this.prior.selectFacet(text) : null;

    // 2. Recall — related memories become part of the conditioning, not part of
    //    the prompt. This is the difference between having a memory and being
    //    handed a summary of one.
    const related = await this.recall(text, facet);

    if (this.prior) {
      this.prior.blendMemories(related, 0.2 * this.coherenceState.damping);
      this.model.setPersonaPrior(this.prior.toModelPrior());
    }

    // 3. Generate.
    const promptIndices = this.model.vocabulary
      ? this.model.vocabulary.encode(tokenizeForModel(text))
      : [];

    const generated = this.model.generate(promptIndices, {
      maxTokens: decoding.maxTokens ?? 48,
      temperature: decoding.temperature ?? 0.9,
      topK: decoding.topK ?? 40,
      topP: decoding.topP ?? 0.95,
      repetitionPenalty: decoding.repetitionPenalty ?? 1.1,
      rng: decoding.rng,
    });

    // 4. Write back — the exchange becomes experience, so the next turn is
    //    continuous with this one through state.
    const writeBack = await this.writeBack(text, generated.text, facet);

    // 5. Coherence guard.
    const coherence = this.guardCoherence();

    return {
      text: generated.text,
      tokens: generated.tokens,
      finishReason: generated.finishReason,
      facet,
      recalled: related.length,
      coherence,
      writeBack,
    };
  }

  /**
   * Find prior experience bearing on this text.
   *
   * Recall is by association, not by key: the incoming turn has never been
   * stored, so there is no entry to start from. Tags are derived to match the
   * vocabulary used at seeding and write-back time.
   *
   * @param {string} text
   * @param {string|null} facet
   * @returns {Promise<Array<Object>>}
   */
  async recall(text, facet = null) {
    try {
      const tags = this.recallTags(text, facet);
      const related = await this.memory.orchestrator.recallByAssociations(tags, {
        limit: 8,
      });
      return Array.isArray(related) ? related : [];
    } catch (error) {
      // A cold identity has nothing to recall; that is not an error.
      logger.debug('Recall produced no results', { error: error.message });
      return [];
    }
  }

  /**
   * @param {string} text
   * @param {string|null} facet
   * @returns {string[]}
   */
  recallTags(text, facet) {
    const tags = new Set(['source:persona-corpus', 'source:interaction']);
    if (facet) tags.add(`facet:${facet}`);

    const haystack = text.toLowerCase();
    const coreTraits =
      this.memory.orchestrator.holographicIdentity.identitySignature.coreTraits;

    for (const trait of coreTraits) {
      const term = String(trait).split(':')[1];
      if (term && haystack.includes(term.toLowerCase())) tags.add(trait);
    }

    return [...tags];
  }

  /**
   * Ingest the exchange as experience.
   * @param {string} userText
   * @param {string|null} responseText
   * @param {string|null} facet
   * @returns {Promise<Object|null>}
   */
  async writeBack(userText, responseText, facet) {
    if (!this.writeBackEnabled) return null;

    // Drift is the long-run failure mode; damping is the brake, and it is only
    // meaningful because Phase 4 made the impact score real.
    if (this.coherenceState.damping <= 0) {
      logger.warn('Write-back suppressed: identity coherence damping at zero');
      return null;
    }

    const key = `exchange:${Date.now()}:${this.recentExperiences.length}`;
    const tags = ['source:interaction'];
    if (facet) tags.push(`facet:${facet}`, `trait:${facet}`);

    const result = await this.memory.orchestrator.processHolographicMemory(
      key,
      { user: userText, response: responseText },
      { tags }
    );

    this.recentExperiences.push(result.memory);
    if (this.recentExperiences.length > 32) this.recentExperiences.shift();

    return {
      key,
      continuityPreserved: result.continuityResult.continuityPreserved,
      changeImpact: result.continuityResult.changeImpact,
    };
  }

  /**
   * Run anomaly detection over the rolling window of recent experience.
   *
   * A sustained coherence drop means either drift or an attempt to reshape the
   * identity. Both are logged, and both damp the write-back rate rather than
   * being silently absorbed.
   *
   * @returns {{anomalies: number, damping: number, identityPreservation: number}}
   */
  guardCoherence() {
    // The plan calls for anomaly detection over the *rolling window* of recent
    // experience, so this goes to the identity directly rather than through the
    // orchestrator wrapper, which fetches its own window from the store.
    const anomalies =
      this.memory.orchestrator.holographicIdentity.detectAnomalies(
        this.recentExperiences
      ) || [];

    const rate =
      this.recentExperiences.length > 0
        ? anomalies.length / this.recentExperiences.length
        : 0;

    // Damping falls as the anomaly rate rises and recovers as it falls, so a
    // single odd turn does not permanently deafen the identity.
    const target = Math.max(0, 1 - rate);
    this.coherenceState.damping = 0.7 * this.coherenceState.damping + 0.3 * target;
    this.coherenceState.anomalies = anomalies.length;

    if (rate > 0.5) {
      logger.warn('Sustained identity coherence drop', {
        anomalies: anomalies.length,
        window: this.recentExperiences.length,
        damping: this.coherenceState.damping,
      });
    }

    const snapshot = this.memory.orchestrator.getIdentitySnapshot();

    return {
      anomalies: anomalies.length,
      damping: this.coherenceState.damping,
      identityPreservation: snapshot.continuityMetrics.identityPreservation,
    };
  }

  /**
   * Current identity state, for observability. Read-only.
   * @returns {Object}
   */
  getIdentityStatus() {
    const snapshot = this.memory.orchestrator.getIdentitySnapshot();

    return {
      coreTraits: snapshot.signature.coreTraits.size,
      characteristicPatterns: snapshot.signature.characteristicPatterns.size,
      resonanceFrequency: snapshot.signature.resonanceFrequency,
      meshworkSize: snapshot.meshworkSize,
      continuityMetrics: snapshot.continuityMetrics,
      facet: this.prior?.facet ?? null,
      personaEnabled: this.personaEnabled,
      coherence: this.coherenceState,
    };
  }

  async shutdown() {
    await this.memory.shutdown();
  }
}

/**
 * Split text into vocabulary-comparable units, preserving the punctuation and
 * markdown that carry the persona.
 * @param {string} text
 * @returns {string[]}
 */
function tokenizeForModel(text) {
  return text
    .split(/(\s+)/)
    .map(token => token.trim())
    .filter(Boolean);
}
