// Archecho Desk — Main Process
// DTE-KSM-Evo-Autogenesis ⊗ Echo(Time-Crystal-NN) Control Surface
// Composition: /dte-ksm-evo-autogenesis ( UnrealEngineCog ) -> /echo ( /time-crystal-nn [ /echo ] )

const { app, BrowserWindow, ipcMain, dialog } = require('electron');
const path = require('path');
const fs = require('fs');
const { EmbodiedCognitionBridge, TRAINING_SCENARIOS } = require('./lib/embodied-bridge');

// Global embodied cognition bridge instance
let bridge = null;

// Resolve the UnrealEngineCog root relative to this app
const UE_COG_ROOT = path.resolve(__dirname, '..', '..');
const ARCHECHO_ROOT = path.resolve(__dirname, '..');

let mainWindow = null;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1440,
    height: 900,
    minWidth: 1024,
    minHeight: 700,
    title: 'Archecho Desk — Deep Tree Echo Cognitive Architecture',
    backgroundColor: '#0a0e17',
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
      sandbox: false
    },
    frame: false,
    titleBarStyle: 'hidden',
    titleBarOverlay: {
      color: '#0a0e17',
      symbolColor: '#7fdbca',
      height: 36
    }
  });

  mainWindow.loadFile(path.join(__dirname, 'renderer', 'index.html'));

  if (process.argv.includes('--dev')) {
    mainWindow.webContents.openDevTools({ mode: 'detach' });
  }

  mainWindow.on('closed', () => { mainWindow = null; });
}

app.whenReady().then(() => {
  initBridge();
  createWindow();
});
app.on('window-all-closed', () => { if (process.platform !== 'darwin') app.quit(); });
app.on('activate', () => { if (BrowserWindow.getAllWindows().length === 0) createWindow(); });

// ─── Embodied Bridge Lifecycle ─────────────────────────────────────────────

function initBridge() {
  bridge = new EmbodiedCognitionBridge();
  console.log('[Archecho] Embodied Cognition Bridge initialized');
  console.log('[Archecho] Persona:', bridge.persona.name);
  console.log('[Archecho] Scenarios:', Object.keys(TRAINING_SCENARIOS).join(', '));
}

// ─── IPC Handlers ───────────────────────────────────────────────────────────

// Scan the UnrealEngineCog repository structure
ipcMain.handle('scan-repository', async () => {
  const result = {
    root: UE_COG_ROOT,
    archecho: {},
    source: {},
    engines: [],
    figures: []
  };

  // Scan Archecho plugins
  const archechoDir = path.join(ARCHECHO_ROOT);
  try {
    const entries = fs.readdirSync(archechoDir, { withFileTypes: true });
    for (const entry of entries) {
      if (entry.isDirectory() && entry.name.startsWith('unreal-')) {
        const pluginPath = path.join(archechoDir, entry.name);
        const readme = path.join(pluginPath, 'README.md');
        result.archecho[entry.name] = {
          path: pluginPath,
          hasReadme: fs.existsSync(readme),
          contents: fs.readdirSync(pluginPath).filter(f => !f.startsWith('.'))
        };
      }
    }
  } catch (e) { /* directory may not exist */ }

  // Scan Source modules
  const sourceDir = path.join(UE_COG_ROOT, 'Source');
  try {
    const entries = fs.readdirSync(sourceDir, { withFileTypes: true });
    for (const entry of entries) {
      if (entry.isDirectory()) {
        const modPath = path.join(sourceDir, entry.name);
        result.source[entry.name] = {
          path: modPath,
          files: fs.readdirSync(modPath).filter(f => !f.startsWith('.'))
        };
      }
    }
  } catch (e) { /* directory may not exist */ }

  // Scan engines
  const enginesDir = path.join(ARCHECHO_ROOT, 'unreal-echo', 'engines');
  try {
    result.engines = fs.readdirSync(enginesDir).filter(f => f.endsWith('.py'));
  } catch (e) { /* */ }

  // Scan figures
  const figuresDir = path.join(ARCHECHO_ROOT, 'unreal-echo', 'figures');
  try {
    const figDirs = fs.readdirSync(figuresDir, { withFileTypes: true });
    for (const d of figDirs) {
      if (d.isDirectory()) {
        const figs = fs.readdirSync(path.join(figuresDir, d.name)).filter(f => f.endsWith('.png') || f.endsWith('.svg'));
        result.figures.push({ group: d.name, files: figs });
      }
    }
  } catch (e) { /* */ }

  return result;
});

// Read a file from the repository
ipcMain.handle('read-file', async (event, filePath) => {
  try {
    const resolved = path.resolve(filePath);
    // Security: only allow reading within UE_COG_ROOT
    if (!resolved.startsWith(UE_COG_ROOT)) {
      throw new Error('Access denied: path outside repository');
    }
    return fs.readFileSync(resolved, 'utf-8');
  } catch (e) {
    return { error: e.message };
  }
});

// Get the autogenesis evolution state
ipcMain.handle('get-evolution-state', async () => {
  const statePath = path.join(__dirname, 'data', 'evolution-state.json');
  try {
    if (fs.existsSync(statePath)) {
      return JSON.parse(fs.readFileSync(statePath, 'utf-8'));
    }
  } catch (e) { /* */ }
  return {
    currentLevel: 1,
    targetLevel: 5,
    experiments: [],
    coherenceScore: 1.0,
    cycleCount: 0,
    timestamp: new Date().toISOString(),
    crystalState: {
      levels: Array.from({ length: 9 }, (_, i) => ({
        id: i,
        period: [0.008, 0.026, 0.052, 0.11, 0.16, 0.25, 0.33, 0.5, 1.0][i],
        phase: Math.random() * Math.PI * 2,
        amplitude: 0.5 + Math.random() * 0.5,
        label: ['Protein', 'Ion-Channel', 'Membrane', 'AIS', 'Dendritic', 'Synaptic', 'Soma', 'Network', 'Global'][i]
      }))
    },
    brainState: {
      levels: Array.from({ length: 12 }, (_, i) => ({
        id: i,
        name: ['Microtubule', 'Neuron', 'CorticalBranch', 'CortexDomain', 'Cerebellum', 'Hypothalamus', 'Hippocampus', 'ThalamicBody', 'SkinNerveNet', 'CranialNerve', 'ThoracicNerve', 'BloodVessel'][i],
        activity: Math.random(),
        coherence: 0.5 + Math.random() * 0.5
      }))
    },
    dove9: {
      streams: {
        PRIMARY: { phase: 0, active: true },
        SECONDARY: { phase: 120, active: true },
        TERTIARY: { phase: 240, active: true }
      },
      clockStep: 0,
      terms: ['T1_PERCEPTION', 'T2_IDEA_FORMATION', 'T4_SENSORY_INPUT', 'T5_ACTION_SEQUENCE', 'T7_MEMORY_ENCODING', 'T8_BALANCED_RESPONSE']
    }
  };
});

// Save evolution state
ipcMain.handle('save-evolution-state', async (event, state) => {
  const dataDir = path.join(__dirname, 'data');
  if (!fs.existsSync(dataDir)) fs.mkdirSync(dataDir, { recursive: true });
  const statePath = path.join(dataDir, 'evolution-state.json');
  fs.writeFileSync(statePath, JSON.stringify(state, null, 2));
  return { success: true };
});

// Run an autogenesis experiment step
ipcMain.handle('run-experiment-step', async (event, params) => {
  const { hypothesis, scope, metric } = params;
  // Simulate an experiment step (in production, this would execute real commands)
  const result = {
    id: Date.now(),
    hypothesis,
    metric: metric + (Math.random() - 0.4) * 0.1,
    coherenceScore: Math.max(0, Math.min(1, 0.7 + Math.random() * 0.3)),
    status: 'keep',
    timestamp: new Date().toISOString()
  };
  if (result.metric < metric) result.status = 'discard';
  if (result.coherenceScore < 0.6) result.status = 'discard';
  return result;
});

// Advance the Dove9 triadic clock
ipcMain.handle('advance-clock', async (event, currentState) => {
  const step = (currentState.clockStep + 1) % 30;
  const phase = (step * 12) % 360;
  const activeTermIndex = step % currentState.terms.length;
  return {
    clockStep: step,
    phase,
    activeTerm: currentState.terms[activeTermIndex],
    streams: {
      PRIMARY: { phase: phase % 360, active: true },
      SECONDARY: { phase: (phase + 120) % 360, active: true },
      TERTIARY: { phase: (phase + 240) % 360, active: true }
    }
  };
});

// ─── Embodied Cognition Bridge IPC ─────────────────────────────────────────

// Tick the bridge (called from renderer animation loop)
ipcMain.handle('bridge-tick', async (event, dt) => {
  if (!bridge) return null;
  return bridge.tick(dt || 0.016);
});

// Get full unified state
ipcMain.handle('bridge-state', async () => {
  if (!bridge) return null;
  return bridge.getUnifiedState();
});

// Start training scenario
ipcMain.handle('bridge-start-training', async (event, scenarioKey) => {
  if (!bridge) return false;
  return bridge.startTraining(scenarioKey);
});

// Stop training
ipcMain.handle('bridge-stop-training', async () => {
  if (!bridge) return;
  bridge.stopTraining();
});

// Set training speed
ipcMain.handle('bridge-set-speed', async (event, speed) => {
  if (!bridge) return;
  bridge.setTrainingSpeed(speed);
});

// Trigger a specific gameplay event
ipcMain.handle('bridge-trigger-event', async (event, eventName) => {
  if (!bridge) return null;
  bridge.triggerEvent(eventName);
  return bridge.getUnifiedState();
});

// Get available training scenarios
ipcMain.handle('bridge-scenarios', async () => {
  return TRAINING_SCENARIOS;
});

// Save persona state to disk
ipcMain.handle('bridge-save-persona', async () => {
  if (!bridge) return { success: false };
  const dataDir = path.join(__dirname, 'data');
  if (!fs.existsSync(dataDir)) fs.mkdirSync(dataDir, { recursive: true });
  const personaPath = path.join(dataDir, 'persona-state.json');
  fs.writeFileSync(personaPath, JSON.stringify(bridge.persona.getFullState(), null, 2));
  return { success: true };
});

// Load persona state from disk
ipcMain.handle('bridge-load-persona', async () => {
  const personaPath = path.join(__dirname, 'data', 'persona-state.json');
  try {
    if (fs.existsSync(personaPath)) {
      return JSON.parse(fs.readFileSync(personaPath, 'utf-8'));
    }
  } catch (e) { /* */ }
  return null;
});
