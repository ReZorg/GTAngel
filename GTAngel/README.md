# AngelClaw Game World — Deep Tree Echo Training Environment

A WPF desktop application built with .NET 10 that integrates **GTA3 Definitive Edition** game assets with the **Deep Tree Echo** cognitive training framework, implementing the **DTE-KSM-Evo-Autogenesis** evolution loop for game-world-based cognitive architecture training.

## Overview

This application serves as the training environment for the AngelClaw cognitive architecture, using GTA3DE game world assets (maps, textures, characters, vehicles, weapons, environments) as the sensory substrate for a 3-layer Echo State Network (ESN) reservoir computing system. The system evolves through Alexander's 15 Properties of Living Structure, assessed via the KSM 12-step evolution cycle, driving the architecture from reactive behavior toward full autogenesis (Autonomy Level 5).

## Architecture

The application follows the MVVM pattern with CommunityToolkit.Mvvm and consists of three main tabs:

### Tab 1: Game World Assets
- **Asset Scanner**: Reads the `GTA3DE.Assets.zip` archive (13,757 files) and catalogs all game assets by category
- **Category Breakdown**: Maps, Textures, Blueprints, Audio, Characters, Vehicles, Weapons, Environments, UI, Config
- **Search & Filter**: Real-time search across asset names with category filtering
- **Distribution Chart**: Visual pie chart of asset categories using LiveCharts2

### Tab 2: DTE Training Dashboard
- **Training Controls**: Start/Stop/Reset training with configurable parameters (spectral radius, leaking rate, max experiments)
- **Reward Progression Chart**: Real-time line chart of episode rewards
- **ESN Reservoir Activation**: 3-layer (Sensory 128 / Cognitive 256 / Executive 512) reservoir state visualization
- **Experiment Log**: DataGrid of autogenesis experiments with hypothesis, result, and keep/discard decisions
- **Cognitive State Panel**: Real-time display of 4E cognition dimensions (Embodied, Embedded, Enacted, Extended), valence/arousal, and wisdom level

### Tab 3: KSM Evolution
- **12-Step Cycle Tracker**: Visual progress through the KSM evolution steps (Perception → Integration → Expression → Regeneration)
- **Alexander's 15 Properties**: Scored assessment of living structure properties (Levels of Scale, Strong Centers, Boundaries, Alternating Repetition, Positive Space, Good Shape, Local Symmetries, Deep Interlock, Contrast, Gradients, Roughness, Echoes, The Void, Simplicity, Not-Separateness)
- **Stream Coherence Chart**: Temporal coherence tracking across training episodes
- **Autonomy Level Progress**: Visual indicator of current autonomy level (0-5)

## Technical Stack

| Component | Technology |
|-----------|-----------|
| Framework | .NET 10 Preview, WPF |
| MVVM | CommunityToolkit.Mvvm 8.4.2 |
| Charts | LiveChartsCore.SkiaSharpView.WPF 2.0.0 |
| JSON | Newtonsoft.Json 13.0.4 |
| Pattern | MVVM with ObservableProperty source generators |
| Theme | Custom dark theme (Navy/Cyan/Orange accent palette) |

## Project Structure

```
AngelClaw.GameWorld/
├── AngelClaw.GameWorld.csproj    # Project file with NuGet references
├── App.xaml                       # Application resources and dark theme
├── App.xaml.cs                    # Application entry point
├── MainWindow.xaml                # Full UI layout (3 tabs, 680+ lines XAML)
├── MainWindow.xaml.cs             # Code-behind (minimal)
├── Models/
│   ├── GameWorldAsset.cs          # Asset catalog models (GameWorldAsset, AssetCategory, MapData)
│   ├── CognitiveState.cs          # DTE cognitive state (4E cognition, valence/arousal)
│   ├── EchoStateNetwork.cs        # 3-layer ESN reservoir (128/256/512 neurons)
│   └── TrainingModels.cs          # Training episodes, autogenesis experiments, Alexander's 15 properties
├── Services/
│   ├── AssetCatalogService.cs     # ZIP archive scanner and asset categorizer
│   └── TrainingEngine.cs          # DTE training engine with ESN, autogenesis loop, KSM cycle
├── ViewModels/
│   └── MainViewModel.cs           # Central ViewModel with all bindings and commands
└── Converters/
    └── ValueConverters.cs         # Bool-to-Visibility, Score-to-Color, InverseBool converters
```

## How to Build and Run

```powershell
cd E:\u9n\angelclaw\AngelClaw.GameWorld
dotnet build
dotnet run
```

## How to Use

1. **Launch the application** — it auto-detects `E:\u9n\angelclaw\GTA3DE.Assets.zip`
2. **Tab 1 (Game World Assets)**: Click "Scan Assets" to catalog all 13,757 GTA3DE assets
3. **Tab 2 (DTE Training)**: Configure parameters and click "Start Training" to begin the autogenesis loop
4. **Tab 3 (KSM Evolution)**: Monitor the 12-step evolution cycle and Alexander's 15 property scores

## Integration with AngelClaw Repository

This application connects to the [rzonedevops/angelclaw](https://github.com/rzonedevops/angelclaw) repository's GameTraining module architecture:

- **GameTrainingEnvironment**: Maps to the `TrainingEngine` service which manages the ESN reservoir and cognitive state
- **DeepTreeEcho**: The ESN reservoir implements the 3-layer architecture (Sensory/Cognitive/Executive) from the DTE framework
- **Autogenesis Loop**: Implements the hypothesis-test-evaluate-evolve cycle from the dte-ksm-evo-autogenesis skill
- **KSM 12-Step Cycle**: Maps the Knowledge Sharing Mechanism evolution steps to game-world training episodes
- **Alexander's 15 Properties**: Scores the living structure coherence of the evolved cognitive architecture

## Autonomy Levels

| Level | Name | Description |
|-------|------|-------------|
| 0 | Reactive | Simple stimulus-response |
| 1 | Adaptive | Pattern recognition and basic learning |
| 2 | Deliberative | Planning and goal-directed behavior |
| 3 | Reflective | Self-monitoring and meta-cognition |
| 4 | Autonomous | Self-directed learning and exploration |
| 5 | Autogenesis | Self-creating, self-evolving architecture |

## KSM Cycle 7: Deep Tree Echo Evolution (Latest)

### GamerGirl 4E Controller Interface
Implements the "gamer girl" virtual game controller as a 4E embodied cognition feature:
- **Embodied**: Controller grip posture tracking (Aggressive/Precision/Defensive/Relaxed)
- **Embedded**: Haptic feedback loop integrating controller rumble with avatar arousal
- **Enacted**: Gaming intent detection (Exploring/Rushing/Stealth/Combat/Traversal)
- **Extended**: Flow state detection (Csíkszentmihályi model adapted for gaming)
- **Combo detection**: Rapid successive action pattern recognition

### DTE Autognosis Service (ESN Self-Monitoring)
DAO-style governance system for autonomous ESN health management:
- **Observe**: Continuous reservoir health metrics (spectral radius, entropy, coherence)
- **Diagnose**: Anomaly detection with DAO governance voting
- **Prescribe**: KSM repair strategy selection from pattern library
- **Apply**: Structure-preserving transformation execution
- **Verify**: Pre/post metric comparison for repair validation
- **Evolve**: Governance weight adaptation based on repair outcomes

### MetaHuman Avatar Profile Service (UE5 Integration)
Evaluates Deep Tree Echo 3D avatar profile integration for UE5 MetaHuman:
- Profile image → facial landmark extraction → MetaHuman DNA mapping
- FACS Action Unit calibration (46 AUs) for facial animation
- Avatar readiness scoring across 7 categories
- MetaHuman Blueprint configuration generation
- JSON export for UE5 import pipeline

## License

Part of the AngelClaw project. See the main repository for license details.
