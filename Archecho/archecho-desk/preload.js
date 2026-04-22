// Archecho Desk — Preload Script
// Secure bridge between renderer and main process

const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('archecho', {
  // Repository scanning
  scanRepository: () => ipcRenderer.invoke('scan-repository'),
  readFile: (path) => ipcRenderer.invoke('read-file', path),

  // Evolution state management
  getEvolutionState: () => ipcRenderer.invoke('get-evolution-state'),
  saveEvolutionState: (state) => ipcRenderer.invoke('save-evolution-state', state),

  // Autogenesis experiment loop
  runExperimentStep: (params) => ipcRenderer.invoke('run-experiment-step', params),

  // Dove9 triadic clock
  advanceClock: (state) => ipcRenderer.invoke('advance-clock', state),

  // Embodied Cognition Bridge
  bridgeTick: (dt) => ipcRenderer.invoke('bridge-tick', dt),
  bridgeState: () => ipcRenderer.invoke('bridge-state'),
  bridgeStartTraining: (scenario) => ipcRenderer.invoke('bridge-start-training', scenario),
  bridgeStopTraining: () => ipcRenderer.invoke('bridge-stop-training'),
  bridgeSetSpeed: (speed) => ipcRenderer.invoke('bridge-set-speed', speed),
  bridgeTriggerEvent: (event) => ipcRenderer.invoke('bridge-trigger-event', event),
  bridgeScenarios: () => ipcRenderer.invoke('bridge-scenarios'),
  bridgeSavePersona: () => ipcRenderer.invoke('bridge-save-persona'),
  bridgeLoadPersona: () => ipcRenderer.invoke('bridge-load-persona'),
});
