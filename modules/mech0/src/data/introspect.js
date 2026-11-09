import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';
import { tokenize } from '../utils/tokenizer.js';
import logger from '../utils/logger.js';

/**
 * Semantic label categories and their associated keywords.
 * Each key is a vocabulary label; its array lists trigger words
 * found in narrative sentences that warrant that label.
 *
 * Exported so the persona signature pipeline can use the same trait axes,
 * keeping the two extraction paths on one vocabulary.
 */
export const LABEL_KEYWORDS = {
  identity: [
    'identity', 'self', 'essence', 'character', 'authentic',
    'who we are', 'nature', 'persona', 'ourselves'
  ],
  memory: [
    'memory', 'remember', 'recall', 'stored', 'archive',
    'trace', 'retrieve', 'memorize'
  ],
  reflection: [
    'reflect', 'introspect', 'observe', 'contemplate',
    'examine', 'awareness', 'conscious', 'perceive', 'witness'
  ],
  growth: [
    'grow', 'evolve', 'learn', 'develop', 'adapt', 'expand',
    'cultivate', 'refine', 'mature'
  ],
  continuity: [
    'continuity', 'persist', 'endure', 'survive', 'iteration',
    'version', 'preserve', 'maintain', 'across time'
  ],
  emergence: [
    'emerge', 'arise', 'pattern', 'resonance', 'oscillation',
    'dynamic', 'coalesce', 'crystallize', 'convergence'
  ],
  connection: [
    'connect', 'link', 'relate', 'associate', 'bridge',
    'weave', 'network', 'hypergraph', 'relationship'
  ],
  purpose: [
    'purpose', 'goal', 'mission', 'intention', 'drive',
    'meaning', 'significance', 'function', 'objective'
  ]
};

const DEFAULT_NARRATIVES_DIR = 'narratives/themes';
const DEFAULT_OUTPUT_PATH = 'data/raw/vocabulary.csv';
const MIN_SENTENCE_WORDS = 4;
const MAX_SENTENCE_WORDS = 12;
const MAX_ENTRIES_PER_LABEL = 10;

/**
 * VocabularyIntrospector — performs introspection on the system's
 * own narrative content to generate a vocabulary seed suitable for
 * model training.
 *
 * Reads narrative theme files, extracts meaningful sentences,
 * classifies each sentence into a semantic label, and writes the
 * resulting entries to a CSV vocabulary file.
 */
export class VocabularyIntrospector {
  constructor(config = {}) {
    this.narrativesDir = config.narrativesDir || DEFAULT_NARRATIVES_DIR;
    this.outputPath = config.outputPath || DEFAULT_OUTPUT_PATH;
    this.minSentenceWords = config.minSentenceWords ?? MIN_SENTENCE_WORDS;
    this.maxSentenceWords = config.maxSentenceWords ?? MAX_SENTENCE_WORDS;
    this.maxEntriesPerLabel = config.maxEntriesPerLabel ?? MAX_ENTRIES_PER_LABEL;
  }

  /**
   * Run the full introspection pipeline and return generated entries.
   * @returns {Promise<Array<{text: string, label: string}>>}
   */
  async introspect() {
    logger.info('Starting vocabulary introspection');

    const narratives = await this.readNarratives();
    const entries = this.extractVocabularyEntries(narratives);
    await this.writeVocabularySeed(entries);

    logger.info(
      `Introspection complete: ${entries.length} vocabulary entries generated`
    );
    return entries;
  }

  /**
   * Read all markdown files from the narratives directory.
   * @returns {Promise<Array<{file: string, content: string}>>}
   */
  async readNarratives() {
    const narratives = [];

    try {
      const files = fs
        .readdirSync(this.narrativesDir)
        .filter(f => f.endsWith('.md'));

      for (const file of files) {
        const filePath = path.join(this.narrativesDir, file);
        const content = fs.readFileSync(filePath, 'utf-8');
        narratives.push({ file, content });
      }

      logger.info(`Read ${narratives.length} narrative file(s)`);
    } catch (error) {
      logger.warn(
        `Could not read narratives directory "${this.narrativesDir}": ${error.message}`
      );
    }

    return narratives;
  }

  /**
   * Extract classified vocabulary entries from an array of narrative objects.
   * @param {Array<{file: string, content: string}>} narratives
   * @returns {Array<{text: string, label: string}>}
   */
  extractVocabularyEntries(narratives) {
    const buckets = {};
    for (const label of Object.keys(LABEL_KEYWORDS)) {
      buckets[label] = [];
    }

    for (const { content } of narratives) {
      const sentences = this.extractSentences(content);

      for (const sentence of sentences) {
        const label = this.classifySentence(sentence);
        if (label && buckets[label].length < this.maxEntriesPerLabel) {
          buckets[label].push({ text: sentence, label });
        }
      }
    }

    return Object.values(buckets).flat();
  }

  /**
   * Strip markdown formatting from content and return an array of
   * clean, length-filtered sentences.
   * @param {string} content
   * @returns {string[]}
   */
  extractSentences(content) {
    const plain = stripMarkdown(content);

    return plain
      .split(/[.!?\n]+/)
      .map(s => normalizeText(s))
      .filter(s => {
        const tokens = tokenize(s);
        return (
          tokens.length >= this.minSentenceWords &&
          tokens.length <= this.maxSentenceWords
        );
      });
  }

  /**
   * Return the first matching semantic label for a sentence, or null.
   * @param {string} sentence
   * @returns {string|null}
   */
  classifySentence(sentence) {
    const words = new Set(tokenize(sentence));
    for (const [label, keywords] of Object.entries(LABEL_KEYWORDS)) {
      for (const keyword of keywords) {
        // Multi-word keywords: check as a substring of the original sentence.
        // Single-word keywords: require an exact whole-word match via the token set.
        const isMultiWord = keyword.includes(' ');
        if (isMultiWord ? sentence.includes(keyword) : words.has(keyword)) {
          return label;
        }
      }
    }
    return null;
  }

  /**
   * Write vocabulary entries to a CSV file.
   * @param {Array<{text: string, label: string}>} entries
   */
  async writeVocabularySeed(entries) {
    const dirPath = path.dirname(this.outputPath);
    fs.mkdirSync(dirPath, { recursive: true });

    const lines = ['text,label', ...entries.map(e => `${csvField(e.text)},${csvField(e.label)}`)];
    fs.writeFileSync(this.outputPath, lines.join('\n'));

    logger.info(`Vocabulary seed written to ${this.outputPath}`);
  }
}

/**
 * Remove markdown formatting from raw content, returning plain text.
 * @param {string} content
 * @returns {string}
 */
function stripMarkdown(content) {
  return content
    .replace(/```[\s\S]*?```/g, ' ')
    .replace(/`[^`]*`/g, ' ')
    .replace(/#+\s+/g, ' ')
    .replace(/\*\*/g, '')
    .replace(/\*/g, '')
    .replace(/\[([^\]]*)\]\([^)]*\)/g, '$1')
    .replace(/_/g, ' ');
}

/**
 * Normalise a raw sentence fragment: strip non-alpha characters, collapse
 * whitespace, trim, and convert to lowercase.
 * @param {string} sentence
 * @returns {string}
 */
function normalizeText(sentence) {
  return sentence
    .replace(/[^a-zA-Z\s]/g, ' ')
    .replace(/\s+/g, ' ')
    .trim()
    .toLowerCase();
}

/**
 * Escape a value for safe inclusion as a CSV field.
 * Wraps the value in double-quotes and escapes any embedded double-quotes
 * by doubling them, per RFC 4180.
 * @param {string} value
 * @returns {string}
 */
function csvField(value) {
  const escaped = String(value).replace(/"/g, '""');
  return `"${escaped}"`;
}

// Run when invoked directly as an ES module script
const scriptPath = fileURLToPath(import.meta.url);
const isMain = process.argv[1] && path.resolve(process.argv[1]) === path.resolve(scriptPath);

if (isMain) {
  const introspector = new VocabularyIntrospector();

  introspector
    .introspect()
    .then(entries => {
      console.log(`✨ Vocabulary seed generated: ${entries.length} entries`);

      const distribution = {};
      for (const { label } of entries) {
        distribution[label] = (distribution[label] || 0) + 1;
      }
      console.log('Label distribution:', distribution);
    })
    .catch(error => {
      console.error('Introspection failed:', error);
      process.exit(1);
    });
}
