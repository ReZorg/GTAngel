import { buildAndWriteCorpus } from './corpus/builder.js';
import logger from '../utils/logger.js';

/**
 * CLI entry point: consolidate the persona source materials into a curated,
 * provenance-tracked exemplar corpus under `data/processed/`.
 *
 * Usage: npm run build-corpus [-- --include-flagged]
 */
function main() {
  const includeFlagged = process.argv.includes('--include-flagged');

  const { exemplars, manifest, corpusPath, manifestPath } = buildAndWriteCorpus({
    includeFlagged,
  });

  logger.info('Persona corpus build complete', {
    exemplars: exemplars.length,
    excluded: manifest.counts.excluded,
    duplicates: manifest.counts.duplicates,
    authenticRatio: manifest.provenance.authenticRatio,
    corpusPath,
    manifestPath,
  });
}

try {
  main();
} catch (error) {
  logger.error('Persona corpus build failed:', error);
  process.exit(1);
}
