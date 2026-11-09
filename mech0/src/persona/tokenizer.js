/**
 * Persona-aware tokenizer.
 *
 * `src/utils/tokenizer.js` lowercases and drops every non-alphabetic token,
 * which destroys precisely the features that *are* the persona: em-dashes,
 * `**bold**` emphasis, exclamation marks and the `{{root.reservoir.fit(...)}}`
 * self-execution quirk. This tokenizer is deliberately lossless with respect to
 * those features and is used for every persona measurement and for the
 * generative vocabulary.
 *
 * Token kinds emitted:
 *   - `template`  — `{{ ... }}` inline self-execution constructs, kept whole
 *   - `code`      — fenced or inline code spans, kept whole
 *   - `emphasis`  — `**bold**` / `*italic*` runs, kept whole
 *   - `heading`   — markdown heading markers
 *   - `divider`   — `---` section dividers
 *   - `word`      — words, including apostrophes and internal hyphens
 *   - `punct`     — punctuation, including em-dashes
 *   - `newline`   — paragraph structure
 */

const PATTERNS = [
  { kind: 'template', re: /\{\{[\s\S]*?\}\}/y },
  { kind: 'code', re: /```[\s\S]*?```|`[^`\n]*`/y },
  { kind: 'emphasis', re: /\*\*[^*\n]+\*\*|__[^_\n]+__|\*[^*\n]+\*/y },
  { kind: 'divider', re: /^-{3,}$/y },
  { kind: 'heading', re: /#{1,6}(?=\s)/y },
  { kind: 'newline', re: /\n+/y },
  { kind: 'space', re: /[ \t]+/y },
  { kind: 'word', re: /[A-Za-z0-9]+(?:[''\-_.][A-Za-z0-9]+)*/y },
  { kind: 'punct', re: /[^\sA-Za-z0-9]/y },
];

/**
 * Tokenize text into persona-preserving tokens.
 * @param {string} text
 * @returns {Array<{kind: string, value: string}>}
 */
export function tokenizePersona(text) {
  const input = typeof text === 'string' ? text : '';
  const tokens = [];
  let cursor = 0;

  while (cursor < input.length) {
    let matched = false;

    for (const { kind, re } of PATTERNS) {
      if (kind === 'divider') {
        const atLineStart = cursor === 0 || input[cursor - 1] === '\n';
        if (!atLineStart) continue;
      }

      re.lastIndex = cursor;
      const match = re.exec(input);
      if (!match || match.index !== cursor) continue;

      if (kind !== 'space') {
        tokens.push({
          kind,
          value: kind === 'newline' ? '\n' : match[0],
        });
      }

      cursor += match[0].length;
      matched = true;
      break;
    }

    if (!matched) cursor++;
  }

  return tokens;
}

/**
 * Flatten persona tokens to their surface strings, which is what the generative
 * vocabulary indexes.
 * @param {string} text
 * @returns {string[]}
 */
export function tokenizeToStrings(text) {
  return tokenizePersona(text).map(token => token.value);
}

/**
 * Reassemble a token stream into text. Spacing is reconstructed rather than
 * stored, so `detokenize(tokenizeToStrings(x))` is close to but not byte-equal
 * with `x`; it is close enough to read as Echo.
 * @param {string[]} tokens
 * @returns {string}
 */
export function detokenize(tokens) {
  let out = '';

  for (const token of tokens) {
    if (token === '\n') {
      out += '\n';
      continue;
    }
    const needsSpace =
      out.length > 0 &&
      !out.endsWith('\n') &&
      !out.endsWith(' ') &&
      !/^[,.;:!?)\]}%]$/.test(token) &&
      !/[([{]$/.test(out);

    out += (needsSpace ? ' ' : '') + token;
  }

  return out.replace(/[ \t]+\n/g, '\n').trim();
}

/**
 * Lowercased word tokens only — the unit used for lexical-signature counting.
 *
 * Words inside `**emphasis**` runs are included: in this corpus bold text is
 * where the highest-signal vocabulary lives, so dropping it would blind the
 * lexical signature to the persona's most characteristic terms. Template and
 * code spans stay out — they are measured separately as quirks.
 *
 * @param {string} text
 * @returns {string[]}
 */
export function contentWords(text) {
  const words = [];

  for (const token of tokenizePersona(text)) {
    if (token.kind === 'word') {
      words.push(token.value.toLowerCase());
      continue;
    }
    if (token.kind === 'emphasis') {
      const inner = token.value.replace(/^[*_]+|[*_]+$/g, '');
      for (const match of inner.matchAll(/[A-Za-z0-9]+(?:['’\-_.][A-Za-z0-9]+)*/g)) {
        words.push(match[0].toLowerCase());
      }
    }
  }

  return words;
}

/**
 * Contiguous word n-grams, used for characteristic-phrase extraction.
 * @param {string[]} words
 * @param {number} n
 * @returns {string[]}
 */
export function ngrams(words, n) {
  if (n <= 0 || words.length < n) return [];
  const grams = [];
  for (let i = 0; i + n <= words.length; i++) {
    grams.push(words.slice(i, i + n).join(' '));
  }
  return grams;
}
