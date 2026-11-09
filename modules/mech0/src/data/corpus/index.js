export {
  buildCorpus,
  buildAndWriteCorpus,
  deduplicate,
  loadCorpus,
  readAllSources,
  DEFAULT_CORPUS_PATH,
  DEFAULT_MANIFEST_PATH,
  DEFAULT_SOURCES,
} from './builder.js';

export {
  REVIEWED_EXCLUSIONS,
  SCREEN_SIGNALS,
  curationRuleset,
  exclusionReason,
  matchedSignals,
  screenExemplar,
} from './curation.js';

export {
  AUTHENTICITY,
  CORPUS_SCHEMA_VERSION,
  CURATION_RULESET_VERSION,
  MIN_EXEMPLAR_LENGTH,
  SAFETY_VERDICT,
  SOURCE,
  SOURCE_WEIGHTS,
} from './types.js';

export {
  contentHash,
  dedupeKey,
  normalizeUtterance,
  sourceHash,
} from './text.js';
