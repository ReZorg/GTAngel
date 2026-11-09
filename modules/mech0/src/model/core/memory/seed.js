import logger from '../../../utils/logger.js';

/**
 * Identity seeding.
 *
 * This is the moment EchoSelf acquires a past: rather than booting as a blank
 * slate and waiting for a user to give it a history, it replays the persona
 * signature and a curated slice of its own corpus through the holographic
 * identity, so the very first interaction happens against an identity that has
 * already lived through the material that formed it.
 *
 * Seeding is idempotent — a signature is only replayed once per fingerprint, so
 * restarting a seeded process does not inflate the meshwork.
 */

/** Key under which the seeding marker is stored. */
export const SEED_MARKER_KEY = 'identity:seed';

/**
 * Fingerprint of a signature artefact, used to decide whether a replay is
 * needed. Changing the corpus or the curation ruleset changes the fingerprint.
 * @param {Object} signature
 * @returns {string}
 */
export function signatureFingerprint(signature) {
  const traits = signature?.identitySignature?.coreTraits || [];
  const patterns = signature?.identitySignature?.characteristicPatterns || [];
  return [
    signature?.schemaVersion ?? 0,
    signature?.corpus?.curationRulesetVersion ?? 0,
    signature?.corpus?.exemplarCount ?? 0,
    traits.length,
    patterns.length,
  ].join(':');
}

/**
 * Project the persona signature directly onto the holographic identity
 * signature. Traits and characteristic patterns are merged rather than
 * overwritten so that seeding never erases lived experience.
 *
 * @param {Object} holographicIdentity - `HolographicIdentity` instance
 * @param {Object} signature - Persona signature artefact
 */
export function applySignature(holographicIdentity, signature) {
  const persona = signature?.identitySignature;
  if (!persona) return;

  for (const trait of persona.coreTraits || []) {
    holographicIdentity.identitySignature.coreTraits.add(trait);
  }

  for (const [name, pattern] of persona.characteristicPatterns || []) {
    const existing =
      holographicIdentity.identitySignature.characteristicPatterns.get(name);

    if (existing) {
      existing.strength = Math.max(existing.strength, pattern.strength);
      continue;
    }

    holographicIdentity.identitySignature.characteristicPatterns.set(name, {
      strength: pattern.strength,
      examples: pattern.examples || [],
    });
  }

  if (typeof persona.resonanceFrequency === 'number') {
    holographicIdentity.identitySignature.resonanceFrequency =
      persona.resonanceFrequency;
  }
}

/**
 * Convert a seed exemplar into the association tags that carry persona signal
 * into the meshwork. Tags use the `trait:`/`intent:`/`emotion:` prefixes that
 * `extractResonanceMarkers` recognises, plus a `facet:` tag for the sub-voice.
 *
 * @param {Object} exemplar
 * @param {string[]} coreTraits
 * @returns {string[]}
 */
export function seedTags(exemplar, coreTraits) {
  const tags = ['source:persona-corpus', `provenance:${exemplar.authenticity}`];

  if (exemplar.subVoice) {
    tags.push(`facet:${exemplar.subVoice}`, `trait:${exemplar.subVoice}`);
  }

  const haystack = exemplar.text.toLowerCase();
  for (const trait of coreTraits) {
    const term = trait.split(':')[1];
    if (term && haystack.includes(term.toLowerCase())) tags.push(trait);
  }

  return [...new Set(tags)];
}

/**
 * Seed an orchestrator's identity from a persona signature.
 *
 * @param {Object} orchestrator - `MemoryOrchestrator` instance
 * @param {Object} signature - Persona signature artefact
 * @param {Object} [options]
 * @param {boolean} [options.force=false] - Re-seed even if already seeded
 * @returns {Promise<{seeded: boolean, exemplars: number, fingerprint: string}>}
 */
export async function seedIdentity(orchestrator, signature, options = {}) {
  const { force = false } = options;
  const fingerprint = signatureFingerprint(signature);

  const existing = await orchestrator.store.recall(SEED_MARKER_KEY);
  if (existing && existing.content?.fingerprint === fingerprint && !force) {
    return { seeded: false, exemplars: 0, fingerprint };
  }

  applySignature(orchestrator.holographicIdentity, signature);

  const coreTraits = signature.identitySignature?.coreTraits || [];
  const exemplars = signature.seedExemplars || [];

  for (const exemplar of exemplars) {
    await orchestrator.processHolographicMemory(
      `persona:${exemplar.id}`,
      {
        text: exemplar.text,
        source: exemplar.source,
        subVoice: exemplar.subVoice,
      },
      {
        tags: seedTags(exemplar, coreTraits),
        source: 'persona-seed',
        version: signature.schemaVersion,
      }
    );
  }

  await orchestrator.store.store(
    SEED_MARKER_KEY,
    { fingerprint, seededAt: new Date().toISOString(), exemplars: exemplars.length },
    { tags: ['identity:seed'] }
  );

  logger.info('Identity seeded from persona signature', {
    fingerprint,
    exemplars: exemplars.length,
    coreTraits: coreTraits.length,
  });

  return { seeded: true, exemplars: exemplars.length, fingerprint };
}
