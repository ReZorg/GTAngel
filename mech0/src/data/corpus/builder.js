import fs from 'fs';
import path from 'path';
import logger from '../../utils/logger.js';
import { readSftCorpus } from './sources/sft-jsonl.js';
import { readChatExport } from './sources/chat-export.js';
import { readThemeNarratives } from './sources/theme-narratives.js';
import { readHarvest } from './sources/harvest.js';
import { curationRuleset, screenExemplar } from './curation.js';
import { dedupeKey } from './text.js';
import {
  AUTHENTICITY,
  CORPUS_SCHEMA_VERSION,
  SAFETY_VERDICT,
  SOURCE,
  SOURCE_WEIGHTS,
} from './types.js';

/**
 * Persona corpus builder.
 *
 * Turns four inert source materials at the repository root into one normalised,
 * curated, provenance-tracked exemplar corpus plus a manifest. Provenance is
 * non-negotiable: every exemplar records where it came from, so a bad source can
 * be retracted without rebuilding everything, and so the reason Echo talks a
 * certain way can be traced back to specific material.
 */

export const DEFAULT_SOURCES = {
  sft: 'training_dataset_dtesnn.jsonl',
  chatExport: 'deep_tree_echo_dan_conversation.jsonl',
  themeNarratives: 'narratives/themes',
  harvest: 'narratives/harvest.json',
};

export const DEFAULT_CORPUS_PATH = 'data/processed/persona_corpus.jsonl';
export const DEFAULT_MANIFEST_PATH = 'data/processed/persona_corpus.manifest.json';

/**
 * Deduplicate exemplars, keeping the highest-weighted source for each duplicate
 * group. Record 0 of the SFT corpus is verbatim the first assistant turn of the
 * conversation export, and the theme narratives overlap heavily with the long
 * combined narrative, so this is not a theoretical concern.
 *
 * @param {Array<Object>} records
 * @returns {{kept: Array<Object>, duplicates: Array<{id: string, duplicateOf: string}>}}
 */
export function deduplicate(records) {
  const byKey = new Map();
  const duplicates = [];

  const ranked = [...records].sort(
    (a, b) => (SOURCE_WEIGHTS[b.source] || 0) - (SOURCE_WEIGHTS[a.source] || 0)
  );

  for (const record of ranked) {
    const key = dedupeKey(record.text);
    if (key.length === 0) continue;

    if (byKey.has(key)) {
      duplicates.push({ id: record.id, duplicateOf: byKey.get(key).id });
      continue;
    }
    byKey.set(key, record);
  }

  const keptIds = new Set([...byKey.values()].map(r => r.id));
  const kept = records.filter(r => keptIds.has(r.id));

  return { kept, duplicates };
}

/**
 * Read every source and return the raw (uncurated, undeduplicated) records.
 * @param {Object} sources - Overrides for {@link DEFAULT_SOURCES}
 * @returns {Array<Object>} Reader results
 */
export function readAllSources(sources = {}) {
  const paths = { ...DEFAULT_SOURCES, ...sources };
  const results = [];

  const readers = [
    [paths.sft, readSftCorpus, SOURCE.SFT],
    [paths.chatExport, readChatExport, SOURCE.CHAT_EXPORT],
    [paths.themeNarratives, readThemeNarratives, SOURCE.THEME_NARRATIVES],
    [paths.harvest, readHarvest, SOURCE.HARVEST],
  ];

  for (const [sourcePath, reader, sourceName] of readers) {
    if (!sourcePath || !fs.existsSync(sourcePath)) {
      logger.warn(`Persona corpus source missing, skipping: ${sourceName}`);
      continue;
    }
    try {
      results.push({ ...reader(sourcePath), sourcePath });
    } catch (error) {
      logger.error(`Failed to read persona corpus source ${sourceName}:`, error);
      throw error;
    }
  }

  return results;
}

/**
 * Build the curated corpus in memory.
 * @param {Object} [options]
 * @param {Object} [options.sources] - Source path overrides
 * @param {boolean} [options.includeFlagged=false] - Keep heuristically flagged records
 * @returns {{exemplars: Array<Object>, manifest: Object}}
 */
export function buildCorpus(options = {}) {
  const { sources = {}, includeFlagged = false } = options;

  const readerResults = readAllSources(sources);
  const raw = readerResults.flatMap(result => result.records);

  const screened = raw.map(record => ({
    ...record,
    safety: screenExemplar(record),
  }));

  const admissible = screened.filter(record =>
    record.safety.verdict === SAFETY_VERDICT.ACCEPTED ||
    (includeFlagged && record.safety.verdict === SAFETY_VERDICT.FLAGGED)
  );

  const { kept, duplicates } = deduplicate(admissible);

  const admissibleIds = new Set(admissible.map(record => record.id));
  const exclusions = screened
    .filter(record => !admissibleIds.has(record.id))
    .map(record => ({
      id: record.id,
      source: record.source,
      sourceIndex: record.sourceIndex,
      verdict: record.safety.verdict,
      reasons: record.safety.reasons,
    }));

  const manifest = buildManifest({
    readerResults,
    raw,
    kept,
    duplicates,
    exclusions,
    includeFlagged,
  });

  return { exemplars: kept, manifest };
}

/**
 * Assemble the manifest that accompanies a corpus artefact.
 * @param {Object} parts
 * @returns {Object}
 */
function buildManifest({ readerResults, raw, kept, duplicates, exclusions, includeFlagged }) {
  const bySource = {};
  for (const result of readerResults) {
    bySource[result.source] = {
      path: result.sourcePath,
      sourceHash: result.sourceHash,
      readerStats: result.stats,
      rawCount: result.records.length,
      keptCount: kept.filter(r => r.source === result.source).length,
      weight: SOURCE_WEIGHTS[result.source] ?? 0,
    };
  }

  const authenticCount = kept.filter(
    r => r.authenticity === AUTHENTICITY.AUTHENTIC
  ).length;

  const subVoices = {};
  for (const record of kept) {
    if (!record.subVoice) continue;
    subVoices[record.subVoice] = (subVoices[record.subVoice] || 0) + 1;
  }

  return {
    schemaVersion: CORPUS_SCHEMA_VERSION,
    generatedAt: new Date().toISOString(),
    curation: curationRuleset(),
    includeFlagged,
    counts: {
      raw: raw.length,
      kept: kept.length,
      excluded: exclusions.length,
      duplicates: duplicates.length,
    },
    provenance: {
      authentic: authenticCount,
      synthetic: kept.length - authenticCount,
      authenticRatio: kept.length > 0 ? authenticCount / kept.length : 0,
    },
    subVoices,
    sources: bySource,
    exclusions,
    duplicates,
  };
}

/**
 * Build the corpus and write both artefacts to disk.
 * @param {Object} [options]
 * @param {string} [options.corpusPath]
 * @param {string} [options.manifestPath]
 * @returns {{exemplars: Array<Object>, manifest: Object, corpusPath: string, manifestPath: string}}
 */
export function buildAndWriteCorpus(options = {}) {
  const corpusPath = options.corpusPath || DEFAULT_CORPUS_PATH;
  const manifestPath = options.manifestPath || DEFAULT_MANIFEST_PATH;

  const { exemplars, manifest } = buildCorpus(options);

  fs.mkdirSync(path.dirname(corpusPath), { recursive: true });
  fs.writeFileSync(
    corpusPath,
    exemplars.map(record => JSON.stringify(record)).join('\n') + '\n'
  );

  fs.mkdirSync(path.dirname(manifestPath), { recursive: true });
  fs.writeFileSync(
    manifestPath,
    JSON.stringify({ ...manifest, corpusPath }, null, 2)
  );

  logger.info(
    `Persona corpus written: ${exemplars.length} exemplars -> ${corpusPath}`
  );

  return { exemplars, manifest, corpusPath, manifestPath };
}

/**
 * Load a previously built corpus artefact.
 * @param {string} [corpusPath]
 * @returns {Array<Object>}
 */
export function loadCorpus(corpusPath = DEFAULT_CORPUS_PATH) {
  if (!fs.existsSync(corpusPath)) {
    throw new Error(
      `Persona corpus not found at ${corpusPath}. Run "npm run build-corpus" first.`
    );
  }

  return fs
    .readFileSync(corpusPath, 'utf-8')
    .split('\n')
    .filter(line => line.trim().length > 0)
    .map(line => JSON.parse(line));
}
