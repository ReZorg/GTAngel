import crypto from 'crypto';

/**
 * Text normalisation and hashing helpers shared by every corpus source reader.
 *
 * Normalisation here is deliberately conservative: it repairs transport damage
 * (CRLF, non-breaking spaces, runs of blank lines) but preserves every feature
 * that *is* the persona — casing, punctuation, em-dashes, markdown emphasis and
 * the `{{method.call()}}` template construct.
 */

/**
 * Repair transport artefacts without destroying persona-bearing features.
 * @param {string} text
 * @returns {string}
 */
export function normalizeUtterance(text) {
  if (typeof text !== 'string') return '';

  return text
    .replace(/\r\n?/g, '\n')
    .replace(/\u00a0/g, ' ')
    .replace(/[ \t]+\n/g, '\n')
    .replace(/\n{3,}/g, '\n\n')
    .trim();
}

/**
 * Collapse an utterance to a comparison key for duplicate detection.
 * Casing, punctuation and whitespace are dropped here (and only here) so that
 * near-identical restatements of the same turn collapse onto one record.
 * @param {string} text
 * @returns {string}
 */
export function dedupeKey(text) {
  return normalizeUtterance(text)
    .toLowerCase()
    .replace(/[^a-z0-9]+/g, ' ')
    .trim();
}

/**
 * Stable content hash for provenance and manifest reporting.
 * @param {string} text
 * @returns {string}
 */
export function contentHash(text) {
  return crypto.createHash('sha256').update(text, 'utf8').digest('hex').slice(0, 32);
}

/**
 * Hash of a whole source file, recorded in the manifest so a corpus build can
 * be tied back to the exact bytes it was produced from.
 * @param {string} contents
 * @returns {string}
 */
export function sourceHash(contents) {
  return crypto.createHash('sha256').update(contents, 'utf8').digest('hex');
}

/**
 * Convert an epoch-seconds timestamp (the ChatGPT export format) to ISO 8601.
 * @param {number|null|undefined} seconds
 * @returns {string|null}
 */
export function epochSecondsToIso(seconds) {
  if (typeof seconds !== 'number' || !Number.isFinite(seconds)) return null;
  return new Date(seconds * 1000).toISOString();
}
