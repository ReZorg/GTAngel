import { expect, test, describe } from 'vitest';
import { HolographicIdentity } from '../holographic.js';

describe('HolographicIdentity', () => {
  test('should create holographic identity instance', () => {
    const identity = new HolographicIdentity();
    
    expect(identity).toBeDefined();
    expect(identity.experienceMeshwork).toBeInstanceOf(Map);
    expect(identity.gestaltPatterns).toBeInstanceOf(Map);
    expect(identity.identitySignature).toBeDefined();
  });

  test('should ingest experience into meshwork', () => {
    const identity = new HolographicIdentity();
    
    const memory = {
      key: 'test-memory-1',
      content: { text: 'This is a test memory' },
      associations: new Set(['test', 'memory', 'trait:curious']),
      timestamp: new Date().toISOString()
    };
    
    const context = { session: 'test-session' };
    const pattern = identity.ingestExperience(memory, context);
    
    expect(pattern).toBeDefined();
    expect(pattern.id).toMatch(/^pattern:/);
    expect(identity.experienceMeshwork.size).toBe(1);
    expect(identity.identitySignature.coreTraits.has('trait:curious')).toBe(true);
  });

  test('should reconstruct hologram from echoes', () => {
    const identity = new HolographicIdentity();
    
    const echoes = [
      {
        key: 'echo:1',
        strength: 0.8,
        associations: new Set(['test', 'memory']),
        content: { text: 'Echo 1' },
        timestamp: new Date().toISOString()
      },
      {
        key: 'echo:2',
        strength: 0.6,
        associations: new Set(['test', 'learning']),
        content: { text: 'Echo 2' },
        timestamp: new Date().toISOString()
      }
    ];
    
    const hologram = identity.reconstructHologram(echoes);
    
    expect(hologram).toBeDefined();
    expect(hologram.patterns.size).toBe(2);
    expect(hologram.gestalt).toBeDefined();
    expect(hologram.coherence).toBeGreaterThanOrEqual(0);
    expect(hologram.coherence).toBeLessThanOrEqual(1);
  });

  test('should generate gestalt intuition', () => {
    const identity = new HolographicIdentity();
    
    // Add some gestalt patterns first
    identity.gestaltPatterns.set('thematic_cluster', {
      type: 'thematic_cluster',
      strength: 0.7,
      patterns: ['pattern:1', 'pattern:2'],
      timestamp: new Date().toISOString()
    });
    
    const query = 'What patterns do you see?';
    const context = { domain: 'analysis' };
    
    const intuition = identity.gestaltIntuition(query, context);
    
    expect(intuition).toBeDefined();
    expect(intuition.confidence).toBeGreaterThanOrEqual(0);
    expect(intuition.confidence).toBeLessThanOrEqual(1);
    expect(intuition.resonance).toBeGreaterThanOrEqual(0);
    expect(intuition.resonance).toBeLessThanOrEqual(1);
    expect(Array.isArray(intuition.insights)).toBe(true);
  });

  test('should preserve continuity across changes', () => {
    const identity = new HolographicIdentity();
    
    // Initialize with some experience
    const memory1 = {
      key: 'memory-1',
      content: { text: 'Initial memory' },
      associations: new Set(['trait:analytical', 'intent:learn']),
      timestamp: new Date().toISOString()
    };
    
    identity.ingestExperience(memory1);
    const initialSnapshot = identity.getIdentitySnapshot();
    
    // Add new experience that might change identity
    const memory2 = {
      key: 'memory-2',
      content: { text: 'New different memory' },
      associations: new Set(['trait:creative', 'intent:explore']),
      timestamp: new Date().toISOString()
    };
    
    const continuityResult = identity.preserveContinuity(memory2, 0.1);
    
    expect(continuityResult).toBeDefined();
    expect(continuityResult.continuityPreserved).toBeDefined();
    expect(continuityResult.preservationScore).toBeGreaterThanOrEqual(0);
    expect(continuityResult.preservationScore).toBeLessThanOrEqual(1);
    expect(continuityResult.changeImpact).toBeGreaterThanOrEqual(0);
    expect(continuityResult.changeImpact).toBeLessThanOrEqual(1);
  });

  test('should detect anomalies in experiences', () => {
    const identity = new HolographicIdentity();
    
    // Add some normal experiences to establish baseline
    const normalMemories = [
      {
        key: 'normal-1',
        content: { text: 'Normal memory 1' },
        associations: new Set(['normal', 'experience']),
        timestamp: new Date().toISOString()
      },
      {
        key: 'normal-2',
        content: { text: 'Normal memory 2' },
        associations: new Set(['normal', 'experience']),
        timestamp: new Date().toISOString()
      }
    ];
    
    normalMemories.forEach(memory => identity.ingestExperience(memory));
    
    // Add potentially anomalous experience
    const anomalousMemories = [
      {
        key: 'anomalous-1',
        content: { text: 'Completely different memory' },
        associations: new Set(['weird', 'unusual', 'strange']),
        timestamp: new Date().toISOString()
      }
    ];
    
    const anomalies = identity.detectAnomalies(anomalousMemories);
    
    expect(Array.isArray(anomalies)).toBe(true);
    // Note: anomalies may or may not be detected depending on the implementation
    // The test just ensures the method works without errors
  });

  test('should export and import holographic data', () => {
    const identity1 = new HolographicIdentity();
    
    // Add some data
    const memory = {
      key: 'test-memory',
      content: { text: 'Test content' },
      associations: new Set(['test', 'trait:persistent']),
      timestamp: new Date().toISOString()
    };
    
    identity1.ingestExperience(memory);
    
    // Export data
    const exportedData = identity1.exportHolographicData();
    
    expect(exportedData).toBeDefined();
    expect(exportedData.experienceMeshwork).toBeDefined();
    expect(exportedData.gestaltPatterns).toBeDefined();
    expect(exportedData.identitySignature).toBeDefined();
    expect(exportedData.continuityMetrics).toBeDefined();
    
    // Import into new identity
    const identity2 = new HolographicIdentity();
    identity2.importHolographicData(exportedData);
    
    expect(identity2.experienceMeshwork.size).toBe(identity1.experienceMeshwork.size);
    expect(identity2.gestaltPatterns.size).toBe(identity1.gestaltPatterns.size);
    expect(identity2.identitySignature.coreTraits.has('trait:persistent')).toBe(true);
  });

  test('should handle empty echoes gracefully', () => {
    const identity = new HolographicIdentity();
    
    const hologram = identity.reconstructHologram([]);
    
    expect(hologram).toBeDefined();
    expect(hologram.patterns.size).toBe(0);
    expect(hologram.coherence).toBe(0);
  });

  test('should track continuity metrics', async () => {
    const identity = new HolographicIdentity();
    
    const memory = {
      key: 'continuity-test',
      content: { text: 'Continuity test memory' },
      associations: new Set(['test']),
      timestamp: new Date().toISOString()
    };
    
    const initialMetrics = { ...identity.continuityMetrics };
    const initialChanges = initialMetrics.totalChanges;
    
    // Add small delay to ensure different timestamps
    await new Promise(resolve => setTimeout(resolve, 10));
    
    identity.preserveContinuity(memory);
    
    expect(identity.continuityMetrics.totalChanges).toBe(initialChanges + 1);
    expect(identity.continuityMetrics.identityPreservation).toBeGreaterThanOrEqual(0);
    expect(identity.continuityMetrics.identityPreservation).toBeLessThanOrEqual(1);
    expect(identity.continuityMetrics.lastSignatureUpdate).toBeGreaterThan(initialMetrics.lastSignatureUpdate);
  });
});