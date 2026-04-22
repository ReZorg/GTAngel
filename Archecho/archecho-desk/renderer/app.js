// Archecho Desk — Renderer Application
// DTE-KSM-Evo-Autogenesis ⊗ Echo(Time-Crystal-NN) Control Surface

// ─── State ──────────────────────────────────────────────────────────────────
const state = {
  repo: null,
  evolution: null,
  crystalTime: 0,
  autoRunning: false,
  dove9AutoRunning: false,
  experiments: [],
  animationFrames: {}
};

// ─── Navigation ─────────────────────────────────────────────────────────────
document.querySelectorAll('.nav-btn').forEach(btn => {
  btn.addEventListener('click', () => {
    document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
    btn.classList.add('active');
    const panelId = btn.dataset.panel;
    document.querySelectorAll('.panel').forEach(p => p.classList.remove('active'));
    document.getElementById(`panel-${panelId}`).classList.add('active');
    // Initialize panel-specific content
    if (panelId === 'crystal') initCrystalPanel();
    if (panelId === 'brain') initBrainPanel();
    if (panelId === 'dove9') initDove9Panel();
    if (panelId === 'ksm') initKSMPanel();
    if (panelId === 'repository') initRepoPanel();
    if (panelId === 'modules') initModulesPanel();
  });
});

// ─── Initialization ─────────────────────────────────────────────────────────
async function init() {
  document.getElementById('status-text').textContent = 'Scanning repository...';
  try {
    state.repo = await window.archecho.scanRepository();
    state.evolution = await window.archecho.getEvolutionState();
    updateDashboard();
    document.getElementById('status-text').textContent = 'Connected';
  } catch (e) {
    document.getElementById('status-text').textContent = 'Error: ' + e.message;
  }
}

// ─── Dashboard ──────────────────────────────────────────────────────────────
function updateDashboard() {
  if (!state.repo || !state.evolution) return;

  // Autonomy level
  const level = state.evolution.currentLevel;
  document.getElementById('autonomy-badge').textContent = `Level ${level}`;
  document.getElementById('autonomy-progress').style.width = `${(level / 5) * 100}%`;
  document.querySelectorAll('.level-marker').forEach((m, i) => {
    m.classList.toggle('active', i + 1 === level);
    m.classList.toggle('completed', i + 1 < level);
  });

  // Coherence
  const coherence = state.evolution.coherenceScore;
  document.getElementById('coherence-value').textContent = coherence.toFixed(2);
  drawCoherenceGauge(coherence);

  // Clock
  const clockStep = state.evolution.dove9 ? state.evolution.dove9.clockStep : 0;
  document.getElementById('clock-step').textContent = `${clockStep}/30`;
  drawClockCanvas(clockStep);

  // Repo stats
  document.getElementById('plugin-count').textContent = Object.keys(state.repo.archecho).length;
  document.getElementById('module-count').textContent = Object.keys(state.repo.source).length;
  document.getElementById('engine-count').textContent = state.repo.engines.length;
  document.getElementById('repo-path').textContent = state.repo.root;

  // Crystal mini visualization
  drawCrystalMini();

  // Start animation loops
  startDashboardAnimations();
}

function startDashboardAnimations() {
  if (state.animationFrames.dashboard) cancelAnimationFrame(state.animationFrames.dashboard);
  let t = 0;
  function animate() {
    t += 0.016;
    drawCrystalMini(t);
    state.animationFrames.dashboard = requestAnimationFrame(animate);
  }
  animate();
}

// ─── Coherence Gauge ────────────────────────────────────────────────────────
function drawCoherenceGauge(value) {
  const canvas = document.getElementById('coherence-gauge');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  ctx.clearRect(0, 0, w, h);

  const cx = w / 2, cy = h - 10;
  const r = 70;
  const startAngle = Math.PI;
  const endAngle = 2 * Math.PI;
  const valueAngle = startAngle + (endAngle - startAngle) * value;

  // Background arc
  ctx.beginPath();
  ctx.arc(cx, cy, r, startAngle, endAngle);
  ctx.strokeStyle = '#1e2d42';
  ctx.lineWidth = 8;
  ctx.lineCap = 'round';
  ctx.stroke();

  // Value arc
  const grad = ctx.createLinearGradient(cx - r, cy, cx + r, cy);
  grad.addColorStop(0, '#f59e0b');
  grad.addColorStop(1, '#7fdbca');
  ctx.beginPath();
  ctx.arc(cx, cy, r, startAngle, valueAngle);
  ctx.strokeStyle = grad;
  ctx.lineWidth = 8;
  ctx.lineCap = 'round';
  ctx.stroke();

  // Needle
  const nx = cx + (r - 15) * Math.cos(valueAngle);
  const ny = cy + (r - 15) * Math.sin(valueAngle);
  ctx.beginPath();
  ctx.moveTo(cx, cy);
  ctx.lineTo(nx, ny);
  ctx.strokeStyle = '#e2e8f0';
  ctx.lineWidth = 2;
  ctx.stroke();
}

// ─── Clock Canvas ───────────────────────────────────────────────────────────
function drawClockCanvas(step) {
  const canvas = document.getElementById('clock-canvas');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  const cx = w / 2, cy = h / 2, r = 80;
  ctx.clearRect(0, 0, w, h);

  // Draw 30 tick marks
  for (let i = 0; i < 30; i++) {
    const angle = (i / 30) * Math.PI * 2 - Math.PI / 2;
    const inner = i === step ? r - 20 : r - 10;
    const outer = r;
    ctx.beginPath();
    ctx.moveTo(cx + inner * Math.cos(angle), cy + inner * Math.sin(angle));
    ctx.lineTo(cx + outer * Math.cos(angle), cy + outer * Math.sin(angle));
    ctx.strokeStyle = i === step ? '#7fdbca' : i % 5 === 0 ? '#64748b' : '#1e2d42';
    ctx.lineWidth = i === step ? 3 : i % 5 === 0 ? 2 : 1;
    ctx.stroke();
  }

  // Draw three stream hands
  const streams = [
    { phase: step * 12, color: '#f59e0b', len: r - 25 },
    { phase: step * 12 + 120, color: '#7fdbca', len: r - 30 },
    { phase: step * 12 + 240, color: '#a78bfa', len: r - 35 }
  ];
  streams.forEach(s => {
    const angle = (s.phase / 360) * Math.PI * 2 - Math.PI / 2;
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(cx + s.len * Math.cos(angle), cy + s.len * Math.sin(angle));
    ctx.strokeStyle = s.color;
    ctx.lineWidth = 2;
    ctx.lineCap = 'round';
    ctx.stroke();
  });

  // Center dot
  ctx.beginPath();
  ctx.arc(cx, cy, 4, 0, Math.PI * 2);
  ctx.fillStyle = '#e2e8f0';
  ctx.fill();
}

// ─── Crystal Mini Visualization ─────────────────────────────────────────────
function drawCrystalMini(t = 0) {
  const canvas = document.getElementById('crystal-mini');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  ctx.clearRect(0, 0, w, h);

  if (!state.evolution || !state.evolution.crystalState) return;
  const levels = state.evolution.crystalState.levels;
  const colors = ['#f59e0b', '#f97316', '#ef4444', '#ec4899', '#a78bfa', '#60a5fa', '#2dd4bf', '#7fdbca', '#22c55e'];

  levels.forEach((level, i) => {
    const freq = 1 / level.period;
    const y0 = 10 + (i / levels.length) * (h - 20);
    ctx.beginPath();
    for (let x = 0; x < w; x++) {
      const phase = level.phase + t * freq * Math.PI * 2;
      const val = Math.sin((x / w) * Math.PI * 4 * (i + 1) + phase) * level.amplitude;
      const y = y0 + val * 6;
      if (x === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.strokeStyle = colors[i];
    ctx.lineWidth = 1.5;
    ctx.globalAlpha = 0.7;
    ctx.stroke();
    ctx.globalAlpha = 1;
  });
}

// ─── Time Crystal Panel ─────────────────────────────────────────────────────
function initCrystalPanel() {
  if (!state.evolution || !state.evolution.crystalState) return;
  const levels = state.evolution.crystalState.levels;
  const container = document.getElementById('crystal-levels');
  container.innerHTML = '';
  const colors = ['#f59e0b', '#f97316', '#ef4444', '#ec4899', '#a78bfa', '#60a5fa', '#2dd4bf', '#7fdbca', '#22c55e'];

  levels.forEach((level, i) => {
    const el = document.createElement('div');
    el.className = 'crystal-level';
    el.innerHTML = `
      <span style="color:${colors[i]}">L${i}</span>
      <span>${level.label}</span>
      <span>${level.period}s</span>
      <div class="crystal-level-bar">
        <div class="crystal-level-fill" style="width:${level.amplitude * 100}%; background:${colors[i]}"></div>
      </div>
    `;
    container.appendChild(el);
  });

  startCrystalAnimation();
}

function startCrystalAnimation() {
  if (state.animationFrames.crystal) cancelAnimationFrame(state.animationFrames.crystal);
  let t = 0;
  function animate() {
    t += 0.016;
    drawCrystalMain(t);
    state.animationFrames.crystal = requestAnimationFrame(animate);
  }
  animate();
}

function drawCrystalMain(t) {
  const canvas = document.getElementById('crystal-main');
  if (!canvas || !state.evolution) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  ctx.clearRect(0, 0, w, h);

  const levels = state.evolution.crystalState.levels;
  const colors = ['#f59e0b', '#f97316', '#ef4444', '#ec4899', '#a78bfa', '#60a5fa', '#2dd4bf', '#7fdbca', '#22c55e'];
  const levelHeight = h / levels.length;

  levels.forEach((level, i) => {
    const freq = 1 / level.period;
    const y0 = (i + 0.5) * levelHeight;

    // Draw level label
    ctx.fillStyle = colors[i];
    ctx.font = '10px monospace';
    ctx.fillText(`${level.label} (${level.period}s)`, 8, y0 - levelHeight * 0.3);

    // Draw waveform
    ctx.beginPath();
    for (let x = 80; x < w - 10; x++) {
      const phase = level.phase + t * freq * Math.PI * 2;
      const val = Math.sin((x / (w - 90)) * Math.PI * 6 * (i + 1) + phase) * level.amplitude;
      const y = y0 + val * (levelHeight * 0.35);
      if (x === 80) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.strokeStyle = colors[i];
    ctx.lineWidth = 2;
    ctx.globalAlpha = 0.8;
    ctx.stroke();
    ctx.globalAlpha = 1;

    // Draw coupling lines between adjacent levels
    if (i > 0) {
      const prevFreq = 1 / levels[i - 1].period;
      const coupling = Math.abs(Math.sin(t * (freq + prevFreq) * 0.5));
      ctx.beginPath();
      ctx.moveTo(70, y0 - levelHeight * 0.5);
      ctx.lineTo(70, y0);
      ctx.strokeStyle = `rgba(127, 219, 202, ${coupling * 0.3})`;
      ctx.lineWidth = 1;
      ctx.stroke();
    }
  });
}

// Crystal controls
document.getElementById('crystal-dt')?.addEventListener('input', (e) => {
  document.getElementById('crystal-dt-val').textContent = parseFloat(e.target.value).toFixed(3) + 's';
});
document.getElementById('crystal-radius')?.addEventListener('input', (e) => {
  document.getElementById('crystal-radius-val').textContent = parseFloat(e.target.value).toFixed(2);
});
document.getElementById('crystal-step-btn')?.addEventListener('click', () => {
  if (!state.evolution) return;
  const dt = parseFloat(document.getElementById('crystal-dt').value);
  state.evolution.crystalState.levels.forEach(level => {
    level.phase += dt * (1 / level.period) * Math.PI * 2;
    level.amplitude = Math.max(0.1, Math.min(1, level.amplitude + (Math.random() - 0.5) * 0.05));
  });
  window.archecho.saveEvolutionState(state.evolution);
});
document.getElementById('crystal-reset-btn')?.addEventListener('click', () => {
  if (!state.evolution) return;
  state.evolution.crystalState.levels.forEach(level => {
    level.phase = Math.random() * Math.PI * 2;
    level.amplitude = 0.5 + Math.random() * 0.5;
  });
  window.archecho.saveEvolutionState(state.evolution);
});

// ─── Brain Model Panel ──────────────────────────────────────────────────────
function initBrainPanel() {
  if (!state.evolution || !state.evolution.brainState) return;
  const regions = state.evolution.brainState.levels;
  const container = document.getElementById('brain-regions');
  container.innerHTML = '';
  const colors = ['#f59e0b', '#f97316', '#ef4444', '#ec4899', '#a78bfa', '#60a5fa', '#2dd4bf', '#7fdbca', '#22c55e', '#84cc16', '#f472b6', '#fb923c'];

  regions.forEach((region, i) => {
    const el = document.createElement('div');
    el.className = 'brain-region';
    el.innerHTML = `
      <div class="brain-region-dot" style="background:${colors[i]}"></div>
      <span class="brain-region-name">${region.name}</span>
      <span class="brain-region-val">${region.activity.toFixed(2)}</span>
    `;
    container.appendChild(el);
  });

  startBrainAnimation();
}

function startBrainAnimation() {
  if (state.animationFrames.brain) cancelAnimationFrame(state.animationFrames.brain);
  let t = 0;
  function animate() {
    t += 0.016;
    drawBrainCanvas(t);
    state.animationFrames.brain = requestAnimationFrame(animate);
  }
  animate();
}

function drawBrainCanvas(t) {
  const canvas = document.getElementById('brain-canvas');
  if (!canvas || !state.evolution) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  const cx = w / 2, cy = h / 2;
  ctx.clearRect(0, 0, w, h);

  const regions = state.evolution.brainState.levels;
  const colors = ['#f59e0b', '#f97316', '#ef4444', '#ec4899', '#a78bfa', '#60a5fa', '#2dd4bf', '#7fdbca', '#22c55e', '#84cc16', '#f472b6', '#fb923c'];
  const n = regions.length;

  // Draw concentric rings for hierarchy
  for (let i = 0; i < n; i++) {
    const r = 40 + i * 20;
    const activity = regions[i].activity;
    const pulse = Math.sin(t * (1 + i * 0.3)) * 0.1 + 0.9;

    ctx.beginPath();
    ctx.arc(cx, cy, r * pulse, 0, Math.PI * 2);
    ctx.strokeStyle = colors[i];
    ctx.lineWidth = 2 + activity * 3;
    ctx.globalAlpha = 0.3 + activity * 0.5;
    ctx.stroke();
    ctx.globalAlpha = 1;

    // Label
    const angle = (i / n) * Math.PI * 2 - Math.PI / 2;
    const lx = cx + (r + 10) * Math.cos(angle);
    const ly = cy + (r + 10) * Math.sin(angle);
    ctx.fillStyle = colors[i];
    ctx.font = '9px monospace';
    ctx.globalAlpha = 0.7;
    ctx.fillText(regions[i].name, lx - 20, ly);
    ctx.globalAlpha = 1;
  }

  // Draw connections between regions
  for (let i = 0; i < n - 1; i++) {
    const r1 = 40 + i * 20;
    const r2 = 40 + (i + 1) * 20;
    const angle = t * 0.5 + i;
    const x1 = cx + r1 * Math.cos(angle);
    const y1 = cy + r1 * Math.sin(angle);
    const x2 = cx + r2 * Math.cos(angle + 0.3);
    const y2 = cy + r2 * Math.sin(angle + 0.3);

    const coherence = regions[i].coherence;
    ctx.beginPath();
    ctx.moveTo(x1, y1);
    ctx.lineTo(x2, y2);
    ctx.strokeStyle = `rgba(127, 219, 202, ${coherence * 0.4})`;
    ctx.lineWidth = 1;
    ctx.stroke();
  }

  // Center label
  ctx.fillStyle = '#e2e8f0';
  ctx.font = 'bold 12px sans-serif';
  ctx.textAlign = 'center';
  ctx.fillText('nn9c', cx, cy - 5);
  ctx.font = '10px monospace';
  ctx.fillStyle = '#64748b';
  ctx.fillText('12 Levels', cx, cy + 10);
  ctx.textAlign = 'left';
}

// ─── Autogenesis Panel ──────────────────────────────────────────────────────
document.getElementById('auto-threshold')?.addEventListener('input', (e) => {
  document.getElementById('auto-threshold-val').textContent = parseFloat(e.target.value).toFixed(2);
});

document.getElementById('auto-start-btn')?.addEventListener('click', startAutogenesis);
document.getElementById('auto-stop-btn')?.addEventListener('click', () => { state.autoRunning = false; });

async function startAutogenesis() {
  if (state.autoRunning) return;
  state.autoRunning = true;
  document.getElementById('auto-start-btn').disabled = true;
  document.getElementById('auto-stop-btn').disabled = false;

  const maxExperiments = parseInt(document.getElementById('auto-max').value);
  const threshold = parseFloat(document.getElementById('auto-threshold').value);
  let metric = 0.5;
  const log = document.getElementById('auto-log');
  log.innerHTML = '';
  state.experiments = [];

  for (let i = 0; i < maxExperiments && state.autoRunning; i++) {
    const result = await window.archecho.runExperimentStep({
      hypothesis: `Experiment ${i + 1}: optimize temporal coherence`,
      scope: 'crystal-levels',
      metric
    });

    state.experiments.push(result);
    if (result.status === 'keep') metric = result.metric;

    const entry = document.createElement('div');
    entry.className = `log-entry ${result.status === 'keep' ? 'log-keep' : 'log-discard'}`;
    entry.innerHTML = `<span>#${i + 1} ${result.status.toUpperCase()}</span><span>m=${result.metric.toFixed(4)} c=${result.coherenceScore.toFixed(2)}</span>`;
    log.appendChild(entry);
    log.scrollTop = log.scrollHeight;

    // Update dashboard experiment log too
    const dashLog = document.getElementById('experiment-log');
    if (dashLog) {
      if (dashLog.querySelector('.log-empty')) dashLog.innerHTML = '';
      const dashEntry = entry.cloneNode(true);
      dashLog.appendChild(dashEntry);
      dashLog.scrollTop = dashLog.scrollHeight;
    }

    drawAutoChart();

    // Update evolution state
    if (state.evolution) {
      state.evolution.cycleCount++;
      if (result.status === 'keep') {
        state.evolution.coherenceScore = result.coherenceScore;
      }
      // Check for level advancement
      const keepCount = state.experiments.filter(e => e.status === 'keep').length;
      if (keepCount >= 5 && state.evolution.currentLevel < 5) {
        state.evolution.currentLevel = Math.min(5, state.evolution.currentLevel + 1);
        updateDashboard();
      }
      await window.archecho.saveEvolutionState(state.evolution);
    }

    // Safety halt
    if (result.coherenceScore < 0.15) {
      const halt = document.createElement('div');
      halt.className = 'log-entry log-discard';
      halt.innerHTML = '<span>⚠ SAFETY HALT: Coherence below 0.15</span>';
      log.appendChild(halt);
      break;
    }

    await new Promise(r => setTimeout(r, 300));
  }

  state.autoRunning = false;
  document.getElementById('auto-start-btn').disabled = false;
  document.getElementById('auto-stop-btn').disabled = true;
}

function drawAutoChart() {
  const canvas = document.getElementById('auto-chart');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  ctx.clearRect(0, 0, w, h);

  if (state.experiments.length === 0) return;

  const padding = { top: 20, right: 20, bottom: 30, left: 50 };
  const plotW = w - padding.left - padding.right;
  const plotH = h - padding.top - padding.bottom;

  // Axes
  ctx.strokeStyle = '#1e2d42';
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(padding.left, padding.top);
  ctx.lineTo(padding.left, h - padding.bottom);
  ctx.lineTo(w - padding.right, h - padding.bottom);
  ctx.stroke();

  // Labels
  ctx.fillStyle = '#64748b';
  ctx.font = '10px monospace';
  ctx.fillText('Metric', padding.left - 40, padding.top + 10);
  ctx.fillText('Experiment #', w / 2 - 30, h - 5);

  const metrics = state.experiments.map(e => e.metric);
  const minM = Math.min(...metrics) - 0.05;
  const maxM = Math.max(...metrics) + 0.05;

  // Draw metric line
  ctx.beginPath();
  state.experiments.forEach((exp, i) => {
    const x = padding.left + (i / (state.experiments.length - 1 || 1)) * plotW;
    const y = padding.top + (1 - (exp.metric - minM) / (maxM - minM || 1)) * plotH;
    if (i === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  });
  ctx.strokeStyle = '#7fdbca';
  ctx.lineWidth = 2;
  ctx.stroke();

  // Draw points
  state.experiments.forEach((exp, i) => {
    const x = padding.left + (i / (state.experiments.length - 1 || 1)) * plotW;
    const y = padding.top + (1 - (exp.metric - minM) / (maxM - minM || 1)) * plotH;
    ctx.beginPath();
    ctx.arc(x, y, 4, 0, Math.PI * 2);
    ctx.fillStyle = exp.status === 'keep' ? '#7fdbca' : '#f87171';
    ctx.fill();
  });

  // Draw coherence line
  ctx.beginPath();
  state.experiments.forEach((exp, i) => {
    const x = padding.left + (i / (state.experiments.length - 1 || 1)) * plotW;
    const y = padding.top + (1 - exp.coherenceScore) * plotH;
    if (i === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  });
  ctx.strokeStyle = '#f59e0b';
  ctx.lineWidth = 1.5;
  ctx.setLineDash([4, 4]);
  ctx.stroke();
  ctx.setLineDash([]);
}

// ─── Dove9 Panel ────────────────────────────────────────────────────────────
function initDove9Panel() {
  if (!state.evolution || !state.evolution.dove9) return;
  const terms = state.evolution.dove9.terms;
  const termList = document.getElementById('term-list');
  termList.innerHTML = '';
  terms.forEach(term => {
    const el = document.createElement('div');
    el.className = 'term-item';
    el.textContent = term;
    termList.appendChild(el);
  });
  updateDove9Display();
  startDove9Animation();
}

function updateDove9Display() {
  if (!state.evolution || !state.evolution.dove9) return;
  const d = state.evolution.dove9;
  document.getElementById('active-term').textContent = d.terms[d.clockStep % d.terms.length];

  const streams = d.streams;
  document.querySelector('#stream-primary .stream-phase').textContent = `${streams.PRIMARY.phase}°`;
  document.querySelector('#stream-secondary .stream-phase').textContent = `${streams.SECONDARY.phase}°`;
  document.querySelector('#stream-tertiary .stream-phase').textContent = `${streams.TERTIARY.phase}°`;

  // Highlight active term
  document.querySelectorAll('.term-item').forEach((el, i) => {
    el.classList.toggle('active', i === d.clockStep % d.terms.length);
  });
}

function startDove9Animation() {
  if (state.animationFrames.dove9) cancelAnimationFrame(state.animationFrames.dove9);
  let t = 0;
  function animate() {
    t += 0.016;
    drawDove9Canvas(t);
    state.animationFrames.dove9 = requestAnimationFrame(animate);
  }
  animate();
}

function drawDove9Canvas(t) {
  const canvas = document.getElementById('dove9-canvas');
  if (!canvas || !state.evolution) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  const cx = w / 2, cy = h / 2, r = 220;
  ctx.clearRect(0, 0, w, h);

  const d = state.evolution.dove9;
  const step = d.clockStep;

  // Draw outer ring with 30 segments
  for (let i = 0; i < 30; i++) {
    const a1 = (i / 30) * Math.PI * 2 - Math.PI / 2;
    const a2 = ((i + 1) / 30) * Math.PI * 2 - Math.PI / 2;
    ctx.beginPath();
    ctx.arc(cx, cy, r, a1, a2);
    ctx.arc(cx, cy, r - 20, a2, a1, true);
    ctx.closePath();
    if (i === step) {
      ctx.fillStyle = 'rgba(127, 219, 202, 0.4)';
    } else if (i % 5 === 0) {
      ctx.fillStyle = 'rgba(245, 158, 11, 0.15)';
    } else {
      ctx.fillStyle = 'rgba(30, 45, 66, 0.5)';
    }
    ctx.fill();
    ctx.strokeStyle = '#1e2d42';
    ctx.lineWidth = 1;
    ctx.stroke();
  }

  // Draw 12-step inner ring
  for (let i = 0; i < 12; i++) {
    const a1 = (i / 12) * Math.PI * 2 - Math.PI / 2;
    const a2 = ((i + 1) / 12) * Math.PI * 2 - Math.PI / 2;
    ctx.beginPath();
    ctx.arc(cx, cy, r - 30, a1, a2);
    ctx.arc(cx, cy, r - 60, a2, a1, true);
    ctx.closePath();
    const active = i === step % 12;
    ctx.fillStyle = active ? 'rgba(245, 158, 11, 0.3)' : 'rgba(17, 24, 39, 0.5)';
    ctx.fill();
    ctx.strokeStyle = '#1e2d42';
    ctx.stroke();

    // Step number
    const ma = (a1 + a2) / 2;
    const mx = cx + (r - 45) * Math.cos(ma);
    const my = cy + (r - 45) * Math.sin(ma);
    ctx.fillStyle = active ? '#f59e0b' : '#64748b';
    ctx.font = active ? 'bold 11px monospace' : '10px monospace';
    ctx.textAlign = 'center';
    ctx.fillText(i + 1, mx, my + 4);
  }

  // Draw three stream triangles
  const streamColors = ['#f59e0b', '#7fdbca', '#a78bfa'];
  const streamPhases = [0, 120, 240];
  streamPhases.forEach((phase, si) => {
    const baseAngle = ((step * 12 + phase) / 360) * Math.PI * 2 - Math.PI / 2;
    const triR = r - 80;
    const points = [];
    for (let j = 0; j < 3; j++) {
      const a = baseAngle + (j / 3) * Math.PI * 2;
      points.push([cx + triR * Math.cos(a), cy + triR * Math.sin(a)]);
    }
    ctx.beginPath();
    ctx.moveTo(points[0][0], points[0][1]);
    points.slice(1).forEach(p => ctx.lineTo(p[0], p[1]));
    ctx.closePath();
    ctx.strokeStyle = streamColors[si];
    ctx.lineWidth = 2;
    ctx.globalAlpha = 0.5 + Math.sin(t * 2 + si) * 0.2;
    ctx.stroke();
    ctx.globalAlpha = 1;
  });

  // Center text
  ctx.textAlign = 'center';
  ctx.fillStyle = '#e2e8f0';
  ctx.font = 'bold 14px sans-serif';
  ctx.fillText('Dove9', cx, cy - 10);
  ctx.font = '11px monospace';
  ctx.fillStyle = '#7fdbca';
  ctx.fillText(`Step ${step}/30`, cx, cy + 8);
  ctx.fillStyle = '#f59e0b';
  ctx.font = '10px monospace';
  ctx.fillText(d.terms[step % d.terms.length], cx, cy + 24);
  ctx.textAlign = 'left';
}

document.getElementById('dove9-step-btn')?.addEventListener('click', async () => {
  if (!state.evolution || !state.evolution.dove9) return;
  const result = await window.archecho.advanceClock(state.evolution.dove9);
  state.evolution.dove9.clockStep = result.clockStep;
  state.evolution.dove9.streams = result.streams;
  updateDove9Display();
  updateDashboard();
  await window.archecho.saveEvolutionState(state.evolution);
});

document.getElementById('dove9-auto-btn')?.addEventListener('click', () => {
  if (state.dove9AutoRunning) {
    state.dove9AutoRunning = false;
    document.getElementById('dove9-auto-btn').textContent = 'Auto-Run';
    return;
  }
  state.dove9AutoRunning = true;
  document.getElementById('dove9-auto-btn').textContent = 'Stop';
  async function autoStep() {
    if (!state.dove9AutoRunning) return;
    const result = await window.archecho.advanceClock(state.evolution.dove9);
    state.evolution.dove9.clockStep = result.clockStep;
    state.evolution.dove9.streams = result.streams;
    updateDove9Display();
    updateDashboard();
    setTimeout(autoStep, 500);
  }
  autoStep();
});

// ─── KSM Panel ──────────────────────────────────────────────────────────────
const ALEXANDER_15 = [
  'Levels of Scale', 'Strong Centers', 'Boundaries', 'Alternating Repetition',
  'Positive Space', 'Good Shape', 'Local Symmetries', 'Deep Interlock',
  'Contrast', 'Gradients', 'Roughness', 'Echoes',
  'The Void', 'Simplicity & Inner Calm', 'Not-Separateness'
];

function initKSMPanel() {
  const container = document.getElementById('properties-list');
  container.innerHTML = '';
  ALEXANDER_15.forEach((prop, i) => {
    const score = 0.5 + Math.random() * 0.5;
    const el = document.createElement('div');
    el.className = 'property-item';
    el.innerHTML = `
      <div class="property-num">${i + 1}</div>
      <span class="property-name">${prop}</span>
      <span class="property-score">${score.toFixed(2)}</span>
    `;
    container.appendChild(el);
  });
  startKSMAnimation();
}

function startKSMAnimation() {
  if (state.animationFrames.ksm) cancelAnimationFrame(state.animationFrames.ksm);
  let t = 0;
  function animate() {
    t += 0.016;
    drawKSMCanvas(t);
    state.animationFrames.ksm = requestAnimationFrame(animate);
  }
  animate();
}

function drawKSMCanvas(t) {
  const canvas = document.getElementById('ksm-canvas');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width, h = canvas.height;
  const cx = w / 2, cy = h / 2;
  ctx.clearRect(0, 0, w, h);

  // Draw 12-step cycle wheel
  const r = 220;
  const steps = [
    'Observe', 'Diagnose', 'Hypothesize', 'Design',
    'Implement', 'Test', 'Measure', 'Assess',
    'Integrate', 'Stabilize', 'Document', 'Evolve'
  ];
  const stepColors = [
    '#f59e0b', '#f97316', '#ef4444', '#ec4899',
    '#a78bfa', '#60a5fa', '#2dd4bf', '#7fdbca',
    '#22c55e', '#84cc16', '#eab308', '#f59e0b'
  ];

  for (let i = 0; i < 12; i++) {
    const a1 = (i / 12) * Math.PI * 2 - Math.PI / 2;
    const a2 = ((i + 1) / 12) * Math.PI * 2 - Math.PI / 2;
    const pulse = Math.sin(t + i * 0.5) * 0.05 + 1;

    ctx.beginPath();
    ctx.arc(cx, cy, r * pulse, a1, a2);
    ctx.arc(cx, cy, (r - 40) * pulse, a2, a1, true);
    ctx.closePath();
    ctx.fillStyle = stepColors[i] + '30';
    ctx.fill();
    ctx.strokeStyle = stepColors[i];
    ctx.lineWidth = 1.5;
    ctx.stroke();

    // Label
    const ma = (a1 + a2) / 2;
    const mx = cx + (r - 20) * Math.cos(ma);
    const my = cy + (r - 20) * Math.sin(ma);
    ctx.save();
    ctx.translate(mx, my);
    ctx.rotate(ma + Math.PI / 2);
    ctx.fillStyle = stepColors[i];
    ctx.font = '10px sans-serif';
    ctx.textAlign = 'center';
    ctx.fillText(steps[i], 0, 4);
    ctx.restore();
  }

  // Inner ring: 15 properties
  const innerR = r - 60;
  for (let i = 0; i < 15; i++) {
    const angle = (i / 15) * Math.PI * 2 - Math.PI / 2;
    const score = 0.5 + Math.sin(t * 0.5 + i) * 0.3;
    const dotR = 4 + score * 6;
    const x = cx + innerR * Math.cos(angle);
    const y = cy + innerR * Math.sin(angle);

    ctx.beginPath();
    ctx.arc(x, y, dotR, 0, Math.PI * 2);
    ctx.fillStyle = `rgba(127, 219, 202, ${0.3 + score * 0.5})`;
    ctx.fill();
    ctx.strokeStyle = '#7fdbca';
    ctx.lineWidth = 1;
    ctx.stroke();

    // Connect to center
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(x, y);
    ctx.strokeStyle = `rgba(127, 219, 202, ${score * 0.15})`;
    ctx.lineWidth = 1;
    ctx.stroke();
  }

  // Center
  ctx.beginPath();
  ctx.arc(cx, cy, 30, 0, Math.PI * 2);
  ctx.fillStyle = 'rgba(245, 158, 11, 0.15)';
  ctx.fill();
  ctx.strokeStyle = '#f59e0b';
  ctx.lineWidth = 2;
  ctx.stroke();
  ctx.fillStyle = '#e2e8f0';
  ctx.font = 'bold 11px sans-serif';
  ctx.textAlign = 'center';
  ctx.fillText('KSM', cx, cy - 3);
  ctx.font = '9px monospace';
  ctx.fillStyle = '#f59e0b';
  ctx.fillText('12-Step', cx, cy + 10);
  ctx.textAlign = 'left';
}

// ─── Repository Explorer ────────────────────────────────────────────────────
function initRepoPanel() {
  if (!state.repo) return;
  const tree = document.getElementById('repo-tree');
  tree.innerHTML = '';

  // Build tree from repo data
  function addFolder(name, contents, parent, basePath) {
    const folder = document.createElement('div');
    const item = document.createElement('div');
    item.className = 'tree-item tree-folder';
    item.textContent = `📁 ${name}`;
    item.addEventListener('click', () => {
      const children = folder.querySelector('.tree-children');
      if (children) children.style.display = children.style.display === 'none' ? 'block' : 'none';
    });
    folder.appendChild(item);

    if (Array.isArray(contents)) {
      const childDiv = document.createElement('div');
      childDiv.className = 'tree-children';
      contents.forEach(f => {
        const file = document.createElement('div');
        file.className = 'tree-item tree-file';
        file.textContent = `📄 ${f}`;
        file.addEventListener('click', async () => {
          const filePath = basePath + '/' + f;
          const content = await window.archecho.readFile(filePath);
          const preview = document.getElementById('repo-preview');
          if (content && !content.error) {
            preview.textContent = content;
            preview.classList.remove('preview-empty');
          } else {
            preview.textContent = `Error: ${content?.error || 'Could not read file'}`;
          }
        });
        childDiv.appendChild(file);
      });
      folder.appendChild(childDiv);
    }
    parent.appendChild(folder);
  }

  // Archecho plugins
  const archechoSection = document.createElement('div');
  const archechoHeader = document.createElement('div');
  archechoHeader.className = 'tree-item tree-folder';
  archechoHeader.textContent = '📁 Archecho';
  archechoSection.appendChild(archechoHeader);
  const archechoChildren = document.createElement('div');
  archechoChildren.className = 'tree-children';
  Object.entries(state.repo.archecho).forEach(([name, data]) => {
    addFolder(name, data.contents, archechoChildren, data.path);
  });
  archechoSection.appendChild(archechoChildren);
  tree.appendChild(archechoSection);

  // Source modules
  const sourceSection = document.createElement('div');
  const sourceHeader = document.createElement('div');
  sourceHeader.className = 'tree-item tree-folder';
  sourceHeader.textContent = '📁 Source';
  sourceSection.appendChild(sourceHeader);
  const sourceChildren = document.createElement('div');
  sourceChildren.className = 'tree-children';
  Object.entries(state.repo.source).forEach(([name, data]) => {
    addFolder(name, data.files, sourceChildren, data.path);
  });
  sourceSection.appendChild(sourceChildren);
  tree.appendChild(sourceSection);
}

// ─── UE Modules Panel ──────────────────────────────────────────────────────
function initModulesPanel() {
  if (!state.repo) return;
  const grid = document.getElementById('modules-grid');
  grid.innerHTML = '';

  // Source modules
  Object.entries(state.repo.source).forEach(([name, data]) => {
    const card = document.createElement('div');
    card.className = 'module-card';
    card.innerHTML = `
      <div class="module-name">${name}</div>
      <div class="module-files">
        ${data.files.map(f => `<span class="module-file">${f}</span>`).join('')}
      </div>
    `;
    grid.appendChild(card);
  });

  // Archecho plugins
  Object.entries(state.repo.archecho).forEach(([name, data]) => {
    const card = document.createElement('div');
    card.className = 'module-card';
    card.innerHTML = `
      <div class="module-name" style="color: var(--accent-orange)">${name}</div>
      <div class="module-files">
        ${data.contents.map(f => `<span class="module-file">${f}</span>`).join('')}
      </div>
    `;
    grid.appendChild(card);
  });
}

// ─── Boot ───────────────────────────────────────────────────────────────────
init();
