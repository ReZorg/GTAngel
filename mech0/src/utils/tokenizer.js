import natural from 'natural';

const tokenizer = new natural.WordTokenizer();

export function tokenize(text) {
  // Convert to lowercase and tokenize
  const tokens = tokenizer.tokenize(text.toLowerCase());
  
  // Remove punctuation and numbers
  return tokens.filter(token => 
    token.match(/^[a-zA-Z]+$/)
  );
}

export function createVocabulary(tokens) {
  const vocabulary = new Set();
  tokens.forEach(token => vocabulary.add(token));
  return Array.from(vocabulary);
}

/**
 * Index map for a vocabulary array.
 *
 * `encodeTokens` used `Array.indexOf` per token — O(n) in the vocabulary for
 * every token encoded, which is quadratic overall and unusable at persona-corpus
 * scale. The index is memoised per array so that repeated calls across batches
 * do not rebuild it.
 * @param {string[]|Map<string, number>} vocabulary
 * @returns {Map<string, number>}
 */
const vocabularyIndexCache = new WeakMap();

export function vocabularyIndex(vocabulary) {
  if (vocabulary instanceof Map) return vocabulary;

  const cached = vocabularyIndexCache.get(vocabulary);
  if (cached && cached.length === vocabulary.length) return cached.index;

  const index = new Map();
  vocabulary.forEach((token, i) => {
    if (!index.has(token)) index.set(token, i);
  });

  vocabularyIndexCache.set(vocabulary, { index, length: vocabulary.length });
  return index;
}

export function encodeTokens(tokens, vocabulary) {
  const index = vocabularyIndex(vocabulary);

  const encoded = [];
  for (const token of tokens) {
    const position = index.get(token);
    if (position !== undefined) encoded.push(position);
  }
  return encoded;
}