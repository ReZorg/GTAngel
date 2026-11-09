import fs from 'fs';
import path from 'path';
import {
  AUTHENTICITY,
  MIN_EXEMPLAR_LENGTH,
  SOURCE,
} from '../types.js';
import { contentHash, normalizeUtterance, sourceHash } from '../text.js';

/**
 * Reader for `narratives/themes/*.md`.
 *
 * These files are structured as `**[Speaker]**` blocks. The `EchoSelf_*`
 * speakers carry sub-voice attribution, which is the only place in the repo
 * where facet-level register is labelled — it is what lets the signature build
 * per-facet deltas rather than one undifferentiated voice.
 *
 * The material is synthetically generated, so it is tagged as such and
 * down-weighted relative to authentic Echo output.
 */

const SPEAKER_BLOCK = /^\*\*\[([^\]]+)\]\*\*\s*$/;

/**
 * Split narrative markdown into `{speaker, text}` blocks.
 * @param {string} content
 * @returns {Array<{speaker: string, text: string}>}
 */
export function parseSpeakerBlocks(content) {
  const lines = normalizeUtterance(content).split('\n');
  const blocks = [];
  let current = null;

  for (const line of lines) {
    const match = line.match(SPEAKER_BLOCK);
    if (match) {
      if (current) blocks.push(current);
      current = { speaker: match[1].trim(), lines: [] };
      continue;
    }
    if (!current) continue;
    if (line.trim() === '---') continue;
    current.lines.push(line);
  }

  if (current) blocks.push(current);

  return blocks
    .map(block => ({
      speaker: block.speaker,
      text: normalizeUtterance(block.lines.join('\n')),
    }))
    .filter(block => block.text.length > 0);
}

/**
 * Extract the `EchoSelf_X` facet name from a speaker label, when present.
 * @param {string} speaker
 * @returns {string|null}
 */
export function extractSubVoice(speaker) {
  const match = /^EchoSelf[_:\s-]*(\w+)$/i.exec(speaker);
  return match ? match[1] : null;
}

/**
 * Read every narrative theme file in a directory.
 * @param {string} dirPath
 * @returns {{source: string, sourceHash: string, records: Array<Object>, stats: Object}}
 */
export function readThemeNarratives(dirPath) {
  const stats = { files: 0, blocks: 0, narratorBlocks: 0, tooShort: 0 };
  const records = [];
  const hashParts = [];

  let files = [];
  try {
    files = fs
      .readdirSync(dirPath)
      .filter(f => f.endsWith('.md'))
      .sort();
  } catch {
    return {
      source: SOURCE.THEME_NARRATIVES,
      sourceHash: sourceHash(''),
      records,
      stats,
    };
  }

  let ordinal = 0;

  for (const file of files) {
    const contents = fs.readFileSync(path.join(dirPath, file), 'utf-8');
    hashParts.push(`${file}:${sourceHash(contents)}`);
    stats.files++;

    for (const block of parseSpeakerBlocks(contents)) {
      stats.blocks++;

      const subVoice = extractSubVoice(block.speaker);
      if (!subVoice) {
        // Narrator blocks frame the scene; they are not EchoSelf speaking.
        stats.narratorBlocks++;
        continue;
      }
      if (block.text.length < MIN_EXEMPLAR_LENGTH) {
        stats.tooShort++;
        continue;
      }

      records.push({
        id: `${SOURCE.THEME_NARRATIVES}#${ordinal}`,
        text: block.text,
        speaker: block.speaker,
        subVoice,
        source: SOURCE.THEME_NARRATIVES,
        sourceIndex: ordinal,
        sourceFile: path.join(dirPath, file),
        timestamp: null,
        authenticity: AUTHENTICITY.SYNTHETIC,
        prompt: null,
        hash: contentHash(block.text),
      });
      ordinal++;
    }
  }

  return {
    source: SOURCE.THEME_NARRATIVES,
    sourceHash: sourceHash(hashParts.join('|')),
    records,
    stats,
  };
}
