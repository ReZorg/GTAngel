// ═══════════════════════════════════════════════════════════════════════════
// Gamer Girl Persona Engine — "Lucy"
// Hyper-Chaotic Hardcore Gamer Girl whose skills are unmatched
// Composition: SuperHotGirlPersonality ⊗ HyperChaoticBehavior ⊗ 4E Cognition
// ═══════════════════════════════════════════════════════════════════════════

'use strict';

// ─── Personality Trait System ────────────────────────────────────────────────
// Maps to UE5 FSuperHotGirlTraits + FHyperChaoticProperties

const PERSONALITY_TRAITS = {
  // SuperHotGirl core traits
  Confidence:     { value: 0.95, min: 0.7, max: 1.0, decay: 0.001, recovery: 0.01 },
  Charm:          { value: 0.85, min: 0.5, max: 1.0, decay: 0.002, recovery: 0.008 },
  Playfulness:    { value: 0.90, min: 0.6, max: 1.0, decay: 0.003, recovery: 0.012 },
  Wit:            { value: 0.88, min: 0.5, max: 1.0, decay: 0.001, recovery: 0.005 },
  Sass:           { value: 0.92, min: 0.6, max: 1.0, decay: 0.002, recovery: 0.010 },

  // Gamer-specific traits
  Aggression:     { value: 0.80, min: 0.3, max: 1.0, decay: 0.005, recovery: 0.015 },
  Precision:      { value: 0.85, min: 0.4, max: 1.0, decay: 0.002, recovery: 0.008 },
  Adaptability:   { value: 0.90, min: 0.5, max: 1.0, decay: 0.001, recovery: 0.010 },
  RiskTolerance:  { value: 0.75, min: 0.2, max: 1.0, decay: 0.003, recovery: 0.012 },
  FlowState:      { value: 0.60, min: 0.0, max: 1.0, decay: 0.010, recovery: 0.005 },

  // HyperChaotic properties
  Randomness:     { value: 0.70, min: 0.3, max: 0.95, decay: 0.002, recovery: 0.008 },
  Unpredictability: { value: 0.80, min: 0.4, max: 0.95, decay: 0.003, recovery: 0.010 },
  EmotionalVolatility: { value: 0.65, min: 0.2, max: 0.90, decay: 0.004, recovery: 0.012 }
};

// ─── Gamer Skill Domains ────────────────────────────────────────────────────

const SKILL_DOMAINS = {
  FPS: {
    name: 'First-Person Shooter',
    skills: {
      aimPrecision:     { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 100 },
      reactionTime:     { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 120 },
      mapAwareness:     { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 80 },
      flicking:         { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 150 },
      tracking:         { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 130 },
      crosshairPlacement: { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 90 }
    }
  },
  MOBA: {
    name: 'MOBA / Strategy',
    skills: {
      lastHitting:      { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 80 },
      mapControl:       { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 100 },
      teamfightPos:     { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 110 },
      objectiveTiming:  { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 90 },
      championMastery:  { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 140 },
      shotcalling:      { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 120 }
    }
  },
  Fighting: {
    name: 'Fighting Games',
    skills: {
      comboExecution:   { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 130 },
      frameData:        { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 150 },
      mixupGame:        { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 110 },
      spacing:          { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 100 },
      reads:            { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 120 },
      clutchFactor:     { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 140 }
    }
  },
  Survival: {
    name: 'Battle Royale / Survival',
    skills: {
      looting:          { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 70 },
      rotationSense:    { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 90 },
      buildSpeed:       { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 120 },
      editSpeed:        { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 130 },
      endgameIQ:        { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 110 },
      resourceMgmt:     { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 80 }
    }
  },
  Meta: {
    name: 'Meta-Gaming Intelligence',
    skills: {
      patchAdaptation:  { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 100 },
      tiltResistance:   { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 150 },
      mindGames:        { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 120 },
      streamPresence:   { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 80 },
      trashTalk:        { level: 0, xp: 0, maxLevel: 100, xpPerLevel: 60 },
      clutchPerformance:{ level: 0, xp: 0, maxLevel: 100, xpPerLevel: 140 }
    }
  }
};

// ─── Gameplay Emotional States ──────────────────────────────────────────────
// Maps to endocrine system events for MetaHuman expression

const GAMEPLAY_EMOTIONS = {
  CLUTCH_MOMENT:    { dopamine_phasic: 0.9, norepinephrine: 0.8, cortisol: 0.3, expression: 'FOCUSED_INTENSE' },
  VICTORY_ROYALE:   { dopamine_phasic: 1.0, serotonin: 0.8, oxytocin: 0.3, expression: 'TRIUMPHANT_GRIN' },
  EPIC_PLAY:        { dopamine_phasic: 0.95, norepinephrine: 0.5, anandamide: 0.4, expression: 'SMUG_SATISFACTION' },
  GETTING_TILTED:   { cortisol: 0.8, norepinephrine: 0.7, il6: 0.3, expression: 'FRUSTRATED_FOCUS' },
  TRASH_TALKING:    { dopamine_tonic: 0.6, norepinephrine: 0.4, oxytocin: -0.2, expression: 'SASSY_SMIRK' },
  FLOW_STATE:       { dopamine_tonic: 0.7, serotonin: 0.6, anandamide: 0.5, expression: 'SERENE_FOCUS' },
  SURPRISE_ATTACK:  { norepinephrine: 0.9, cortisol: 0.5, dopamine_phasic: 0.3, expression: 'STARTLED_ALERT' },
  TEAM_CARRY:       { dopamine_tonic: 0.8, oxytocin: 0.5, serotonin: 0.4, expression: 'CONFIDENT_LEADER' },
  RAGE_QUIT_RESIST: { cortisol: 0.9, serotonin: -0.3, norepinephrine: 0.8, expression: 'CONTROLLED_FURY' },
  BORED_STOMPING:   { dopamine_tonic: 0.3, serotonin: 0.5, melatonin: 0.2, expression: 'PLAYFUL_BORED' }
};

// ─── 4E Embodied Cognition Mapping ──────────────────────────────────────────
// Maps Vervaeke's 4E framework to gameplay behavior

const EMBODIED_COGNITION_4E = {
  Embodied: {
    description: 'Body schema drives gameplay mechanics',
    channels: [
      { name: 'Hand-Eye Coordination', source: 'nn4c.Dendritic', target: 'FPS.aimPrecision' },
      { name: 'Muscle Memory', source: 'nn4c.Synaptic', target: 'Fighting.comboExecution' },
      { name: 'Reaction Reflex', source: 'nn4c.AIS', target: 'FPS.reactionTime' },
      { name: 'Somatic Markers', source: 'endocrine.cortisol', target: 'Meta.tiltResistance' },
      { name: 'Proprioceptive Aim', source: 'nn4c.Membrane', target: 'FPS.tracking' }
    ]
  },
  Embedded: {
    description: 'Environmental context shapes strategy',
    channels: [
      { name: 'Map Reading', source: 'nn9c.CortexDomain', target: 'FPS.mapAwareness' },
      { name: 'Audio Cues', source: 'nn9c.CranialNerve', target: 'Survival.rotationSense' },
      { name: 'UI Parsing', source: 'nn9c.ThalamicBody', target: 'MOBA.objectiveTiming' },
      { name: 'Spawn Timing', source: 'dove9.Clock30', target: 'MOBA.mapControl' },
      { name: 'Zone Awareness', source: 'nn9c.SkinNerveNet', target: 'Survival.endgameIQ' }
    ]
  },
  Enacted: {
    description: 'Meaning created through gameplay interaction',
    channels: [
      { name: 'Combo Discovery', source: 'dove9.T2_IDEA_FORMATION', target: 'Fighting.mixupGame' },
      { name: 'Build Creativity', source: 'dove9.T5_ACTION_SEQUENCE', target: 'Survival.buildSpeed' },
      { name: 'Play-Making', source: 'dove9.T8_BALANCED_RESPONSE', target: 'MOBA.teamfightPos' },
      { name: 'Improvisation', source: 'chaos.lorenz', target: 'Meta.mindGames' },
      { name: 'Style Expression', source: 'aesthetic.Charisma', target: 'Meta.streamPresence' }
    ]
  },
  Extended: {
    description: 'Cognition extends through tools and community',
    channels: [
      { name: 'Controller Mastery', source: 'nn9c.ThoracicNerve', target: 'Fighting.frameData' },
      { name: 'Team Synergy', source: 'endocrine.oxytocin', target: 'MOBA.shotcalling' },
      { name: 'Meta Knowledge', source: 'nn9c.Hippocampus', target: 'Meta.patchAdaptation' },
      { name: 'Chat Presence', source: 'aesthetic.Confidence', target: 'Meta.trashTalk' },
      { name: 'Stream Performance', source: 'aesthetic.EyeSparkle', target: 'Meta.streamPresence' }
    ]
  }
};

// ─── Lorenz Attractor (Chaotic Micro-Expression Generator) ──────────────────

class LorenzAttractor {
  constructor(sigma = 10.0, rho = 28.0, beta = 8/3) {
    this.sigma = sigma;
    this.rho = rho;
    this.beta = beta;
    this.x = 1.0 + Math.random() * 0.1;
    this.y = 1.0 + Math.random() * 0.1;
    this.z = 1.0 + Math.random() * 0.1;
    this.lyapunovSum = 0;
    this.lyapunovCount = 0;
    // Shadow trajectory for Lyapunov estimation
    this.sx = this.x + 1e-8;
    this.sy = this.y;
    this.sz = this.z;
  }

  step(dt = 0.01) {
    // RK4 integration
    const k1 = this._derivatives(this.x, this.y, this.z);
    const k2 = this._derivatives(this.x + k1.dx * dt/2, this.y + k1.dy * dt/2, this.z + k1.dz * dt/2);
    const k3 = this._derivatives(this.x + k2.dx * dt/2, this.y + k2.dy * dt/2, this.z + k2.dz * dt/2);
    const k4 = this._derivatives(this.x + k3.dx * dt, this.y + k3.dy * dt, this.z + k3.dz * dt);

    this.x += (k1.dx + 2*k2.dx + 2*k3.dx + k4.dx) * dt / 6;
    this.y += (k1.dy + 2*k2.dy + 2*k3.dy + k4.dy) * dt / 6;
    this.z += (k1.dz + 2*k2.dz + 2*k3.dz + k4.dz) * dt / 6;

    // Shadow trajectory for Lyapunov
    const sk1 = this._derivatives(this.sx, this.sy, this.sz);
    this.sx += sk1.dx * dt;
    this.sy += sk1.dy * dt;
    this.sz += sk1.dz * dt;

    const dist = Math.sqrt((this.x-this.sx)**2 + (this.y-this.sy)**2 + (this.z-this.sz)**2);
    if (dist > 0) {
      this.lyapunovSum += Math.log(dist / 1e-8);
      this.lyapunovCount++;
      // Renormalize shadow
      const scale = 1e-8 / dist;
      this.sx = this.x + (this.sx - this.x) * scale;
      this.sy = this.y + (this.sy - this.y) * scale;
      this.sz = this.z + (this.sz - this.z) * scale;
    }

    return { x: this.x, y: this.y, z: this.z };
  }

  _derivatives(x, y, z) {
    return {
      dx: this.sigma * (y - x),
      dy: x * (this.rho - z) - y,
      dz: x * y - this.beta * z
    };
  }

  getLyapunovExponent() {
    return this.lyapunovCount > 0 ? this.lyapunovSum / this.lyapunovCount : 0;
  }

  // Normalized outputs for micro-expression channels
  getMicroExpressions(chaosIntensity = 0.15) {
    const nx = this.x / 25; // Normalize to ~[-1, 1]
    const ny = this.y / 30;
    const nz = this.z / 50;
    return {
      microBrow:        nx * chaosIntensity * 0.3,
      microEyeSquint:   ny * chaosIntensity * 0.2,
      microMouthCorner: nz * chaosIntensity * 0.15,
      microNoseWrinkle: (nx + ny) * chaosIntensity * 0.1,
      microJaw:         nz * chaosIntensity * 0.05
    };
  }
}

// ─── Virtual Endocrine System (16-channel hormone bus) ──────────────────────

class EndocrineSystem {
  constructor() {
    this.hormones = {
      CRH:             { id: 0,  value: 0.05, baseline: 0.05, halfLife: 5,   label: 'Stress Signal' },
      ACTH:            { id: 1,  value: 0.05, baseline: 0.05, halfLife: 10,  label: 'Stress Relay' },
      Cortisol:        { id: 2,  value: 0.15, baseline: 0.15, halfLife: 30,  label: 'Resource Mobilization' },
      DopamineTonic:   { id: 3,  value: 0.30, baseline: 0.30, halfLife: 20,  label: 'Baseline Motivation' },
      DopaminePhasic:  { id: 4,  value: 0.00, baseline: 0.00, halfLife: 3,   label: 'Reward Spike' },
      Serotonin:       { id: 5,  value: 0.40, baseline: 0.40, halfLife: 50,  label: 'Mood / Patience' },
      Norepinephrine:  { id: 6,  value: 0.10, baseline: 0.10, halfLife: 8,   label: 'Arousal / Vigilance' },
      Oxytocin:        { id: 7,  value: 0.10, baseline: 0.10, halfLife: 15,  label: 'Trust / Bonding' },
      T3T4:            { id: 8,  value: 0.50, baseline: 0.50, halfLife: 100, label: 'Processing Rate' },
      Melatonin:       { id: 9,  value: 0.00, baseline: 0.00, halfLife: 12,  label: 'Circadian' },
      Insulin:         { id: 10, value: 0.20, baseline: 0.20, halfLife: 10,  label: 'Energy Conservation' },
      Glucagon:        { id: 11, value: 0.10, baseline: 0.10, halfLife: 8,   label: 'Energy Mobilization' },
      IL6:             { id: 12, value: 0.05, baseline: 0.05, halfLife: 20,  label: 'System Health' },
      Anandamide:      { id: 13, value: 0.10, baseline: 0.10, halfLife: 6,   label: 'Noise Reduction' },
      Reserved1:       { id: 14, value: 0.00, baseline: 0.00, halfLife: 10,  label: 'Extension 1' },
      Reserved2:       { id: 15, value: 0.00, baseline: 0.00, halfLife: 10,  label: 'Extension 2' }
    };

    this.mode = 'RESTING';
    this.modeHistory = [];
  }

  // Signal a gameplay event
  signalEvent(eventType) {
    const emotion = GAMEPLAY_EMOTIONS[eventType];
    if (!emotion) return;

    for (const [hormone, delta] of Object.entries(emotion)) {
      if (hormone === 'expression') continue;
      const h = this._findHormone(hormone);
      if (h) {
        h.value = Math.max(0, Math.min(1, h.value + delta * 0.3));
      }
    }
  }

  _findHormone(name) {
    const map = {
      dopamine_phasic: 'DopaminePhasic',
      dopamine_tonic: 'DopamineTonic',
      serotonin: 'Serotonin',
      norepinephrine: 'Norepinephrine',
      cortisol: 'Cortisol',
      oxytocin: 'Oxytocin',
      melatonin: 'Melatonin',
      anandamide: 'Anandamide',
      il6: 'IL6'
    };
    return this.hormones[map[name] || name];
  }

  // Exponential decay toward baselines
  tick(dt = 1) {
    for (const h of Object.values(this.hormones)) {
      const decayRate = Math.log(2) / h.halfLife;
      h.value += (h.baseline - h.value) * decayRate * dt;
      h.value = Math.max(0, Math.min(1, h.value));
    }
    this._detectMode();
  }

  _detectMode() {
    const h = this.hormones;
    const cortisol = h.Cortisol.value;
    const dopP = h.DopaminePhasic.value;
    const dopT = h.DopamineTonic.value;
    const sero = h.Serotonin.value;
    const nore = h.Norepinephrine.value;
    const oxy = h.Oxytocin.value;
    const mel = h.Melatonin.value;

    let newMode = 'RESTING';
    if (dopP > 0.5) newMode = 'REWARD';
    else if (cortisol > 0.5 && nore > 0.5) newMode = 'THREAT';
    else if (cortisol > 0.4) newMode = 'STRESSED';
    else if (nore > 0.5) newMode = 'VIGILANT';
    else if (dopT > 0.5 && sero > 0.4) newMode = 'FOCUSED';
    else if (oxy > 0.4) newMode = 'SOCIAL';
    else if (nore > 0.3 && dopT > 0.3) newMode = 'EXPLORATORY';
    else if (sero > 0.5) newMode = 'REFLECTIVE';
    else if (mel > 0.3) newMode = 'MAINTENANCE';

    if (newMode !== this.mode) {
      this.modeHistory.push({ from: this.mode, to: newMode, time: Date.now() });
      if (this.modeHistory.length > 100) this.modeHistory = this.modeHistory.slice(-100);
      this.mode = newMode;
    }
  }

  getState() {
    return {
      hormones: Object.fromEntries(
        Object.entries(this.hormones).map(([k, v]) => [k, { ...v }])
      ),
      mode: this.mode,
      modeHistory: [...this.modeHistory]
    };
  }
}

// ─── FACS Action Unit System ────────────────────────────────────────────────
// Maps endocrine + cognitive state to MetaHuman CTRL_ morph targets

class FACSExpressionSystem {
  constructor() {
    this.actionUnits = {};
    this.aesthetics = {
      ConfidencePosture: 0.85,
      Charisma: 0.80,
      EyeSparkle: 0.75,
      GracefulMovement: 0.70,
      EmissiveGlow: 0.20
    };
  }

  // Compute AU values from endocrine state
  computeFromEndocrine(endoState) {
    const h = endoState.hormones;
    const aus = {};

    // Hormone → AU mappings (from endocrine-expression-mapping.md)
    aus.AU1  = (h.Cortisol.value * 0.5);
    aus.AU2  = 0;
    aus.AU4  = (h.Cortisol.value * 0.8) + (h.IL6.value * 0.5);
    aus.AU5  = (h.Norepinephrine.value * 0.8);
    aus.AU6  = (h.DopaminePhasic.value * 0.7) + (h.Serotonin.value * 0.4) + (h.Oxytocin.value * 0.6) + (h.Anandamide.value * 0.5);
    aus.AU7  = (h.Norepinephrine.value * 0.5) + (h.Melatonin.value * 0.4);
    aus.AU9  = 0;
    aus.AU10 = (h.IL6.value * 0.4);
    aus.AU12 = (h.DopaminePhasic.value * 0.9) + (h.DopamineTonic.value * 0.3) + (h.Serotonin.value * 0.3) + (h.Oxytocin.value * 0.5);
    aus.AU14 = 0;
    aus.AU15 = (h.Cortisol.value * 0.4);
    aus.AU17 = 0;
    aus.AU20 = (h.Norepinephrine.value * 0.3);
    aus.AU23 = 0;
    aus.AU25 = (h.Oxytocin.value * 0.3) + (h.Anandamide.value * 0.3);
    aus.AU26 = 0;
    aus.AU28 = 0;
    aus.AU43 = (h.Melatonin.value * 0.7);
    aus.AU45 = 0;
    aus.AU46 = 0;

    this.actionUnits = aus;
    return aus;
  }

  // Add cognitive state modulation
  addCognitiveModulation(cogState) {
    const valence = cogState.valence || 0;
    const arousal = cogState.arousal || 0;
    const load = cogState.cognitiveLoad || 0;

    this.actionUnits.AU6  = Math.min(1, (this.actionUnits.AU6 || 0) + (valence > 0 ? valence * 0.5 : 0));
    this.actionUnits.AU12 = Math.min(1, (this.actionUnits.AU12 || 0) + (valence > 0 ? valence * 0.6 : 0));
    this.actionUnits.AU15 = Math.min(1, (this.actionUnits.AU15 || 0) + (valence < 0 ? Math.abs(valence) * 0.4 : 0));
    this.actionUnits.AU5  = Math.min(1, (this.actionUnits.AU5 || 0) + arousal * 0.4);
    this.actionUnits.AU25 = Math.min(1, (this.actionUnits.AU25 || 0) + arousal * 0.3);
    this.actionUnits.AU26 = Math.min(1, (this.actionUnits.AU26 || 0) + arousal * 0.2);
    this.actionUnits.AU4  = Math.min(1, (this.actionUnits.AU4 || 0) + load * 0.5);
    this.actionUnits.AU7  = Math.min(1, (this.actionUnits.AU7 || 0) + load * 0.3);
  }

  // Apply SuperHotGirl aesthetic bias
  applyAestheticBias() {
    // Confidence biases toward subtle smile and raised chin
    this.actionUnits.AU12 = Math.min(1, (this.actionUnits.AU12 || 0) + this.aesthetics.ConfidencePosture * 0.15);
    this.actionUnits.AU17 = Math.min(1, (this.actionUnits.AU17 || 0) + this.aesthetics.ConfidencePosture * 0.1);
    // Charisma boosts cheek raise and eye contact
    this.actionUnits.AU6  = Math.min(1, (this.actionUnits.AU6 || 0) + this.aesthetics.Charisma * 0.1);
    // Sparkle widens eyes slightly
    this.actionUnits.AU5  = Math.min(1, (this.actionUnits.AU5 || 0) + this.aesthetics.EyeSparkle * 0.08);
  }

  // Add chaotic micro-expressions
  addMicroExpressions(microExpr) {
    this.actionUnits.AU1  = Math.max(0, Math.min(1, (this.actionUnits.AU1 || 0) + microExpr.microBrow));
    this.actionUnits.AU7  = Math.max(0, Math.min(1, (this.actionUnits.AU7 || 0) + microExpr.microEyeSquint));
    this.actionUnits.AU12 = Math.max(0, Math.min(1, (this.actionUnits.AU12 || 0) + microExpr.microMouthCorner));
    this.actionUnits.AU9  = Math.max(0, Math.min(1, (this.actionUnits.AU9 || 0) + microExpr.microNoseWrinkle));
    this.actionUnits.AU26 = Math.max(0, Math.min(1, (this.actionUnits.AU26 || 0) + microExpr.microJaw));
  }

  // Map to MetaHuman CTRL_ morph targets
  toMorphTargets() {
    return {
      CTRL_brow_inner_UP:     this.actionUnits.AU1  || 0,
      CTRL_brow_outer_UP:     this.actionUnits.AU2  || 0,
      CTRL_brow_down:         this.actionUnits.AU4  || 0,
      CTRL_eye_upperLid_UP:   this.actionUnits.AU5  || 0,
      CTRL_cheek_raise:       this.actionUnits.AU6  || 0,
      CTRL_eye_squint:        this.actionUnits.AU7  || 0,
      CTRL_nose_wrinkle:      this.actionUnits.AU9  || 0,
      CTRL_mouth_upperLip_UP: this.actionUnits.AU10 || 0,
      CTRL_mouth_cornerPull:  this.actionUnits.AU12 || 0,
      CTRL_mouth_dimple:      this.actionUnits.AU14 || 0,
      CTRL_mouth_cornerDepress: this.actionUnits.AU15 || 0,
      CTRL_chin_raise:        this.actionUnits.AU17 || 0,
      CTRL_mouth_stretch:     this.actionUnits.AU20 || 0,
      CTRL_mouth_tighten:     this.actionUnits.AU23 || 0,
      CTRL_mouth_lipsPart:    this.actionUnits.AU25 || 0,
      CTRL_jaw_open:          this.actionUnits.AU26 || 0,
      CTRL_mouth_lipSuck:     this.actionUnits.AU28 || 0,
      CTRL_eye_blink:         this.actionUnits.AU43 || 0
    };
  }

  getCompositeExpression() {
    const au = this.actionUnits;
    if ((au.AU6 || 0) > 0.5 && (au.AU12 || 0) > 0.5) return 'Genuine Smile';
    if ((au.AU12 || 0) > 0.4 && (au.AU46 || 0) > 0.3) return 'Flirtatious';
    if ((au.AU4 || 0) > 0.5 && (au.AU7 || 0) > 0.4) return 'Focused Intensity';
    if ((au.AU1 || 0) > 0.3 && (au.AU5 || 0) > 0.4) return 'Curious';
    if ((au.AU12 || 0) > 0.3 && (au.AU17 || 0) > 0.2) return 'Confident';
    if ((au.AU12 || 0) > 0.3 && (au.AU25 || 0) > 0.3) return 'Playful';
    if ((au.AU15 || 0) > 0.4) return 'Displeased';
    return 'Neutral';
  }
}

// ─── Main Persona Engine ────────────────────────────────────────────────────

class GamerGirlPersona {
  constructor(name = 'Lucy') {
    this.name = name;
    this.traits = JSON.parse(JSON.stringify(PERSONALITY_TRAITS));
    this.skills = JSON.parse(JSON.stringify(SKILL_DOMAINS));
    this.embodied4E = EMBODIED_COGNITION_4E;
    this.lorenz = new LorenzAttractor();
    this.endocrine = new EndocrineSystem();
    this.facs = new FACSExpressionSystem();
    this.time = 0;
    this.totalXP = 0;
    this.killCount = 0;
    this.deathCount = 0;
    this.winStreak = 0;
    this.matchesPlayed = 0;
    this.trainingLog = [];

    // Cognitive state (fed by time-crystal-nn and dove9)
    this.cognitiveState = {
      valence: 0.3,
      arousal: 0.5,
      cognitiveLoad: 0.4,
      flowLevel: 0,
      chaosIntensity: 0.15
    };
  }

  // ── Training Step ──────────────────────────────────────────────────────
  // Simulates one training iteration: gameplay event → endocrine → expression → skill gain

  trainStep(event = null) {
    this.time++;

    // 1. Generate or receive gameplay event
    if (!event) {
      const events = Object.keys(GAMEPLAY_EMOTIONS);
      event = events[Math.floor(Math.random() * events.length)];
    }

    // 2. Signal endocrine system
    this.endocrine.signalEvent(event);
    this.endocrine.tick(1);

    // 3. Update Lorenz attractor
    for (let i = 0; i < 10; i++) this.lorenz.step(0.01);

    // 4. Compute FACS expression
    const endoState = this.endocrine.getState();
    this.facs.computeFromEndocrine(endoState);
    this.facs.addCognitiveModulation(this.cognitiveState);
    this.facs.applyAestheticBias();
    this.facs.addMicroExpressions(this.lorenz.getMicroExpressions(this.cognitiveState.chaosIntensity));

    // 5. Determine skill gains based on event
    const skillGains = this._computeSkillGains(event);

    // 6. Update personality traits
    this._updateTraits(event);

    // 7. Update cognitive state
    this._updateCognitiveState(event);

    // 8. Record
    const result = {
      step: this.time,
      event,
      endocrineMode: endoState.mode,
      expression: this.facs.getCompositeExpression(),
      morphTargets: this.facs.toMorphTargets(),
      skillGains,
      lyapunov: this.lorenz.getLyapunovExponent(),
      cognitiveState: { ...this.cognitiveState },
      traits: this._getTraitSummary()
    };

    this.trainingLog.push(result);
    if (this.trainingLog.length > 500) this.trainingLog = this.trainingLog.slice(-500);

    return result;
  }

  _computeSkillGains(event) {
    const gains = {};
    const baseXP = 10 + Math.random() * 20;
    const flowMultiplier = 1 + this.cognitiveState.flowLevel * 2;

    // Event-specific skill targeting
    const eventSkillMap = {
      CLUTCH_MOMENT:    ['FPS.reactionTime', 'Fighting.clutchFactor', 'Meta.clutchPerformance'],
      VICTORY_ROYALE:   ['Survival.endgameIQ', 'Meta.tiltResistance', 'Survival.rotationSense'],
      EPIC_PLAY:        ['FPS.aimPrecision', 'Fighting.comboExecution', 'MOBA.teamfightPos'],
      GETTING_TILTED:   ['Meta.tiltResistance', 'Meta.mindGames'],
      TRASH_TALKING:    ['Meta.trashTalk', 'Meta.streamPresence'],
      FLOW_STATE:       ['FPS.tracking', 'Fighting.spacing', 'MOBA.lastHitting'],
      SURPRISE_ATTACK:  ['FPS.reactionTime', 'Survival.rotationSense', 'FPS.mapAwareness'],
      TEAM_CARRY:       ['MOBA.shotcalling', 'MOBA.teamfightPos', 'Meta.streamPresence'],
      RAGE_QUIT_RESIST: ['Meta.tiltResistance', 'Meta.clutchPerformance'],
      BORED_STOMPING:   ['FPS.aimPrecision', 'Meta.mindGames', 'Fighting.reads']
    };

    const targets = eventSkillMap[event] || ['Meta.patchAdaptation'];
    for (const target of targets) {
      const [domain, skill] = target.split('.');
      if (this.skills[domain] && this.skills[domain].skills[skill]) {
        const s = this.skills[domain].skills[skill];
        const xp = baseXP * flowMultiplier * (1 + Math.random() * 0.5);
        s.xp += xp;
        this.totalXP += xp;
        if (s.xp >= s.xpPerLevel * (s.level + 1)) {
          s.level = Math.min(s.maxLevel, s.level + 1);
          s.xp = 0;
        }
        gains[target] = xp;
      }
    }

    return gains;
  }

  _updateTraits(event) {
    // Traits shift based on gameplay events
    const traitShifts = {
      CLUTCH_MOMENT:    { Confidence: 0.02, Aggression: 0.01 },
      VICTORY_ROYALE:   { Confidence: 0.03, Playfulness: 0.02, Sass: 0.01 },
      EPIC_PLAY:        { Confidence: 0.02, Wit: 0.01, FlowState: 0.03 },
      GETTING_TILTED:   { Confidence: -0.02, EmotionalVolatility: 0.03, Aggression: 0.02 },
      TRASH_TALKING:    { Sass: 0.03, Charm: -0.01, Playfulness: 0.01 },
      FLOW_STATE:       { FlowState: 0.05, Precision: 0.02, Adaptability: 0.01 },
      SURPRISE_ATTACK:  { RiskTolerance: 0.01, Unpredictability: 0.02 },
      TEAM_CARRY:       { Confidence: 0.02, Charm: 0.02 },
      RAGE_QUIT_RESIST: { Confidence: 0.01, EmotionalVolatility: -0.02 },
      BORED_STOMPING:   { Playfulness: 0.02, Sass: 0.02, Aggression: -0.01 }
    };

    const shifts = traitShifts[event] || {};
    for (const [trait, delta] of Object.entries(shifts)) {
      if (this.traits[trait]) {
        this.traits[trait].value = Math.max(
          this.traits[trait].min,
          Math.min(this.traits[trait].max, this.traits[trait].value + delta)
        );
      }
    }

    // Natural decay toward baseline
    for (const t of Object.values(this.traits)) {
      if (t.value > (t.min + t.max) / 2) {
        t.value -= t.decay;
      } else {
        t.value += t.recovery * 0.5;
      }
    }
  }

  _updateCognitiveState(event) {
    const emotionMap = GAMEPLAY_EMOTIONS[event];
    if (!emotionMap) return;

    // Valence from dopamine vs cortisol balance
    const dop = (this.endocrine.hormones.DopaminePhasic.value + this.endocrine.hormones.DopamineTonic.value) / 2;
    const cor = this.endocrine.hormones.Cortisol.value;
    this.cognitiveState.valence = Math.max(-1, Math.min(1, (dop - cor) * 2));

    // Arousal from norepinephrine
    this.cognitiveState.arousal = this.endocrine.hormones.Norepinephrine.value;

    // Cognitive load from cortisol + norepinephrine
    this.cognitiveState.cognitiveLoad = Math.min(1, (cor + this.endocrine.hormones.Norepinephrine.value) / 2);

    // Flow state builds over sustained FOCUSED mode
    if (this.endocrine.mode === 'FOCUSED' || this.endocrine.mode === 'REWARD') {
      this.cognitiveState.flowLevel = Math.min(1, this.cognitiveState.flowLevel + 0.05);
    } else {
      this.cognitiveState.flowLevel = Math.max(0, this.cognitiveState.flowLevel - 0.02);
    }

    // Chaos intensity modulated by emotional volatility
    this.cognitiveState.chaosIntensity = 0.10 + this.traits.EmotionalVolatility.value * 0.15;
  }

  _getTraitSummary() {
    return Object.fromEntries(
      Object.entries(this.traits).map(([k, v]) => [k, v.value])
    );
  }

  getFullState() {
    return {
      name: this.name,
      time: this.time,
      totalXP: this.totalXP,
      matchesPlayed: this.matchesPlayed,
      killCount: this.killCount,
      deathCount: this.deathCount,
      winStreak: this.winStreak,
      traits: this._getTraitSummary(),
      skills: this.skills,
      endocrine: this.endocrine.getState(),
      cognitiveState: this.cognitiveState,
      expression: this.facs.getCompositeExpression(),
      morphTargets: this.facs.toMorphTargets(),
      actionUnits: { ...this.facs.actionUnits },
      lyapunov: this.lorenz.getLyapunovExponent(),
      lorenzState: { x: this.lorenz.x, y: this.lorenz.y, z: this.lorenz.z },
      aesthetics: { ...this.facs.aesthetics },
      embodied4E: this.embodied4E
    };
  }
}

module.exports = {
  GamerGirlPersona,
  LorenzAttractor,
  EndocrineSystem,
  FACSExpressionSystem,
  PERSONALITY_TRAITS,
  SKILL_DOMAINS,
  GAMEPLAY_EMOTIONS,
  EMBODIED_COGNITION_4E
};
