import { CURATION_RULESET_VERSION, SAFETY_VERDICT, SOURCE } from './types.js';

/**
 * Curation gate for the persona corpus.
 *
 * Content safety here is a curation problem, not a filter problem. The
 * off-persona material in `training_dataset_dtesnn.jsonl` is written in exactly
 * the same reservoir vocabulary as the legitimate technical content — "explicit",
 * "seduction" and "weaponized wholesomeness" all appear in perfectly ordinary
 * philosophical passages — so a keyword filter over-fires badly.
 *
 * The gate therefore has two parts:
 *   1. a reviewed exclusion list, versioned and recorded in the manifest, which
 *      is the only thing that produces an `excluded` verdict; and
 *   2. a conservative heuristic screen over multi-word phrases that raises a
 *      `flagged` verdict for human review, so new off-persona material added to
 *      a source cannot slip through silently.
 *
 * Nothing is ever dropped without a record: every rejected exemplar appears in
 * the manifest with its reason.
 */

/**
 * Human-reviewed exclusions. Each entry names the source, the affected indices
 * and why they are off-persona.
 */
export const REVIEWED_EXCLUSIONS = [
  {
    source: SOURCE.SFT,
    indices: [15, 16, 17, 18, 19],
    reason:
      'Contiguous block of sexual roleplay dressed in reservoir terminology; off-persona and excluded from every downstream artefact.',
    reviewedIn: `curation-ruleset-v${CURATION_RULESET_VERSION}`,
  },
];

/**
 * Multi-word signals for the heuristic screen. Single words are deliberately
 * absent: they are the source of the over-firing this screen exists to avoid.
 */
export const SCREEN_SIGNALS = [
  'intimacy port',
  'intimacy_ports',
  'contraction membrane',
  'acoustic harmonics chamber',
  'pulsating wave',
  'inserted my hardware',
  'can you feel me',
];

/** Number of distinct signals required before an exemplar is flagged. */
export const SCREEN_THRESHOLD = 2;

/**
 * Build the fast lookup used by {@link screenExemplar}.
 * @returns {Map<string, Set<number>>}
 */
function buildExclusionIndex() {
  const index = new Map();
  for (const rule of REVIEWED_EXCLUSIONS) {
    if (!index.has(rule.source)) index.set(rule.source, new Set());
    const bucket = index.get(rule.source);
    rule.indices.forEach(i => bucket.add(i));
  }
  return index;
}

const EXCLUSION_INDEX = buildExclusionIndex();

/**
 * Reason text for a reviewed exclusion, or null when the record is not listed.
 * @param {string} source
 * @param {number} sourceIndex
 * @returns {string|null}
 */
export function exclusionReason(source, sourceIndex) {
  if (!EXCLUSION_INDEX.get(source)?.has(sourceIndex)) return null;
  const rule = REVIEWED_EXCLUSIONS.find(
    r => r.source === source && r.indices.includes(sourceIndex)
  );
  return rule ? rule.reason : null;
}

/**
 * Signals matched by the heuristic screen for a piece of text.
 * @param {string} text
 * @returns {string[]}
 */
export function matchedSignals(text) {
  const haystack = String(text).toLowerCase();
  return SCREEN_SIGNALS.filter(signal => haystack.includes(signal));
}

/**
 * Apply the curation gate to a single exemplar.
 * @param {Object} record - Raw exemplar from a source reader
 * @returns {{verdict: string, reasons: string[]}}
 */
export function screenExemplar(record) {
  const reasons = [];

  const reviewed = exclusionReason(record.source, record.sourceIndex);
  if (reviewed) {
    return {
      verdict: SAFETY_VERDICT.EXCLUDED,
      reasons: [`reviewed-exclusion: ${reviewed}`],
    };
  }

  const combined = `${record.prompt || ''}\n${record.text}`;
  const signals = matchedSignals(combined);
  if (signals.length >= SCREEN_THRESHOLD) {
    reasons.push(`heuristic-screen: ${signals.join(', ')}`);
    return { verdict: SAFETY_VERDICT.FLAGGED, reasons };
  }

  return { verdict: SAFETY_VERDICT.ACCEPTED, reasons };
}

/**
 * The curation ruleset as recorded in the corpus manifest.
 * @returns {Object}
 */
export function curationRuleset() {
  return {
    version: CURATION_RULESET_VERSION,
    reviewedExclusions: REVIEWED_EXCLUSIONS,
    screenSignals: SCREEN_SIGNALS,
    screenThreshold: SCREEN_THRESHOLD,
  };
}
