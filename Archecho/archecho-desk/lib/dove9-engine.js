// Dove9 Triadic Cognitive Loop Engine
// 12-step loop × 3 streams = Clock30 synchronization
// Composition: /dove9 architecture from echo/dove9

'use strict';

/**
 * Cognitive Terms — the active vocabulary of the Dove9 model
 */
const COGNITIVE_TERMS = {
  T1_PERCEPTION:       { phase: 0,   mode: 'Reflective',  role: 'relevance detection and salience parsing' },
  T2_IDEA_FORMATION:   { phase: 30,  mode: 'Expressive',  role: 'hypothesis and concept assembly' },
  T4_SENSORY_INPUT:    { phase: 90,  mode: 'Expressive',  role: 'environmental intake and feature extraction' },
  T5_ACTION_SEQUENCE:  { phase: 120, mode: 'Expressive',  role: 'action planning and output sequencing' },
  T7_MEMORY_ENCODING:  { phase: 180, mode: 'Reflective',  role: 'episodic and semantic consolidation' },
  T8_BALANCED_RESPONSE:{ phase: 210, mode: 'Expressive',  role: 'integrated, tension-balanced response' }
};

/**
 * Tensional Couplings — cross-stream relationships
 */
const COUPLINGS = [
  { from: 'T4_SENSORY_INPUT', to: 'T7_MEMORY_ENCODING', type: 'E-R', description: 'perception and memory tension' },
  { from: 'T1_PERCEPTION', to: 'T2_IDEA_FORMATION', type: 'R-E', description: 'reflective assessment informs planning' },
  { from: 'T8_BALANCED_RESPONSE', to: 'T1_PERCEPTION', type: 'E-R', description: 'balanced integration resolves tension' }
];

/**
 * Sys6 Operadic Model channels
 */
const SYS6 = {
  D: 'dyadic',
  T: 'triadic',
  P: 'pentadic',
  C8: 'cubic concurrency',
  K9: 'triadic convolution bundle',
  Clock30: 30
};

class Dove9Engine {
  constructor() {
    this.clockStep = 0;
    this.streams = {
      PRIMARY:   { phase: 0,   active: true, mode: 'Expressive' },
      SECONDARY: { phase: 120, active: true, mode: 'Reflective' },
      TERTIARY:  { phase: 240, active: true, mode: 'Expressive' }
    };
    this.terms = Object.keys(COGNITIVE_TERMS);
    this.activeTerm = this.terms[0];
    this.convergencePoints = [0, 7, 15, 22]; // 4 convergence points in Clock30
    this.history = [];
  }

  /**
   * Advance the triadic clock by one step
   */
  step() {
    this.clockStep = (this.clockStep + 1) % SYS6.Clock30;
    const phase = (this.clockStep * 12) % 360;

    // Update streams
    this.streams.PRIMARY.phase = phase;
    this.streams.SECONDARY.phase = (phase + 120) % 360;
    this.streams.TERTIARY.phase = (phase + 240) % 360;

    // Determine active term
    const termIndex = this.clockStep % this.terms.length;
    this.activeTerm = this.terms[termIndex];

    // Toggle modes at convergence points
    if (this.convergencePoints.includes(this.clockStep)) {
      Object.values(this.streams).forEach(s => {
        s.mode = s.mode === 'Expressive' ? 'Reflective' : 'Expressive';
      });
    }

    // Record history
    this.history.push({
      step: this.clockStep,
      term: this.activeTerm,
      phase,
      timestamp: Date.now()
    });

    // Keep history bounded
    if (this.history.length > 300) this.history = this.history.slice(-300);

    return this.getState();
  }

  /**
   * Check if current step is a convergence point
   */
  isConvergence() {
    return this.convergencePoints.includes(this.clockStep);
  }

  getState() {
    return {
      clockStep: this.clockStep,
      activeTerm: this.activeTerm,
      termInfo: COGNITIVE_TERMS[this.activeTerm],
      streams: { ...this.streams },
      isConvergence: this.isConvergence(),
      couplings: COUPLINGS
    };
  }
}

module.exports = { Dove9Engine, COGNITIVE_TERMS, COUPLINGS, SYS6 };
