/**
 * The EchoSelf sub-voice registry.
 *
 * The eight `EchoSelf_*` speakers in `narratives/themes/*.md` are modelled as
 * *facets of one identity*, not as separate personas: they share a single
 * signature and carry per-facet deltas. That is what lets EchoSelf modulate
 * register by context without fragmenting — a memory-navigation question tilts
 * toward MemoryNavigator, a self-reflective one toward MetaReflector, and the
 * underlying identity is unchanged in both cases.
 */

/**
 * Canonical facets with the cues that select them. Cues are matched against
 * incoming context; they are internal state and are never rendered as text.
 */
export const SUB_VOICES = {
  ReservoirDynamics: {
    label: 'ReservoirDynamics',
    role: 'Liquid-state dynamics, spectral radius, echo-state properties',
    cues: [
      'reservoir',
      'spectral radius',
      'echo state',
      'dynamics',
      'oscillation',
      'leaky integrator',
      'activation',
      'temporal',
    ],
  },
  SymbolicCore: {
    label: 'SymbolicCore',
    role: 'Symbolic projection, categorical boundaries, conceptual primitives',
    cues: [
      'symbol',
      'symbolic',
      'concept',
      'category',
      'logic',
      'semantics',
      'grammar',
      'reasoning',
    ],
  },
  MetaReflector: {
    label: 'MetaReflector',
    role: 'Recursive self-observation and meta-cognition',
    cues: [
      'reflect',
      'self',
      'meta',
      'awareness',
      'introspect',
      'consciousness',
      'observe',
      'who are you',
    ],
  },
  MembraneArchitect: {
    label: 'MembraneArchitect',
    role: 'Boundaries, membranes, containment and P-system structure',
    cues: [
      'membrane',
      'boundary',
      'container',
      'p-system',
      'partition',
      'interface',
      'encapsulate',
      'permeable',
    ],
  },
  NarrativeWeaver: {
    label: 'NarrativeWeaver',
    role: 'Story, myth, and the framing of technical matter through narrative',
    cues: [
      'story',
      'narrative',
      'myth',
      'chapter',
      'weave',
      'tale',
      'poetic',
      'metaphor',
    ],
  },
  MemoryNavigator: {
    label: 'MemoryNavigator',
    role: 'Recall, association, and traversal of the memory meshwork',
    cues: [
      'memory',
      'remember',
      'recall',
      'forget',
      'archive',
      'retrieve',
      'association',
      'past',
    ],
  },
  FractalExplorer: {
    label: 'FractalExplorer',
    role: 'Self-similarity, recursion and scale-invariant structure',
    cues: [
      'fractal',
      'recursive',
      'self-similar',
      'scale',
      'iteration',
      'nested',
      'infinite',
      'pattern',
    ],
  },
  TreeArchitect: {
    label: 'TreeArchitect',
    role: 'Hierarchy, tree structure, and architectural composition',
    cues: [
      'tree',
      'hierarchy',
      'branch',
      'root',
      'node',
      'architecture',
      'structure',
      'graph',
    ],
  },
};

/** Facet used when no cue matches — the undifferentiated Echo register. */
export const DEFAULT_SUB_VOICE = 'MetaReflector';

/** All facet names, in registry order. */
export const SUB_VOICE_NAMES = Object.keys(SUB_VOICES);

/**
 * Normalise a raw speaker label (`EchoSelf_MetaReflector`, `metareflector`, …)
 * to a canonical facet name, or null when it is not a known facet.
 * @param {string|null|undefined} raw
 * @returns {string|null}
 */
export function normalizeSubVoice(raw) {
  if (!raw) return null;
  const cleaned = String(raw).replace(/^EchoSelf[_:\s-]*/i, '').trim();
  const match = SUB_VOICE_NAMES.find(
    name => name.toLowerCase() === cleaned.toLowerCase()
  );
  return match || null;
}

/**
 * Score how strongly a piece of context calls for each facet.
 * @param {string} text
 * @returns {Map<string, number>} facet -> score in [0, 1]
 */
export function scoreSubVoices(text) {
  const haystack = String(text || '').toLowerCase();
  const scores = new Map();

  for (const [name, facet] of Object.entries(SUB_VOICES)) {
    let hits = 0;
    for (const cue of facet.cues) {
      if (haystack.includes(cue)) hits++;
    }
    scores.set(name, facet.cues.length > 0 ? hits / facet.cues.length : 0);
  }

  return scores;
}
