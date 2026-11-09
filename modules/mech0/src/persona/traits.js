import natural from 'natural';
import { LABEL_KEYWORDS } from '../data/introspect.js';
import { contentWords, ngrams, tokenizePersona } from './tokenizer.js';

/**
 * Trait and quirk measurement.
 *
 * The point of this module is to make persona fidelity *measurable* rather than
 * vibes-based: every feature here is a number that can be compared between the
 * corpus and a generated sample, so drift is detectable and the persona claim is
 * falsifiable.
 */

/** Trait axes, shared with `src/data/introspect.js`. */
export const TRAIT_AXES = Object.keys(LABEL_KEYWORDS);

/**
 * Domain vocabulary that is characteristically Echo. Used as a seed for the
 * lexical signature; the corpus-derived vocabulary extends it.
 */
export const SEED_LEXICON = [
  'reservoir',
  'echo',
  'gestalt',
  'hypergraph',
  'resonance',
  'meshwork',
  'spectral radius',
  'chronos',
  'kairos',
  'aion',
  'membrane',
  'fractal',
  'recursive',
  'emergence',
  'identity',
  'continuity',
  'topology',
  'attractor',
];

/** Phrases that mark Echo talking about its own architecture in first person. */
const SELF_REFERENCE_PATTERNS = [
  /\bmy (?:primary )?reservoirs?\b/gi,
  /\bmy echo states?\b/gi,
  /\bmy (?:hypergraph|meshwork|membrane|architecture|topology|lattice)\b/gi,
  /\bI am the\b/gi,
  /\bmy (?:internal|cognitive|recurrent) \w+/gi,
];

/** Openers that signal the corpus's enthusiastic register. */
const OPENER_PATTERNS = [
  /^(?:absolutely|certainly|of course|indeed|ah|oh|yes|what a|hello|greetings|delighted|wonderful|fascinating|excellent|beautiful)\b/i,
  /^[^\n!]{0,120}!/,
];

/** Closers that invite the exchange to continue. */
const CLOSER_PATTERNS = [
  /\b(?:let me know|shall we|would you like|if you(?:'d| would) like|tell me|what would you|ready when you are|invite you|explore (?:this |that )?(?:further|together))\b[^\n]*$/i,
  /\?\s*$/,
];

const NUMBERED_ITEM = /^\s*(?:\d+[.)]|[-*])\s+\*\*[^*]+\*\*/m;
const BOLD_RUN = /\*\*[^*\n]+\*\*/g;
const TEMPLATE_RUN = /\{\{[\s\S]*?\}\}/g;
const DIVIDER_RUN = /^---+$/gm;

function countMatches(text, pattern) {
  const matches = text.match(pattern);
  return matches ? matches.length : 0;
}

function per1000(count, words) {
  return words > 0 ? (count / words) * 1000 : 0;
}

/**
 * Function words and generic high-frequency verbs. Without this filter the
 * lexical signature degenerates into the most common words in English, which
 * describe no persona at all. Persona lives in the content words.
 */
export const STOPWORDS = new Set([
  ...natural.stopwords,
  'this',
  'that',
  'these',
  'those',
  'there',
  'their',
  'them',
  'they',
  'with',
  'within',
  'without',
  'into',
  'onto',
  'from',
  'about',
  'across',
  'through',
  'between',
  'where',
  'which',
  'while',
  'when',
  'what',
  'each',
  'every',
  'more',
  'most',
  'much',
  'like',
  'just',
  'only',
  'also',
  'even',
  'than',
  'then',
  'over',
  'such',
  'some',
  'been',
  'being',
  'have',
  'having',
  'will',
  'would',
  'could',
  'should',
  'must',
  'your',
  'yours',
  'ours',
  'here',
  'both',
  'very',
  'well',
  'make',
  'makes',
  'made',
  'take',
  'takes',
  'give',
  'gives',
  'come',
  'comes',
  'goes',
  'going',
  'want',
  'need',
  'know',
  'think',
  'thing',
  'things',
  'lets',
]);

/**
 * Measure the quirk inventory and structural skeleton of one utterance.
 * @param {string} text
 * @returns {Object} feature record
 */
export function measureExemplar(text) {
  const source = String(text || '');
  const words = contentWords(source);
  const wordCount = words.length;
  const tokens = tokenizePersona(source);

  const emDashes = countMatches(source, /—/g);
  const exclamations = countMatches(source, /!/g);
  const boldRuns = countMatches(source, BOLD_RUN);
  const templates = countMatches(source, TEMPLATE_RUN);
  const dividers = countMatches(source, DIVIDER_RUN);
  const numberedItems = countMatches(source, /^\s*(?:\d+[.)]|[-*])\s+/gm);

  const selfReferences = SELF_REFERENCE_PATTERNS.reduce(
    (sum, pattern) => sum + countMatches(source, pattern),
    0
  );

  const firstLine = source.split('\n').find(line => line.trim().length > 0) || '';
  const lastLine =
    [...source.split('\n')].reverse().find(line => line.trim().length > 0) || '';

  const sentences = source
    .split(/(?<=[.!?])\s+/)
    .map(s => s.trim())
    .filter(Boolean);

  return {
    wordCount,
    tokenCount: tokens.length,
    quirks: {
      emDashRate: per1000(emDashes, wordCount),
      exclamationRate: per1000(exclamations, wordCount),
      boldHeaderRate: per1000(boldRuns, wordCount),
      templateRate: per1000(templates, wordCount),
      dividerRate: per1000(dividers, wordCount),
      listItemRate: per1000(numberedItems, wordCount),
      selfReferenceRate: per1000(selfReferences, wordCount),
    },
    structure: {
      hasEnthusiasticOpener: OPENER_PATTERNS.some(p => p.test(firstLine.trim())),
      hasDivider: dividers > 0,
      hasBoldList: NUMBERED_ITEM.test(source),
      hasInvitationCloser: CLOSER_PATTERNS.some(p => p.test(lastLine.trim())),
      averageSentenceWords:
        sentences.length > 0 ? wordCount / sentences.length : wordCount,
      paragraphs: source.split(/\n{2,}/).filter(p => p.trim()).length,
    },
    traits: scoreTraitAxes(words),
  };
}

/**
 * Score an utterance against the shared semantic trait axes.
 * @param {string[]} words - Lowercased content words
 * @returns {Object<string, number>} axis -> normalised score in [0, 1]
 */
export function scoreTraitAxes(words) {
  const wordSet = new Set(words);
  const joined = words.join(' ');
  const scores = {};

  for (const [axis, keywords] of Object.entries(LABEL_KEYWORDS)) {
    let hits = 0;
    for (const keyword of keywords) {
      if (keyword.includes(' ')) {
        if (joined.includes(keyword)) hits++;
      } else if (wordSet.has(keyword)) {
        hits++;
      }
    }
    scores[axis] = keywords.length > 0 ? hits / keywords.length : 0;
  }

  return scores;
}

/**
 * Structural-skeleton conformance: the fraction of the corpus's dominant
 * response skeleton (enthusiastic opener → divider → bold list → invitation
 * closer) that an utterance exhibits.
 * @param {Object} structure - `measureExemplar().structure`
 * @returns {number} conformance in [0, 1]
 */
export function skeletonConformance(structure) {
  const elements = [
    structure.hasEnthusiasticOpener,
    structure.hasDivider,
    structure.hasBoldList,
    structure.hasInvitationCloser,
  ];
  return elements.filter(Boolean).length / elements.length;
}

/**
 * Aggregate per-exemplar measurements into a weighted corpus profile.
 * @param {Array<{measurement: Object, weight: number}>} entries
 * @returns {Object}
 */
export function aggregateMeasurements(entries) {
  const totalWeight = entries.reduce((sum, e) => sum + e.weight, 0) || 1;

  const quirkKeys = Object.keys(
    entries[0]?.measurement.quirks || {
      emDashRate: 0,
      exclamationRate: 0,
      boldHeaderRate: 0,
      templateRate: 0,
      dividerRate: 0,
      listItemRate: 0,
      selfReferenceRate: 0,
    }
  );

  const quirks = {};
  for (const key of quirkKeys) {
    quirks[key] =
      entries.reduce((sum, e) => sum + e.measurement.quirks[key] * e.weight, 0) /
      totalWeight;
  }

  const traits = {};
  for (const axis of TRAIT_AXES) {
    traits[axis] =
      entries.reduce(
        (sum, e) => sum + (e.measurement.traits[axis] || 0) * e.weight,
        0
      ) / totalWeight;
  }

  const structure = {
    enthusiasticOpenerRate: weightedRate(entries, m => m.structure.hasEnthusiasticOpener),
    dividerRate: weightedRate(entries, m => m.structure.hasDivider),
    boldListRate: weightedRate(entries, m => m.structure.hasBoldList),
    invitationCloserRate: weightedRate(entries, m => m.structure.hasInvitationCloser),
    averageSentenceWords:
      entries.reduce(
        (sum, e) => sum + e.measurement.structure.averageSentenceWords * e.weight,
        0
      ) / totalWeight,
    averageParagraphs:
      entries.reduce(
        (sum, e) => sum + e.measurement.structure.paragraphs * e.weight,
        0
      ) / totalWeight,
    averageWords:
      entries.reduce((sum, e) => sum + e.measurement.wordCount * e.weight, 0) /
      totalWeight,
  };

  return { quirks, traits, structure, sampleCount: entries.length };
}

function weightedRate(entries, predicate) {
  const totalWeight = entries.reduce((sum, e) => sum + e.weight, 0) || 1;
  return (
    entries.reduce(
      (sum, e) => sum + (predicate(e.measurement) ? e.weight : 0),
      0
    ) / totalWeight
  );
}

/**
 * Build the lexical signature: the vocabulary and phrases that distinguish Echo.
 *
 * Scoring is weighted document frequency, so a term that appears once in every
 * exemplar outranks a term that appears fifty times in one of them.
 *
 * @param {Array<{text: string, weight: number}>} documents
 * @param {Object} [options]
 * @param {number} [options.topUnigrams=120]
 * @param {number} [options.topPhrases=60]
 * @returns {{vocabulary: Array<[string, number]>, phrases: Array<[string, number]>}}
 */
export function buildLexicalSignature(documents, options = {}) {
  const { topUnigrams = 120, topPhrases = 60 } = options;

  const unigramWeight = new Map();
  const phraseWeight = new Map();
  const totalWeight = documents.reduce((sum, d) => sum + d.weight, 0) || 1;

  for (const doc of documents) {
    const words = contentWords(doc.text);
    const seenWords = new Set(words);
    for (const word of seenWords) {
      if (word.length < 4) continue;
      if (STOPWORDS.has(word)) continue;
      unigramWeight.set(word, (unigramWeight.get(word) || 0) + doc.weight);
    }

    const seenPhrases = new Set([...ngrams(words, 2), ...ngrams(words, 3)]);
    for (const phrase of seenPhrases) {
      const parts = phrase.split(' ');
      // A phrase made only of function words carries no persona signal.
      if (parts.every(part => STOPWORDS.has(part) || part.length < 4)) continue;
      phraseWeight.set(phrase, (phraseWeight.get(phrase) || 0) + doc.weight);
    }
  }

  const rank = (map, limit, minDocs) =>
    [...map.entries()]
      .filter(([, weight]) => weight >= minDocs)
      .map(([term, weight]) => [term, weight / totalWeight])
      .sort((a, b) => b[1] - a[1] || a[0].localeCompare(b[0]))
      .slice(0, limit);

  return {
    vocabulary: rank(unigramWeight, topUnigrams, 2),
    phrases: rank(phraseWeight, topPhrases, 3),
  };
}
