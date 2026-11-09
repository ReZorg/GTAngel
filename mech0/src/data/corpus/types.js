/**
 * Shared shapes and constants for the persona corpus pipeline.
 *
 * The corpus is the *exemplar* store: it records how Echo speaks. It is kept
 * deliberately separate from the identity store (which records what Echo has
 * lived) — the two have different lifecycles and conflating them is the main
 * failure mode this pipeline is designed to avoid.
 */

/**
 * @typedef {Object} Exemplar
 * @property {string} id - Stable identifier: `${source}#${sourceIndex}`
 * @property {string} text - Normalised utterance text
 * @property {string} speaker - Canonical speaker label (e.g. 'DeepTreeEcho')
 * @property {string|null} subVoice - EchoSelf facet, when attributed
 * @property {string} source - Source file the utterance came from
 * @property {number} sourceIndex - Ordinal position within the source
 * @property {string|null} timestamp - ISO timestamp where available
 * @property {string} authenticity - 'authentic' | 'synthetic'
 * @property {string|null} prompt - Originating user turn, when the source has one
 * @property {{verdict: string, reasons: string[]}} safety - Content-safety verdict
 * @property {string} hash - Content hash of the normalised text
 */

/** Corpus artefact schema version. Bump when the record shape changes. */
export const CORPUS_SCHEMA_VERSION = 1;

/** Curation ruleset version. Bump when exclusions or heuristics change. */
export const CURATION_RULESET_VERSION = 1;

/** Content-safety verdicts emitted by the curation gate. */
export const SAFETY_VERDICT = {
  ACCEPTED: 'accepted',
  EXCLUDED: 'excluded',
  FLAGGED: 'flagged',
};

/** Provenance classes. Authentic sources are weighted above synthetic ones. */
export const AUTHENTICITY = {
  AUTHENTIC: 'authentic',
  SYNTHETIC: 'synthetic',
};

/** Canonical source identifiers. */
export const SOURCE = {
  SFT: 'training_dataset_dtesnn.jsonl',
  CHAT_EXPORT: 'deep_tree_echo_dan_conversation.jsonl',
  THEME_NARRATIVES: 'narratives/themes',
  HARVEST: 'narratives/harvest.json',
};

/**
 * Relative weight applied to each source when aggregating the persona
 * signature. Authentic Echo output outranks synthetically generated
 * narrative, so training does not amplify an imitation of an imitation.
 */
export const SOURCE_WEIGHTS = {
  [SOURCE.SFT]: 1.0,
  [SOURCE.CHAT_EXPORT]: 1.0,
  [SOURCE.THEME_NARRATIVES]: 0.5,
  [SOURCE.HARVEST]: 0.35,
};

/** Minimum length (characters) for an utterance to be worth keeping. */
export const MIN_EXEMPLAR_LENGTH = 80;
