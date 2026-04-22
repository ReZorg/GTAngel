# Archecho Desk

**Deep Tree Echo Cognitive Architecture Desktop**

DTE-KSM-Evo-Autogenesis ⊗ Time-Crystal-NN Control Surface for UnrealEngineCog

## Composition

```
/dte-ksm-evo-autogenesis ( orgitcog/u9n <> UnrealEngineCog ) -> /echo ( /time-crystal-nn [ /echo ] )
```

This Electron desktop application composes:

- **DTE-KSM-Evo-Autogenesis** — Autonomous metric-driven evolution toward Autonomy Level 5
- **Time-Crystal-NN** — nn4c (9 temporal scales) and nn9c (12 hierarchy levels) oscillatory neural networks
- **Dove9 Triadic Cognitive Loop** — 12-step loop × 3 streams = Clock30 synchronization
- **KSM 12-Step Living Structure** — Alexander's 15 Properties of Living Structure × Evolution Cycle
- **UnrealEngineCog Repository Explorer** — Browse Archecho plugins and UE Source modules

## Quick Start

```bash
cd Archecho/archecho-desk
npm start
```

## Panels

| Panel | Description |
|-------|-------------|
| **Dashboard** | Overview of autonomy level, coherence, clock, crystal oscillation, and repo stats |
| **Time Crystal** | Interactive nn4c visualization with 9 temporal levels and phase-coupled dynamics |
| **Brain Model** | nn9c whole brain visualization with 12 concentric hierarchy rings |
| **Autogenesis** | DTE-KSM evolution loop with experiment tracking, safety constraints, and metric charts |
| **Dove9 Triadic** | Interactive Clock30 visualization with 3 phase-shifted streams |
| **KSM Cycle** | 12-step evolution wheel with Alexander's 15 Properties radar |
| **Explorer** | File tree browser for the UnrealEngineCog repository |
| **UE Modules** | Card grid of all Source modules and Archecho plugins |

## Architecture

```
archecho-desk/
├── main.js              Electron main process (IPC handlers, repo scanning)
├── preload.js           Secure context bridge
├── renderer/
│   ├── index.html       Main UI with 8 panel views
│   ├── styles.css       Orangey-green archmage theme
│   └── app.js           Renderer logic, canvas visualizations, state management
├── lib/
│   ├── time-crystal.js  nn4c/nn9c JavaScript implementation
│   ├── dove9-engine.js  Triadic cognitive loop engine
│   └── autogenesis.js   DTE-KSM evolution loop engine
├── data/                Persisted evolution state
└── assets/              Application assets
```

## Tech Stack

- **Electron** — Cross-platform desktop framework
- **Canvas 2D** — Real-time animated visualizations
- **Node.js IPC** — Secure main/renderer communication
- **File System** — Direct repository scanning and file reading

## License

MIT
