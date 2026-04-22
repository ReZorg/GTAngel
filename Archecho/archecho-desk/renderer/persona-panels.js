// ═══════════════════════════════════════════════════════════════════════════
// Persona Panels — Lucy Gamer Girl UI
// Panels: Persona, MetaHuman Face, Training Arena, Skill Tree
// ═══════════════════════════════════════════════════════════════════════════

const personaState = {
  bridge: null,
  trainingActive: false,
  trainingInterval: null,
  lorenzHistory: [],
  metricsHistory: [],
  selectedScenario: null
};

// ─── Bridge Tick Loop ────────────────────────────────────────────────────────

let bridgeTickRunning = false;

async function startBridgeLoop() {
  if (bridgeTickRunning) return;
  bridgeTickRunning = true;

  async function loop() {
    if (!bridgeTickRunning) return;
    try {
      personaState.bridge = await window.archecho.bridgeTick(0.016);
      updateActivePanel();
    } catch (e) { /* bridge not ready */ }
    requestAnimationFrame(loop);
  }
  loop();
}

function updateActivePanel() {
  const active = document.querySelector('.panel.active');
  if (!active || !personaState.bridge) return;
  const id = active.id;
  if (id === 'panel-persona') updatePersonaPanel();
  if (id === 'panel-face') updateFacePanel();
  if (id === 'panel-training') updateTrainingPanel();
  if (id === 'panel-skills') updateSkillsPanel();
}

// ─── Persona Panel ──────────────────────────────────────────────────────────

function initPersonaPanel() {
  startBridgeLoop();
  updatePersonaPanel();
}

function updatePersonaPanel() {
  const b = personaState.bridge;
  if (!b || !b.persona) return;
  const p = b.persona;

  // Identity
  document.getElementById('persona-name').textContent = p.name;
  document.getElementById('persona-autonomy').textContent = b.autonomy.label;
  document.getElementById('p-xp').textContent = Math.floor(p.totalXP).toLocaleString();
  document.getElementById('p-matches').textContent = p.matchesPlayed;
  document.getElementById('p-kills').textContent = p.killCount;
  document.getElementById('p-streak').textContent = p.winStreak;

  // Cognitive meters
  const cog = p.cognitiveState;
  setMeter('m-valence', (cog.valence + 1) / 2, 'v-valence', cog.valence.toFixed(2));
  setMeter('m-arousal', cog.arousal, 'v-arousal', cog.arousal.toFixed(2));
  setMeter('m-flow', cog.flowLevel, 'v-flow', cog.flowLevel.toFixed(2));
  setMeter('m-chaos', cog.chaosIntensity / 0.3, 'v-chaos', cog.chaosIntensity.toFixed(3));

  // Endocrine mode
  document.getElementById('endo-mode').textContent = p.endocrine.mode;
  document.getElementById('endo-mode').className = 'endo-mode mode-' + p.endocrine.mode.toLowerCase();

  // Hormone bars
  const endoEl = document.getElementById('endo-hormones');
  endoEl.innerHTML = '';
  const importantHormones = ['Cortisol', 'DopaminePhasic', 'DopamineTonic', 'Serotonin', 'Norepinephrine', 'Oxytocin'];
  for (const name of importantHormones) {
    const h = p.endocrine.hormones[name];
    if (!h) continue;
    const el = document.createElement('div');
    el.className = 'endo-bar';
    el.innerHTML = `<span class="endo-label">${name}</span><div class="endo-fill-bg"><div class="endo-fill" style="width:${h.value*100}%"></div></div><span class="endo-val">${h.value.toFixed(2)}</span>`;
    endoEl.appendChild(el);
  }

  // Traits radar
  drawTraitsRadar(p.traits);

  // Traits list
  const traitsList = document.getElementById('traits-list');
  traitsList.innerHTML = '';
  for (const [name, val] of Object.entries(p.traits)) {
    const el = document.createElement('div');
    el.className = 'trait-item';
    el.innerHTML = `<span class="trait-name">${name}</span><div class="trait-bar-bg"><div class="trait-bar" style="width:${val*100}%"></div></div><span class="trait-val">${val.toFixed(2)}</span>`;
    traitsList.appendChild(el);
  }

  // 4E channels
  const e4El = document.getElementById('e4-channels');
  e4El.innerHTML = '';
  for (const [dim, data] of Object.entries(p.embodied4E)) {
    const section = document.createElement('div');
    section.className = 'e4-section';
    section.innerHTML = `<div class="e4-title">${dim}</div><div class="e4-desc">${data.description}</div>`;
    const chList = document.createElement('div');
    chList.className = 'e4-list';
    for (const ch of data.channels) {
      const chEl = document.createElement('div');
      chEl.className = 'e4-channel';
      chEl.innerHTML = `<span class="e4-ch-name">${ch.name}</span><span class="e4-ch-arrow">→</span><span class="e4-ch-target">${ch.target}</span>`;
      chList.appendChild(chEl);
    }
    section.appendChild(chList);
    e4El.appendChild(section);
  }
}

function setMeter(barId, ratio, valId, text) {
  const bar = document.getElementById(barId);
  const val = document.getElementById(valId);
  if (bar) bar.style.width = Math.max(0, Math.min(100, ratio * 100)) + '%';
  if (val) val.textContent = text;
}

function drawTraitsRadar(traits) {
  const canvas = document.getElementById('traits-radar');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  const cx = w / 2, cy = h / 2, r = 150;
  ctx.clearRect(0, 0, w, h);

  const keys = Object.keys(traits);
  const n = keys.length;

  // Draw rings
  for (let ring = 1; ring <= 4; ring++) {
    ctx.beginPath();
    for (let i = 0; i <= n; i++) {
      const angle = (i / n) * Math.PI * 2 - Math.PI / 2;
      const rr = (ring / 4) * r;
      const x = cx + rr * Math.cos(angle);
      const y = cy + rr * Math.sin(angle);
      if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    }
    ctx.strokeStyle = 'rgba(30, 45, 66, 0.6)';
    ctx.lineWidth = 1;
    ctx.stroke();
  }

  // Draw spokes
  for (let i = 0; i < n; i++) {
    const angle = (i / n) * Math.PI * 2 - Math.PI / 2;
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(cx + r * Math.cos(angle), cy + r * Math.sin(angle));
    ctx.strokeStyle = 'rgba(30, 45, 66, 0.4)';
    ctx.lineWidth = 1;
    ctx.stroke();

    // Labels
    const lx = cx + (r + 18) * Math.cos(angle);
    const ly = cy + (r + 18) * Math.sin(angle);
    ctx.fillStyle = '#94a3b8';
    ctx.font = '9px monospace';
    ctx.textAlign = 'center';
    ctx.fillText(keys[i].slice(0, 8), lx, ly + 3);
  }

  // Draw filled polygon
  ctx.beginPath();
  keys.forEach((key, i) => {
    const angle = (i / n) * Math.PI * 2 - Math.PI / 2;
    const val = traits[key];
    const x = cx + r * val * Math.cos(angle);
    const y = cy + r * val * Math.sin(angle);
    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  });
  ctx.closePath();
  ctx.fillStyle = 'rgba(245, 158, 11, 0.15)';
  ctx.fill();
  ctx.strokeStyle = '#f59e0b';
  ctx.lineWidth = 2;
  ctx.stroke();

  // Draw dots
  keys.forEach((key, i) => {
    const angle = (i / n) * Math.PI * 2 - Math.PI / 2;
    const val = traits[key];
    const x = cx + r * val * Math.cos(angle);
    const y = cy + r * val * Math.sin(angle);
    ctx.beginPath();
    ctx.arc(x, y, 3, 0, Math.PI * 2);
    ctx.fillStyle = '#f59e0b';
    ctx.fill();
  });
  ctx.textAlign = 'left';
}

// ─── MetaHuman Face Panel ───────────────────────────────────────────────────

function initFacePanel() {
  startBridgeLoop();
  updateFacePanel();
}

function updateFacePanel() {
  const b = personaState.bridge;
  if (!b || !b.persona) return;
  const p = b.persona;

  // Expression badge
  document.getElementById('face-expression').textContent = p.expression;

  // AU list
  const auList = document.getElementById('au-list');
  auList.innerHTML = '';
  const auNames = {
    AU1: 'Inner Brow Raise', AU2: 'Outer Brow Raise', AU4: 'Brow Lowerer',
    AU5: 'Upper Lid Raise', AU6: 'Cheek Raise', AU7: 'Lid Tightener',
    AU9: 'Nose Wrinkle', AU10: 'Upper Lip Raise', AU12: 'Smile',
    AU14: 'Dimpler', AU15: 'Lip Corner Depress', AU17: 'Chin Raise',
    AU20: 'Lip Stretch', AU23: 'Lip Tightener', AU25: 'Lips Part',
    AU26: 'Jaw Drop', AU28: 'Lip Suck', AU43: 'Eyes Closed'
  };
  for (const [au, desc] of Object.entries(auNames)) {
    const val = p.actionUnits[au] || 0;
    if (val < 0.01) continue;
    const el = document.createElement('div');
    el.className = 'au-item';
    el.innerHTML = `<span class="au-name">${au}</span><span class="au-desc">${desc}</span><div class="au-bar-bg"><div class="au-bar" style="width:${val*100}%"></div></div><span class="au-val">${val.toFixed(2)}</span>`;
    auList.appendChild(el);
  }

  // Lorenz
  document.getElementById('lyapunov-val').textContent = p.lyapunov.toFixed(3);
  document.getElementById('chaos-intensity').textContent = p.cognitiveState.chaosIntensity.toFixed(3);
  personaState.lorenzHistory.push(p.lorenzState);
  if (personaState.lorenzHistory.length > 200) personaState.lorenzHistory = personaState.lorenzHistory.slice(-200);
  drawLorenz();

  // Aesthetics
  const aestheticsList = document.getElementById('aesthetics-list');
  aestheticsList.innerHTML = '';
  for (const [name, val] of Object.entries(p.aesthetics)) {
    const el = document.createElement('div');
    el.className = 'aesthetic-item';
    el.innerHTML = `<span>${name}</span><div class="aesthetic-bar-bg"><div class="aesthetic-bar" style="width:${val*100}%"></div></div><span>${val.toFixed(2)}</span>`;
    aestheticsList.appendChild(el);
  }

  // Morph targets
  const morphList = document.getElementById('morph-list');
  morphList.innerHTML = '';
  for (const [name, val] of Object.entries(p.morphTargets)) {
    if (val < 0.01) continue;
    const el = document.createElement('div');
    el.className = 'morph-item';
    el.innerHTML = `<span class="morph-name">${name}</span><span class="morph-val">${val.toFixed(3)}</span>`;
    morphList.appendChild(el);
  }

  // Draw face
  drawFaceCanvas(p);
}

function drawLorenz() {
  const canvas = document.getElementById('lorenz-canvas');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  ctx.clearRect(0, 0, w, h);

  if (personaState.lorenzHistory.length < 2) return;

  // Project XZ plane
  ctx.beginPath();
  personaState.lorenzHistory.forEach((p, i) => {
    const x = (p.x / 30 + 0.5) * w;
    const y = (1 - p.z / 50) * h;
    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  });
  ctx.strokeStyle = '#f59e0b';
  ctx.lineWidth = 1;
  ctx.globalAlpha = 0.7;
  ctx.stroke();
  ctx.globalAlpha = 1;

  // Current point
  const last = personaState.lorenzHistory[personaState.lorenzHistory.length - 1];
  const lx = (last.x / 30 + 0.5) * w;
  const ly = (1 - last.z / 50) * h;
  ctx.beginPath();
  ctx.arc(lx, ly, 4, 0, Math.PI * 2);
  ctx.fillStyle = '#7fdbca';
  ctx.fill();
}

function drawFaceCanvas(p) {
  const canvas = document.getElementById('face-canvas');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  ctx.clearRect(0, 0, w, h);

  const au = p.actionUnits;
  const cx = w / 2, cy = h / 2 - 20;

  // Face outline
  ctx.beginPath();
  ctx.ellipse(cx, cy, 130, 170, 0, 0, Math.PI * 2);
  ctx.strokeStyle = '#2d4a6f';
  ctx.lineWidth = 2;
  ctx.stroke();

  // Eyes
  const eyeY = cy - 30;
  const eyeSpacing = 55;
  const lidOpen = 1 - (au.AU43 || 0);
  const squint = au.AU7 || 0;
  const upperLid = au.AU5 || 0;

  for (const side of [-1, 1]) {
    const ex = cx + side * eyeSpacing;
    // Eye shape
    const eyeH = 12 * lidOpen * (1 - squint * 0.5) + upperLid * 4;
    ctx.beginPath();
    ctx.ellipse(ex, eyeY, 18, Math.max(2, eyeH), 0, 0, Math.PI * 2);
    ctx.fillStyle = '#1a2332';
    ctx.fill();
    ctx.strokeStyle = '#7fdbca';
    ctx.lineWidth = 1.5;
    ctx.stroke();

    // Iris
    if (eyeH > 3) {
      ctx.beginPath();
      ctx.arc(ex, eyeY, 6, 0, Math.PI * 2);
      ctx.fillStyle = '#60a5fa';
      ctx.fill();
      // Sparkle
      const sparkle = p.aesthetics.EyeSparkle || 0;
      if (sparkle > 0.3) {
        ctx.beginPath();
        ctx.arc(ex + 2, eyeY - 2, 2 * sparkle, 0, Math.PI * 2);
        ctx.fillStyle = `rgba(255, 255, 255, ${sparkle})`;
        ctx.fill();
      }
    }
  }

  // Eyebrows
  const browY = eyeY - 25;
  const browInner = au.AU1 || 0;
  const browOuter = au.AU2 || 0;
  const browDown = au.AU4 || 0;

  for (const side of [-1, 1]) {
    const bx = cx + side * eyeSpacing;
    ctx.beginPath();
    ctx.moveTo(bx - side * 25, browY + browDown * 8 - browOuter * 6);
    ctx.quadraticCurveTo(bx, browY - 8 - browInner * 8 + browDown * 10, bx + side * 25, browY + browDown * 5);
    ctx.strokeStyle = '#e2e8f0';
    ctx.lineWidth = 2.5;
    ctx.stroke();
  }

  // Nose
  const noseWrinkle = au.AU9 || 0;
  ctx.beginPath();
  ctx.moveTo(cx, cy - 5);
  ctx.lineTo(cx - 10, cy + 20);
  ctx.lineTo(cx + 10, cy + 20);
  ctx.strokeStyle = `rgba(127, 219, 202, ${0.3 + noseWrinkle * 0.5})`;
  ctx.lineWidth = 1.5;
  ctx.stroke();

  // Mouth
  const mouthY = cy + 55;
  const smile = au.AU12 || 0;
  const depress = au.AU15 || 0;
  const lipsPart = au.AU25 || 0;
  const jawOpen = au.AU26 || 0;
  const stretch = au.AU20 || 0;

  const mouthWidth = 35 + stretch * 15;
  const cornerY = mouthY + (depress - smile) * 15;

  // Upper lip
  ctx.beginPath();
  ctx.moveTo(cx - mouthWidth, cornerY);
  ctx.quadraticCurveTo(cx, mouthY - 8 - smile * 5, cx + mouthWidth, cornerY);
  ctx.strokeStyle = '#f472b6';
  ctx.lineWidth = 2;
  ctx.stroke();

  // Lower lip / jaw
  if (lipsPart > 0.1 || jawOpen > 0.1) {
    const openAmount = Math.max(lipsPart, jawOpen) * 15;
    ctx.beginPath();
    ctx.moveTo(cx - mouthWidth, cornerY);
    ctx.quadraticCurveTo(cx, mouthY + openAmount, cx + mouthWidth, cornerY);
    ctx.strokeStyle = '#f472b6';
    ctx.lineWidth = 1.5;
    ctx.stroke();

    // Mouth fill
    ctx.beginPath();
    ctx.moveTo(cx - mouthWidth, cornerY);
    ctx.quadraticCurveTo(cx, mouthY - 8 - smile * 5, cx + mouthWidth, cornerY);
    ctx.quadraticCurveTo(cx, mouthY + openAmount, cx - mouthWidth, cornerY);
    ctx.fillStyle = 'rgba(30, 15, 20, 0.8)';
    ctx.fill();
  }

  // Chin
  const chinRaise = au.AU17 || 0;
  if (chinRaise > 0.1) {
    ctx.beginPath();
    ctx.arc(cx, cy + 90, 15, 0, Math.PI);
    ctx.strokeStyle = `rgba(226, 232, 240, ${chinRaise * 0.3})`;
    ctx.lineWidth = 1;
    ctx.stroke();
  }

  // Expression label
  ctx.fillStyle = '#f59e0b';
  ctx.font = 'bold 14px sans-serif';
  ctx.textAlign = 'center';
  ctx.fillText(p.expression, cx, h - 30);

  // Glow effect (EmissiveGlow)
  const glow = p.aesthetics.EmissiveGlow || 0;
  if (glow > 0.05) {
    ctx.beginPath();
    ctx.ellipse(cx, cy, 135, 175, 0, 0, Math.PI * 2);
    ctx.strokeStyle = `rgba(245, 158, 11, ${glow * 0.3})`;
    ctx.lineWidth = 3;
    ctx.stroke();
  }

  ctx.textAlign = 'left';
}

// ─── Training Arena Panel ───────────────────────────────────────────────────

async function initTrainingPanel() {
  startBridgeLoop();

  // Load scenarios
  const scenarios = await window.archecho.bridgeScenarios();
  const grid = document.getElementById('scenario-grid');
  grid.innerHTML = '';
  for (const [key, sc] of Object.entries(scenarios)) {
    const el = document.createElement('div');
    el.className = 'scenario-card' + (personaState.selectedScenario === key ? ' selected' : '');
    el.innerHTML = `<div class="sc-name">${sc.name}</div><div class="sc-desc">${sc.description}</div><div class="sc-meta">Duration: ${sc.duration} | Chaos: ${sc.chaosMultiplier}x</div>`;
    el.addEventListener('click', () => {
      document.querySelectorAll('.scenario-card').forEach(c => c.classList.remove('selected'));
      el.classList.add('selected');
      personaState.selectedScenario = key;
    });
    grid.appendChild(el);
  }

  // Event buttons
  const events = ['CLUTCH_MOMENT', 'VICTORY_ROYALE', 'EPIC_PLAY', 'GETTING_TILTED', 'TRASH_TALKING', 'FLOW_STATE', 'SURPRISE_ATTACK', 'TEAM_CARRY', 'RAGE_QUIT_RESIST', 'BORED_STOMPING'];
  const evtEl = document.getElementById('event-buttons');
  evtEl.innerHTML = '';
  for (const evt of events) {
    const btn = document.createElement('button');
    btn.className = 'btn btn-sm';
    btn.textContent = evt.replace(/_/g, ' ');
    btn.addEventListener('click', async () => {
      await window.archecho.bridgeTriggerEvent(evt);
      addTrainingLogEntry(evt);
    });
    evtEl.appendChild(btn);
  }

  // Speed control
  document.getElementById('train-speed').addEventListener('input', (e) => {
    const speed = parseInt(e.target.value);
    document.getElementById('train-speed-val').textContent = speed + 'x';
    window.archecho.bridgeSetSpeed(speed);
  });

  // Start/stop
  document.getElementById('train-start-btn').addEventListener('click', async () => {
    if (!personaState.selectedScenario) {
      personaState.selectedScenario = 'RANKED_MATCH';
      document.querySelectorAll('.scenario-card')[1]?.classList.add('selected');
    }
    await window.archecho.bridgeStartTraining(personaState.selectedScenario);
    personaState.trainingActive = true;
    document.getElementById('train-start-btn').disabled = true;
    document.getElementById('train-stop-btn').disabled = false;
  });

  document.getElementById('train-stop-btn').addEventListener('click', async () => {
    await window.archecho.bridgeStopTraining();
    personaState.trainingActive = false;
    document.getElementById('train-start-btn').disabled = false;
    document.getElementById('train-stop-btn').disabled = true;
  });

  // Save
  document.getElementById('train-save-btn').addEventListener('click', async () => {
    await window.archecho.bridgeSavePersona();
    addTrainingLogEntry('PERSONA SAVED');
  });
}

function updateTrainingPanel() {
  const b = personaState.bridge;
  if (!b) return;

  // Record metrics
  if (b.metrics && b.metrics.length > 0) {
    personaState.metricsHistory = b.metrics;
  }

  drawTrainingChart();

  // Update training log with latest event
  if (b.persona && b.persona.cognitiveState) {
    const log = document.getElementById('training-log');
    if (log && log.children.length > 50) {
      log.removeChild(log.firstChild);
    }
  }
}

function addTrainingLogEntry(text) {
  const log = document.getElementById('training-log');
  if (!log) return;
  const entry = document.createElement('div');
  entry.className = 'train-log-entry';
  const b = personaState.bridge;
  const mode = b ? b.persona.endocrine.mode : '?';
  const expr = b ? b.persona.expression : '?';
  entry.innerHTML = `<span class="tl-event">${text}</span><span class="tl-mode">${mode}</span><span class="tl-expr">${expr}</span>`;
  log.appendChild(entry);
  log.scrollTop = log.scrollHeight;
}

function drawTrainingChart() {
  const canvas = document.getElementById('training-chart');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  ctx.clearRect(0, 0, w, h);

  const data = personaState.metricsHistory;
  if (!data || data.length < 2) return;

  const pad = { top: 20, right: 20, bottom: 25, left: 45 };
  const pw = w - pad.left - pad.right;
  const ph = h - pad.top - pad.bottom;

  // Axes
  ctx.strokeStyle = '#1e2d42';
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(pad.left, pad.top);
  ctx.lineTo(pad.left, h - pad.bottom);
  ctx.lineTo(w - pad.right, h - pad.bottom);
  ctx.stroke();

  // Draw lines for valence, arousal, flow
  const lines = [
    { key: 'valence', color: '#f59e0b', transform: v => (v + 1) / 2 },
    { key: 'arousal', color: '#7fdbca', transform: v => v },
    { key: 'flow', color: '#a78bfa', transform: v => v },
    { key: 'confidence', color: '#f472b6', transform: v => v }
  ];

  for (const line of lines) {
    ctx.beginPath();
    data.forEach((d, i) => {
      const x = pad.left + (i / (data.length - 1)) * pw;
      const y = pad.top + (1 - line.transform(d[line.key] || 0)) * ph;
      if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    });
    ctx.strokeStyle = line.color;
    ctx.lineWidth = 1.5;
    ctx.stroke();
  }

  // Legend
  ctx.font = '10px monospace';
  lines.forEach((line, i) => {
    ctx.fillStyle = line.color;
    ctx.fillText(line.key, pad.left + 10 + i * 80, pad.top + 12);
  });

  // XP line (scaled)
  const maxXP = Math.max(...data.map(d => d.totalXP || 0), 1);
  ctx.beginPath();
  data.forEach((d, i) => {
    const x = pad.left + (i / (data.length - 1)) * pw;
    const y = pad.top + (1 - (d.totalXP || 0) / maxXP) * ph;
    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  });
  ctx.strokeStyle = '#22c55e';
  ctx.lineWidth = 2;
  ctx.setLineDash([4, 4]);
  ctx.stroke();
  ctx.setLineDash([]);
  ctx.fillStyle = '#22c55e';
  ctx.fillText('XP', pad.left + 10 + lines.length * 80, pad.top + 12);
}

// ─── Skill Tree Panel ───────────────────────────────────────────────────────

function initSkillsPanel() {
  startBridgeLoop();
  updateSkillsPanel();
}

function updateSkillsPanel() {
  const b = personaState.bridge;
  if (!b || !b.persona) return;

  const domains = b.persona.skills;
  const container = document.getElementById('skill-domains');
  container.innerHTML = '';

  const domainColors = {
    FPS: '#ef4444', MOBA: '#60a5fa', Fighting: '#f59e0b', Survival: '#22c55e', Meta: '#a78bfa'
  };

  for (const [key, domain] of Object.entries(domains)) {
    const card = document.createElement('div');
    card.className = 'skill-domain-card';

    const totalLevel = Object.values(domain.skills).reduce((s, sk) => s + sk.level, 0);
    const maxTotal = Object.values(domain.skills).length * 100;
    const pct = (totalLevel / maxTotal * 100).toFixed(1);

    card.innerHTML = `
      <div class="sd-header" style="border-color:${domainColors[key] || '#7fdbca'}">
        <h3>${domain.name}</h3>
        <span class="sd-pct">${pct}%</span>
      </div>
      <div class="sd-progress"><div class="sd-bar" style="width:${pct}%;background:${domainColors[key] || '#7fdbca'}"></div></div>
    `;

    const skillList = document.createElement('div');
    skillList.className = 'sd-skills';

    for (const [skillName, skill] of Object.entries(domain.skills)) {
      const skillEl = document.createElement('div');
      skillEl.className = 'sd-skill';
      const levelPct = (skill.level / skill.maxLevel * 100).toFixed(0);
      const xpPct = skill.xpPerLevel > 0 ? (skill.xp / (skill.xpPerLevel * (skill.level + 1)) * 100).toFixed(0) : 0;
      skillEl.innerHTML = `
        <div class="sdk-top">
          <span class="sdk-name">${skillName}</span>
          <span class="sdk-level">Lv.${skill.level}</span>
        </div>
        <div class="sdk-bars">
          <div class="sdk-bar-bg"><div class="sdk-bar" style="width:${levelPct}%;background:${domainColors[key] || '#7fdbca'}"></div></div>
          <div class="sdk-xp-bg"><div class="sdk-xp" style="width:${xpPct}%"></div></div>
        </div>
      `;
      skillList.appendChild(skillEl);
    }

    card.appendChild(skillList);
    container.appendChild(card);
  }
}

// ─── Panel Navigation Hook ──────────────────────────────────────────────────

document.querySelectorAll('.nav-btn').forEach(btn => {
  btn.addEventListener('click', () => {
    const panelId = btn.dataset.panel;
    if (panelId === 'persona') initPersonaPanel();
    if (panelId === 'face') initFacePanel();
    if (panelId === 'training') initTrainingPanel();
    if (panelId === 'skills') initSkillsPanel();
  });
});
