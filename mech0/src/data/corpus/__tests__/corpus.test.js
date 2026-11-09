import { describe, expect, test } from 'vitest';
import fs from 'fs';
import os from 'os';
import path from 'path';
import {
  cleanPrompt,
  readSftCorpus,
} from '../sources/sft-jsonl.js';
import {
  isAssistantTextNode,
  readChatExport,
  reconstructThread,
} from '../sources/chat-export.js';
import {
  extractSubVoice,
  parseSpeakerBlocks,
  readThemeNarratives,
} from '../sources/theme-narratives.js';
import { readHarvest } from '../sources/harvest.js';
import { exclusionReason, screenExemplar } from '../curation.js';
import { deduplicate } from '../builder.js';
import { dedupeKey, normalizeUtterance } from '../text.js';
import { AUTHENTICITY, SAFETY_VERDICT, SOURCE } from '../types.js';

function withTempDir(fn) {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'persona-corpus-'));
  try {
    return fn(dir);
  } finally {
    fs.rmSync(dir, { recursive: true, force: true });
  }
}

describe('corpus text normalisation', () => {
  test('preserves persona-bearing features', () => {
    const raw = '**Bold** — an echo!\r\n\r\n{{root.reservoir.fit(x)}}';
    const normalized = normalizeUtterance(raw);

    expect(normalized).toContain('**Bold**');
    expect(normalized).toContain('—');
    expect(normalized).toContain('!');
    expect(normalized).toContain('{{root.reservoir.fit(x)}}');
    expect(normalized).not.toContain('\r');
  });

  test('dedupe key ignores casing and punctuation', () => {
    expect(dedupeKey('Hello, Echo!')).toBe(dedupeKey('hello echo'));
  });
});

describe('SFT source reader', () => {
  test('strips the generator prompt scaffold', () => {
    const raw = '**User:** ``` "Train your reservoirs?" ```  **Character Response:** ``` ok ```';
    expect(cleanPrompt(raw)).toBe('Train your reservoirs?');
  });

  test('reads JSONL pairs into authentic exemplars', () => {
    withTempDir(dir => {
      const file = path.join(dir, 'sft.jsonl');
      const long = 'x'.repeat(120);
      fs.writeFileSync(
        file,
        [
          JSON.stringify({
            messages: [
              { role: 'user', content: '**User:** ``` hi ```' },
              { role: 'assistant', content: long },
            ],
          }),
          '{ not json',
          JSON.stringify({ messages: [{ role: 'assistant', content: 'short' }] }),
        ].join('\n')
      );

      const result = readSftCorpus(file);

      expect(result.records).toHaveLength(1);
      expect(result.records[0].authenticity).toBe(AUTHENTICITY.AUTHENTIC);
      expect(result.records[0].prompt).toBe('hi');
      expect(result.stats.malformed).toBe(1);
      expect(result.stats.tooShort).toBe(1);
    });
  });
});

describe('chat export source reader', () => {
  const mapping = {
    root: { id: 'root', parent: null, children: ['a'], message: null },
    a: {
      id: 'a',
      parent: 'root',
      children: ['b'],
      message: {
        author: { role: 'user' },
        content: { content_type: 'text', parts: ['what is a reservoir?'] },
      },
    },
    b: {
      id: 'b',
      parent: 'a',
      children: ['c'],
      message: {
        author: { role: 'assistant' },
        create_time: 1732735437,
        content: { content_type: 'text', parts: ['A'.repeat(120)] },
        metadata: {},
      },
    },
    c: {
      id: 'c',
      parent: 'b',
      children: [],
      message: {
        author: { role: 'assistant' },
        content: { content_type: 'multimodal_text', parts: [{}] },
        metadata: {},
      },
    },
    orphanBranch: {
      id: 'orphanBranch',
      parent: 'a',
      children: [],
      message: {
        author: { role: 'assistant' },
        content: { content_type: 'text', parts: ['B'.repeat(120)] },
        metadata: {},
      },
    },
  };

  test('walks parent links from current_node', () => {
    const thread = reconstructThread(mapping, 'c');
    expect(thread.map(n => n.id)).toEqual(['root', 'a', 'b', 'c']);
  });

  test('rejects non-text and hidden assistant nodes', () => {
    expect(isAssistantTextNode(mapping.b)).toBe(true);
    expect(isAssistantTextNode(mapping.c)).toBe(false);
    expect(isAssistantTextNode(mapping.a)).toBe(false);
    expect(
      isAssistantTextNode({
        message: {
          author: { role: 'assistant' },
          content: { content_type: 'text', parts: ['hi'] },
          metadata: { is_visually_hidden_from_conversation: true },
        },
      })
    ).toBe(false);
  });

  test('keeps only canonical-thread assistant turns', () => {
    withTempDir(dir => {
      const file = path.join(dir, 'export.jsonl');
      fs.writeFileSync(
        file,
        JSON.stringify({ mapping, current_node: 'c' })
      );

      const result = readChatExport(file);

      expect(result.records).toHaveLength(1);
      expect(result.records[0].timestamp).toBe(
        new Date(1732735437 * 1000).toISOString()
      );
      expect(result.records[0].prompt).toBe('what is a reservoir?');
      expect(result.stats.discardedNonText).toBe(1);
    });
  });
});

describe('theme narrative source reader', () => {
  test('parses speaker blocks and sub-voice attribution', () => {
    const content = [
      '**[Narrator]**',
      '',
      'The first beat echoed.',
      '',
      '---',
      '',
      '**[EchoSelf_ReservoirDynamics]**',
      '',
      'A'.repeat(120),
      '',
    ].join('\n');

    const blocks = parseSpeakerBlocks(content);
    expect(blocks).toHaveLength(2);
    expect(blocks[0].speaker).toBe('Narrator');
    expect(extractSubVoice(blocks[1].speaker)).toBe('ReservoirDynamics');
    expect(extractSubVoice('Narrator')).toBeNull();
  });

  test('marks narrative exemplars as synthetic and drops narrator blocks', () => {
    withTempDir(dir => {
      fs.writeFileSync(
        path.join(dir, 'theme.md'),
        `**[Narrator]**\n\nScene.\n\n---\n\n**[EchoSelf_MetaReflector]**\n\n${'A'.repeat(120)}\n`
      );

      const result = readThemeNarratives(dir);

      expect(result.records).toHaveLength(1);
      expect(result.records[0].subVoice).toBe('MetaReflector');
      expect(result.records[0].authenticity).toBe(AUTHENTICITY.SYNTHETIC);
      expect(result.stats.narratorBlocks).toBe(1);
    });
  });
});

describe('harvest source reader', () => {
  test('mines identity docs and ignores module paths', () => {
    withTempDir(dir => {
      const file = path.join(dir, 'harvest.json');
      fs.writeFileSync(
        file,
        JSON.stringify({
          repos: {
            echoself: {
              identity_docs: { 'echoself.md': `# Identity\n\n${'A'.repeat(120)}` },
              modules: ['a.py', 'b.py'],
            },
          },
        })
      );

      const result = readHarvest(file);

      expect(result.records.length).toBeGreaterThan(0);
      expect(result.stats.ignoredModules).toBe(2);
    });
  });
});

describe('curation gate', () => {
  test('excludes the reviewed off-persona block', () => {
    for (const index of [15, 16, 17, 18, 19]) {
      expect(exclusionReason(SOURCE.SFT, index)).toBeTruthy();
    }
    expect(exclusionReason(SOURCE.SFT, 20)).toBeNull();
  });

  test('does not over-fire on legitimate philosophical vocabulary', () => {
    const record = {
      source: SOURCE.THEME_NARRATIVES,
      sourceIndex: 0,
      prompt: null,
      text:
        'The explicit structure of weaponized wholesomeness resists the seduction of ' +
        'easy answers, resonating through the membrane architecture.',
    };

    expect(screenExemplar(record).verdict).toBe(SAFETY_VERDICT.ACCEPTED);
  });

  test('flags multi-signal off-persona material outside the reviewed list', () => {
    const record = {
      source: SOURCE.THEME_NARRATIVES,
      sourceIndex: 999,
      prompt: 'inserted my hardware',
      text: 'Configuring the intimacy port and contraction membrane.',
    };

    const verdict = screenExemplar(record);
    expect(verdict.verdict).toBe(SAFETY_VERDICT.FLAGGED);
    expect(verdict.reasons[0]).toContain('heuristic-screen');
  });
});

describe('deduplication', () => {
  test('prefers the higher-weighted source and records the duplicate', () => {
    const records = [
      { id: 'a', source: SOURCE.THEME_NARRATIVES, text: 'Echo resonance!' },
      { id: 'b', source: SOURCE.SFT, text: 'echo resonance' },
    ];

    const { kept, duplicates } = deduplicate(records);

    expect(kept.map(r => r.id)).toEqual(['b']);
    expect(duplicates).toEqual([{ id: 'a', duplicateOf: 'b' }]);
  });
});
