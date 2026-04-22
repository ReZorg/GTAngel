// DTE-KSM Evo-Autogenesis Engine
// Autonomous metric-driven evolution toward Autonomy Level 5
// Composition: ( /dte-autonomy-evolution ⊗ /ksm-evolve ) ⊕ /autoresearch

'use strict';

/**
 * Alexander's 15 Properties of Living Structure
 */
const ALEXANDER_15_PROPERTIES = [
  { id: 1,  name: 'Levels of Scale',           description: 'Hierarchy of nested scales' },
  { id: 2,  name: 'Strong Centers',            description: 'Coherent focal points of organization' },
  { id: 3,  name: 'Boundaries',                description: 'Defined edges that create distinction' },
  { id: 4,  name: 'Alternating Repetition',    description: 'Rhythmic patterns of variation' },
  { id: 5,  name: 'Positive Space',            description: 'Every part has positive shape' },
  { id: 6,  name: 'Good Shape',                description: 'Geometrically coherent forms' },
  { id: 7,  name: 'Local Symmetries',          description: 'Symmetry within parts, not globally' },
  { id: 8,  name: 'Deep Interlock & Ambiguity',description: 'Interpenetrating boundaries' },
  { id: 9,  name: 'Contrast',                  description: 'Clear distinction between elements' },
  { id: 10, name: 'Gradients',                 description: 'Smooth transitions between states' },
  { id: 11, name: 'Roughness',                 description: 'Organic irregularity, not mechanical' },
  { id: 12, name: 'Echoes',                    description: 'Self-similar patterns across scales' },
  { id: 13, name: 'The Void',                  description: 'Empty space that gives meaning' },
  { id: 14, name: 'Simplicity & Inner Calm',   description: 'Reduction to essential structure' },
  { id: 15, name: 'Not-Separateness',          description: 'Integration with the whole' }
];

/**
 * DTE Autonomy Levels
 */
const AUTONOMY_LEVELS = [
  { level: 1, name: 'Reactive',     description: 'Stimulus-response behavior' },
  { level: 2, name: 'Adaptive',     description: 'Learning from experience' },
  { level: 3, name: 'Cognitive',    description: 'Internal world model and reasoning' },
  { level: 4, name: 'Embodied',     description: '4E cognition: embodied, embedded, enacted, extended' },
  { level: 5, name: 'Autogenesis',  description: 'Autonomous self-modification and evolution' }
];

/**
 * KSM 12-Step Evolution Cycle
 */
const KSM_12_STEPS = [
  { step: 1,  name: 'Observe',     property: 'Levels of Scale',   action: 'Analyze previous results' },
  { step: 2,  name: 'Diagnose',    property: 'Strong Centers',    action: 'Identify weakest center' },
  { step: 3,  name: 'Hypothesize', property: 'Boundaries',        action: 'Generate experiment hypothesis' },
  { step: 4,  name: 'Design',      property: 'Good Shape',        action: 'Design minimal intervention' },
  { step: 5,  name: 'Implement',   property: 'Contrast',          action: 'Edit and commit changes' },
  { step: 6,  name: 'Test',        property: 'Gradients',         action: 'Run metric command' },
  { step: 7,  name: 'Measure',     property: 'Roughness',         action: 'Extract numeric metric' },
  { step: 8,  name: 'Assess',      property: 'Not-Separateness',  action: 'Property coherence check' },
  { step: 9,  name: 'Decide',      property: 'Echoes',            action: 'Keep or discard' },
  { step: 10, name: 'Integrate',   property: 'Deep Interlock',    action: 'Merge successful changes' },
  { step: 11, name: 'Stabilize',   property: 'Inner Calm',        action: 'Verify system stability' },
  { step: 12, name: 'Evolve',      property: 'Positive Space',    action: 'Advance to next cycle' }
];

class AutogenesisEngine {
  constructor(config = {}) {
    this.targetLevel = config.targetLevel || 5;
    this.maxExperiments = config.maxExperiments || 20;
    this.coherenceThreshold = config.coherenceThreshold || 0.6;
    this.safetyHaltThreshold = 0.15;
    this.deltaClamp = 0.2;

    this.currentLevel = 1;
    this.experiments = [];
    this.bestMetric = 0;
    this.coherenceScore = 1.0;
    this.cycleStep = 0;
    this.running = false;
    this.halted = false;
    this.haltReason = null;

    this.propertyScores = ALEXANDER_15_PROPERTIES.map(() => 0.5 + Math.random() * 0.5);
  }

  /**
   * Calculate overall property coherence score (0.0 - 1.0)
   */
  calculateCoherence() {
    return this.propertyScores.reduce((a, b) => a + b, 0) / this.propertyScores.length;
  }

  /**
   * Run a single experiment step
   */
  runStep(hypothesis) {
    if (this.halted) return { error: 'System halted: ' + this.haltReason };

    this.cycleStep = (this.cycleStep % 12) + 1;
    const ksmStep = KSM_12_STEPS[this.cycleStep - 1];

    // Simulate metric change
    const metricDelta = (Math.random() - 0.4) * 0.1;
    const newMetric = this.bestMetric + metricDelta;

    // Simulate property impact
    const propertyIndex = (ksmStep.step - 1) % 15;
    const propertyDelta = (Math.random() - 0.3) * 0.1;
    this.propertyScores[propertyIndex] = Math.max(0, Math.min(1,
      this.propertyScores[propertyIndex] + propertyDelta));

    this.coherenceScore = this.calculateCoherence();

    // Decide: keep or discard
    let status = 'keep';
    let reason = '';

    if (newMetric < this.bestMetric) {
      status = 'discard';
      reason = 'Metric regression';
    } else if (this.coherenceScore < this.coherenceThreshold) {
      status = 'discard';
      reason = 'Coherence below threshold';
    } else if (Math.abs(metricDelta) > this.deltaClamp) {
      status = 'discard';
      reason = 'Delta clamp exceeded';
    }

    if (status === 'keep') {
      this.bestMetric = newMetric;
    }

    // Safety halt check
    if (this.coherenceScore < this.safetyHaltThreshold) {
      this.halted = true;
      this.haltReason = `Coherence dropped to ${this.coherenceScore.toFixed(3)} (below ${this.safetyHaltThreshold})`;
      status = 'halt';
    }

    const result = {
      id: this.experiments.length + 1,
      hypothesis,
      ksmStep: ksmStep.name,
      property: ksmStep.property,
      metric: status === 'keep' ? newMetric : this.bestMetric,
      metricDelta,
      coherenceScore: this.coherenceScore,
      status,
      reason,
      timestamp: new Date().toISOString()
    };

    this.experiments.push(result);

    // Check for level advancement
    const keepCount = this.experiments.filter(e => e.status === 'keep').length;
    const keepRatio = keepCount / this.experiments.length;
    if (keepRatio > 0.6 && keepCount >= 5 * this.currentLevel && this.currentLevel < this.targetLevel) {
      this.currentLevel++;
    }

    return result;
  }

  getState() {
    return {
      currentLevel: this.currentLevel,
      targetLevel: this.targetLevel,
      bestMetric: this.bestMetric,
      coherenceScore: this.coherenceScore,
      cycleStep: this.cycleStep,
      experimentCount: this.experiments.length,
      keepCount: this.experiments.filter(e => e.status === 'keep').length,
      discardCount: this.experiments.filter(e => e.status === 'discard').length,
      halted: this.halted,
      haltReason: this.haltReason,
      propertyScores: this.propertyScores,
      properties: ALEXANDER_15_PROPERTIES,
      autonomyLevels: AUTONOMY_LEVELS,
      ksmSteps: KSM_12_STEPS
    };
  }
}

module.exports = { AutogenesisEngine, ALEXANDER_15_PROPERTIES, AUTONOMY_LEVELS, KSM_12_STEPS };
