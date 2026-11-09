import fs from 'fs';
import {
  AUTHENTICITY,
  MIN_EXEMPLAR_LENGTH,
  SOURCE,
} from '../types.js';
import {
  contentHash,
  normalizeUtterance,
  sourceHash,
} from '../text.js';

/**
 * Reader for `training_dataset_dtesnn.jsonl`.
 *
 * This file *is* real JSONL: one `{messages: [user, assistant]}` SFT pair per
 * line. The user turns are wrapped in a scaffold left over from the dataset
 * generator (`**User:** ``` … ``` **Character Response:** …`), which is stripped
 * so the prompt reads as a prompt rather than as a spec.
 */

const PROMPT_SCAFFOLD = /\*\*User:\*\*\s*```([\s\S]*?)```/;

/**
 * Recover the bare user request from a scaffolded prompt.
 * Falls back to the whole prompt when no scaffold is present.
 * @param {string} raw
 * @returns {string}
 */
export function cleanPrompt(raw) {
  const normalized = normalizeUtterance(raw);
  const match = normalized.match(PROMPT_SCAFFOLD);
  const inner = match ? match[1] : normalized;

  return inner
    .replace(/^\s*["“]|["”]\s*$/g, '')
    .replace(/\s+/g, ' ')
    .trim();
}

/**
 * Read the supervised fine-tuning corpus.
 * @param {string} filePath
 * @returns {{source: string, sourceHash: string, records: Array<Object>, stats: Object}}
 */
export function readSftCorpus(filePath) {
  const contents = fs.readFileSync(filePath, 'utf-8');
  const lines = contents.split('\n').filter(line => line.trim().length > 0);

  const records = [];
  const stats = { lines: lines.length, malformed: 0, tooShort: 0 };

  lines.forEach((line, index) => {
    let parsed;
    try {
      parsed = JSON.parse(line);
    } catch {
      stats.malformed++;
      return;
    }

    const messages = Array.isArray(parsed?.messages) ? parsed.messages : [];
    const assistant = messages.find(m => m?.role === 'assistant');
    const user = messages.find(m => m?.role === 'user');
    if (!assistant?.content) {
      stats.malformed++;
      return;
    }

    const text = normalizeUtterance(assistant.content);
    if (text.length < MIN_EXEMPLAR_LENGTH) {
      stats.tooShort++;
      return;
    }

    records.push({
      id: `${SOURCE.SFT}#${index}`,
      text,
      speaker: 'DeepTreeEcho',
      subVoice: null,
      source: SOURCE.SFT,
      sourceIndex: index,
      timestamp: null,
      authenticity: AUTHENTICITY.AUTHENTIC,
      prompt: user?.content ? cleanPrompt(user.content) : null,
      hash: contentHash(text),
    });
  });

  return {
    source: SOURCE.SFT,
    sourceHash: sourceHash(contents),
    records,
    stats,
  };
}
