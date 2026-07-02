# Changelog

All notable changes to GTAngel will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Production deployment infrastructure
- Self-contained publish profiles for win-x64 and win-arm64
- WiX MSI installer project
- GitHub Actions release workflow
- Environment-specific configuration (Development, Production)

### Changed
- Updated GTAngel.csproj with production publishing settings
- Added Microsoft.SourceLink.GitHub for improved crash diagnostics

## [1.84.3] - 2026-07-02

### Added
- DTE Cognitive Core Service with ECAN attention and MOSES pattern mining
- ESN Reservoir Pipeline with 3-layer Echo State Network
- ML Vision Capture Service (768×768 DXGI frame capture)
- UE5 Launch Orchestrator with 4-stage pipeline
- Avatar Embodiment Service with FACS expressions
- Game World Navigation Service with A* pathfinding
- Multi-Agent Trainer for reinforcement learning
- Named Pipe IPC for UE5 communication

### Changed
- Upgraded to .NET 8 WPF
- Integrated CommunityToolkit.Mvvm for MVVM pattern
- Added LiveCharts for training visualization

### Security
- Local-only Named Pipe IPC connections
- ONNX models loaded from local paths only
- No external API keys in source

## [1.84.0] - 2026-06-15

### Added
- Initial GTAngel framework
- WPF desktop application with MVVM architecture
- Core services infrastructure
- Archecho UE5 cognitive plugin modules
- xUnit test project

### Fixed
- Initial release - no fixes

---

## Version History

| Version | Date | Highlights |
|---------|------|------------|
| 1.84.3 | 2026-07-02 | Production deployment, DTE cognitive core |
| 1.84.0 | 2026-06-15 | Initial release |

[Unreleased]: https://github.com/ReZorg/GTAngel/compare/v1.84.3...HEAD
[1.84.3]: https://github.com/ReZorg/GTAngel/compare/v1.84.0...v1.84.3
[1.84.0]: https://github.com/ReZorg/GTAngel/releases/tag/v1.84.0
