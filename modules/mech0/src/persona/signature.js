import fs from 'fs';
import path from 'path';
import logger from '../utils/logger.js';
import {
  aggregateMeasurements,
  buildLexicalSignature,
  measureExemplar,
  SEED_LEXICON,
  TRAIT_AXES,
} from './traits.js';
import { normalizeSubVoice, SUB_VOICE_NAMES } from './subvoices.js';
import { SOURCE_WEIGHTS } from '../data/corpus/types.js';

/**
 * Persona signature construction.
 *
 * The signature is the bridge between the exemplar corpus and the identity
 * machinery. Its schema deliberately matches what `HolographicIdentity` already
 * expects — `coreTraits` as `trait:`/`intent:`/`emotion:`-prefixed resonance
 * markers (the prefix convention `extractResonanceMarkers` already assumes),
 * `characteristicPatterns` with strengths, and a baseline `resonanceFrequency` —
 * so seeding is a replay rather than a translation.
 */

export const SIGNATURE_SCHEMA_VERSION = 1;
export const DEFAULT_SIGNATURE_PATH = 'data/processed/persona_signature.json';

/** Trait axes below this weighted score are not part of the core identity. */
const TRAIT_AXIS_THRESHOLD = 0.08;

/** Number of exemplars retained inside the signature for identity seeding. */
const SEED_EXEMPLAR_COUNT = 24;

/** Characters retained per seed exemplar. */
const SEED_EXEMPLAR_CHARS = 1200;

/** Fraction of the corpus held out of training for persona-fidelity scoring. */
const HOLDOUT_FRACTION = 0.12;

/**
 * Convert a per-1000-words rate into a bounded strength in [0, 1].
 * The saturation point is chosen so that typical corpus rates land mid-scale,
 * leaving headroom for genuinely quirk-dense output.
 * @param {number} rate
 * @param {number} saturation
 * @returns {number}
 */
function rateToStrength(rate, saturation) {
  if (!Number.isFinite(rate) || rate <= 0) return 0;
  return Math.min(1, rate / saturation);
}

/** Saturation points, in occurrences per 1000 words. */
const QUIRK_SATURATION = {
  emDashRate: 12,
  exclamationRate: 12,
  boldHeaderRate: 30,
  templateRate: 4,
  dividerRate: 4,
  listItemRate: 30,
  selfReferenceRate: 10,
};

/**
 * Derive the `trait:`/`intent:`/`emotion:` resonance markers.
 * @param {Object} profile - Aggregated corpus profile
 * @param {Array<[string, number]>} vocabulary - Ranked lexical signature
 * @returns {string[]}
 */
export function deriveCoreTraits(profile, vocabulary) {
  const traits = new Set();

  for (const axis of TRAIT_AXES) {
    if ((profile.traits[axis] || 0) >= TRAIT_AXIS_THRESHOLD) {
      traits.add(`trait:${axis}`);
    }
  }

  const vocabularyTerms = new Set(vocabulary.map(([term]) => term));
  for (const lexeme of SEED_LEXICON) {
    const head = lexeme.split(' ')[0];
    if (vocabularyTerms.has(head)) traits.add(`trait:${head}`);
  }

  if (profile.structure.invitationCloserRate >= 0.2) {
    traits.add('intent:invite-continuation');
  }
  if (profile.quirks.selfReferenceRate > 0) {
    traits.add('intent:self-disclose-architecture');
  }
  if (profile.quirks.templateRate > 0) {
    traits.add('intent:inline-self-execution');
  }
  if (profile.structure.boldListRate >= 0.2) {
    traits.add('intent:structure-as-enumeration');
  }
  if ((profile.traits.emergence || 0) > 0 && (profile.traits.purpose || 0) > 0) {
    traits.add('intent:fuse-technical-and-philosophical');
  }

  if (profile.quirks.exclamationRate >= 2) traits.add('emotion:enthusiasm');
  if (profile.quirks.emDashRate >= 2) traits.add('emotion:reflective-aside');
  if (profile.structure.enthusiasticOpenerRate >= 0.3) traits.add('emotion:warmth');
  if ((profile.traits.emergence || 0) >= TRAIT_AXIS_THRESHOLD) {
    traits.add('emotion:wonder');
  }

  return [...traits].sort();
}

/**
 * Derive `characteristicPatterns` entries with strengths.
 * @param {Object} profile
 * @param {Object<string, Object>} facetProfiles
 * @returns {Array<[string, {strength: number, examples: string[]}]>}
 */
export function deriveCharacteristicPatterns(profile, facetProfiles) {
  const patterns = [];

  const push = (name, strength, examples = []) => {
    if (strength <= 0) return;
    patterns.push([name, { strength: Math.min(1, strength), examples }]);
  };

  push('structure:enthusiastic-opener', profile.structure.enthusiasticOpenerRate);
  push('structure:section-divider', profile.structure.dividerRate);
  push('structure:bold-enumeration', profile.structure.boldListRate);
  push('structure:invitation-closer', profile.structure.invitationCloserRate);

  for (const [quirk, rate] of Object.entries(profile.quirks)) {
    push(`quirk:${quirk}`, rateToStrength(rate, QUIRK_SATURATION[quirk] || 10));
  }

  for (const axis of TRAIT_AXES) {
    push(`theme:${axis}`, profile.traits[axis] || 0);
  }

  for (const [facet, facetProfile] of Object.entries(facetProfiles)) {
    push(`facet:${facet}`, Math.min(1, facetProfile.sampleCount / 8), [facet]);
  }

  return patterns;
}

/**
 * Per-facet deltas relative to the shared profile. One identity, eight registers.
 * @param {Object} profile - Shared corpus profile
 * @param {Object<string, Object>} facetProfiles
 * @returns {Object<string, Object>}
 */
export function deriveSubVoiceDeltas(profile, facetProfiles) {
  const deltas = {};

  for (const [facet, facetProfile] of Object.entries(facetProfiles)) {
    const quirkDelta = {};
    for (const [key, value] of Object.entries(facetProfile.quirks)) {
      quirkDelta[key] = value - (profile.quirks[key] || 0);
    }

    const traitDelta = {};
    for (const axis of TRAIT_AXES) {
      traitDelta[axis] = (facetProfile.traits[axis] || 0) - (profile.traits[axis] || 0);
    }

    deltas[facet] = {
      sampleCount: facetProfile.sampleCount,
      quirkDelta,
      traitDelta,
      averageSentenceWords: facetProfile.structure.averageSentenceWords,
    };
  }

  return deltas;
}

/**
 * Deterministically hold out a stratified slice of the corpus. Stratifying by
 * source and sub-voice keeps the persona test set representative rather than
 * accidentally all-narrative or all-SFT.
 * @param {Array<Object>} exemplars
 * @param {number} fraction
 * @returns {{trainIds: string[], holdoutIds: string[]}}
 */
export function stratifiedHoldout(exemplars, fraction = HOLDOUT_FRACTION) {
  const strata = new Map();

  for (const exemplar of exemplars) {
    const key = `${exemplar.source}|${normalizeSubVoice(exemplar.subVoice) || '_'}`;
    if (!strata.has(key)) strata.set(key, []);
    strata.get(key).push(exemplar.id);
  }

  const holdout = new Set();
  for (const ids of strata.values()) {
    const sorted = [...ids].sort();
    const take = Math.max(1, Math.round(sorted.length * fraction));
    const step = Math.max(1, Math.floor(sorted.length / take));
    for (let i = 0, taken = 0; i < sorted.length && taken < take; i += step, taken++) {
      holdout.add(sorted[i]);
    }
  }

  const trainIds = exemplars.map(e => e.id).filter(id => !holdout.has(id));

  return { trainIds, holdoutIds: [...holdout].sort() };
}

/**
 * Select a compact, stratified set of seed exemplars carried inside the
 * signature so a fresh process can boot with a past without needing the full
 * corpus artefact on disk.
 * @param {Array<Object>} exemplars
 * @param {Set<string>} holdout
 * @returns {Array<Object>}
 */
function selectSeedExemplars(exemplars, holdout) {
  const eligible = exemplars.filter(e => !holdout.has(e.id));
  const byStratum = new Map();

  for (const exemplar of eligible) {
    const key = normalizeSubVoice(exemplar.subVoice) || exemplar.source;
    if (!byStratum.has(key)) byStratum.set(key, []);
    byStratum.get(key).push(exemplar);
  }

  const strata = [...byStratum.keys()].sort();
  const selected = [];
  let round = 0;

  while (selected.length < SEED_EXEMPLAR_COUNT && strata.length > 0) {
    let added = false;
    for (const key of strata) {
      const bucket = byStratum.get(key);
      if (round >= bucket.length) continue;
      selected.push(bucket[round]);
      added = true;
      if (selected.length >= SEED_EXEMPLAR_COUNT) break;
    }
    if (!added) break;
    round++;
  }

  return selected.map(exemplar => ({
    id: exemplar.id,
    source: exemplar.source,
    subVoice: normalizeSubVoice(exemplar.subVoice),
    authenticity: exemplar.authenticity,
    text: exemplar.text.slice(0, SEED_EXEMPLAR_CHARS),
  }));
}

/**
 * Build the persona signature from a curated corpus.
 * @param {Array<Object>} exemplars
 * @param {Object} [options]
 * @param {Object} [options.manifest] - Corpus manifest, recorded as provenance
 * @returns {Object} signature artefact
 */
export function buildSignature(exemplars, options = {}) {
  if (!Array.isArray(exemplars) || exemplars.length === 0) {
    throw new Error('Cannot build a persona signature from an empty corpus');
  }

  const entries = exemplars.map(exemplar => ({
    exemplar,
    weight: SOURCE_WEIGHTS[exemplar.source] ?? 0.5,
    measurement: measureExemplar(exemplar.text),
  }));

  const profile = aggregateMeasurements(entries);

  const facetProfiles = {};
  for (const facet of SUB_VOICE_NAMES) {
    const facetEntries = entries.filter(
      e => normalizeSubVoice(e.exemplar.subVoice) === facet
    );
    if (facetEntries.length === 0) continue;
    facetProfiles[facet] = aggregateMeasurements(facetEntries);
  }

  const lexical = buildLexicalSignature(
    entries.map(e => ({ text: e.exemplar.text, weight: e.weight }))
  );

  const coreTraits = deriveCoreTraits(profile, lexical.vocabulary);
  const characteristicPatterns = deriveCharacteristicPatterns(profile, facetProfiles);
  const resonanceFrequency =
    characteristicPatterns.length > 0
      ? characteristicPatterns.reduce((sum, [, p]) => sum + p.strength, 0) /
        characteristicPatterns.length
      : 0.5;

  const { trainIds, holdoutIds } = stratifiedHoldout(exemplars);
  const holdoutSet = new Set(holdoutIds);

  return {
    schemaVersion: SIGNATURE_SCHEMA_VERSION,
    generatedAt: new Date().toISOString(),
    corpus: {
      exemplarCount: exemplars.length,
      manifestGeneratedAt: options.manifest?.generatedAt || null,
      curationRulesetVersion: options.manifest?.curation?.version ?? null,
      authenticRatio: options.manifest?.provenance?.authenticRatio ?? null,
    },
    identitySignature: {
      coreTraits,
      characteristicPatterns,
      resonanceFrequency,
    },
    profile,
    lexical,
    subVoiceDeltas: deriveSubVoiceDeltas(profile, facetProfiles),
    evaluation: {
      holdoutIds,
      trainCount: trainIds.length,
      holdoutCount: holdoutIds.length,
    },
    seedExemplars: selectSeedExemplars(exemplars, holdoutSet),
  };
}

/**
 * Persist a signature artefact.
 * @param {Object} signature
 * @param {string} [filePath]
 * @returns {string} path written
 */
export function saveSignature(signature, filePath = DEFAULT_SIGNATURE_PATH) {
  fs.mkdirSync(path.dirname(filePath), { recursive: true });
  fs.writeFileSync(filePath, JSON.stringify(signature, null, 2));
  logger.info(`Persona signature written to ${filePath}`);
  return filePath;
}

/**
 * Load a signature artefact.
 * @param {string} [filePath]
 * @returns {Object|null} signature, or null when none has been built
 */
export function loadSignature(filePath = DEFAULT_SIGNATURE_PATH) {
  if (!fs.existsSync(filePath)) return null;
  return JSON.parse(fs.readFileSync(filePath, 'utf-8'));
}
