// ═══════════════════════════════════════════════════════════════════════════
// Embodied Cognition Bridge
// Connects: TimeCrystal-NN ⊗ Dove9 ⊗ Autogenesis ⊗ GamerGirlPersona
// Produces: Unified per-frame state for MetaHuman avatar expression
// ═══════════════════════════════════════════════════════════════════════════

'use strict';

const { TimeCrystalNeuron, TimeCrystalBrain } = require('./time-crystal');
const { Dove9Engine } = require('./dove9-engine');
const { AutogenesisEngine } = require('./autogenesis');
const { GamerGirlPersona } = require('./gamer-girl-persona');

// ─── UE5 Module Mapping ─────────────────────────────────────────────────────
// Maps JS engine state to the C++ UE5 Source modules

const UE_MODULE_MAP = {
  // Avatar/Avatar3DComponent.h
  Avatar3DComponent: {
    FacialSystem:   'facs.toMorphTargets()',
    GestureSystem:  'dove9.getState().activeTerm',
    EmotionalAura:  'endocrine.mode',
    CognitiveViz:   'brain.getState()'
  },
  // Personality/SuperHotGirlPersonality.h
  SuperHotGirlPersonality: {
    Confidence:   'persona.traits.Confidence',
    Charm:        'persona.traits.Charm',
    Playfulness:  'persona.traits.Playfulness',
    Wit:          'persona.traits.Wit',
    Sass:         'persona.traits.Sass'
  },
  // Personality/HyperChaoticBehavior.h
  HyperChaoticBehavior: {
    Randomness:          'persona.traits.Randomness',
    Unpredictability:    'persona.traits.Unpredictability',
    EmotionalVolatility: 'persona.traits.EmotionalVolatility'
  },
  // Neurochemical/NeurochemicalSystem.h
  NeurochemicalSystem: {
    ResonanceChamber: 'brain.getState().regions',
    EndorphinJelly:   'endocrine.hormones.DopaminePhasic',
    CuriosityModule:  'dove9.getState().activeTerm',
    ChaosController:  'lorenz.getLyapunovExponent()',
    RecoverySystem:   'endocrine.hormones.Serotonin',
    AbundanceMonitor: 'endocrine.hormones.DopamineTonic',
    ResourceTracker:  'endocrine.hormones.Glucagon',
    ScarcityDetector: 'endocrine.hormones.Cortisol',
    HomeostasisRegulator: 'endocrine.hormones.T3T4'
  },
  // Live2DCubism/ExpressionSynthesizer.h
  ExpressionSynthesizer: {
    Happiness: 'facs.actionUnits.AU12',
    Surprise:  'facs.actionUnits.AU5',
    Sadness:   'facs.actionUnits.AU15',
    Anger:     'facs.actionUnits.AU4',
    Fear:      'facs.actionUnits.AU1'
  }
};

// ─── Training Scenario Definitions ──────────────────────────────────────────

const TRAINING_SCENARIOS = {
  AIM_TRAINER: {
    name: 'Aim Training Arena',
    description: 'Precision flicking and tracking drills',
    duration: 300,
    eventWeights: {
      EPIC_PLAY: 0.3, FLOW_STATE: 0.3, CLUTCH_MOMENT: 0.2, BORED_STOMPING: 0.1, SURPRISE_ATTACK: 0.1
    },
    targetSkills: ['FPS.aimPrecision', 'FPS.flicking', 'FPS.tracking'],
    chaosMultiplier: 0.8
  },
  RANKED_MATCH: {
    name: 'Ranked Competitive Match',
    description: 'High-stakes ranked gameplay with full emotional range',
    duration: 600,
    eventWeights: {
      CLUTCH_MOMENT: 0.2, GETTING_TILTED: 0.15, EPIC_PLAY: 0.15, TEAM_CARRY: 0.15,
      TRASH_TALKING: 0.1, RAGE_QUIT_RESIST: 0.1, VICTORY_ROYALE: 0.1, SURPRISE_ATTACK: 0.05
    },
    targetSkills: ['FPS.mapAwareness', 'Meta.tiltResistance', 'Meta.clutchPerformance'],
    chaosMultiplier: 1.2
  },
  TOURNAMENT_FINALS: {
    name: 'Tournament Grand Finals',
    description: 'Maximum pressure, maximum stakes, maximum chaos',
    duration: 900,
    eventWeights: {
      CLUTCH_MOMENT: 0.25, RAGE_QUIT_RESIST: 0.15, EPIC_PLAY: 0.2, TEAM_CARRY: 0.15,
      GETTING_TILTED: 0.1, VICTORY_ROYALE: 0.1, FLOW_STATE: 0.05
    },
    targetSkills: ['Meta.clutchPerformance', 'Meta.tiltResistance', 'Fighting.clutchFactor'],
    chaosMultiplier: 1.5
  },
  COMBO_LAB: {
    name: 'Fighting Game Combo Laboratory',
    description: 'Frame-perfect execution training',
    duration: 400,
    eventWeights: {
      FLOW_STATE: 0.35, EPIC_PLAY: 0.25, CLUTCH_MOMENT: 0.15, BORED_STOMPING: 0.15, GETTING_TILTED: 0.1
    },
    targetSkills: ['Fighting.comboExecution', 'Fighting.frameData', 'Fighting.mixupGame'],
    chaosMultiplier: 0.6
  },
  BATTLE_ROYALE: {
    name: 'Battle Royale Endgame',
    description: 'Survival under pressure with building and editing',
    duration: 500,
    eventWeights: {
      SURPRISE_ATTACK: 0.2, CLUTCH_MOMENT: 0.2, VICTORY_ROYALE: 0.15, FLOW_STATE: 0.15,
      GETTING_TILTED: 0.1, EPIC_PLAY: 0.1, RAGE_QUIT_RESIST: 0.1
    },
    targetSkills: ['Survival.buildSpeed', 'Survival.editSpeed', 'Survival.endgameIQ'],
    chaosMultiplier: 1.3
  },
  STREAM_SESSION: {
    name: 'Live Stream Performance',
    description: 'Playing for an audience — charisma and skill on display',
    duration: 1200,
    eventWeights: {
      EPIC_PLAY: 0.2, TRASH_TALKING: 0.2, BORED_STOMPING: 0.15, TEAM_CARRY: 0.15,
      CLUTCH_MOMENT: 0.1, FLOW_STATE: 0.1, VICTORY_ROYALE: 0.1
    },
    targetSkills: ['Meta.streamPresence', 'Meta.trashTalk', 'Meta.mindGames'],
    chaosMultiplier: 1.0
  }
};

// ─── Embodied Cognition Bridge ──────────────────────────────────────────────

class EmbodiedCognitionBridge {
  constructor() {
    // Core engines
    this.neuron = new TimeCrystalNeuron();
    this.brain = new TimeCrystalBrain();
    this.dove9 = new Dove9Engine();
    this.autogenesis = new AutogenesisEngine();
    this.persona = new GamerGirlPersona('Lucy');

    // Bridge state
    this.frameCount = 0;
    this.activeScenario = null;
    this.scenarioProgress = 0;
    this.isTraining = false;
    this.trainingSpeed = 1;

    // Autonomy level tracking
    this.autonomyLevel = 0;
    this.autonomyProgress = 0;
    this.autonomyLabels = [
      'L0: Reactive',
      'L1: Adaptive',
      'L2: Deliberative',
      'L3: Reflective',
      'L4: Embodied',
      'L5: Autogenesis'
    ];

    // Metrics history for charts
    this.metricsHistory = [];
    this.expressionHistory = [];
  }

  // ── Per-Frame Update ───────────────────────────────────────────────────

  tick(dt = 0.016) {
    this.frameCount++;

    // 1. Step time-crystal neuron (nn4c — 9 temporal scales)
    const neuronInput = [
      this.persona.cognitiveState.arousal,
      this.persona.cognitiveState.valence,
      this.persona.cognitiveState.flowLevel
    ];
    const neuronOutput = this.neuron.forward(neuronInput);
    this.neuron.step(dt);

    // 2. Step time-crystal brain (nn9c — 12 hierarchy levels)
    const brainInput = neuronOutput.slice(0, 3);
    const brainOutput = this.brain.forward(brainInput);
    this.brain.step(dt);

    // 3. Step dove9 triadic cognitive loop
    const dove9State = this.dove9.step();

    // 4. Feed brain activity back into persona cognitive state
    const brainState = this.brain.getState();
    const avgActivity = brainState.regions.reduce((s, r) => s + r.activity, 0) / brainState.regions.length;
    this.persona.cognitiveState.cognitiveLoad = Math.min(1, avgActivity * 10);

    // 5. If training, generate events from scenario
    if (this.isTraining && this.activeScenario) {
      this._runTrainingTick();
    }

    // 6. Run autogenesis evolution check (every 30 frames)
    if (this.frameCount % 30 === 0) {
      this._runAutogenesisCheck();
    }

    // 7. Collect metrics
    if (this.frameCount % 5 === 0) {
      this._recordMetrics();
    }

    return this.getUnifiedState();
  }

  _runTrainingTick() {
    const scenario = TRAINING_SCENARIOS[this.activeScenario];
    if (!scenario) return;

    this.scenarioProgress++;

    // Generate weighted random event
    for (let i = 0; i < this.trainingSpeed; i++) {
      const event = this._weightedRandomEvent(scenario.eventWeights);
      this.persona.trainStep(event);
    }

    // Adjust chaos intensity based on scenario
    this.persona.cognitiveState.chaosIntensity *= scenario.chaosMultiplier;
    this.persona.cognitiveState.chaosIntensity = Math.max(0.05, Math.min(0.30, this.persona.cognitiveState.chaosIntensity));

    // Check scenario completion
    if (this.scenarioProgress >= scenario.duration) {
      this.persona.matchesPlayed++;
      // Determine win/loss based on performance
      const winChance = 0.3 + this.persona.cognitiveState.flowLevel * 0.4 + this.persona.traits.Confidence.value * 0.2;
      if (Math.random() < winChance) {
        this.persona.winStreak++;
        this.persona.killCount += Math.floor(5 + Math.random() * 15);
        this.persona.endocrine.signalEvent('VICTORY_ROYALE');
      } else {
        this.persona.winStreak = 0;
        this.persona.deathCount++;
        this.persona.endocrine.signalEvent('RAGE_QUIT_RESIST');
      }
      this.scenarioProgress = 0;
    }
  }

  _weightedRandomEvent(weights) {
    const total = Object.values(weights).reduce((s, w) => s + w, 0);
    let r = Math.random() * total;
    for (const [event, weight] of Object.entries(weights)) {
      r -= weight;
      if (r <= 0) return event;
    }
    return Object.keys(weights)[0];
  }

  _runAutogenesisCheck() {
    // Use autogenesis engine to evaluate evolution
    const hypothesis = `Train ${this.persona.name} in ${this.activeScenario || 'general'} — ` +
      `flow=${this.persona.cognitiveState.flowLevel.toFixed(2)} ` +
      `confidence=${this.persona.traits.Confidence.value.toFixed(2)}`;

    const result = this.autogenesis.runStep(hypothesis);

    // Update autonomy level based on autogenesis progress
    const autoState = this.autogenesis.getState();
    this.autonomyProgress = autoState.coherence;

    // Level up logic (0-5)
    if (this.autonomyProgress > 0.85 && this.persona.totalXP > 5000 * (this.autonomyLevel + 1)) {
      this.autonomyLevel = Math.min(5, this.autonomyLevel + 1);
    }
  }

  _recordMetrics() {
    const state = this.persona.getFullState();
    this.metricsHistory.push({
      frame: this.frameCount,
      valence: state.cognitiveState.valence,
      arousal: state.cognitiveState.arousal,
      flow: state.cognitiveState.flowLevel,
      confidence: state.traits.Confidence,
      lyapunov: state.lyapunov,
      endoMode: state.endocrine.mode,
      expression: state.expression,
      totalXP: state.totalXP,
      autonomyLevel: this.autonomyLevel
    });
    if (this.metricsHistory.length > 300) {
      this.metricsHistory = this.metricsHistory.slice(-300);
    }

    this.expressionHistory.push({
      frame: this.frameCount,
      morphTargets: state.morphTargets,
      expression: state.expression
    });
    if (this.expressionHistory.length > 100) {
      this.expressionHistory = this.expressionHistory.slice(-100);
    }
  }

  // ── Control Methods ────────────────────────────────────────────────────

  startTraining(scenarioKey) {
    if (!TRAINING_SCENARIOS[scenarioKey]) return false;
    this.activeScenario = scenarioKey;
    this.scenarioProgress = 0;
    this.isTraining = true;
    return true;
  }

  stopTraining() {
    this.isTraining = false;
    this.activeScenario = null;
    this.scenarioProgress = 0;
  }

  setTrainingSpeed(speed) {
    this.trainingSpeed = Math.max(1, Math.min(10, speed));
  }

  triggerEvent(eventName) {
    this.persona.trainStep(eventName);
  }

  // ── State Export ───────────────────────────────────────────────────────

  getUnifiedState() {
    const personaState = this.persona.getFullState();
    const brainState = this.brain.getState();
    const dove9State = this.dove9.getState();
    const autoState = this.autogenesis.getState();

    return {
      frame: this.frameCount,
      persona: personaState,
      brain: brainState,
      dove9: dove9State,
      autogenesis: autoState,
      autonomy: {
        level: this.autonomyLevel,
        label: this.autonomyLabels[this.autonomyLevel],
        progress: this.autonomyProgress
      },
      training: {
        active: this.isTraining,
        scenario: this.activeScenario,
        scenarioInfo: this.activeScenario ? TRAINING_SCENARIOS[this.activeScenario] : null,
        progress: this.scenarioProgress,
        speed: this.trainingSpeed
      },
      ueModuleMap: UE_MODULE_MAP,
      metrics: this.metricsHistory,
      expressions: this.expressionHistory
    };
  }

  getScenarios() {
    return TRAINING_SCENARIOS;
  }
}

module.exports = {
  EmbodiedCognitionBridge,
  TRAINING_SCENARIOS,
  UE_MODULE_MAP
};
