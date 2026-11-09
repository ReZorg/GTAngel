import fs from 'fs';
import path from 'path';
import crypto from 'crypto';
import logger from '../../../utils/logger.js';
import { readJsonSafe, writeJsonAtomic } from './fs-utils.js';
import { DEFAULT_IDENTITY_DIR } from './file-store.js';
import {
  deserializeHolographic,
  MEMORY_SNAPSHOT_VERSION,
  restoreNetwork,
  serializeHolographic,
  serializeNetwork,
} from './serialization.js';

/**
 * Stable digest of a snapshot, used to avoid burning a version slot on an
 * identity that has not actually changed.
 * @param {Object} snapshot
 * @returns {string}
 */
function contentDigest(snapshot) {
  return crypto
    .createHash('sha256')
    .update(JSON.stringify(snapshot))
    .digest('hex')
    .slice(0, 16);
}

/**
 * Durable identity persistence.
 *
 * This is the load-bearing piece of "persistent identity": without it, the
 * signature is recomputed from nothing on every boot and the claim is empty.
 *
 * Three tiers are persisted with different cadences:
 *   - the identity signature, written whenever it changes (the crown jewels);
 *   - the memory store, checkpointed on the maintenance interval; and
 *   - the memory network, checkpointed alongside the store.
 *
 * Identity snapshots are versioned in the same shape as `models/versions.json`,
 * so a signature can be rolled back exactly as a model version can.
 */
export class IdentityPersistence {
  constructor(config = {}) {
    const fileConfig = config.memory?.file || {};
    this.directory = fileConfig.directory || DEFAULT_IDENTITY_DIR;

    this.identityPath =
      fileConfig.identityPath || path.join(this.directory, 'identity.json');
    this.networkPath =
      fileConfig.networkPath || path.join(this.directory, 'memory-network.json');
    this.versionsPath =
      fileConfig.versionsPath || path.join(this.directory, 'versions.json');

    this.maxVersions = fileConfig.maxVersions ?? 10;

    // Seeded from the registry so that a restart which changes nothing does not
    // immediately write a duplicate version.
    this.lastDigest = this.loadVersions().versions.at(-1)?.digest ?? null;
  }

  /**
   * Load the current identity snapshot, ready for `importHolographicData()`.
   * @returns {Object|null}
   */
  loadIdentity() {
    const snapshot = readJsonSafe(this.identityPath);
    if (!snapshot) return null;

    if (snapshot.version !== MEMORY_SNAPSHOT_VERSION) {
      logger.warn(
        `Identity snapshot version ${snapshot.version} does not match ` +
          `${MEMORY_SNAPSHOT_VERSION}; ignoring snapshot.`
      );
      return null;
    }

    return deserializeHolographic(snapshot);
  }

  /**
   * Persist the identity signature and record a rollback-able version.
   * @param {Object} exported - `HolographicIdentity.exportHolographicData()`
   * @param {Object} [metrics] - Snapshot metrics recorded in the version registry
   * @returns {Object} version info
   */
  saveIdentity(exported, metrics = {}) {
    const snapshot = serializeHolographic(exported);
    const digest = contentDigest(snapshot);

    // An unchanged identity should not consume a version slot; otherwise routine
    // maintenance ticks would evict real history out of the rollback window.
    if (digest === this.lastDigest) return null;

    writeJsonAtomic(this.identityPath, snapshot);

    const registry = this.loadVersions();
    const versionNumber = (registry.currentVersion ?? registry.versions.length) + 1;
    const versionFile = `identity_v${versionNumber}.json`;

    writeJsonAtomic(path.join(this.directory, versionFile), snapshot);

    const versionInfo = {
      version: versionNumber,
      timestamp: new Date().toISOString(),
      snapshotVersion: MEMORY_SNAPSHOT_VERSION,
      digest,
      metrics,
      file: versionFile,
    };

    registry.versions.push(versionInfo);
    registry.currentVersion = versionNumber;
    writeJsonAtomic(this.versionsPath, registry);

    this.pruneOldVersions();
    this.lastDigest = digest;

    logger.info(`Persisted identity snapshot v${versionNumber}`);
    return versionInfo;
  }

  /**
   * Load the identity version registry.
   * @returns {{versions: Array<Object>, currentVersion: number|null}}
   */
  loadVersions() {
    return (
      readJsonSafe(this.versionsPath, null) || {
        versions: [],
        currentVersion: null,
      }
    );
  }

  /**
   * Load a specific historical identity version.
   * @param {number} versionNumber
   * @returns {Object|null}
   */
  loadIdentityVersion(versionNumber) {
    const registry = this.loadVersions();
    const info = registry.versions.find(v => v.version === versionNumber);
    if (!info) return null;

    const snapshot = readJsonSafe(path.join(this.directory, info.file));
    return snapshot ? deserializeHolographic(snapshot) : null;
  }

  /**
   * Roll the current identity back to a previous version.
   * @param {number} versionNumber
   * @returns {Object|null} the restored data, or null when unavailable
   */
  rollbackTo(versionNumber) {
    const registry = this.loadVersions();
    const info = registry.versions.find(v => v.version === versionNumber);
    if (!info) return null;

    const snapshot = readJsonSafe(path.join(this.directory, info.file));
    if (!snapshot) return null;

    writeJsonAtomic(this.identityPath, snapshot);
    registry.currentVersion = versionNumber;
    writeJsonAtomic(this.versionsPath, registry);
    this.lastDigest = info.digest ?? contentDigest(snapshot);

    logger.warn(`Rolled identity back to v${versionNumber}`);
    return deserializeHolographic(snapshot);
  }

  /**
   * Drop version files beyond the retention window, keeping the registry entries
   * so the history remains inspectable.
   */
  pruneOldVersions() {
    const registry = this.loadVersions();
    const excess = registry.versions.length - this.maxVersions;
    if (excess <= 0) return;

    for (const info of registry.versions.slice(0, excess)) {
      if (info.pruned) continue;
      try {
        fs.rmSync(path.join(this.directory, info.file), { force: true });
      } catch {
        // Best-effort cleanup: a leftover snapshot file is harmless.
      }
      info.pruned = true;
    }

    writeJsonAtomic(this.versionsPath, registry);
  }

  /**
   * Load the persisted memory network.
   * @param {Object} network - `MemoryNetwork` instance to populate
   * @returns {boolean} true when state was restored
   */
  loadNetwork(network) {
    const snapshot = readJsonSafe(this.networkPath);
    if (!snapshot) return false;

    restoreNetwork(network, snapshot);
    logger.info(`Restored memory network from ${this.networkPath}`, {
      nodes: network.nodes.size,
      edges: network.edges.size,
    });
    return true;
  }

  /**
   * Persist the memory network.
   * @param {Object} network - `MemoryNetwork` instance
   */
  saveNetwork(network) {
    writeJsonAtomic(this.networkPath, serializeNetwork(network));
  }
}
