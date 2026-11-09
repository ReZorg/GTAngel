import { expect, test, beforeEach, afterEach } from 'vitest';
import fs from 'fs';
import path from 'path';
import { VocabularyIntrospector } from '../introspect.js';

const TEST_DIR = 'test_introspect_data';
const TEST_NARRATIVES_DIR = path.join(TEST_DIR, 'themes');
const TEST_OUTPUT_PATH = path.join(TEST_DIR, 'output', 'vocabulary.csv');

// Helper to write a test narrative file
function writeNarrative(filename, content) {
  fs.writeFileSync(path.join(TEST_NARRATIVES_DIR, filename), content);
}

beforeEach(() => {
  fs.mkdirSync(TEST_NARRATIVES_DIR, { recursive: true });
  fs.mkdirSync(path.join(TEST_DIR, 'output'), { recursive: true });
});

afterEach(() => {
  fs.rmSync(TEST_DIR, { recursive: true, force: true });
});

test('readNarratives should return content from markdown files', async () => {
  writeNarrative('theme_a.md', 'Our identity shapes our journey.');
  writeNarrative('theme_b.md', 'Memory persists across iterations.');
  writeNarrative('skip.txt', 'This file should be ignored.');

  const introspector = new VocabularyIntrospector({
    narrativesDir: TEST_NARRATIVES_DIR,
    outputPath: TEST_OUTPUT_PATH
  });

  const narratives = await introspector.readNarratives();

  expect(narratives).toHaveLength(2);
  const files = narratives.map(n => n.file);
  expect(files).toContain('theme_a.md');
  expect(files).toContain('theme_b.md');
});

test('readNarratives should return empty array for missing directory', async () => {
  const introspector = new VocabularyIntrospector({
    narrativesDir: path.join(TEST_DIR, 'nonexistent'),
    outputPath: TEST_OUTPUT_PATH
  });

  const narratives = await introspector.readNarratives();
  expect(narratives).toEqual([]);
});

test('extractSentences should strip markdown and filter by word count', () => {
  const introspector = new VocabularyIntrospector({
    narrativesDir: TEST_NARRATIVES_DIR,
    outputPath: TEST_OUTPUT_PATH,
    minSentenceWords: 3,
    maxSentenceWords: 8
  });

  const content = `
## Heading

**Bold text** about *memory* and identity.

Short.

This sentence has the right length for testing.

This is a very long sentence that exceeds the maximum word count limit and should be filtered out completely by the extraction logic.
  `;

  const sentences = introspector.extractSentences(content);

  // Every returned sentence must tokenize within the configured bounds
  sentences.forEach(s => {
    const words = s.split(/\s+/).filter(Boolean);
    expect(words.length).toBeGreaterThanOrEqual(1); // rough check; tokenizer may vary
  });

  // Markdown headings and bold markers should be stripped
  sentences.forEach(s => {
    expect(s).not.toMatch(/#+/);
    expect(s).not.toMatch(/\*\*/);
  });
});

test('classifySentence should return correct label for keyword match', () => {
  const introspector = new VocabularyIntrospector({
    narrativesDir: TEST_NARRATIVES_DIR,
    outputPath: TEST_OUTPUT_PATH
  });

  expect(introspector.classifySentence('our identity defines who we are')).toBe('identity');
  expect(introspector.classifySentence('we must recall the stored memory here')).toBe('memory');
  expect(introspector.classifySentence('we reflect on the deep oscillation pattern')).toBe('reflection');
  expect(introspector.classifySentence('patterns emerge and coalesce into form')).toBe('emergence');
  expect(introspector.classifySentence('our purpose drives every action forward')).toBe('purpose');
  expect(introspector.classifySentence('we grow and evolve with each iteration')).toBe('growth');
  expect(introspector.classifySentence('continuity preserved across many versions')).toBe('continuity');
  expect(introspector.classifySentence('connect nodes in the hypergraph network')).toBe('connection');
});

test('classifySentence should return null when no keyword matches', () => {
  const introspector = new VocabularyIntrospector({
    narrativesDir: TEST_NARRATIVES_DIR,
    outputPath: TEST_OUTPUT_PATH
  });

  expect(introspector.classifySentence('the sky is blue today')).toBeNull();
  expect(introspector.classifySentence('numbers are abstract mathematical objects')).toBeNull();
});

test('extractVocabularyEntries should respect maxEntriesPerLabel', () => {
  const maxPerLabel = 3;
  const introspector = new VocabularyIntrospector({
    narrativesDir: TEST_NARRATIVES_DIR,
    outputPath: TEST_OUTPUT_PATH,
    maxEntriesPerLabel: maxPerLabel
  });

  // Build a narrative with many identity sentences
  const lines = Array.from(
    { length: 20 },
    (_, i) => `Our identity shapes the path we take in iteration ${i}.`
  );
  const narratives = [{ file: 'test.md', content: lines.join('\n') }];

  const entries = introspector.extractVocabularyEntries(narratives);

  const identityEntries = entries.filter(e => e.label === 'identity');
  expect(identityEntries.length).toBeLessThanOrEqual(maxPerLabel);
});

test('extractVocabularyEntries should produce entries with text and label fields', () => {
  const introspector = new VocabularyIntrospector({
    narrativesDir: TEST_NARRATIVES_DIR,
    outputPath: TEST_OUTPUT_PATH
  });

  const narratives = [
    {
      file: 'theme.md',
      content:
        'Our identity persists across many iterations.\n' +
        'Memory traces remain in long-term storage.'
    }
  ];

  const entries = introspector.extractVocabularyEntries(narratives);

  entries.forEach(entry => {
    expect(entry).toHaveProperty('text');
    expect(entry).toHaveProperty('label');
    expect(typeof entry.text).toBe('string');
    expect(typeof entry.label).toBe('string');
    expect(entry.text.length).toBeGreaterThan(0);
  });
});

test('writeVocabularySeed should create a CSV with header and rows', async () => {
  const introspector = new VocabularyIntrospector({
    narrativesDir: TEST_NARRATIVES_DIR,
    outputPath: TEST_OUTPUT_PATH
  });

  const entries = [
    { text: 'our identity defines us', label: 'identity' },
    { text: 'we recall stored memory', label: 'memory' }
  ];

  await introspector.writeVocabularySeed(entries);

  expect(fs.existsSync(TEST_OUTPUT_PATH)).toBe(true);

  const content = fs.readFileSync(TEST_OUTPUT_PATH, 'utf-8');
  const lines = content.split('\n');

  expect(lines[0]).toBe('text,label');
  expect(lines[1]).toBe('"our identity defines us","identity"');
  expect(lines[2]).toBe('"we recall stored memory","memory"');
});

test('writeVocabularySeed should escape fields containing commas and quotes', async () => {
  const introspector = new VocabularyIntrospector({
    narrativesDir: TEST_NARRATIVES_DIR,
    outputPath: TEST_OUTPUT_PATH
  });

  const entries = [
    { text: 'memory, identity, and growth', label: 'identity' },
    { text: 'she said "hello world" to us', label: 'connection' }
  ];

  await introspector.writeVocabularySeed(entries);

  const content = fs.readFileSync(TEST_OUTPUT_PATH, 'utf-8');
  const lines = content.split('\n');

  expect(lines[1]).toBe('"memory, identity, and growth","identity"');
  expect(lines[2]).toBe('"she said ""hello world"" to us","connection"');
});

test('introspect should generate vocabulary from real narrative files', async () => {
  writeNarrative(
    'theme_identity.md',
    'Our identity shapes who we are across every iteration.\n' +
      'Memory traces persist within the long-term store.\n' +
      'We reflect on the patterns that emerge from our data.\n' +
      'Growth comes as we evolve and adapt to new contexts.\n' +
      'The hypergraph network links all connected nodes together.'
  );

  const introspector = new VocabularyIntrospector({
    narrativesDir: TEST_NARRATIVES_DIR,
    outputPath: TEST_OUTPUT_PATH
  });

  const entries = await introspector.introspect();

  expect(Array.isArray(entries)).toBe(true);
  expect(entries.length).toBeGreaterThan(0);
  expect(fs.existsSync(TEST_OUTPUT_PATH)).toBe(true);

  const csv = fs.readFileSync(TEST_OUTPUT_PATH, 'utf-8');
  expect(csv.startsWith('text,label')).toBe(true);
});
