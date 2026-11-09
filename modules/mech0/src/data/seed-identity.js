#!/usr/bin/env node
/**
 * Seed a durable identity from the persona signature.
 *
 * Usage:
 *   node src/data/seed-identity.js [--signature <path>] [--dir <path>] [--force]
 *
 * This gives a fresh deployment an identity that has already lived through its
 * own corpus, rather than one that starts blank and waits to be told who it is.
 */
import { MemorySystem } from '../model/core/memory/system.js';
import { loadSignature, DEFAULT_SIGNATURE_PATH } from '../persona/signature.js';
import { DEFAULT_IDENTITY_DIR } from '../model/core/memory/file-store.js';
import logger from '../utils/logger.js';

function parseArgs(argv) {
  const args = { signature: DEFAULT_SIGNATURE_PATH, dir: DEFAULT_IDENTITY_DIR, force: false };

  for (let i = 0; i < argv.length; i += 1) {
    if (argv[i] === '--signature') args.signature = argv[++i];
    else if (argv[i] === '--dir') args.dir = argv[++i];
    else if (argv[i] === '--force') args.force = true;
  }

  return args;
}

async function main() {
  const args = parseArgs(process.argv.slice(2));
  const signature = loadSignature(args.signature);

  const system = new MemorySystem({
    memory: { provider: 'file', file: { directory: args.dir } },
  });

  const result = await system.seed(signature, { force: args.force });
  await system.shutdown();

  const snapshot = system.orchestrator.getIdentitySnapshot();

  process.stdout.write(
    [
      '',
      'Identity seeding',
      '================',
      `signature:      ${args.signature}`,
      `identity dir:   ${args.dir}`,
      `fingerprint:    ${result.fingerprint}`,
      `seeded:         ${result.seeded ? 'yes' : 'already current'}`,
      `exemplars:      ${result.exemplars}`,
      `core traits:    ${snapshot.signature.coreTraits.size}`,
      `patterns:       ${snapshot.signature.characteristicPatterns.size}`,
      `meshwork:       ${snapshot.meshworkSize}`,
      `preservation:   ${snapshot.continuityMetrics.identityPreservation.toFixed(3)}`,
      '',
    ].join('\n')
  );
}

main().catch((error) => {
  logger.error('Identity seeding failed:', error);
  process.exitCode = 1;
});
