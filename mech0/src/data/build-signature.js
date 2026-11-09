import fs from 'fs';
import logger from '../utils/logger.js';
import {
  DEFAULT_CORPUS_PATH,
  DEFAULT_MANIFEST_PATH,
  loadCorpus,
} from './corpus/builder.js';
import { buildSignature, saveSignature } from '../persona/signature.js';

/**
 * CLI entry point: derive the persona signature from the curated corpus.
 *
 * Usage: npm run build-signature
 */
function main() {
  const exemplars = loadCorpus(DEFAULT_CORPUS_PATH);

  const manifest = fs.existsSync(DEFAULT_MANIFEST_PATH)
    ? JSON.parse(fs.readFileSync(DEFAULT_MANIFEST_PATH, 'utf-8'))
    : null;

  const signature = buildSignature(exemplars, { manifest });
  const outputPath = saveSignature(signature);

  logger.info('Persona signature build complete', {
    coreTraits: signature.identitySignature.coreTraits.length,
    characteristicPatterns: signature.identitySignature.characteristicPatterns.length,
    resonanceFrequency: signature.identitySignature.resonanceFrequency,
    facets: Object.keys(signature.subVoiceDeltas).length,
    holdout: signature.evaluation.holdoutCount,
    outputPath,
  });
}

try {
  main();
} catch (error) {
  logger.error('Persona signature build failed:', error);
  process.exit(1);
}
