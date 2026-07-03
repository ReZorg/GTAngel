# Getting Started

## First Launch

When you run GTAngel for the first time:

1. **Application loads** — the splash screen appears while services initialize
2. **Configuration detected** — the app reads `appsettings.Production.json` automatically
3. **Model verification** — ONNX models are validated for integrity (SHA-256)
4. **Update check** — a background check for new versions runs (configurable)
5. **Main window opens** — you're ready to train

---

## Directory Structure

After installation, GTAngel uses the following layout:

```
GTAngel/
├── GTAngel.exe           # Main application
├── appsettings.json      # Base configuration
├── appsettings.Production.json  # Production overrides
├── Assets/
│   ├── Models/           # ONNX ML models
│   ├── Config/           # SDK configuration
│   └── Images/           # UI assets
├── Archecho/             # UE5 cognitive plugins
├── Engine/               # UE5 engine binaries
└── logs/                 # Application logs (auto-created)
```

User settings are stored in:
```
%LOCALAPPDATA%\GTAngel\gtangel_settings.json
```

---

## Configuration

### UE5 Engine Path

GTAngel needs to know where your UE5 engine is installed. Options:

1. **Environment variable:** Set `UE5_ENGINE_PATH` to your engine root
2. **Settings UI:** Configure in the application Settings panel
3. **Default:** The bundled `./Engine` directory

### Update Channels

Configure in `appsettings.Production.json` under `Updates.Channel`:

| Channel | Description |
|---------|-------------|
| `Stable` | Production releases only (default) |
| `Beta` | Pre-release builds for testing |
| `Canary` | Cutting-edge development builds |

---

## Training Workflow

1. **Launch UE5 Project** — GTAngel orchestrates the 4-stage UE5 launch pipeline
2. **Frame Capture** — DXGI captures 768×768 frames at configured FPS
3. **Feature Extraction** — ONNX CNN processes frames into feature vectors
4. **ESN Processing** — Echo State Network processes temporal sequences
5. **DTE Training** — Deep Tree Echo cognitive core trains continuously
6. **Avatar Embodiment** — Results manifest through FACS expressions + IK

---

## Logs

Logs are written to the `logs/` directory next to the executable:

- **Production:** Daily rotation, 14-day retention, max 50MB per file
- **Development:** Hourly rotation, 48-hour retention, max 10MB per file

Log location: `<install-dir>/logs/gtangel-YYYY-MM-DD.log`

---

## Next Steps

- Review [System Requirements](../release/SYSTEM_REQUIREMENTS.md) for UE5 integration
- Check [Troubleshooting](troubleshooting.md) if you encounter issues
- Report bugs via [GitHub Issues](https://github.com/ReZorg/GTAngel/issues)
