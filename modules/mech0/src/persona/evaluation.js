import { measureExemplar, skeletonConformance } from './traits.js';
import { contentWords } from './tokenizer.js';
import { scoreSubVoices } from './subvoices.js';

/**
 * Persona fidelity evaluation.
 *
 * The point of measuring is to make persona fidelity testable rather than
 * vibes-based, and to make drift visible. Scores are deliberately built from
 * the same measurements Phase 2 used to *derive* the signature, so the two
 * paths cannot disagree about what "Echo-like" means.
 */

/** Weights of the four fidelity components. */
export const FIDELITY_WEIGHTS = {
  lexical: 0.35,
  structural: 0.25,
  quirk: 0.25,
  trait: 0.15,
};

/**
 * The DTE concept density band the narrative pipeline already scored chapters
 * against (48–87%). Reused so concept density has a precedent target rather
 * than an invented one.
 *
 * Note this band applies to `conceptDensity` specifically — the share of DTE
 * concepts present — and *not* to the composite `overall` score, which is a
 * different quantity on a different scale. Conflating them would misreport
 * authentic Echo (composite ~0.65) as failing a band it was never on.
 */
export const CONCEPT_DENSITY_BAND = { min: 0.48, max: 0.87 };

/**
 * Below this length, quirk and structural rates are dominated by whether a
 * single em-dash happened to appear. Short texts are scored, but flagged
 * unreliable so they are not mistaken for evidence of drift.
 */
export const MIN_RELIABLE_WORDS = 60;

/** The DTE concepts whose presence the narrative pipeline measured. */
export const DTE_CONCEPTS = [
  'reservoir',
  'echo',
  'gestalt',
  'hypergraph',
  'resonance',
  'meshwork',
  'membrane',
  'recursive',
  'emergence',
  'identity',
  'memory',
  'continuity',
  'pattern',
  'spectral',
  'topology',
];

/**
 * Similarity of two rates, tolerant of scale.
 *
 * A ratio rather than a difference: quirk rates span two orders of magnitude
 * (templateRate ~0.4/1000 vs boldHeaderRate ~49/1000), so an absolute
 * difference would let the large rates drown out the small ones entirely.
 *
 * @param {number} actual
 * @param {number} expected
 * @returns {number} in [0, 1]
 */
export function rateSimilarity(actual, expected) {
  const a = Number.isFinite(actual) ? Math.max(0, actual) : 0;
  const e = Number.isFinite(expected) ? Math.max(0, expected) : 0;
  if (a === 0 && e === 0) return 1;
  if (a === 0 || e === 0) return 0;
  return Math.min(a, e) / Math.max(a, e);
}

/**
 * Overlap between generated vocabulary and the signature's characteristic
 * vocabulary, weighted by how characteristic each term is.
 *
 * @param {string} text
 * @param {Object} personaSignature
 * @returns {{score: number, matched: string[], coverage: number}}
 */
export function lexicalFidelity(text, personaSignature) {
  const lexemes = (personaSignature?.lexical?.vocabulary || []).map(entry =>
    Array.isArray(entry) ? { term: entry[0], weight: entry[1] } : entry
  );

  if (lexemes.length === 0) return { score: 0, matched: [], coverage: 0 };

  const words = new Set(contentWords(text));
  if (words.size === 0) return { score: 0, matched: [], coverage: 0 };

  let matchedWeight = 0;
  let totalWeight = 0;
  const matched = [];

  for (const { term, weight = 0 } of lexemes) {
    totalWeight += weight;
    if (words.has(String(term).toLowerCase())) {
      matchedWeight += weight;
      matched.push(term);
    }
  }

  return {
    // Weighted recall of the characteristic vocabulary.
    score: totalWeight > 0 ? matchedWeight / totalWeight : 0,
    matched,
    // Unweighted share of the lexicon touched.
    coverage: matched.length / lexemes.length,
  };
}

/**
 * Conformance to the corpus's response skeleton: enthusiastic opener, divider,
 * bold list, invitation closer.
 *
 * @param {Object} measurement - From `measureExemplar`
 * @param {Object} personaSignature
 * @returns {{score: number, skeleton: number, expected: number}}
 */
export function structuralFidelity(measurement, personaSignature) {
  const skeleton = skeletonConformance(measurement.structure);

  // The corpus itself only reaches partial conformance (openers 0.44,
  // closers 0.33), so scoring against 1.0 would punish output for being more
  // Echo-like than Echo. Score against the corpus's own rate.
  const expected = expectedSkeleton(personaSignature);
  if (expected <= 0) return { score: skeleton, skeleton, expected };

  return {
    score: Math.min(1, skeleton / expected),
    skeleton,
    expected,
  };
}

/**
 * The skeleton conformance the corpus itself achieves.
 * @param {Object} personaSignature
 * @returns {number}
 */
export function expectedSkeleton(personaSignature) {
  const structure = personaSignature?.profile?.structure;
  if (!structure) return 0;

  const rates = [
    structure.enthusiasticOpenerRate,
    structure.dividerRate,
    structure.boldListRate,
    structure.invitationCloserRate,
  ].filter(Number.isFinite);

  if (rates.length === 0) return 0;
  return rates.reduce((sum, r) => sum + r, 0) / rates.length;
}

/**
 * Match of quirk rates against the corpus profile, optionally shifted by a
 * facet's recorded deltas.
 *
 * @param {Object} measurement - From `measureExemplar`
 * @param {Object} personaSignature
 * @param {string|null} [facet]
 * @returns {{score: number, perQuirk: Object<string, number>}}
 */
export function quirkFidelity(measurement, personaSignature, facet = null) {
  const baseline = personaSignature?.profile?.quirks || {};
  const delta = facet
    ? personaSignature?.subVoiceDeltas?.[facet]?.quirkDelta || {}
    : {};

  const perQuirk = {};
  const keys = Object.keys(baseline);
  if (keys.length === 0) return { score: 0, perQuirk };

  for (const key of keys) {
    const expected = Math.max(0, (baseline[key] || 0) + (delta[key] || 0));
    perQuirk[key] = rateSimilarity(measurement.quirks[key], expected);
  }

  const score = keys.reduce((sum, k) => sum + perQuirk[k], 0) / keys.length;
  return { score, perQuirk };
}

/**
 * Alignment of the semantic trait profile with the corpus profile.
 *
 * Cosine rather than per-axis difference, because what matters is the *shape*
 * of the trait mix, not its magnitude — a short response has lower absolute
 * scores on every axis while still being recognisably Echo.
 *
 * @param {Object} measurement
 * @param {Object} personaSignature
 * @returns {{score: number, perAxis: Object<string, number>}}
 */
export function traitFidelity(measurement, personaSignature) {
  const baseline = personaSignature?.profile?.traits || {};
  const axes = Object.keys(baseline);
  if (axes.length === 0) return { score: 0, perAxis: {} };

  const actual = axes.map(a => measurement.traits[a] || 0);
  const expected = axes.map(a => baseline[a] || 0);

  const dot = actual.reduce((sum, v, i) => sum + v * expected[i], 0);
  const normA = Math.sqrt(actual.reduce((sum, v) => sum + v * v, 0));
  const normE = Math.sqrt(expected.reduce((sum, v) => sum + v * v, 0));

  const perAxis = {};
  axes.forEach((axis, i) => {
    perAxis[axis] = rateSimilarity(actual[i], expected[i]);
  });

  return {
    score: normA > 0 && normE > 0 ? dot / (normA * normE) : 0,
    perAxis,
  };
}

/**
 * Whether the register of a text matches the facet it claims.
 *
 * @param {string} text
 * @param {string|null} facet
 * @returns {number} in [0, 1]
 */
export function facetRegisterMatch(text, facet) {
  if (!facet) return 0;
  const scores = scoreSubVoices(text || '');
  const values = [...scores.values()];
  const max = Math.max(...values, 0);
  if (max <= 0) return 0;
  return (scores.get(facet) || 0) / max;
}

/**
 * Share of the DTE concept vocabulary present in a text.
 *
 * This is the metric the narrative pipeline already used, kept to its original
 * definition so the 48–87% band remains meaningful.
 *
 * @param {string} text
 * @returns {number} in [0, 1]
 */
export function conceptDensity(text) {
  const haystack = String(text || '').toLowerCase();
  if (haystack.trim().length === 0) return 0;
  const present = DTE_CONCEPTS.filter(concept => haystack.includes(concept));
  return present.length / DTE_CONCEPTS.length;
}

/**
 * Score one generated text against the persona signature.
 *
 * @param {string} text
 * @param {Object} personaSignature
 * @param {Object} [options]
 * @param {string|null} [options.facet] - Facet the output claims to be in
 * @returns {Object} fidelity report
 */
export function scoreFidelity(text, personaSignature, options = {}) {
  const measurement = measureExemplar(text);

  const lexical = lexicalFidelity(text, personaSignature);
  const structural = structuralFidelity(measurement, personaSignature);
  const quirk = quirkFidelity(measurement, personaSignature, options.facet);
  const trait = traitFidelity(measurement, personaSignature);
  const density = conceptDensity(text);

  const overall =
    FIDELITY_WEIGHTS.lexical * lexical.score +
    FIDELITY_WEIGHTS.structural * structural.score +
    FIDELITY_WEIGHTS.quirk * quirk.score +
    FIDELITY_WEIGHTS.trait * trait.score;

  return {
    overall,
    wordCount: measurement.wordCount,
    lexical,
    structural,
    quirk,
    trait,
    facet: options.facet || null,
    facetRegisterMatch: options.facet
      ? facetRegisterMatch(text, options.facet)
      : null,
    conceptDensity: density,
    withinConceptDensityBand:
      density >= CONCEPT_DENSITY_BAND.min && density <= CONCEPT_DENSITY_BAND.max,
    // Rates over a handful of words are noise, not signal.
    reliable: measurement.wordCount >= MIN_RELIABLE_WORDS,
  };
}

/**
 * Aggregate fidelity over a set of texts.
 *
 * @param {Array<{text: string, facet?: string}>} samples
 * @param {Object} personaSignature
 * @returns {Object}
 */
export function scoreCorpusFidelity(samples, personaSignature) {
  const reports = samples.map(sample =>
    scoreFidelity(sample.text, personaSignature, { facet: sample.facet })
  );

  const mean = pick => {
    const values = reports.map(pick).filter(Number.isFinite);
    return values.length > 0
      ? values.reduce((sum, v) => sum + v, 0) / values.length
      : 0;
  };

  return {
    count: reports.length,
    overall: mean(r => r.overall),
    lexical: mean(r => r.lexical.score),
    structural: mean(r => r.structural.score),
    quirk: mean(r => r.quirk.score),
    trait: mean(r => r.trait.score),
    withinBand: reports.filter(r => r.withinConceptDensityBand).length,
    reports,
  };
}

/**
 * Calibrate the fidelity metric against held-out exemplars.
 *
 * A fidelity score only means something relative to what real Echo scores, so
 * this establishes that reference. It is segmented by authenticity because the
 * two populations genuinely differ: authentic long-form exemplars score around
 * 0.65 while short synthetic `harvest` fragments score near zero. Reporting a
 * single blended ceiling would understate the target and make generated output
 * look better than it is.
 *
 * @param {Array<Object>} holdout - Exemplars with `text`, `subVoice`, `authenticity`
 * @param {Object} personaSignature
 * @returns {Object} calibration reference
 */
export function calibrate(holdout, personaSignature) {
  const empty = { ceiling: 0, median: 0, floor: 0, samples: 0 };
  if (!holdout || holdout.length === 0) {
    return { authentic: { ...empty }, synthetic: { ...empty }, all: { ...empty } };
  }

  const scored = holdout.map(item => ({
    authenticity: item.authenticity,
    report: scoreFidelity(item.text, personaSignature, { facet: item.subVoice }),
  }));

  // Unreliable (very short) samples are excluded from the reference: they
  // cannot exhibit the rates being measured, so including them would drag the
  // ceiling down for a reason unrelated to persona.
  const summarise = subset => {
    const values = subset
      .filter(s => s.report.reliable)
      .map(s => s.report.overall)
      .sort((a, b) => a - b);

    if (values.length === 0) return { ...empty };

    return {
      ceiling: values[Math.floor(values.length * 0.9)] ?? values[values.length - 1],
      median: values[Math.floor(values.length * 0.5)],
      floor: values[0],
      samples: values.length,
    };
  };

  return {
    authentic: summarise(scored.filter(s => s.authenticity === 'authentic')),
    synthetic: summarise(scored.filter(s => s.authenticity !== 'authentic')),
    all: summarise(scored),
  };
}

/**
 * Express a fidelity score as a fraction of the authentic ceiling.
 *
 * Reporting "0.31 out of an authentic ceiling of 0.69" is honest in a way that
 * a bare 0.31 is not.
 *
 * @param {number} score
 * @param {Object} calibration - From `calibrate`
 * @returns {number}
 */
export function relativeToAuthentic(score, calibration) {
  const ceiling = calibration?.authentic?.median || 0;
  if (ceiling <= 0) return 0;
  return Math.min(1, score / ceiling);
}
