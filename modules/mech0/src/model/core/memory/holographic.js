import { MEMORY_CONSTANTS } from './types.js';
import logger from '../../../utils/logger.js';

/**
 * Function words carry no identity signal, so they are dropped before terms are
 * compared. Deliberately small: this is a similarity aid, not a language model.
 */
const TERM_STOPWORDS = new Set([
  'a', 'about', 'all', 'an', 'and', 'any', 'are', 'as', 'at', 'be', 'been',
  'but', 'by', 'can', 'did', 'do', 'does', 'for', 'from', 'had', 'has', 'have',
  'how', 'in', 'into', 'is', 'it', 'its', 'me', 'my', 'not', 'of', 'on', 'or',
  'so', 'that', 'the', 'their', 'them', 'then', 'there', 'these', 'they', 'this',
  'to', 'was', 'were', 'what', 'when', 'where', 'which', 'who', 'why', 'will',
  'with', 'you', 'your'
]);

const MIN_TERM_LENGTH = 3;

function clamp01(value) {
  if (!Number.isFinite(value)) return 0;
  return Math.min(1, Math.max(0, value));
}

function isResonanceMarker(tag) {
  return (
    typeof tag === 'string' &&
    (tag.startsWith('trait:') || tag.startsWith('intent:') || tag.startsWith('emotion:'))
  );
}

/**
 * Normalise arbitrary text into comparable content terms. Tag-shaped input
 * (`trait:analytical`) also yields its bare term so that a natural-language
 * query can match a tagged memory.
 * @param {string} text
 * @returns {string[]}
 */
function extractTerms(text) {
  if (typeof text !== 'string' || text.length === 0) return [];

  return text
    .toLowerCase()
    .split(/[^a-z0-9]+/)
    .filter(term => term.length >= MIN_TERM_LENGTH && !TERM_STOPWORDS.has(term));
}

/**
 * Flatten a memory's content into text for term extraction.
 * @param {*} content
 * @returns {string}
 */
function contentText(content) {
  if (content === null || content === undefined) return '';
  if (typeof content === 'string') return content;
  if (typeof content !== 'object') return String(content);

  return Object.values(content)
    .filter(value => typeof value === 'string' || typeof value === 'number')
    .join(' ');
}

/**
 * HolographicIdentity - Implements the core holographic identity system
 * Creates a dense meshwork of experience that operates on gestalt-level intuition
 * Preserves continuity across iterative changes (Ship of Theseus principle)
 */
export class HolographicIdentity {
  constructor(config = {}) {
    this.config = config;
    
    // Dense meshwork of experience patterns
    this.experienceMeshwork = new Map();
    
    // Gestalt-level patterns and emergent properties
    this.gestaltPatterns = new Map();
    
    // Identity signature for continuity preservation
    this.identitySignature = {
      coreTraits: new Set(),
      characteristicPatterns: new Map(),
      intentionality: new Map(),
      resonanceFrequency: 0.0
    };
    
    // Holographic reconstruction capabilities
    this.hologramResolution = config.hologramResolution || 100;
    this.minPatternStrength = config.minPatternStrength || 0.3;
    this.minPatternRelevance = config.minPatternRelevance ?? 0.15;
    
    // Ship of Theseus continuity tracking
    this.continuityMetrics = {
      totalChanges: 0,
      identityPreservation: 1.0,
      lastSignatureUpdate: Date.now()
    };
  }

  /**
   * Ingest new experience into the holographic meshwork
   */
  ingestExperience(memory, context = {}) {
    try {
      // Extract experience patterns
      const experiencePattern = this.extractExperiencePattern(memory, context);
      
      // Update meshwork with new pattern
      this.updateMeshwork(experiencePattern);
      
      // Refine gestalt patterns
      this.refineGestaltPatterns(experiencePattern);
      
      // Update identity signature
      this.updateIdentitySignature(experiencePattern);
      
      logger.info('Experience ingested into holographic identity', {
        patternId: experiencePattern.id,
        meshworkSize: this.experienceMeshwork.size
      });
      
      return experiencePattern;
    } catch (error) {
      logger.error('Failed to ingest experience:', error);
      throw error;
    }
  }

  /**
   * Reconstruct holographic representation from distributed echoes
   */
  reconstructHologram(echoes, targetResolution = this.hologramResolution) {
    const hologram = {
      resolution: targetResolution,
      patterns: new Map(),
      gestalt: null,
      coherence: 0.0,
      timestamp: new Date().toISOString()
    };

    // Merge echo patterns into holographic representation
    echoes.forEach(echo => {
      const pattern = this.extractPatternFromEcho(echo);
      if (pattern.strength >= this.minPatternStrength) {
        this.mergePatternIntoHologram(hologram, pattern);
      }
    });

    // Synthesize gestalt from patterns
    hologram.gestalt = this.synthesizeGestalt(hologram.patterns);
    hologram.coherence = this.calculateCoherence(hologram);

    return hologram;
  }

  /**
   * Operate on gestalt-level intuition
   */
  gestaltIntuition(query, context = {}) {
    // Find relevant gestalt patterns
    const relevantPatterns = this.findRelevantGestaltPatterns(query, context);
    
    // Synthesize intuitive response
    const intuition = {
      confidence: 0.0,
      insights: [],
      emergentProperties: new Map(),
      resonance: 0.0
    };

    relevantPatterns.forEach(([pattern, relevance]) => {
      const insight = this.extractInsight(pattern, query, context);
      if (insight.strength > 0.5) {
        intuition.insights.push(insight);
        intuition.confidence = Math.max(intuition.confidence, insight.strength * relevance);
      }
    });

    // Calculate overall resonance with identity
    intuition.resonance = this.calculateResonance(intuition, this.identitySignature);

    return intuition;
  }

  /**
   * Preserve continuity across changes (Ship of Theseus principle)
   */
  preserveContinuity(newExperience, changeWeight = 0.1) {
    const oldSignature = { ...this.identitySignature };
    
    // Calculate impact of change on identity
    const changeImpact = this.calculateChangeImpact(newExperience, oldSignature);
    
    // Update identity signature gradually
    this.updateIdentitySignatureGradually(newExperience, changeWeight);
    
    // Update continuity metrics
    this.continuityMetrics.totalChanges++;
    this.continuityMetrics.identityPreservation = this.calculatePreservation(oldSignature, this.identitySignature);
    this.continuityMetrics.lastSignatureUpdate = Date.now();
    
    // Detect anomalies that might indicate identity fragmentation
    if (changeImpact > 0.7) {
      logger.warn('High identity change impact detected', {
        impact: changeImpact,
        preservation: this.continuityMetrics.identityPreservation
      });
    }
    
    return {
      continuityPreserved: changeImpact <= 0.7,
      preservationScore: this.continuityMetrics.identityPreservation,
      changeImpact
    };
  }

  /**
   * Detect tampering or anomalies against coherence
   */
  detectAnomalies(recentExperiences) {
    const anomalies = [];
    const expectedCoherence = this.calculateExpectedCoherence();
    
    recentExperiences.forEach(experience => {
      const pattern = this.extractExperiencePattern(experience);
      const coherenceScore = this.calculatePatternCoherence(pattern);
      
      if (coherenceScore < expectedCoherence * 0.7) {
        anomalies.push({
          type: 'coherence_anomaly',
          experience: experience.key,
          expectedCoherence,
          actualCoherence: coherenceScore,
          severity: (expectedCoherence - coherenceScore) / expectedCoherence
        });
      }
    });
    
    return anomalies;
  }

  // Private helper methods

  extractExperiencePattern(memory, context) {
    return {
      id: `pattern:${Date.now()}:${Math.random()}`,
      content: memory.content,
      associations: new Set(memory.associations || []),
      context: { ...context },
      strength: 1.0,
      timestamp: new Date().toISOString(),
      resonanceMarkers: this.extractResonanceMarkers(memory)
    };
  }

  updateMeshwork(pattern) {
    // Add to meshwork with weighted connections
    this.experienceMeshwork.set(pattern.id, pattern);
    
    // Create connections to related patterns
    this.experienceMeshwork.forEach((existingPattern, patternId) => {
      if (patternId !== pattern.id) {
        const similarity = this.calculatePatternSimilarity(pattern, existingPattern);
        if (similarity > 0.5) {
          pattern.connections = pattern.connections || new Set();
          pattern.connections.add(patternId);
          existingPattern.connections = existingPattern.connections || new Set();
          existingPattern.connections.add(pattern.id);
        }
      }
    });
  }

  refineGestaltPatterns(newPattern) {
    // Look for emergent gestalt patterns
    const emergentPatterns = this.detectEmergentPatterns(newPattern);
    
    emergentPatterns.forEach(gestaltPattern => {
      if (this.gestaltPatterns.has(gestaltPattern.type)) {
        const existing = this.gestaltPatterns.get(gestaltPattern.type);
        existing.strength = Math.min(1.0, existing.strength + 0.1);
        existing.lastReinforced = Date.now();
      } else {
        this.gestaltPatterns.set(gestaltPattern.type, gestaltPattern);
      }
    });
  }

  updateIdentitySignature(pattern) {
    // Extract core traits from pattern
    pattern.resonanceMarkers.forEach(marker => {
      this.identitySignature.coreTraits.add(marker);
    });
    
    // Update characteristic patterns
    const patternType = this.classifyPattern(pattern);
    if (this.identitySignature.characteristicPatterns.has(patternType)) {
      const existing = this.identitySignature.characteristicPatterns.get(patternType);
      existing.strength = Math.min(1.0, existing.strength + 0.05);
    } else {
      this.identitySignature.characteristicPatterns.set(patternType, {
        strength: 0.5,
        examples: [pattern.id]
      });
    }
    
    // Update resonance frequency
    this.identitySignature.resonanceFrequency = this.calculateResonanceFrequency();
  }

  extractResonanceMarkers(memory) {
    const markers = [];
    
    // Extract key concepts, emotions, intentions from memory
    if (memory.associations) {
      memory.associations.forEach(tag => {
        if (tag.startsWith('trait:') || tag.startsWith('intent:') || tag.startsWith('emotion:')) {
          markers.push(tag);
        }
      });
    }
    
    return markers;
  }

  calculatePatternSimilarity(pattern1, pattern2) {
    // Simple similarity based on shared associations and resonance markers
    const sharedAssociations = new Set([...pattern1.associations].filter(x => pattern2.associations.has(x)));
    const sharedMarkers = new Set([...(pattern1.resonanceMarkers || [])].filter(x => (pattern2.resonanceMarkers || []).includes(x)));
    
    const maxAssociations = Math.max(pattern1.associations.size, pattern2.associations.size);
    const maxMarkers = Math.max((pattern1.resonanceMarkers || []).length, (pattern2.resonanceMarkers || []).length);
    
    const associationSimilarity = maxAssociations > 0 ? sharedAssociations.size / maxAssociations : 0;
    const markerSimilarity = maxMarkers > 0 ? sharedMarkers.size / maxMarkers : 0;
    
    return (associationSimilarity + markerSimilarity) / 2;
  }

  detectEmergentPatterns(newPattern) {
    // Simple emergent pattern detection
    const emergent = [];
    
    // Look for recurring themes
    const connectedPatterns = Array.from(newPattern.connections || [])
      .map(id => this.experienceMeshwork.get(id))
      .filter(p => p);
    
    if (connectedPatterns.length >= 3) {
      emergent.push({
        type: 'thematic_cluster',
        strength: Math.min(1.0, connectedPatterns.length / 10),
        patterns: [newPattern.id, ...newPattern.connections],
        timestamp: new Date().toISOString()
      });
    }
    
    return emergent;
  }

  classifyPattern(pattern) {
    // Simple pattern classification
    if (pattern.resonanceMarkers.some(m => m.startsWith('intent:'))) {
      return 'intentional';
    }
    if (pattern.resonanceMarkers.some(m => m.startsWith('emotion:'))) {
      return 'emotional';
    }
    if (pattern.associations.size > 5) {
      return 'complex';
    }
    return 'simple';
  }

  calculateResonanceFrequency() {
    // Calculate overall resonance frequency based on current patterns
    let totalResonance = 0;
    let count = 0;
    
    this.identitySignature.characteristicPatterns.forEach(pattern => {
      totalResonance += pattern.strength;
      count++;
    });
    
    return count > 0 ? totalResonance / count : 0.5;
  }

  extractPatternFromEcho(echo) {
    return {
      id: echo.key,
      strength: echo.strength || 0.5,
      associations: echo.associations || new Set(),
      content: echo.content,
      timestamp: echo.timestamp,
      resonanceMarkers: this.extractResonanceMarkers(echo) || []
    };
  }

  mergePatternIntoHologram(hologram, pattern) {
    if (hologram.patterns.has(pattern.id)) {
      const existing = hologram.patterns.get(pattern.id);
      existing.strength = Math.max(existing.strength, pattern.strength);
    } else {
      hologram.patterns.set(pattern.id, pattern);
    }
  }

  synthesizeGestalt(patterns) {
    const gestalt = {
      dominantThemes: new Map(),
      emergentProperties: new Map(),
      coherenceLevel: 0.0
    };
    
    // Analyze dominant themes
    const themeFrequency = new Map();
    patterns.forEach(pattern => {
      pattern.associations.forEach(association => {
        themeFrequency.set(association, (themeFrequency.get(association) || 0) + pattern.strength);
      });
    });
    
    // Identify top themes
    const sortedThemes = Array.from(themeFrequency.entries())
      .sort((a, b) => b[1] - a[1])
      .slice(0, 5);
    
    sortedThemes.forEach(([theme, strength]) => {
      gestalt.dominantThemes.set(theme, strength / patterns.size);
    });
    
    gestalt.coherenceLevel = this.calculateCoherence({ patterns });
    
    return gestalt;
  }

  calculateCoherence(hologram) {
    if (hologram.patterns.size === 0) return 0.0;
    
    let totalCoherence = 0;
    let pairCount = 0;
    
    const patternArray = Array.from(hologram.patterns.values());
    for (let i = 0; i < patternArray.length; i++) {
      for (let j = i + 1; j < patternArray.length; j++) {
        totalCoherence += this.calculatePatternSimilarity(patternArray[i], patternArray[j]);
        pairCount++;
      }
    }
    
    return pairCount > 0 ? totalCoherence / pairCount : 0.0;
  }

  findRelevantGestaltPatterns(query, context) {
    const relevant = [];
    const queryPattern = this.buildQueryPattern(query, context);

    this.gestaltPatterns.forEach(pattern => {
      const relevance = this.calculatePatternRelevance(pattern, query, context, queryPattern);
      if (relevance > this.minPatternRelevance) {
        relevant.push([pattern, relevance]);
      }
    });

    return relevant.sort((a, b) => b[1] - a[1]);
  }

  /**
   * Project a natural-language query and its context into the same shape used
   * for experience patterns, so a single similarity metric covers both.
   */
  buildQueryPattern(query, context = {}) {
    const associations = new Set();
    const resonanceMarkers = [];

    for (const term of extractTerms(query)) associations.add(term);

    for (const [key, value] of Object.entries(context || {})) {
      if (value === null || value === undefined || typeof value === 'object') continue;
      const tag = `${key}:${value}`;
      associations.add(tag);
      if (isResonanceMarker(tag)) resonanceMarkers.push(tag);
      for (const term of extractTerms(String(value))) associations.add(term);
    }

    return { associations, resonanceMarkers };
  }

  /**
   * Resolve a gestalt pattern into the experience patterns that constitute it,
   * unioning their associations and markers. A gestalt is only ever an index
   * over lived experience, so its meaning has to be read back out of the
   * meshwork rather than stored twice.
   */
  projectGestaltPattern(pattern) {
    const associations = new Set();
    const markers = new Set();
    const members = [];

    for (const term of extractTerms(pattern.type)) associations.add(term);

    for (const memberId of pattern.patterns || []) {
      const member = this.experienceMeshwork.get(memberId);
      if (!member) continue;

      members.push(member);
      member.associations.forEach(association => {
        associations.add(association);
        for (const term of extractTerms(association)) associations.add(term);
      });
      (member.resonanceMarkers || []).forEach(marker => markers.add(marker));
      for (const term of extractTerms(contentText(member.content))) associations.add(term);
    }

    return { associations, resonanceMarkers: [...markers], members };
  }

  /**
   * How much this gestalt pattern speaks to the query.
   *
   * Structural similarity reuses `calculatePatternSimilarity` so relevance and
   * coherence are measured with one metric; lexical coverage answers the
   * complementary question of how much of what was *asked* the pattern
   * addresses. Strength scales the result because a weak gestalt is weak
   * evidence even when it matches.
   */
  calculatePatternRelevance(pattern, query, context, precomputedQueryPattern) {
    const queryPattern =
      precomputedQueryPattern || this.buildQueryPattern(query, context);
    const projection = this.projectGestaltPattern(pattern);

    if (queryPattern.associations.size === 0) return 0;

    const structural = this.calculatePatternSimilarity(queryPattern, projection);

    let covered = 0;
    queryPattern.associations.forEach(term => {
      if (projection.associations.has(term)) covered += 1;
    });
    const coverage = covered / queryPattern.associations.size;

    const strength = clamp01(pattern.strength ?? 0.5);
    const score = (0.4 * structural + 0.6 * coverage) * (0.5 + 0.5 * strength);

    return clamp01(score);
  }

  /**
   * Derive an insight from what the pattern actually connects, rather than
   * announcing that a pattern exists.
   */
  extractInsight(pattern, query, context) {
    const projection = this.projectGestaltPattern(pattern);
    const themes = this.rankThemes(projection.members);
    const strongest = projection.members
      .slice()
      .sort((a, b) => (b.strength || 0) - (a.strength || 0))
      .slice(0, 3);

    const evidence = projection.members.length;
    const meanStrength =
      evidence > 0
        ? projection.members.reduce((sum, m) => sum + (m.strength || 0), 0) / evidence
        : 0;

    // Confidence grows with corroboration but saturates: three independent
    // patterns is meaningfully more than one, thirty is not ten times more.
    const corroboration = evidence > 0 ? 1 - 1 / (1 + evidence) : 0;
    const confidence = clamp01(
      0.5 * corroboration + 0.3 * meanStrength + 0.2 * clamp01(pattern.strength ?? 0)
    );

    const content = themes.length
      ? `${pattern.type}: ${themes.map(([theme]) => theme).join(', ')}`
      : `${pattern.type}: no resolved supporting experience`;

    return {
      content,
      strength: pattern.strength,
      confidence,
      source: pattern.type,
      themes,
      markers: projection.resonanceMarkers,
      supporting: strongest.map(m => m.id),
      evidenceCount: evidence
    };
  }

  /**
   * Rank the themes carried by a set of experience patterns, weighted by the
   * strength of the patterns that carry them.
   */
  rankThemes(members, limit = 5) {
    const weights = new Map();

    for (const member of members) {
      const weight = member.strength || 0;
      member.associations.forEach(association => {
        weights.set(association, (weights.get(association) || 0) + weight);
      });
    }

    const total = [...weights.values()].reduce((sum, w) => sum + w, 0);
    if (total === 0) return [];

    return [...weights.entries()]
      .sort((a, b) => b[1] - a[1])
      .slice(0, limit)
      .map(([theme, weight]) => [theme, weight / total]);
  }

  /**
   * How strongly an intuition resonates with who this identity already is.
   *
   * Overlap between the intuition's markers and the signature's core traits is
   * the primary term; alignment with the strengths of characteristic patterns
   * is secondary; the baseline resonance frequency supplies the floor.
   */
  calculateResonance(intuition, signature) {
    const markers = new Set();
    for (const insight of intuition.insights || []) {
      for (const marker of insight.markers || []) markers.add(marker);
      for (const [theme] of insight.themes || []) {
        if (isResonanceMarker(theme)) markers.add(theme);
      }
    }

    let traitOverlap = 0;
    if (markers.size > 0) {
      let shared = 0;
      markers.forEach(marker => {
        if (signature.coreTraits.has(marker)) shared += 1;
      });
      traitOverlap = shared / markers.size;
    }

    let patternAlignment = 0;
    const sources = (intuition.insights || []).map(insight => insight.source);
    if (sources.length > 0) {
      const strengths = sources.map(
        source => signature.characteristicPatterns.get(source)?.strength ?? 0
      );
      patternAlignment = strengths.reduce((sum, s) => sum + s, 0) / strengths.length;
    }

    const baseline = clamp01(signature.resonanceFrequency ?? 0.5);

    return clamp01(0.5 * traitOverlap + 0.3 * patternAlignment + 0.2 * baseline);
  }

  /**
   * How much absorbing this experience would move the identity.
   *
   * Deterministic and monotone in both terms, because this feeds the
   * fragmentation warning and `detectAnomalies`: noise here makes both useless.
   *
   * Signature displacement is 1 − Jaccard between the traits held now and the
   * traits that would be held afterwards, which gives the Ship-of-Theseus
   * scaling for free — replacing one plank of two is a great deal more than
   * replacing one of two hundred. Experiential novelty asks the complementary
   * question of how alien the experience is to everything lived so far.
   */
  calculateChangeImpact(newExperience, oldSignature) {
    const pattern = this.extractExperiencePattern(newExperience);
    const markers = new Set(pattern.resonanceMarkers || []);
    const existingTraits = oldSignature?.coreTraits ?? new Set();

    // With no prior traits there is no identity to disrupt: the first
    // experience constitutes the identity rather than changing it. This mirrors
    // `calculatePreservation`, which likewise returns full preservation when
    // there is nothing yet to preserve.
    if (existingTraits.size === 0) return 0;

    let novelMarkers = 0;
    markers.forEach(marker => {
      if (!existingTraits.has(marker)) novelMarkers += 1;
    });

    const union = existingTraits.size + novelMarkers;
    const displacement = union > 0 ? 1 - existingTraits.size / union : 0;

    if (this.experienceMeshwork.size === 0) return clamp01(displacement);

    let maxSimilarity = 0;
    this.experienceMeshwork.forEach(existing => {
      maxSimilarity = Math.max(
        maxSimilarity,
        this.calculatePatternSimilarity(pattern, existing)
      );
    });
    const novelty = 1 - clamp01(maxSimilarity);

    return clamp01(0.65 * displacement + 0.35 * novelty);
  }

  updateIdentitySignatureGradually(newExperience, changeWeight) {
    // Gradually update the identity signature to preserve continuity
    const pattern = this.extractExperiencePattern(newExperience);
    
    // Weighted update of core traits
    pattern.resonanceMarkers.forEach(marker => {
      this.identitySignature.coreTraits.add(marker);
    });
    
    // Update resonance frequency gradually
    const newFrequency = this.calculateResonanceFrequency();
    this.identitySignature.resonanceFrequency = 
      this.identitySignature.resonanceFrequency * (1 - changeWeight) + 
      newFrequency * changeWeight;
  }

  calculatePreservation(oldSignature, newSignature) {
    // Calculate how much of the original identity is preserved
    const oldTraits = oldSignature.coreTraits.size;
    const preservedTraits = new Set([...oldSignature.coreTraits].filter(x => newSignature.coreTraits.has(x))).size;
    
    return oldTraits > 0 ? preservedTraits / oldTraits : 1.0;
  }

  calculateExpectedCoherence() {
    return this.gestaltPatterns.size > 0 ? 
      Array.from(this.gestaltPatterns.values()).reduce((sum, p) => sum + p.strength, 0) / this.gestaltPatterns.size :
      0.5;
  }

  calculatePatternCoherence(pattern) {
    // Calculate how coherent this pattern is with existing identity
    let coherence = 0;
    let count = 0;
    
    this.experienceMeshwork.forEach(existingPattern => {
      coherence += this.calculatePatternSimilarity(pattern, existingPattern);
      count++;
    });
    
    return count > 0 ? coherence / count : 0.5;
  }

  // Public API methods for integration

  getIdentitySnapshot() {
    return {
      signature: { ...this.identitySignature },
      meshworkSize: this.experienceMeshwork.size,
      gestaltPatterns: this.gestaltPatterns.size,
      continuityMetrics: { ...this.continuityMetrics },
      hologramResolution: this.hologramResolution
    };
  }

  exportHolographicData() {
    return {
      experienceMeshwork: Array.from(this.experienceMeshwork.entries()),
      gestaltPatterns: Array.from(this.gestaltPatterns.entries()),
      identitySignature: this.identitySignature,
      continuityMetrics: this.continuityMetrics
    };
  }

  importHolographicData(data) {
    this.experienceMeshwork = new Map(data.experienceMeshwork);
    this.gestaltPatterns = new Map(data.gestaltPatterns);
    this.identitySignature = data.identitySignature;
    this.continuityMetrics = data.continuityMetrics;
  }
}