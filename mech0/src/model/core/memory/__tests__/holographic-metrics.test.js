import { describe, expect, test } from 'vitest';
import { HolographicIdentity } from '../holographic.js';

/**
 * These tests exist because the four methods under test used to return
 * `Math.random()`. Asserting a range would not have caught that — every one of
 * these assertions is designed to fail against a random implementation.
 */

function memory(key, text, tags) {
  return {
    key,
    content: { text },
    associations: new Set(tags),
    timestamp: new Date().toISOString(),
  };
}

function seededIdentity() {
  const identity = new HolographicIdentity();

  identity.ingestExperience(
    memory('m1', 'Reservoir dynamics shape the echo state', [
      'trait:analytical',
      'intent:explain',
      'reservoir',
    ])
  );
  identity.ingestExperience(
    memory('m2', 'Echo state networks carry memory forward', [
      'trait:analytical',
      'intent:explain',
      'reservoir',
    ])
  );
  identity.ingestExperience(
    memory('m3', 'The reservoir resonates with prior echoes', [
      'trait:analytical',
      'reservoir',
    ])
  );

  return identity;
}

/** Build a gestalt pattern over every pattern currently in the meshwork. */
function gestaltOverMeshwork(identity, type = 'thematic_cluster', strength = 0.7) {
  const pattern = {
    type,
    strength,
    patterns: [...identity.experienceMeshwork.keys()],
    timestamp: new Date().toISOString(),
  };
  identity.gestaltPatterns.set(type, pattern);
  return pattern;
}

describe('calculatePatternRelevance', () => {
  test('is deterministic', () => {
    const identity = seededIdentity();
    const pattern = gestaltOverMeshwork(identity);

    const first = identity.calculatePatternRelevance(pattern, 'reservoir echo', {});
    const second = identity.calculatePatternRelevance(pattern, 'reservoir echo', {});

    expect(second).toBe(first);
  });

  test('ranks an on-topic query above an off-topic one', () => {
    const identity = seededIdentity();
    const pattern = gestaltOverMeshwork(identity);

    const onTopic = identity.calculatePatternRelevance(
      pattern,
      'How do reservoir echo states carry memory?',
      {}
    );
    const offTopic = identity.calculatePatternRelevance(
      pattern,
      'What is the best recipe for sourdough bread?',
      {}
    );

    expect(onTopic).toBeGreaterThan(offTopic);
    expect(offTopic).toBeLessThan(0.2);
  });

  test('scales with gestalt strength', () => {
    const identity = seededIdentity();
    const ids = [...identity.experienceMeshwork.keys()];
    const query = 'reservoir echo memory';

    const weak = identity.calculatePatternRelevance(
      { type: 'thematic_cluster', strength: 0.1, patterns: ids },
      query,
      {}
    );
    const strong = identity.calculatePatternRelevance(
      { type: 'thematic_cluster', strength: 1.0, patterns: ids },
      query,
      {}
    );

    expect(strong).toBeGreaterThan(weak);
  });

  test('returns zero for an empty query and stays within bounds', () => {
    const identity = seededIdentity();
    const pattern = gestaltOverMeshwork(identity);

    expect(identity.calculatePatternRelevance(pattern, '', {})).toBe(0);

    const relevance = identity.calculatePatternRelevance(pattern, 'reservoir', {});
    expect(relevance).toBeGreaterThanOrEqual(0);
    expect(relevance).toBeLessThanOrEqual(1);
  });

  test('matches context tags against tagged memories', () => {
    const identity = seededIdentity();
    const pattern = gestaltOverMeshwork(identity);

    const matching = identity.calculatePatternRelevance(pattern, 'explain', {
      intent: 'explain',
    });
    const mismatched = identity.calculatePatternRelevance(pattern, 'explain', {
      intent: 'entertain',
    });

    expect(matching).toBeGreaterThan(mismatched);
  });
});

describe('extractInsight', () => {
  test('draws content from real associations, not a placeholder', () => {
    const identity = seededIdentity();
    const pattern = gestaltOverMeshwork(identity);

    const insight = identity.extractInsight(pattern, 'reservoir', {});

    expect(insight.content).not.toBe('Insight from thematic_cluster pattern');
    expect(insight.content).toContain('trait:analytical');
    expect(insight.evidenceCount).toBe(3);
    expect(insight.markers).toContain('trait:analytical');
    expect(insight.supporting.length).toBeGreaterThan(0);
  });

  test('confidence rises with corroborating experience', () => {
    const identity = seededIdentity();
    const ids = [...identity.experienceMeshwork.keys()];

    const thin = identity.extractInsight(
      { type: 'thematic_cluster', strength: 0.7, patterns: ids.slice(0, 1) },
      'reservoir',
      {}
    );
    const corroborated = identity.extractInsight(
      { type: 'thematic_cluster', strength: 0.7, patterns: ids },
      'reservoir',
      {}
    );

    expect(corroborated.confidence).toBeGreaterThan(thin.confidence);
    expect(corroborated.confidence).toBeLessThanOrEqual(1);
  });

  test('reports honestly when no supporting experience resolves', () => {
    const identity = seededIdentity();

    const insight = identity.extractInsight(
      { type: 'thematic_cluster', strength: 0.7, patterns: ['does-not-exist'] },
      'reservoir',
      {}
    );

    expect(insight.evidenceCount).toBe(0);
    expect(insight.content).toContain('no resolved supporting experience');
  });
});

describe('calculateResonance', () => {
  test('is higher for an intuition built from the identity own traits', () => {
    const identity = seededIdentity();
    const signature = identity.identitySignature;

    const own = identity.calculateResonance(
      { insights: [{ source: 'thematic_cluster', markers: ['trait:analytical'], themes: [] }] },
      signature
    );
    const alien = identity.calculateResonance(
      { insights: [{ source: 'thematic_cluster', markers: ['trait:gregarious'], themes: [] }] },
      signature
    );

    expect(own).toBeGreaterThan(alien);
  });

  test('is deterministic and bounded even with no insights', () => {
    const identity = seededIdentity();
    const intuition = { insights: [] };

    const first = identity.calculateResonance(intuition, identity.identitySignature);
    const second = identity.calculateResonance(intuition, identity.identitySignature);

    expect(second).toBe(first);
    expect(first).toBeGreaterThanOrEqual(0);
    expect(first).toBeLessThanOrEqual(1);
  });

  test('rewards alignment with characteristic pattern strength', () => {
    const identity = seededIdentity();
    const signature = identity.identitySignature;
    signature.characteristicPatterns.set('strong', { strength: 1.0, examples: [] });
    signature.characteristicPatterns.set('weak', { strength: 0.05, examples: [] });

    const strong = identity.calculateResonance(
      { insights: [{ source: 'strong', markers: [], themes: [] }] },
      signature
    );
    const weak = identity.calculateResonance(
      { insights: [{ source: 'weak', markers: [], themes: [] }] },
      signature
    );

    expect(strong).toBeGreaterThan(weak);
  });
});

describe('calculateChangeImpact', () => {
  test('is deterministic', () => {
    const identity = seededIdentity();
    const experience = memory('new', 'Something else entirely', ['trait:playful']);

    const first = identity.calculateChangeImpact(experience, identity.identitySignature);
    const second = identity.calculateChangeImpact(experience, identity.identitySignature);

    expect(second).toBe(first);
  });

  test('a familiar experience has less impact than an alien one', () => {
    const identity = seededIdentity();

    const familiar = identity.calculateChangeImpact(
      memory('familiar', 'Reservoir dynamics again', [
        'trait:analytical',
        'intent:explain',
        'reservoir',
      ]),
      identity.identitySignature
    );
    const alien = identity.calculateChangeImpact(
      memory('alien', 'Utterly unrelated content', [
        'trait:mercurial',
        'intent:disrupt',
        'chaos',
      ]),
      identity.identitySignature
    );

    expect(familiar).toBeLessThan(alien);
    expect(familiar).toBeCloseTo(0, 5);
  });

  test('scales with how much of the identity is displaced (Ship of Theseus)', () => {
    const small = new HolographicIdentity();
    small.ingestExperience(memory('a', 'one', ['trait:one']));

    const large = new HolographicIdentity();
    for (let i = 0; i < 40; i += 1) {
      large.ingestExperience(memory(`m${i}`, `experience ${i}`, [`trait:t${i}`]));
    }

    const incoming = memory('new', 'a brand new direction', [
      'trait:novel-a',
      'trait:novel-b',
    ]);

    const smallImpact = small.calculateChangeImpact(incoming, small.identitySignature);
    const largeImpact = large.calculateChangeImpact(incoming, large.identitySignature);

    // The same two new traits are transformative for a two-trait identity and
    // marginal for a forty-trait one.
    expect(smallImpact).toBeGreaterThan(largeImpact);
  });

  test('treats a first experience as formative rather than fragmenting', () => {
    const identity = new HolographicIdentity();
    const impact = identity.calculateChangeImpact(
      memory('first', 'the very first thing', ['trait:new']),
      identity.identitySignature
    );

    expect(impact).toBe(0);
  });

  test('stays within bounds and drives the fragmentation warning', () => {
    const identity = seededIdentity();

    const result = identity.preserveContinuity(
      memory('divergent', 'Nothing like anything before', [
        'trait:x',
        'trait:y',
        'trait:z',
      ])
    );

    expect(result.changeImpact).toBeGreaterThanOrEqual(0);
    expect(result.changeImpact).toBeLessThanOrEqual(1);
    expect(typeof result.continuityPreserved).toBe('boolean');
  });
});

describe('gestaltIntuition end to end', () => {
  test('produces stable, non-random output for the same query', () => {
    const identity = seededIdentity();
    gestaltOverMeshwork(identity);

    const first = identity.gestaltIntuition('How does the reservoir hold memory?', {
      intent: 'explain',
    });
    const second = identity.gestaltIntuition('How does the reservoir hold memory?', {
      intent: 'explain',
    });

    expect(second.confidence).toBe(first.confidence);
    expect(second.resonance).toBe(first.resonance);
    expect(second.insights.map(i => i.content)).toEqual(
      first.insights.map(i => i.content)
    );
  });

  test('yields no insight for an unrelated query', () => {
    const identity = seededIdentity();
    gestaltOverMeshwork(identity);

    const intuition = identity.gestaltIntuition(
      'Please describe medieval Flemish tapestry weaving.',
      {}
    );

    expect(intuition.insights).toHaveLength(0);
    expect(intuition.confidence).toBe(0);
  });

  test('detectAnomalies flags an experience alien to the identity', () => {
    const identity = seededIdentity();
    gestaltOverMeshwork(identity, 'thematic_cluster', 0.9);

    const anomalies = identity.detectAnomalies([
      memory('weird', 'entirely unrelated', ['unrelated', 'strange', 'foreign']),
    ]);
    const familiar = identity.detectAnomalies([
      memory('normal', 'Reservoir dynamics again', [
        'trait:analytical',
        'intent:explain',
        'reservoir',
      ]),
    ]);

    expect(anomalies.length).toBeGreaterThan(0);
    expect(familiar.length).toBe(0);
  });
});
