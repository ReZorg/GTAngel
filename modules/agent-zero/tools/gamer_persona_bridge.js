// Agent Zero bridge: stateful JSON-RPC-ish wrapper around GamerGirlPersona.
// Protocol: one JSON request per line on stdin, one JSON response per line on stdout.
//   {"id": 1, "method": "train_step", "params": {"event": "EPIC_PLAY"}}
//   {"id": 2, "method": "get_state", "params": {"full": false}}
//   {"id": 3, "method": "get_expression"}
//   {"id": 4, "method": "trigger_event", "params": {"event": "VICTORY_ROYALE"}}
//   {"id": 5, "method": "reset"}

'use strict';

const path = require('path');
const readline = require('readline');
const { GamerGirlPersona } = require(path.join(__dirname, '..', '..', 'Archecho', 'archecho-desk', 'lib', 'gamer-girl-persona'));

let persona = new GamerGirlPersona('Lucy');

const VALID_EVENTS = [
  'CLUTCH_MOMENT', 'VICTORY_ROYALE', 'EPIC_PLAY', 'GETTING_TILTED', 'TRASH_TALKING',
  'FLOW_STATE', 'SURPRISE_ATTACK', 'TEAM_CARRY', 'RAGE_QUIT_RESIST', 'BORED_STOMPING'
];

const handlers = {
  train_step({ event = null } = {}) {
    if (event && !VALID_EVENTS.includes(event)) {
      throw new Error(`Unknown event "${event}". Valid: ${VALID_EVENTS.join(', ')}`);
    }
    return persona.trainStep(event);
  },
  get_state({ full = false } = {}) {
    if (full) return persona.getFullState();
    const s = persona.getFullState();
    return {
      name: s.name, time: s.time, totalXP: s.totalXP,
      expression: s.expression, endocrineMode: s.endocrine.mode,
      cognitiveState: s.cognitiveState, traits: s.traits,
      killCount: s.killCount, deathCount: s.deathCount, winStreak: s.winStreak
    };
  },
  get_expression() {
    return { expression: persona.facs.getCompositeExpression(), morphTargets: persona.facs.toMorphTargets() };
  },
  trigger_event({ event } = {}) {
    if (!event || !VALID_EVENTS.includes(event)) {
      throw new Error(`event required. Valid: ${VALID_EVENTS.join(', ')}`);
    }
    return persona.trainStep(event);
  },
  list_events() { return VALID_EVENTS; },
  reset() {
    persona = new GamerGirlPersona('Lucy');
    return { reset: true, name: persona.name };
  }
};

const rl = readline.createInterface({ input: process.stdin, terminal: false });
rl.on('line', (line) => {
  let req;
  try { req = JSON.parse(line); } catch (e) {
    process.stdout.write(JSON.stringify({ id: null, error: `Invalid JSON: ${e.message}` }) + '\n');
    return;
  }
  const { id = null, method, params = {} } = req;
  const fn = handlers[method];
  if (!fn) {
    process.stdout.write(JSON.stringify({ id, error: `Unknown method "${method}". Available: ${Object.keys(handlers).join(', ')}` }) + '\n');
    return;
  }
  try {
    const result = fn(params);
    process.stdout.write(JSON.stringify({ id, result }) + '\n');
  } catch (e) {
    process.stdout.write(JSON.stringify({ id, error: e.message }) + '\n');
  }
});
