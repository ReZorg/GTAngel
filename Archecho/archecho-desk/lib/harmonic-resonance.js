// ═══════════════════════════════════════════════════════════════════════════
// Harmonic Resonance ESN + Autognosis Layer
// Reservoir: superposition of harmonic oscillators (phase/amplitude state)
// Readout: ridge regression over spectral reservoir state
// Autognosis: hierarchical self-image levels mapped onto octave frequency bands
// Composition: /harmonic-resonance-esn ⊗ @AUTOGNOSIS.skill
// ═══════════════════════════════════════════════════════════════════════════

'use strict';

// ─── Harmonic Oscillator Reservoir ──────────────────────────────────────────
// State: per-oscillator phase θ_i and amplitude a_i.
// Input coupling: input channels modulate instantaneous frequency and drive
// amplitude. Leak toward baseline amplitude provides the echo-state property.

class HarmonicReservoir {
  /**
   * @param {object} config
   * @param {number} config.nOscillators   Number of harmonic oscillators
   * @param {number} config.fundamentalFreq Fundamental frequency f0 (cycles/step)
   * @param {number} config.inputDim       Dimensionality of input signal
   * @param {number} config.leak           Amplitude leak rate toward 0 (0..1)
   * @param {number} config.freqCoupling   How strongly input modulates frequency
   * @param {number} config.ampCoupling    How strongly input drives amplitude
   * @param {number} config.seed           Deterministic init seed
   */
  constructor(config = {}) {
    this.n = config.nOscillators || 100;
    this.f0 = config.fundamentalFreq || 0.05;
    this.inputDim = config.inputDim || 1;
    this.leak = config.leak ?? 0.05;
    this.freqCoupling = config.freqCoupling ?? 0.3;
    this.ampCoupling = config.ampCoupling ?? 0.5;

    // Harmonic assignment: oscillator i resonates at harmonic (i % H)+1 of f0,
    // spread across H harmonics with slight deterministic detuning.
    const H = 10;
    const rand = mulberry32(config.seed ?? 1337);
    this.harmonic = new Float64Array(this.n);
    this.detune = new Float64Array(this.n);
    this.phase = new Float64Array(this.n);
    this.amplitude = new Float64Array(this.n);
    // Input mixing matrix W_in: inputDim × n, small random weights
    this.wFreq = new Float64Array(this.inputDim * this.n);
    this.wAmp = new Float64Array(this.inputDim * this.n);

    for (let i = 0; i < this.n; i++) {
      this.harmonic[i] = (i % H) + 1;
      this.detune[i] = (rand() - 0.5) * 0.02; // ±1% detuning
      this.phase[i] = rand() * 2 * Math.PI;
      this.amplitude[i] = 0;
    }
    // Weights are shared per harmonic bucket plus per-oscillator jitter, so
    // oscillators tuned to the same harmonic resonate together when the input
    // contains that frequency (echo-state resonance sensitivity).
    const Hb = 10;
    const wFreqBand = new Float64Array(this.inputDim * Hb);
    const wAmpBand = new Float64Array(this.inputDim * Hb);
    for (let k = 0; k < this.inputDim * Hb; k++) {
      wFreqBand[k] = (rand() - 0.5) * 2;
      wAmpBand[k] = (rand() - 0.5) * 2;
    }
    for (let i = 0; i < this.n; i++) {
      const band = i % Hb;
      for (let c = 0; c < this.inputDim; c++) {
        this.wFreq[c * this.n + i] = wFreqBand[c * Hb + band] + (rand() - 0.5) * 0.1;
        this.wAmp[c * this.n + i] = wAmpBand[c * Hb + band] + (rand() - 0.5) * 0.1;
      }
    }

    this.stepCount = 0;
  }

  /**
   * Advance reservoir one step with input vector u.
   * @param {number[]|Float64Array} u input of length inputDim
   * @returns {Float64Array} spectral state [Re, Im] per oscillator (length 2n)
   */
  step(u) {
    const TWO_PI = 2 * Math.PI;

    // Kuramoto mean field per harmonic bucket: oscillators at the same
    // harmonic pull each other toward a common phase. Coherent input (matching
    // that harmonic's frequency) synchronizes the bucket; incoherent input
    // leaves it dispersed. This is what makes band coherence a meaningful
    // resonance measurement.
    if (!this._bucketOf) {
      this._bucketOf = new Int32Array(this.n);
      this._buckets = new Map();
      for (let i = 0; i < this.n; i++) {
        const h = this.harmonic[i];
        this._bucketOf[i] = h;
        if (!this._buckets.has(h)) this._buckets.set(h, []);
        this._buckets.get(h).push(i);
      }
    }
    const bucketMeanRe = new Map();
    const bucketMeanIm = new Map();
    for (const [h, idx] of this._buckets) {
      let re = 0, im = 0;
      for (const i of idx) { re += Math.cos(this.phase[i]); im += Math.sin(this.phase[i]); }
      bucketMeanRe.set(h, re / idx.length);
      bucketMeanIm.set(h, im / idx.length);
    }
    const coupling = this.syncCoupling ?? 0.4;

    for (let i = 0; i < this.n; i++) {
      let dFreq = 0;
      let dAmp = 0;
      for (let c = 0; c < this.inputDim; c++) {
        const uval = u[c] || 0;
        dFreq += this.wFreq[c * this.n + i] * uval;
        dAmp += this.wAmp[c * this.n + i] * uval;
      }
      // Instantaneous frequency = harmonic base + input modulation
      const freq = this.f0 * this.harmonic[i] * (1 + this.detune[i])
        + this.freqCoupling * dFreq * this.f0;
      let dPhase = TWO_PI * freq;
      // Kuramoto pull toward bucket mean phase
      const h = this._bucketOf[i];
      const meanPhase = Math.atan2(bucketMeanIm.get(h), bucketMeanRe.get(h));
      dPhase += coupling * Math.sin(meanPhase - this.phase[i]);
      this.phase[i] = (this.phase[i] + dPhase) % TWO_PI;
      // Echo-state amplitude dynamics: leak + input drive
      this.amplitude[i] += -this.leak * this.amplitude[i]
        + this.ampCoupling * Math.tanh(dAmp);
      // Clamp amplitude for stability
      if (this.amplitude[i] > 4) this.amplitude[i] = 4;
      else if (this.amplitude[i] < -4) this.amplitude[i] = -4;
    }
    this.stepCount++;
    return this.getSpectralState();
  }

  /** Spectral state: [a_i·cos θ_i, a_i·sin θ_i] per oscillator — the readout feature vector. */
  getSpectralState() {
    const state = new Float64Array(2 * this.n);
    for (let i = 0; i < this.n; i++) {
      state[2 * i] = this.amplitude[i] * Math.cos(this.phase[i]);
      state[2 * i + 1] = this.amplitude[i] * Math.sin(this.phase[i]);
    }
    return state;
  }

  /** Kuramoto order parameter r ∈ [0,1] — global phase coherence. */
  getPhaseCoherence() {
    let re = 0, im = 0;
    for (let i = 0; i < this.n; i++) {
      re += Math.cos(this.phase[i]);
      im += Math.sin(this.phase[i]);
    }
    return Math.sqrt(re * re + im * im) / this.n;
  }

  /** Coherence within a subset of oscillator indices (a frequency band). */
  getBandCoherence(indices) {
    if (indices.length === 0) return 0;
    let re = 0, im = 0;
    for (const i of indices) {
      re += Math.cos(this.phase[i]);
      im += Math.sin(this.phase[i]);
    }
    return Math.sqrt(re * re + im * im) / indices.length;
  }

  /** Mean absolute amplitude within a band. */
  getBandEnergy(indices) {
    if (indices.length === 0) return 0;
    let e = 0;
    for (const i of indices) e += Math.abs(this.amplitude[i]);
    return e / indices.length;
  }

  reset() {
    this.phase.fill(0);
    this.amplitude.fill(0);
    this.stepCount = 0;
  }
}

// ─── Ridge Readout ──────────────────────────────────────────────────────────
// W_out = (XᵀX + λI)⁻¹ XᵀY solved by Gaussian elimination with pivoting.

class RidgeReadout {
  constructor(config = {}) {
    this.ridge = config.ridge ?? 1e-6;
    this.w = null; // [featureDim][outputDim]
    this.trained = false;
  }

  /**
   * @param {Array<Array<number>>|Array<Float64Array>} X states  [samples][features]
   * @param {Array<Array<number>>} Y targets [samples][outputs]
   */
  fit(X, Y) {
    const samples = X.length;
    const f = X[0].length;
    const o = Y[0].length;

    // A = XᵀX + λI  (f×f), B = XᵀY (f×o)
    const A = new Float64Array(f * f);
    const B = new Float64Array(f * o);
    for (let s = 0; s < samples; s++) {
      const x = X[s];
      const y = Y[s];
      for (let i = 0; i < f; i++) {
        const xi = x[i];
        if (xi === 0) continue;
        for (let j = 0; j < f; j++) A[i * f + j] += xi * x[j];
        for (let k = 0; k < o; k++) B[i * o + k] += xi * y[k];
      }
    }
    for (let i = 0; i < f; i++) A[i * f + i] += this.ridge;

    this.w = solveLinearSystem(A, B, f, o);
    this.trained = true;
    return this;
  }

  /** Predict outputs for a single state vector. */
  predict(x) {
    if (!this.trained) throw new Error('RidgeReadout not trained');
    const f = this.w.length;
    const o = this.w[0].length;
    const y = new Array(o).fill(0);
    for (let i = 0; i < f; i++) {
      const xi = x[i];
      if (xi === 0) continue;
      const row = this.w[i];
      for (let k = 0; k < o; k++) y[k] += xi * row[k];
    }
    return y;
  }

  run(X) { return X.map((x) => this.predict(x)); }
}

// ─── Autognosis Layer ───────────────────────────────────────────────────────
// Maps the AUTOGNOSIS four-layer architecture onto harmonic ESN dynamics:
//   Self-Monitoring  → observations drive the reservoir
//   Self-Modeling    → octave bands = hierarchical self-image levels 0..L-1
//   Meta-Cognition   → cross-band coherence relations, recursive insights
//   Self-Optimization→ anomaly-driven optimization opportunities

class AutognosisReservoir {
  /**
   * @param {object} config
   * @param {number} config.levels     Number of hierarchical self-image levels (default 5)
   * @param {number} config.inputDim   Observation vector dimension
   * @param {number} config.nOscillators Total oscillators (split evenly across levels)
   */
  constructor(config = {}) {
    this.levels = config.levels || 5;
    this.inputDim = config.inputDim || 4;
    this.reservoir = new HarmonicReservoir({
      nOscillators: config.nOscillators || 100,
      fundamentalFreq: config.fundamentalFreq || 0.05,
      inputDim: this.inputDim,
      seed: config.seed ?? 42
    });

    // Self-image levels group harmonic buckets (equal base frequency), since
    // phase coherence is only meaningful within a same-frequency band.
    // With H=10 harmonics and L levels, level ℓ owns harmonic buckets
    // ℓ·(H/L) .. (ℓ+1)·(H/L)-1 — low levels = low-frequency (fast observable)
    // patterns, high levels = high-order (meta-cognitive) harmonics.
    const H = 10;
    const perH = Math.max(1, Math.floor(H / this.levels));
    this.bands = [];
    for (let l = 0; l < this.levels; l++) {
      const hStart = l * perH + 1;
      const hEnd = l === this.levels - 1 ? H : hStart + perH - 1;
      const indices = [];
      for (let i = 0; i < this.reservoir.n; i++) {
        const h = this.reservoir.harmonic[i];
        if (h >= hStart && h <= hEnd) indices.push(i);
      }
      this.bands.push(indices);
    }

    this.observationHistory = [];
    this.selfImages = [];
    this.insights = [];
    this.optimizations = [];
    this.maxHistory = 500;
  }

  /** Self-Monitoring Layer: observe system metrics, drive reservoir. */
  observe(metrics) {
    // metrics: array of numbers length inputDim (e.g. [load, errorRate, throughput, latency])
    const u = metrics.slice(0, this.inputDim);
    while (u.length < this.inputDim) u.push(0);
    this.reservoir.step(u);
    this.observationHistory.push({ t: Date.now(), u: [...u] });
    if (this.observationHistory.length > this.maxHistory) {
      this.observationHistory = this.observationHistory.slice(-this.maxHistory);
    }
  }

  /** Self-Modeling Layer: build hierarchical self-image at each level. */
  buildSelfImages() {
    this.selfImages = this.bands.map((indices, level) => {
      const confidence = this.reservoir.getBandCoherence(indices);
      const energy = this.reservoir.getBandEnergy(indices);
      return {
        level,
        confidence,
        energy,
        oscillators: indices.length,
        reflection: this._reflect(level, confidence, energy)
      };
    });
    return this.selfImages;
  }

  _reflect(level, confidence, energy) {
    if (confidence > 0.8) return `Level ${level}: strongly coherent pattern (r=${confidence.toFixed(2)})`;
    if (confidence < 0.3 && energy > 0.5) return `Level ${level}: energetic but decoherent - possible transition`;
    if (energy < 0.05) return `Level ${level}: quiescent band`;
    return `Level ${level}: moderate coherence (r=${confidence.toFixed(2)})`;
  }

  /** Meta-Cognitive Layer: generate insights from self-image relations. */
  generateInsights() {
    if (this.selfImages.length === 0) this.buildSelfImages();
    const insights = [];
    const globalR = this.reservoir.getPhaseCoherence();

    // Self-awareness: relation of levels to global coherence
    const meanConf = this.selfImages.reduce((s, im) => s + im.confidence, 0) / this.levels;
    insights.push({
      type: 'self_awareness',
      score: meanConf,
      description: meanConf > 0.6
        ? `High self-awareness (mean band coherence ${meanConf.toFixed(2)})`
        : meanConf > 0.35
          ? `Moderate self-awareness (${meanConf.toFixed(2)})`
          : `Low self-awareness (${meanConf.toFixed(2)}) - system states decorrelated`
    });

    // Stability: higher levels should be at least as coherent as lower ones
    for (let l = 1; l < this.levels; l++) {
      if (this.selfImages[l].confidence < this.selfImages[l - 1].confidence * 0.5) {
        insights.push({
          type: 'hierarchy_breakdown',
          level: l,
          description: `Level ${l} coherence (${this.selfImages[l].confidence.toFixed(2)}) collapsed vs level ${l - 1} (${this.selfImages[l - 1].confidence.toFixed(2)})`
        });
      }
    }

    // Anomaly: energy spike with low global coherence
    const totalEnergy = this.selfImages.reduce((s, im) => s + im.energy, 0);
    if (totalEnergy > 1.5 && globalR < 0.3) {
      insights.push({
        type: 'anomaly',
        description: `High energy (${totalEnergy.toFixed(2)}) with low global coherence (${globalR.toFixed(2)}) - anomalous regime`
      });
    }

    this.insights.push(...insights.map((i) => ({ ...i, t: Date.now() })));
    return insights;
  }

  /** Self-Optimization Layer: derive optimization opportunities from insights. */
  discoverOptimizations() {
    const opportunities = [];
    for (const insight of this.insights.slice(-50)) {
      if (insight.type === 'hierarchy_breakdown') {
        opportunities.push({
          priority: 'high',
          target: `level_${insight.level}`,
          action: 'Re-tune input coupling weights for decoherent band',
          risk: 'low'
        });
      } else if (insight.type === 'anomaly') {
        opportunities.push({
          priority: 'medium',
          target: 'global',
          action: 'Increase leak rate to damp anomalous energy',
          risk: 'medium'
        });
      } else if (insight.type === 'self_awareness' && insight.score < 0.35) {
        opportunities.push({
          priority: 'medium',
          target: 'global',
          action: 'Widen frequency spread to improve observability',
          risk: 'low'
        });
      }
    }
    // Dedupe by action
    const seen = new Set();
    this.optimizations = opportunities.filter((o) => {
      if (seen.has(o.action)) return false;
      seen.add(o.action);
      return true;
    });
    return this.optimizations;
  }

  /** Full autognosis cycle: observe → model → meta-cognition → optimize. */
  runCycle(metrics) {
    if (metrics) this.observe(metrics);
    const images = this.buildSelfImages();
    const insights = this.generateInsights();
    const optimizations = this.discoverOptimizations();
    return {
      selfImages: images,
      insights,
      optimizations,
      globalCoherence: this.reservoir.getPhaseCoherence(),
      overallSelfAwareness: images.reduce((s, im) => s + im.confidence, 0) / images.length
    };
  }

  getStatus() {
    return {
      levels: this.levels,
      steps: this.reservoir.stepCount,
      observations: this.observationHistory.length,
      insightCount: this.insights.length,
      pendingOptimizations: this.optimizations.length,
      selfImages: this.selfImages
    };
  }
}

// ─── Deterministic PRNG ─────────────────────────────────────────────────────

function mulberry32(seed) {
  let a = seed >>> 0;
  return function () {
    a |= 0; a = (a + 0x6D2B79F5) | 0;
    let t = Math.imul(a ^ (a >>> 15), 1 | a);
    t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

// ─── Linear solver: Gaussian elimination with partial pivoting ─────────────

function solveLinearSystem(A, B, n, m) {
  // Augment A with B; solve in place.
  const M = new Float64Array(A); // copy
  const X = new Float64Array(B); // becomes solution (n×m)

  for (let col = 0; col < n; col++) {
    // Partial pivot
    let pivot = col;
    let max = Math.abs(M[col * n + col]);
    for (let row = col + 1; row < n; row++) {
      const v = Math.abs(M[row * n + col]);
      if (v > max) { max = v; pivot = row; }
    }
    if (max < 1e-12) continue; // singular row — ridge should prevent this
    if (pivot !== col) {
      for (let j = 0; j < n; j++) {
        const tmp = M[col * n + j];
        M[col * n + j] = M[pivot * n + j];
        M[pivot * n + j] = tmp;
      }
      for (let k = 0; k < m; k++) {
        const tmp = X[col * m + k];
        X[col * m + k] = X[pivot * m + k];
        X[pivot * m + k] = tmp;
      }
    }
    // Eliminate below
    const diag = M[col * n + col];
    for (let row = col + 1; row < n; row++) {
      const factor = M[row * n + col] / diag;
      if (factor === 0) continue;
      for (let j = col; j < n; j++) M[row * n + j] -= factor * M[col * n + j];
      for (let k = 0; k < m; k++) X[row * m + k] -= factor * X[col * m + k];
    }
  }

  // Back substitution
  const out = new Float64Array(n * m);
  for (let col = n - 1; col >= 0; col--) {
    const diag = M[col * n + col];
    if (Math.abs(diag) < 1e-12) continue;
    for (let k = 0; k < m; k++) {
      let sum = X[col * m + k];
      for (let j = col + 1; j < n; j++) sum -= M[col * n + j] * out[j * m + k];
      out[col * m + k] = sum / diag;
    }
  }

  // Reshape to array of rows
  const w = [];
  for (let i = 0; i < n; i++) {
    const row = [];
    for (let k = 0; k < m; k++) row.push(out[i * m + k]);
    w.push(row);
  }
  return w;
}

module.exports = {
  HarmonicReservoir,
  RidgeReadout,
  AutognosisReservoir
};
