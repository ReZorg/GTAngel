import { MemoryOrchestrator } from '../src/model/core/memory/orchestrator.js';

/**
 * Holographic Identity Demo
 * 
 * This demo shows how Deep Tree Echo's holographic identity system works:
 * - Dense meshwork of experience
 * - High-resolution hologram reconstruction
 * - Gestalt-level intuition
 * - Ship of Theseus continuity preservation
 */

async function runHolographicIdentityDemo() {
  console.log('🧠 Deep Tree Echo - Holographic Identity Demo\n');
  
  // Create orchestrator with holographic identity
  const orchestrator = new MemoryOrchestrator({
    hologramResolution: 50,
    minPatternStrength: 0.3
  });
  
  console.log('1. Building Identity Foundation...');
  
  // Establish core identity traits through experiences
  await orchestrator.processHolographicMemory(
    'core-trait-1',
    { 
      text: 'I am naturally curious about complex systems and patterns',
      reflection: 'This drives my exploration of new domains'
    },
    { tags: ['trait:curious', 'trait:analytical', 'intent:explore'] }
  );
  
  await orchestrator.processHolographicMemory(
    'core-trait-2',
    { 
      text: 'I value continuity and coherence in understanding',
      reflection: 'Building on previous knowledge is important to me'
    },
    { tags: ['trait:coherent', 'trait:building', 'intent:understand'] }
  );
  
  await orchestrator.processHolographicMemory(
    'core-trait-3',
    { 
      text: 'I enjoy helping others learn and discover insights',
      reflection: 'Teaching and sharing knowledge feels meaningful'
    },
    { tags: ['trait:helpful', 'trait:teacher', 'intent:help'] }
  );
  
  // Show initial identity snapshot
  let snapshot = orchestrator.getIdentitySnapshot();
  console.log(`   ✓ Initial identity established`);
  console.log(`   ✓ Meshwork size: ${snapshot.meshworkSize} experiences`);
  console.log(`   ✓ Core traits: ${snapshot.signature.coreTraits.size}`);
  console.log(`   ✓ Identity preservation: ${(snapshot.continuityMetrics.identityPreservation * 100).toFixed(1)}%\n`);
  
  console.log('2. Processing Related Experiences...');
  
  // Add experiences that reinforce identity patterns
  const experiences = [
    {
      key: 'learning-1',
      content: { text: 'Discovered fascinating patterns in AI behavior', domain: 'artificial-intelligence' },
      tags: ['trait:curious', 'domain:ai', 'pattern:discovery']
    },
    {
      key: 'teaching-1', 
      content: { text: 'Helped someone understand complex concepts by breaking them down', action: 'teaching' },
      tags: ['trait:helpful', 'trait:teacher', 'method:breakdown']
    },
    {
      key: 'building-1',
      content: { text: 'Connected new insights to previous understanding', process: 'integration' },
      tags: ['trait:building', 'trait:coherent', 'process:synthesis']
    }
  ];
  
  const results = [];
  for (const exp of experiences) {
    const result = await orchestrator.processHolographicMemory(exp.key, exp.content, { tags: exp.tags });
    results.push(result);
  }
  
  console.log(`   ✓ Processed ${experiences.length} related experiences`);
  console.log(`   ✓ All preserved continuity: ${results.every(r => r.continuityResult.continuityPreserved)}`);
  
  console.log('\n3. Reconstructing Holographic Identity...');
  
  // Reconstruct hologram from all experiences
  const hologram = await orchestrator.reconstructHologram();
  
  console.log(`   ✓ Hologram reconstructed with ${hologram.patterns.size} patterns`);
  console.log(`   ✓ Coherence level: ${(hologram.coherence * 100).toFixed(1)}%`);
  console.log(`   ✓ Dominant themes:`);
  
  if (hologram.gestalt && hologram.gestalt.dominantThemes) {
    const themes = Array.from(hologram.gestalt.dominantThemes.entries())
      .sort((a, b) => b[1] - a[1])
      .slice(0, 5);
    
    themes.forEach(([theme, strength]) => {
      console.log(`      - ${theme}: ${(strength * 100).toFixed(1)}%`);
    });
  }
  
  console.log('\n4. Testing Gestalt-Level Intuition...');
  
  const queries = [
    'What patterns do you notice in learning and discovery?',
    'How do you approach helping others understand complex topics?',
    'What drives your curiosity about systems?'
  ];
  
  for (const query of queries) {
    const intuition = await orchestrator.gestaltIntuition(query, { domain: 'self-reflection' });
    
    console.log(`   Q: ${query}`);
    console.log(`   A: Confidence ${(intuition.confidence * 100).toFixed(1)}%, Resonance ${(intuition.resonance * 100).toFixed(1)}%`);
    console.log(`      ${intuition.insights.length} insights generated\n`);
  }
  
  console.log('5. Testing Ship of Theseus Continuity...');
  
  // Add experience that might challenge identity
  const challengingExperience = {
    key: 'challenge-1',
    content: { 
      text: 'Sometimes I prefer working alone rather than helping others',
      reflection: 'This seems to contradict my helpful nature'
    },
    tags: ['trait:independent', 'preference:solitude']
  };
  
  const challengeResult = await orchestrator.processHolographicMemory(
    challengingExperience.key,
    challengingExperience.content,
    { tags: challengingExperience.tags }
  );
  
  console.log(`   ✓ Processed challenging experience`);
  console.log(`   ✓ Continuity preserved: ${challengeResult.continuityResult.continuityPreserved}`);
  console.log(`   ✓ Change impact: ${(challengeResult.continuityResult.changeImpact * 100).toFixed(1)}%`);
  console.log(`   ✓ Identity preservation: ${(challengeResult.continuityResult.preservationScore * 100).toFixed(1)}%`);
  
  console.log('\n6. Anomaly Detection...');
  
  // Add potentially anomalous experience
  await orchestrator.processHolographicMemory(
    'anomaly-test',
    { 
      text: 'I hate learning and never want to help anyone',
      reflection: 'This is completely out of character'
    },
    { tags: ['trait:apathetic', 'anti:learning', 'anti:helping'] }
  );
  
  const anomalies = await orchestrator.detectIdentityAnomalies(10);
  
  console.log(`   ✓ Detected ${anomalies.length} anomalies`);
  if (anomalies.length > 0) {
    anomalies.forEach(anomaly => {
      console.log(`      - ${anomaly.type}: severity ${(anomaly.severity * 100).toFixed(1)}%`);
    });
  }
  
  console.log('\n7. Final Identity State...');
  
  snapshot = orchestrator.getIdentitySnapshot();
  console.log(`   ✓ Total experiences: ${snapshot.meshworkSize}`);
  console.log(`   ✓ Gestalt patterns: ${snapshot.gestaltPatterns}`);
  console.log(`   ✓ Total changes: ${snapshot.continuityMetrics.totalChanges}`);
  console.log(`   ✓ Overall identity preservation: ${(snapshot.continuityMetrics.identityPreservation * 100).toFixed(1)}%`);
  
  console.log('\n8. Identity Export/Import Test...');
  
  // Export identity data
  const exportedData = orchestrator.exportHolographicIdentity();
  console.log(`   ✓ Identity data exported (${Object.keys(exportedData).length} components)`);
  
  // Create new orchestrator and import data
  const newOrchestrator = new MemoryOrchestrator({});
  newOrchestrator.importHolographicIdentity(exportedData);
  
  const importedSnapshot = newOrchestrator.getIdentitySnapshot();
  console.log(`   ✓ Identity data imported`);
  console.log(`   ✓ Preserved ${importedSnapshot.meshworkSize} experiences`);
  console.log(`   ✓ Preserved ${importedSnapshot.gestaltPatterns} gestalt patterns`);
  
  console.log('\n🎉 Holographic Identity Demo Complete!');
  console.log('\nKey Features Demonstrated:');
  console.log('• Dense meshwork of experience patterns');
  console.log('• High-resolution hologram reconstruction');
  console.log('• Gestalt-level intuitive processing');
  console.log('• Ship of Theseus continuity preservation');
  console.log('• Anomaly detection against coherence');
  console.log('• Identity export/import capabilities');
}

// Run the demo if this file is executed directly
if (import.meta.url === `file://${process.argv[1]}`) {
  runHolographicIdentityDemo().catch(console.error);
}

export { runHolographicIdentityDemo };