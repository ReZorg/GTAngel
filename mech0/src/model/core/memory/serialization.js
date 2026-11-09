/**
 * Set-aware serialization for the memory subsystem.
 *
 * `MemoryStore` entries hold `associations` as a `Set`, the association index is
 * a `Map<string, Set<string>>`, and `MemoryNetwork` nodes hold `connections` as
 * a `Set`. `JSON.stringify` renders every one of those as `{}` — a silent,
 * total data loss that would leave a "persisted" identity with no associations
 * at all. Every structure that crosses the disk boundary is therefore converted
 * explicitly here rather than relying on structural cloning.
 */

/** On-disk format version for memory snapshots. */
export const MEMORY_SNAPSHOT_VERSION = 1;

/**
 * @param {Set<any>|Array<any>|undefined|null} value
 * @returns {Array<any>}
 */
function setToArray(value) {
  if (!value) return [];
  return Array.isArray(value) ? [...value] : [...value.values()];
}

/**
 * Serialize a single memory entry.
 * @param {Object} entry
 * @returns {Object}
 */
export function serializeEntry(entry) {
  return {
    content: entry.content,
    context: serializeContext(entry.context),
    timestamp: entry.timestamp,
    accessCount: entry.accessCount,
    associations: setToArray(entry.associations),
    version: entry.version,
    ...(entry.strength !== undefined ? { strength: entry.strength } : {}),
  };
}

/**
 * Deserialize a single memory entry, restoring its `Set`.
 * @param {Object} raw
 * @returns {Object}
 */
export function deserializeEntry(raw) {
  return {
    ...raw,
    context: raw.context || {},
    associations: new Set(raw.associations || []),
  };
}

/**
 * Contexts may carry a `memories` Set when a continuity context is active.
 * @param {Object} context
 * @returns {Object}
 */
function serializeContext(context) {
  if (!context || typeof context !== 'object') return {};
  const output = {};
  for (const [key, value] of Object.entries(context)) {
    output[key] = value instanceof Set ? [...value] : value;
  }
  return output;
}

/**
 * Serialize the whole memory store.
 * @param {Object} store - `MemoryStore` instance
 * @returns {Object}
 */
export function serializeStore(store) {
  return {
    version: MEMORY_SNAPSHOT_VERSION,
    shortTerm: [...store.shortTerm.entries()].map(([key, entry]) => [
      key,
      serializeEntry(entry),
    ]),
    longTerm: [...store.longTerm.entries()].map(([key, entry]) => [
      key,
      serializeEntry(entry),
    ]),
    associations: [...store.associations.entries()].map(([tag, keys]) => [
      tag,
      setToArray(keys),
    ]),
    lastAccess: [...store.lastAccess.entries()],
  };
}

/**
 * Restore a memory store in place from a snapshot.
 * @param {Object} store - `MemoryStore` instance to populate
 * @param {Object} snapshot
 */
export function restoreStore(store, snapshot) {
  if (!snapshot) return;

  store.shortTerm = new Map(
    (snapshot.shortTerm || []).map(([key, raw]) => [key, deserializeEntry(raw)])
  );
  store.longTerm = new Map(
    (snapshot.longTerm || []).map(([key, raw]) => [key, deserializeEntry(raw)])
  );
  store.associations = new Map(
    (snapshot.associations || []).map(([tag, keys]) => [tag, new Set(keys)])
  );
  store.lastAccess = new Map(snapshot.lastAccess || []);
}

/**
 * Serialize the memory network (nodes, edges and detected clusters).
 * @param {Object} network - `MemoryNetwork` instance
 * @returns {Object}
 */
export function serializeNetwork(network) {
  return {
    version: MEMORY_SNAPSHOT_VERSION,
    nodes: [...network.nodes.entries()].map(([key, node]) => [
      key,
      {
        memory: serializeEntry(node.memory || {}),
        connections: setToArray(node.connections),
        strength: node.strength,
        lastAccess: node.lastAccess,
      },
    ]),
    edges: [...network.edges.entries()],
    clusters: [...network.clusters.entries()].map(([id, members]) => [
      id,
      setToArray(members),
    ]),
  };
}

/**
 * Restore a memory network in place from a snapshot.
 * @param {Object} network - `MemoryNetwork` instance to populate
 * @param {Object} snapshot
 */
export function restoreNetwork(network, snapshot) {
  if (!snapshot) return;

  network.nodes = new Map(
    (snapshot.nodes || []).map(([key, node]) => [
      key,
      {
        ...node,
        memory: deserializeEntry(node.memory || {}),
        connections: new Set(node.connections || []),
      },
    ])
  );
  network.edges = new Map(snapshot.edges || []);
  network.clusters = new Map(
    (snapshot.clusters || []).map(([id, members]) => [id, new Set(members)])
  );
}

/**
 * Serialize holographic identity data produced by `exportHolographicData()`.
 *
 * Experience patterns carry `associations` and `connections` Sets, and the
 * identity signature carries a `coreTraits` Set and two Maps — all of which
 * need explicit handling.
 *
 * @param {Object} data - Result of `HolographicIdentity.exportHolographicData()`
 * @returns {Object}
 */
export function serializeHolographic(data) {
  return {
    version: MEMORY_SNAPSHOT_VERSION,
    experienceMeshwork: (data.experienceMeshwork || []).map(([id, pattern]) => [
      id,
      {
        ...pattern,
        associations: setToArray(pattern.associations),
        connections: setToArray(pattern.connections),
        context: serializeContext(pattern.context),
      },
    ]),
    gestaltPatterns: (data.gestaltPatterns || []).map(([type, pattern]) => [
      type,
      { ...pattern },
    ]),
    identitySignature: {
      coreTraits: setToArray(data.identitySignature?.coreTraits),
      characteristicPatterns: mapToEntries(
        data.identitySignature?.characteristicPatterns
      ),
      intentionality: mapToEntries(data.identitySignature?.intentionality),
      resonanceFrequency: data.identitySignature?.resonanceFrequency ?? 0,
    },
    continuityMetrics: { ...(data.continuityMetrics || {}) },
  };
}

/**
 * Rehydrate a holographic snapshot into the shape `importHolographicData()`
 * expects.
 * @param {Object} snapshot
 * @returns {Object}
 */
export function deserializeHolographic(snapshot) {
  return {
    experienceMeshwork: (snapshot.experienceMeshwork || []).map(([id, pattern]) => [
      id,
      {
        ...pattern,
        associations: new Set(pattern.associations || []),
        connections: new Set(pattern.connections || []),
      },
    ]),
    gestaltPatterns: (snapshot.gestaltPatterns || []).map(([type, pattern]) => [
      type,
      { ...pattern },
    ]),
    identitySignature: {
      coreTraits: new Set(snapshot.identitySignature?.coreTraits || []),
      characteristicPatterns: new Map(
        snapshot.identitySignature?.characteristicPatterns || []
      ),
      intentionality: new Map(snapshot.identitySignature?.intentionality || []),
      resonanceFrequency: snapshot.identitySignature?.resonanceFrequency ?? 0,
    },
    continuityMetrics: { ...(snapshot.continuityMetrics || {}) },
  };
}

/**
 * @param {Map<any, any>|Array<any>|undefined|null} value
 * @returns {Array<[any, any]>}
 */
function mapToEntries(value) {
  if (!value) return [];
  return Array.isArray(value) ? value : [...value.entries()];
}
