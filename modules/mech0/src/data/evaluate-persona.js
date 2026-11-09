import fs from 'fs';
import path from 'path';
import { EchoEngine } from '../persona/engine.js';
import { loadSignature, DEFAULT_SIGNATURE_PATH } from '../persona/signature.js';
import {
  scoreFidelity,
  scoreCorpusFidelity,
  calibrate,
  relativeToAuthentic,
} from '../persona/evaluation.js';
import { getConfig } from '../utils/config.js';
import logger from '../utils/logger.js';

/**
 * Persona fidelity evaluation.
 *
 * Answers two questions:
 *   1. Is the persona present without prompting? (the decisive test)
 *   2. Does the identity survive a restart unchanged? (continuity regression)
 *
 * Usage: node src/data/evaluate-persona.js
 */

const CORPUS_PATH = 'data/processed/persona_corpus.jsonl';

/**
 * Deterministic RNG so a fidelity number is reproducible between runs and any
 * change is attributable to the model rather than to sampling luck.
 * @param {number} seed
 * @returns {Function}
 */
function seededRng(seed) {
  let state = seed >>> 0;
  return () => {
    state = (Math.imul(state, 1103515245) + 12345) >>> 0;
    return state / 4294967296;
  };
}

/**
 * Load the held-out exemplars named by the signature.
 * @param {Object} signature
 * @returns {Array<Object>}
 */
export function loadHoldout(signature, corpusPath = CORPUS_PATH) {
  if (!fs.existsSync(corpusPath)) return [];
  const ids = new Set(signature.evaluation?.holdoutIds || []);
  return fs
    .readFileSync(corpusPath, 'utf-8')
    .trim()
    .split('\n')
    .filter(Boolean)
    .map(line => JSON.parse(line))
    .filter(exemplar => ids.has(exemplar.id));
}

/**
 * The decisive test: generate with an empty context and no instruction of any
 * kind, with the persona prior on and then off, holding the weights and the
 * sampling seed fixed.
 *
 * An absolute score would only say the output is Echo-ish. This controlled
 * comparison says *why*: if persona-on scores materially higher than
 * persona-off on identical weights and an identical seed, the difference is
 * attributable to persistent identity state and nothing else. There is no
 * prompt in either arm.
 *
 * @param {EchoEngine} engine
 * @param {Object} signature
 * @param {Object} [options]
 * @returns {Object}
 */
export function decisiveTest(engine, signature, options = {}) {
  const samples = options.samples || 5;
  const maxTokens = options.maxTokens || 60;

  const withPrior = engine.prior ? engine.prior.toModelPrior() : null;

  const run = prior => {
    engine.model.setPersonaPrior(prior);
    const reports = [];
    for (let i = 0; i < samples; i += 1) {
      const generated = engine.model.generate([], {
        maxTokens,
        temperature: 0.8,
        topK: 30,
        rng: seededRng(1000 + i),
      });
      reports.push(scoreFidelity(generated.text, signature));
    }
    return reports;
  };

  const on = run(withPrior);
  const off = run(null);
  engine.model.setPersonaPrior(withPrior);

  const mean = reports =>
    reports.reduce((sum, r) => sum + r.overall, 0) / (reports.length || 1);
  const meanLexical = reports =>
    reports.reduce((sum, r) => sum + r.lexical.score, 0) / (reports.length || 1);

  const personaOn = mean(on);
  const personaOff = mean(off);

  return {
    samples,
    personaOn,
    personaOff,
    lift: personaOn - personaOff,
    lexicalOn: meanLexical(on),
    lexicalOff: meanLexical(off),
    // The persona is implicit if identity state alone, with no prompt in
    // either arm, moves the output toward Echo.
    passed: personaOn > personaOff,
    example: on[0]?.overall !== undefined ? on : [],
  };
}

/**
 * Continuity regression: snapshot identity, run a session, restart the process,
 * reload, and assert the signature is unchanged.
 *
 * This is what distinguishes persistent identity from a warm cache.
 *
 * @param {Object} config
 * @returns {Promise<Object>}
 */
export async function continuityRegression(config) {
  // A continuity test run against the RAM-only `local` store would be
  // measuring nothing at all — and with weaker assertions it would *pass*,
  // which is worse than failing. Refuse to report a result rather than report
  // a meaningless one.
  const provider = config?.memory?.provider;
  if (provider !== 'file') {
    return {
      skipped: true,
      reason:
        `Continuity requires a durable memory provider; got '${provider}'. ` +
        "Set memory.provider to 'file'.",
      passed: false,
    };
  }

  const first = new EchoEngine(config);
  await first.seed();

  await first.respond('What do you remember about your own architecture?', {
    maxTokens: 16,
    temperature: 0,
  });

  const before = first.getIdentityStatus();
  const beforeBias = [...first.prior.baseHiddenBias];
  await first.shutdown();

  // Restart: a fresh engine over the same directory.
  const second = new EchoEngine(config);
  const after = second.getIdentityStatus();
  const afterBias = [...second.prior.baseHiddenBias];
  await second.shutdown();

  const biasUnchanged =
    beforeBias.length === afterBias.length &&
    beforeBias.every((v, i) => Math.abs(v - afterBias[i]) < 1e-12);

  return {
    before,
    after,
    traitsPreserved: before.coreTraits === after.coreTraits,
    meshworkPreserved: before.meshworkSize === after.meshworkSize,
    preservationUnchanged:
      before.continuityMetrics.identityPreservation ===
      after.continuityMetrics.identityPreservation,
    conditioningUnchanged: biasUnchanged,
    passed:
      before.coreTraits === after.coreTraits &&
      before.meshworkSize === after.meshworkSize &&
      biasUnchanged,
  };
}

/**
 * Merge persona metrics into the existing evaluation results artefact, so
 * fidelity is tracked alongside accuracy and regressions show up in the
 * version-comparison flow that already exists.
 *
 * @param {Object} personaMetrics
 * @param {string} resultsPath
 */
export function writePersonaMetrics(personaMetrics, resultsPath) {
  let existing = {};
  if (fs.existsSync(resultsPath)) {
    try {
      existing = JSON.parse(fs.readFileSync(resultsPath, 'utf-8'));
    } catch (error) {
      logger.warn('Could not parse existing evaluation results', {
        error: error.message,
      });
    }
  }

  const merged = { ...existing, persona: personaMetrics };
  fs.mkdirSync(path.dirname(resultsPath), { recursive: true });
  fs.writeFileSync(resultsPath, JSON.stringify(merged, null, 2));
  return merged;
}

export async function main() {
  const config = getConfig();
  const signature = loadSignature(DEFAULT_SIGNATURE_PATH);
  const holdout = loadHoldout(signature);

  logger.info(`Evaluating persona fidelity over ${holdout.length} held-out exemplars`);

  const calibration = calibrate(holdout, signature);
  const holdoutScores = scoreCorpusFidelity(
    holdout.map(h => ({ text: h.text, facet: h.subVoice })),
    signature
  );

  // Continuity is about surviving a restart, so it is evaluated against the
  // durable provider regardless of what the ambient config selects.
  const durableConfig = {
    ...config,
    memory: { ...config.memory, provider: 'file' },
  };

  const engine = new EchoEngine(durableConfig);
  await engine.seed();

  const decisive = decisiveTest(engine, signature);
  await engine.shutdown();

  const continuity = await continuityRegression(durableConfig);

  const metrics = {
    evaluatedAt: new Date().toISOString(),
    signatureVersion: signature.schemaVersion,
    calibration,
    holdout: {
      count: holdoutScores.count,
      overall: holdoutScores.overall,
      lexical: holdoutScores.lexical,
      structural: holdoutScores.structural,
      quirk: holdoutScores.quirk,
      trait: holdoutScores.trait,
    },
    decisiveTest: {
      passed: decisive.passed,
      personaOn: decisive.personaOn,
      personaOff: decisive.personaOff,
      lift: decisive.lift,
      relativeToAuthentic: relativeToAuthentic(decisive.personaOn, calibration),
    },
    continuityRegression: continuity.skipped
      ? { passed: false, skipped: true, reason: continuity.reason }
      : {
          passed: continuity.passed,
          traitsPreserved: continuity.traitsPreserved,
          meshworkPreserved: continuity.meshworkPreserved,
          preservationUnchanged: continuity.preservationUnchanged,
          conditioningUnchanged: continuity.conditioningUnchanged,
        },
  };

  const resultsPath = path.join(config.data.modelsDir, 'evaluation_results.json');
  writePersonaMetrics(metrics, resultsPath);

  logger.info('Persona evaluation complete', {
    decisiveTest: metrics.decisiveTest.passed ? 'PASS' : 'FAIL',
    continuity: metrics.continuityRegression.passed ? 'PASS' : 'FAIL',
    personaOn: metrics.decisiveTest.personaOn,
    personaOff: metrics.decisiveTest.personaOff,
    authenticCeiling: calibration.authentic.median,
    resultsPath,
  });

  return metrics;
}

if (process.argv[1] && import.meta.url.endsWith(path.basename(process.argv[1]))) {
  main().catch(error => {
    logger.error('Persona evaluation failed', { error: error.message });
    process.exitCode = 1;
  });
}
