import { describe, it, expect, beforeAll, afterEach } from 'vitest';
import fs from 'fs';
import os from 'os';
import path from 'path';
import {
  rateSimilarity,
  lexicalFidelity,
  structuralFidelity,
  quirkFidelity,
  traitFidelity,
  facetRegisterMatch,
  conceptDensity,
  scoreFidelity,
  scoreCorpusFidelity,
  calibrate,
  relativeToAuthentic,
  expectedSkeleton,
  MIN_RELIABLE_WORDS,
  CONCEPT_DENSITY_BAND,
  DTE_CONCEPTS,
} from '../evaluation.js';
import { loadSignature, DEFAULT_SIGNATURE_PATH } from '../signature.js';
import { EchoEngine } from '../engine.js';
import {
  loadHoldout,
  decisiveTest,
  continuityRegression,
  writePersonaMetrics,
} from '../../data/evaluate-persona.js';

let signature;
let holdout;

beforeAll(() => {
  signature = loadSignature(DEFAULT_SIGNATURE_PATH);
  holdout = loadHoldout(signature);
});

const ECHO_LIKE = `Ah, what a delightful question to explore together!

---

1. **Reservoir Dynamics**: My primary reservoir maintains a spectral radius just below unity — the edge of chaos where memory and computation meet.
2. **Echo States**: Each input leaves a resonance in the meshwork, and those echoes are what let identity persist across time.
3. **Emergent Gestalt**: The hypergraph topology means patterns are not stored so much as continuously re-membered!

Shall we trace this thread further?`;

const GENERIC = `Sure, I can help with that. Here is a short answer to your question. Let me know if you need anything else and I will be happy to assist you further with whatever you need next in our conversation.`;

describe('rateSimilarity', () => {
  it('is 1 for identical rates', () => {
    expect(rateSimilarity(5, 5)).toBe(1);
  });

  it('is 1 when both are absent', () => {
    expect(rateSimilarity(0, 0)).toBe(1);
  });

  it('is 0 when one is absent and the other is not', () => {
    expect(rateSimilarity(0, 5)).toBe(0);
    expect(rateSimilarity(5, 0)).toBe(0);
  });

  it('is scale-tolerant, so small rates are not drowned out', () => {
    // A 2x deviation scores the same whether the rate is 0.4 or 40 per 1000.
    expect(rateSimilarity(0.4, 0.8)).toBeCloseTo(rateSimilarity(40, 80), 10);
  });

  it('is symmetric', () => {
    expect(rateSimilarity(3, 7)).toBe(rateSimilarity(7, 3));
  });

  it('handles non-finite input', () => {
    expect(rateSimilarity(NaN, 5)).toBe(0);
    expect(rateSimilarity(Infinity, 5)).toBeGreaterThanOrEqual(0);
  });
});

describe('component metrics', () => {
  it('scores Echo-like text above generic text lexically', () => {
    expect(lexicalFidelity(ECHO_LIKE, signature).score).toBeGreaterThan(
      lexicalFidelity(GENERIC, signature).score
    );
  });

  it('reports which characteristic terms matched', () => {
    const result = lexicalFidelity(ECHO_LIKE, signature);
    expect(result.matched.length).toBeGreaterThan(0);
    expect(result.coverage).toBeGreaterThan(0);
  });

  it('returns zero lexical fidelity for empty text', () => {
    expect(lexicalFidelity('', signature).score).toBe(0);
  });

  it('scores the response skeleton against the corpus rate, not against 1.0', () => {
    // The corpus itself only partially conforms, so scoring against a perfect
    // skeleton would punish output for being more Echo-like than Echo.
    expect(expectedSkeleton(signature)).toBeGreaterThan(0);
    expect(expectedSkeleton(signature)).toBeLessThan(1);
  });

  it('gives full structural credit to text with the whole skeleton', () => {
    const measurement = { structure: {
      hasEnthusiasticOpener: true,
      hasDivider: true,
      hasBoldList: true,
      hasInvitationCloser: true,
    } };
    expect(structuralFidelity(measurement, signature).score).toBe(1);
  });

  it('scores quirk rates against the corpus profile', () => {
    const measurement = {
      quirks: { ...signature.profile.quirks },
      structure: {},
      traits: {},
    };
    expect(quirkFidelity(measurement, signature).score).toBeCloseTo(1, 6);
  });

  it('shifts the quirk target when a facet is supplied', () => {
    const measurement = { quirks: { ...signature.profile.quirks } };
    const base = quirkFidelity(measurement, signature).score;
    const facet = quirkFidelity(measurement, signature, 'MetaReflector').score;
    expect(facet).not.toBeCloseTo(base, 6);
  });

  it('scores trait shape by cosine, so length does not dominate', () => {
    const full = { traits: { ...signature.profile.traits } };
    const scaled = {
      traits: Object.fromEntries(
        Object.entries(signature.profile.traits).map(([k, v]) => [k, v * 0.1])
      ),
    };
    expect(traitFidelity(full, signature).score).toBeCloseTo(
      traitFidelity(scaled, signature).score,
      6
    );
  });

  it('matches facet register against the claimed facet', () => {
    expect(
      facetRegisterMatch('How do you recall and retrieve a memory?', 'MemoryNavigator')
    ).toBe(1);
    expect(
      facetRegisterMatch('How do you recall and retrieve a memory?', 'TreeArchitect')
    ).toBeLessThan(1);
  });

  it('returns zero facet match without a facet', () => {
    expect(facetRegisterMatch(ECHO_LIKE, null)).toBe(0);
  });
});

describe('conceptDensity', () => {
  it('rises with the number of DTE concepts present', () => {
    expect(conceptDensity(ECHO_LIKE)).toBeGreaterThan(conceptDensity(GENERIC));
  });

  it('is 0 for empty text and 1 when every concept is present', () => {
    expect(conceptDensity('')).toBe(0);
    expect(conceptDensity(DTE_CONCEPTS.join(' '))).toBe(1);
  });

  it('keeps the precedent band separate from the composite score', () => {
    // The 48–87% band came from the narrative pipeline's concept-density
    // metric, not from this composite; conflating them would misreport
    // authentic Echo as failing a band it was never on.
    expect(CONCEPT_DENSITY_BAND.min).toBe(0.48);
    expect(CONCEPT_DENSITY_BAND.max).toBe(0.87);
  });
});

describe('scoreFidelity', () => {
  it('separates Echo-like from generic text', () => {
    expect(scoreFidelity(ECHO_LIKE, signature).overall).toBeGreaterThan(
      scoreFidelity(GENERIC, signature).overall * 1.5
    );
  });

  it('scores unrelated technical prose near zero', () => {
    const wiki =
      'The mitochondrion is an organelle found in most eukaryotic cells. It generates adenosine triphosphate through oxidative phosphorylation.';
    expect(scoreFidelity(wiki, signature).overall).toBeLessThan(0.15);
  });

  it('bounds the overall score to [0, 1]', () => {
    for (const text of [ECHO_LIKE, GENERIC, '', 'a']) {
      const score = scoreFidelity(text, signature).overall;
      expect(score).toBeGreaterThanOrEqual(0);
      expect(score).toBeLessThanOrEqual(1);
    }
  });

  it('flags short text as unreliable rather than treating it as drift', () => {
    expect(scoreFidelity('Reservoir echo.', signature).reliable).toBe(false);
    expect(scoreFidelity(ECHO_LIKE, signature).reliable).toBe(true);
    expect(scoreFidelity(ECHO_LIKE, signature).wordCount).toBeGreaterThanOrEqual(
      MIN_RELIABLE_WORDS
    );
  });

  it('is deterministic', () => {
    expect(scoreFidelity(ECHO_LIKE, signature)).toEqual(
      scoreFidelity(ECHO_LIKE, signature)
    );
  });
});

describe('calibration against held-out exemplars', () => {
  it('loads the holdout named by the signature', () => {
    expect(holdout.length).toBe(signature.evaluation.holdoutIds.length);
  });

  it('holds the persona test set out of training entirely', () => {
    const trainIds = new Set(signature.seedExemplars.map(e => e.id));
    for (const item of holdout) {
      expect(trainIds.has(item.id)).toBe(false);
    }
  });

  it('separates authentic from synthetic sources', () => {
    // The metric independently recovers the contamination split the plan warns
    // about: synthetic `harvest` fragments score materially below authentic
    // long-form exemplars.
    const calibration = calibrate(holdout, signature);
    expect(calibration.authentic.samples).toBeGreaterThan(0);
    expect(calibration.authentic.median).toBeGreaterThan(calibration.synthetic.median);
  });

  it('excludes unreliably short samples from the reference', () => {
    const calibration = calibrate(holdout, signature);
    expect(calibration.all.samples).toBeLessThanOrEqual(holdout.length);
    expect(calibration.all.floor).toBeGreaterThan(0);
  });

  it('reports scores relative to the authentic ceiling', () => {
    const calibration = calibrate(holdout, signature);
    const generic = scoreFidelity(GENERIC, signature).overall;
    expect(relativeToAuthentic(generic, calibration)).toBeLessThan(0.5);
    expect(relativeToAuthentic(calibration.authentic.median, calibration)).toBeCloseTo(
      1,
      6
    );
  });

  it('degrades safely on an empty holdout', () => {
    const calibration = calibrate([], signature);
    expect(calibration.authentic.samples).toBe(0);
    expect(relativeToAuthentic(0.5, calibration)).toBe(0);
  });

  it('aggregates corpus fidelity', () => {
    const aggregate = scoreCorpusFidelity(
      holdout.map(h => ({ text: h.text, facet: h.subVoice })),
      signature
    );
    expect(aggregate.count).toBe(holdout.length);
    expect(aggregate.overall).toBeGreaterThan(0);
    expect(aggregate.reports).toHaveLength(holdout.length);
  });
});

function tempConfig() {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'echo-eval-'));
  return {
    dir,
    config: {
      memory: { provider: 'file', file: { directory: dir, maxVersions: 3 } },
      model: { embeddingDim: 16 },
      persona: {},
    },
  };
}

describe('the decisive test: persona with no prompt of any kind', () => {
  const dirs = [];

  afterEach(() => {
    while (dirs.length) fs.rmSync(dirs.pop(), { recursive: true, force: true });
  });

  it('scores higher with persona state than without, on identical weights', async () => {
    const { dir, config } = tempConfig();
    dirs.push(dir);

    const engine = new EchoEngine(config);
    await engine.seed();

    // Both arms generate from an empty context with no instruction, using the
    // same weights and the same sampling seed. The only difference is whether
    // the persisted identity conditions decoding. If persona-on wins, the
    // persona is in the state — there was never a prompt to remove.
    const result = decisiveTest(engine, signature, { samples: 4, maxTokens: 48 });
    await engine.shutdown();

    expect(result.personaOn).toBeGreaterThan(result.personaOff);
    expect(result.lift).toBeGreaterThan(0);
    expect(result.passed).toBe(true);
  }, 60000);

  it('draws its advantage from Echo characteristic vocabulary', async () => {
    const { dir, config } = tempConfig();
    dirs.push(dir);

    const engine = new EchoEngine(config);
    await engine.seed();
    const result = decisiveTest(engine, signature, { samples: 4, maxTokens: 48 });
    await engine.shutdown();

    expect(result.lexicalOn).toBeGreaterThan(result.lexicalOff);
  }, 60000);

  it('restores the prior it was given after running both arms', async () => {
    const { dir, config } = tempConfig();
    dirs.push(dir);

    const engine = new EchoEngine(config);
    const before = engine.model.personaPrior;
    decisiveTest(engine, signature, { samples: 2, maxTokens: 8 });
    expect(engine.model.personaPrior).not.toBeNull();
    expect(engine.model.personaPrior.hiddenBias).toEqual(before.hiddenBias);
    await engine.shutdown();
  }, 60000);
});

describe('continuity regression', () => {
  it('passes across a restart with a durable provider', async () => {
    const { dir, config } = tempConfig();
    try {
      const result = await continuityRegression(config);
      expect(result.skipped).toBeUndefined();
      expect(result.traitsPreserved).toBe(true);
      expect(result.meshworkPreserved).toBe(true);
      expect(result.preservationUnchanged).toBe(true);
      expect(result.conditioningUnchanged).toBe(true);
      expect(result.passed).toBe(true);
    } finally {
      fs.rmSync(dir, { recursive: true, force: true });
    }
  }, 60000);

  it('refuses to report a result on a RAM-only store', async () => {
    // A continuity test against the in-memory store measures nothing; with
    // weaker assertions it would *pass*, which is worse than failing.
    const result = await continuityRegression({ memory: { provider: 'local' } });
    expect(result.skipped).toBe(true);
    expect(result.passed).toBe(false);
    expect(result.reason).toMatch(/durable/i);
  });
});

describe('writePersonaMetrics', () => {
  it('merges persona metrics without discarding existing results', () => {
    const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'echo-metrics-'));
    const file = path.join(dir, 'evaluation_results.json');
    fs.writeFileSync(file, JSON.stringify({ accuracy: 0.4, averageLoss: 0.9 }));

    try {
      const merged = writePersonaMetrics({ decisiveTest: { passed: true } }, file);
      expect(merged.accuracy).toBe(0.4);
      expect(merged.averageLoss).toBe(0.9);
      expect(merged.persona.decisiveTest.passed).toBe(true);

      const reread = JSON.parse(fs.readFileSync(file, 'utf-8'));
      expect(reread.accuracy).toBe(0.4);
      expect(reread.persona).toBeTruthy();
    } finally {
      fs.rmSync(dir, { recursive: true, force: true });
    }
  });

  it('creates the file when none exists', () => {
    const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'echo-metrics-'));
    const file = path.join(dir, 'nested', 'evaluation_results.json');
    try {
      writePersonaMetrics({ overall: 1 }, file);
      expect(fs.existsSync(file)).toBe(true);
    } finally {
      fs.rmSync(dir, { recursive: true, force: true });
    }
  });

  it('does not destroy an unparseable existing file silently', () => {
    const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'echo-metrics-'));
    const file = path.join(dir, 'evaluation_results.json');
    fs.writeFileSync(file, 'not json');
    try {
      const merged = writePersonaMetrics({ overall: 1 }, file);
      expect(merged.persona.overall).toBe(1);
    } finally {
      fs.rmSync(dir, { recursive: true, force: true });
    }
  });
});
