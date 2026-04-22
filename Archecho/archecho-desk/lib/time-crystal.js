// Time Crystal Neural Network — JavaScript Implementation
// Based on Nanobrain Fig 6.14 (nn4c) and Fig 7.15 (nn9c)
// Composition: /time-crystal-nn [ /echo ]

'use strict';

/**
 * nn4c: Single Neuron Time Crystal
 * 9 temporal processing levels (8ms → 1s)
 * Configuration: [a,b,c,d] = [3,4,3,3] (spatial domains, functional layers, temporal scales, component types)
 */
class TimeCrystalNeuron {
  constructor(config = [3, 4, 3, 3]) {
    this.config = config;
    this.time = 0;
    this.levels = [
      { id: 0, period: 0.008, label: 'Protein',      components: ['Ax', 'Pr-Ch'] },
      { id: 1, period: 0.026, label: 'Ion-Channel',   components: ['Io-Ch', 'Li'] },
      { id: 2, period: 0.052, label: 'Membrane',      components: ['Me', 'Ac'] },
      { id: 3, period: 0.110, label: 'AIS',           components: ['AIS', 'An-n'] },
      { id: 4, period: 0.160, label: 'Dendritic',     components: ['Ch-Co', 'PNN'] },
      { id: 5, period: 0.250, label: 'Synaptic',      components: ['Ca', 'Fi-lo'] },
      { id: 6, period: 0.330, label: 'Soma',          components: ['Rh', 'Soma'] },
      { id: 7, period: 0.500, label: 'Network',       components: ['Gl-S', 'El'] },
      { id: 8, period: 1.000, label: 'Global',        components: ['Me-Rh', 'Sy-c'] }
    ];

    // Initialize oscillatory state
    this.state = this.levels.map(l => ({
      phase: Math.random() * Math.PI * 2,
      amplitude: 0.5 + Math.random() * 0.5,
      frequency: 1 / l.period,
      output: 0
    }));

    // Feedback connections (Fi-lo mechanism)
    this.feedbackAlpha = 0.3;
    this.spectralRadius = 0.95;
  }

  /**
   * Forward pass through all temporal levels
   */
  forward(input) {
    let signal = Array.isArray(input) ? input : [input];
    const outputs = [];

    for (let i = 0; i < this.levels.length; i++) {
      const s = this.state[i];
      const level = this.levels[i];

      // Oscillatory activation with phase modulation
      const oscillation = Math.sin(s.phase + this.time * s.frequency * Math.PI * 2);
      const modulated = oscillation * s.amplitude;

      // Junction processing (Ax-d, El, GlS types)
      const inputSum = signal.reduce((a, b) => a + b, 0) / signal.length;
      s.output = Math.tanh(inputSum * modulated * this.spectralRadius);

      // Feedback loop (Fi-lo mechanism at level 5)
      if (i === 5 && i > 0) {
        const feedback = this.state[i - 1].output * this.feedbackAlpha;
        s.output = Math.tanh(s.output + feedback);
      }

      // Rhythm modulation
      s.output *= 1 + 0.1 * Math.sin(this.time * s.frequency * 0.5);

      outputs.push(s.output);
      signal = [s.output]; // cascade to next level
    }

    return outputs;
  }

  /**
   * Advance time for oscillatory dynamics
   */
  step(dt) {
    this.time += dt;
    for (const s of this.state) {
      s.phase += dt * s.frequency * Math.PI * 2;
      // Phase coupling between adjacent levels
      const idx = this.state.indexOf(s);
      if (idx > 0) {
        const prev = this.state[idx - 1];
        const coupling = 0.05 * Math.sin(prev.phase - s.phase);
        s.phase += coupling;
      }
    }
  }

  getState() {
    return {
      time: this.time,
      levels: this.levels.map((l, i) => ({
        ...l,
        phase: this.state[i].phase,
        amplitude: this.state[i].amplitude,
        output: this.state[i].output
      }))
    };
  }
}

/**
 * nn9c: Whole Brain Time Crystal
 * 12 hierarchy levels (Microtubule → BloodVessel)
 */
class TimeCrystalBrain {
  constructor(config = { inputSize: 256, hiddenSize: 512, outputSize: 256, regionSize: 128 }) {
    this.config = config;
    this.time = 0;
    this.regions = [
      { id: 0,  name: 'Microtubule',    scale: 'Molecular',   module: 'MicrotubuleModule' },
      { id: 1,  name: 'Neuron',          scale: 'Cellular',    module: 'TimeCrystalNeuron' },
      { id: 2,  name: 'CorticalBranch',  scale: 'Columnar',   module: 'CorticalLayerModule' },
      { id: 3,  name: 'CortexDomain',    scale: 'Regional',   module: 'LobeModule' },
      { id: 4,  name: 'Cerebellum',      scale: 'Organ',      module: 'CerebellarLobeModule' },
      { id: 5,  name: 'Hypothalamus',    scale: 'Nuclear',    module: 'HypothalamicModule' },
      { id: 6,  name: 'Hippocampus',     scale: 'Nuclear',    module: 'HippocampalRegionModule' },
      { id: 7,  name: 'ThalamicBody',    scale: 'Relay',      module: 'ThalamicModule' },
      { id: 8,  name: 'SkinNerveNet',    scale: 'Peripheral', module: 'PeripheralNerveModule' },
      { id: 9,  name: 'CranialNerve',    scale: 'Peripheral', module: 'CranialNerveModule' },
      { id: 10, name: 'ThoracicNerve',   scale: 'Spinal',    module: 'SpinalNerveModule' },
      { id: 11, name: 'BloodVessel',     scale: 'Vascular',  module: 'VascularModule' }
    ];

    // Each region has its own neuron-level time crystal
    this.neurons = this.regions.map(() => new TimeCrystalNeuron());

    // Region-level state
    this.regionState = this.regions.map(() => ({
      activity: Math.random(),
      coherence: 0.5 + Math.random() * 0.5,
      memoryTrace: 0
    }));
  }

  forward(input) {
    const outputs = [];
    let signal = input;

    for (let i = 0; i < this.regions.length; i++) {
      const neuronOutput = this.neurons[i].forward(signal);
      const regionActivity = neuronOutput.reduce((a, b) => a + Math.abs(b), 0) / neuronOutput.length;

      this.regionState[i].activity = regionActivity;

      // Hippocampal memory trace (region 6)
      if (i === 6) {
        this.regionState[i].memoryTrace = 0.9 * this.regionState[i].memoryTrace + 0.1 * regionActivity;
      }

      // Thalamic gating (region 7)
      if (i === 7 && i > 0) {
        const gate = Math.tanh(this.regionState[i - 1].activity);
        this.regionState[i].activity *= gate;
      }

      outputs.push(regionActivity);
      signal = neuronOutput.slice(0, 3); // pass top 3 outputs
    }

    return outputs;
  }

  step(dt) {
    this.time += dt;
    for (const neuron of this.neurons) {
      neuron.step(dt);
    }
    // Update coherence between adjacent regions
    for (let i = 1; i < this.regions.length; i++) {
      const diff = Math.abs(this.regionState[i].activity - this.regionState[i - 1].activity);
      this.regionState[i].coherence = 1 - diff;
    }
  }

  getState() {
    return {
      time: this.time,
      regions: this.regions.map((r, i) => ({
        ...r,
        ...this.regionState[i],
        neuronState: this.neurons[i].getState()
      }))
    };
  }
}

module.exports = { TimeCrystalNeuron, TimeCrystalBrain };
