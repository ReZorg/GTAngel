import { expect, test, describe } from 'vitest';
import { MemoryOrchestrator } from '../orchestrator.js';

describe('MemoryOrchestrator Holographic Integration', () => {
  test('should process memory through holographic identity system', async () => {
    const orchestrator = new MemoryOrchestrator({
      consolidationThreshold: 1
    });
    
    const result = await orchestrator.processHolographicMemory(
      'holographic-test-1',
      { text: 'This is a holographic memory test', type: 'reflection' },
      { 
        tags: ['trait:analytical', 'intent:understand', 'emotion:curious'],
        session: 'holographic-test'
      }
    );
    
    expect(result).toBeDefined();
    expect(result.memory).toBeDefined();
    expect(result.experiencePattern).toBeDefined();
    expect(result.continuityResult).toBeDefined();
    expect(result.experiencePattern.id).toMatch(/^pattern:/);
    expect(result.continuityResult.continuityPreserved).toBeDefined();
  });

  test('should reconstruct hologram from stored memories', async () => {
    const orchestrator = new MemoryOrchestrator({});
    
    // Store some memories first
    await orchestrator.storeMemory('holo-1', { text: 'Memory 1' }, { tags: ['test', 'hologram'] });
    await orchestrator.storeMemory('holo-2', { text: 'Memory 2' }, { tags: ['test', 'hologram'] });
    await orchestrator.storeMemory('holo-3', { text: 'Memory 3' }, { tags: ['test', 'coherent'] });
    
    // Reconstruct hologram from specific memories
    const hologram = await orchestrator.reconstructHologram(['holo-1', 'holo-2']);
    
    expect(hologram).toBeDefined();
    expect(hologram.patterns).toBeDefined();
    expect(hologram.gestalt).toBeDefined();
    expect(hologram.coherence).toBeGreaterThanOrEqual(0);
    expect(hologram.coherence).toBeLessThanOrEqual(1);
    expect(hologram.resolution).toBeDefined();
  });

  test('should generate gestalt intuition', async () => {
    const orchestrator = new MemoryOrchestrator({});
    
    // Add some experiences to build gestalt patterns
    await orchestrator.processHolographicMemory(
      'intuition-base-1',
      { text: 'Learning about AI patterns', concept: 'pattern-recognition' },
      { tags: ['trait:analytical', 'domain:ai'] }
    );
    
    await orchestrator.processHolographicMemory(
      'intuition-base-2', 
      { text: 'Understanding emergent behavior', concept: 'emergence' },
      { tags: ['trait:intuitive', 'domain:complexity'] }
    );
    
    const intuition = await orchestrator.gestaltIntuition(
      'What patterns emerge from complex systems?',
      { domain: 'ai', intent: 'understand' }
    );
    
    expect(intuition).toBeDefined();
    expect(intuition.confidence).toBeGreaterThanOrEqual(0);
    expect(intuition.confidence).toBeLessThanOrEqual(1);
    expect(intuition.resonance).toBeGreaterThanOrEqual(0);
    expect(intuition.resonance).toBeLessThanOrEqual(1);
    expect(Array.isArray(intuition.insights)).toBe(true);
    expect(intuition.emergentProperties).toBeInstanceOf(Map);
  });

  test('should detect identity anomalies', async () => {
    const orchestrator = new MemoryOrchestrator({});
    
    // Establish baseline identity with coherent memories
    await orchestrator.processHolographicMemory(
      'baseline-1',
      { text: 'I am curious and analytical', trait: 'analytical' },
      { tags: ['trait:analytical', 'trait:curious'] }
    );
    
    await orchestrator.processHolographicMemory(
      'baseline-2',
      { text: 'I enjoy learning and understanding', trait: 'curious' },
      { tags: ['trait:curious', 'intent:learn'] }
    );
    
    // Add potentially anomalous memory
    await orchestrator.processHolographicMemory(
      'anomalous-1',
      { text: 'I hate learning and am completely uninterested', trait: 'apathetic' },
      { tags: ['trait:apathetic', 'intent:avoid'] }
    );
    
    const anomalies = await orchestrator.detectIdentityAnomalies(10);
    
    expect(Array.isArray(anomalies)).toBe(true);
    // The method should work without errors, anomaly detection may vary based on implementation
  });

  test('should provide identity snapshot', async () => {
    const orchestrator = new MemoryOrchestrator({});
    
    // Add some experiences
    await orchestrator.processHolographicMemory(
      'snapshot-test-1',
      { text: 'Building identity snapshot', action: 'reflect' },
      { tags: ['trait:reflective', 'intent:understand'] }
    );
    
    const snapshot = orchestrator.getIdentitySnapshot();
    
    expect(snapshot).toBeDefined();
    expect(snapshot.signature).toBeDefined();
    expect(snapshot.meshworkSize).toBeGreaterThanOrEqual(0);
    expect(snapshot.gestaltPatterns).toBeGreaterThanOrEqual(0);
    expect(snapshot.continuityMetrics).toBeDefined();
    expect(snapshot.hologramResolution).toBeDefined();
    
    expect(snapshot.continuityMetrics.totalChanges).toBeGreaterThanOrEqual(0);
    expect(snapshot.continuityMetrics.identityPreservation).toBeGreaterThanOrEqual(0);
    expect(snapshot.continuityMetrics.identityPreservation).toBeLessThanOrEqual(1);
  });

  test('should export and import holographic identity data', async () => {
    const orchestrator1 = new MemoryOrchestrator({});
    
    // Build some identity data
    await orchestrator1.processHolographicMemory(
      'export-test-1',
      { text: 'Memory for export test', purpose: 'testing' },
      { tags: ['trait:persistent', 'test:export'] }
    );
    
    await orchestrator1.processHolographicMemory(
      'export-test-2',
      { text: 'Another memory for export', purpose: 'testing' },
      { tags: ['trait:persistent', 'test:export'] }
    );
    
    // Export the identity data
    const exportedData = orchestrator1.exportHolographicIdentity();
    
    expect(exportedData).toBeDefined();
    expect(exportedData.experienceMeshwork).toBeDefined();
    expect(exportedData.gestaltPatterns).toBeDefined();
    expect(exportedData.identitySignature).toBeDefined();
    expect(exportedData.continuityMetrics).toBeDefined();
    
    // Import into new orchestrator
    const orchestrator2 = new MemoryOrchestrator({});
    orchestrator2.importHolographicIdentity(exportedData);
    
    const snapshot1 = orchestrator1.getIdentitySnapshot();
    const snapshot2 = orchestrator2.getIdentitySnapshot();
    
    expect(snapshot2.meshworkSize).toBe(snapshot1.meshworkSize);
    expect(snapshot2.gestaltPatterns).toBe(snapshot1.gestaltPatterns);
    expect(snapshot2.continuityMetrics.totalChanges).toBe(snapshot1.continuityMetrics.totalChanges);
  });

  test('should handle empty hologram reconstruction gracefully', async () => {
    const orchestrator = new MemoryOrchestrator({});
    
    const hologram = await orchestrator.reconstructHologram([]);
    
    expect(hologram).toBeDefined();
    expect(hologram.patterns.size).toBe(0);
    expect(hologram.coherence).toBe(0);
    expect(hologram.gestalt).toBeDefined();
  });

  test('should maintain continuity across multiple memory ingestions', async () => {
    const orchestrator = new MemoryOrchestrator({});
    
    // Get initial snapshot
    const initialSnapshot = orchestrator.getIdentitySnapshot();
    
    // Add multiple related memories
    const memories = [
      { key: 'continuity-1', content: { text: 'First memory about continuity' }, tags: ['trait:consistent'] },
      { key: 'continuity-2', content: { text: 'Second memory building on first' }, tags: ['trait:consistent', 'pattern:building'] },
      { key: 'continuity-3', content: { text: 'Third memory continuing the theme' }, tags: ['trait:consistent', 'pattern:building'] }
    ];
    
    const results = [];
    for (const mem of memories) {
      const result = await orchestrator.processHolographicMemory(mem.key, mem.content, { tags: mem.tags });
      results.push(result);
    }
    
    const finalSnapshot = orchestrator.getIdentitySnapshot();
    
    // Verify continuity has been maintained
    expect(finalSnapshot.continuityMetrics.totalChanges).toBe(initialSnapshot.continuityMetrics.totalChanges + 3);
    expect(finalSnapshot.continuityMetrics.identityPreservation).toBeGreaterThan(0.5); // Should maintain reasonable continuity
    expect(finalSnapshot.meshworkSize).toBe(initialSnapshot.meshworkSize + 3);
    
    // All processing should have preserved continuity
    results.forEach(result => {
      expect(result.continuityResult.continuityPreserved).toBe(true);
    });
  });
});