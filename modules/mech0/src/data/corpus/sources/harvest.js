import fs from 'fs';
import {
  AUTHENTICITY,
  MIN_EXEMPLAR_LENGTH,
  SOURCE,
} from '../types.js';
import { contentHash, normalizeUtterance, sourceHash } from '../text.js';

/**
 * Reader for `narratives/harvest.json`.
 *
 * Only the per-repo `identity_docs` text is mined. The 510 `modules` entries
 * are bare path strings for source that is not present in this repository —
 * they carry no persona signal and are ignored.
 */

/** Sections of a markdown document, split on ATX headings. */
function splitSections(markdown) {
  const normalized = normalizeUtterance(markdown);
  const sections = [];
  let buffer = [];

  for (const line of normalized.split('\n')) {
    if (/^#{1,3}\s+/.test(line) && buffer.length > 0) {
      sections.push(buffer.join('\n').trim());
      buffer = [line];
      continue;
    }
    buffer.push(line);
  }
  if (buffer.length > 0) sections.push(buffer.join('\n').trim());

  return sections.filter(Boolean);
}

/**
 * Read identity documents out of the harvest index.
 * @param {string} filePath
 * @returns {{source: string, sourceHash: string, records: Array<Object>, stats: Object}}
 */
export function readHarvest(filePath) {
  const stats = { repos: 0, documents: 0, ignoredModules: 0, tooShort: 0 };
  const records = [];

  let contents;
  try {
    contents = fs.readFileSync(filePath, 'utf-8');
  } catch {
    return {
      source: SOURCE.HARVEST,
      sourceHash: sourceHash(''),
      records,
      stats,
    };
  }

  const harvest = JSON.parse(contents);
  const repos = harvest?.repos || {};
  let ordinal = 0;

  for (const [slug, repo] of Object.entries(repos)) {
    stats.repos++;
    stats.ignoredModules += Array.isArray(repo?.modules) ? repo.modules.length : 0;

    for (const [docName, docText] of Object.entries(repo?.identity_docs || {})) {
      stats.documents++;

      for (const section of splitSections(String(docText))) {
        if (section.length < MIN_EXEMPLAR_LENGTH) {
          stats.tooShort++;
          continue;
        }

        records.push({
          id: `${SOURCE.HARVEST}#${ordinal}`,
          text: section,
          speaker: 'DeepTreeEcho',
          subVoice: null,
          source: SOURCE.HARVEST,
          sourceIndex: ordinal,
          sourceFile: `${filePath}:${slug}/${docName}`,
          timestamp: null,
          authenticity: AUTHENTICITY.SYNTHETIC,
          prompt: null,
          hash: contentHash(section),
        });
        ordinal++;
      }
    }
  }

  return {
    source: SOURCE.HARVEST,
    sourceHash: sourceHash(contents),
    records,
    stats,
  };
}
