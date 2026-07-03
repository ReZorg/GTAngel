# Installation Guide

## Download Options

| Package | Best For | .NET Required? |
|---------|----------|----------------|
| **Self-Contained (win-x64)** | Most users | No — bundled |
| **Self-Contained (win-arm64)** | Surface Pro X, ARM laptops | No — bundled |
| **Portable** | Users with .NET 8 already installed | Yes |
| **MSI Installer** | Enterprise/managed deployments | No — bundled |

Download the latest release from [GitHub Releases](https://github.com/ReZorg/GTAngel/releases).

---

## Option 1: Self-Contained ZIP (Recommended)

The simplest installation — everything is bundled in one archive.

1. Download `GTAngel-X.Y.Z-win-x64.zip` from Releases
2. Extract to a folder of your choice (e.g., `C:\GTAngel\`)
3. Run `GTAngel.exe`

> **Tip:** Avoid extracting into `C:\Program Files\` as it requires admin permissions for log file creation.

---

## Option 2: MSI Installer

Professional Windows installer with Start Menu integration.

1. Download `GTAngel-X.Y.Z-setup.msi`
2. Double-click to run the installer
3. Follow the setup wizard
4. Launch from **Start Menu → GTAngel**

The MSI installs to `%LOCALAPPDATA%\ReZorg\GTAngel\` (per-user, no admin required).

### Uninstalling

- **Settings → Apps → GTAngel → Uninstall**, or
- Run the MSI again and select Remove

---

## Option 3: Portable (Framework-Dependent)

Smaller download if you already have .NET 8 Desktop Runtime.

1. Install [.NET 8 Desktop Runtime](https://dotnet.microsoft.com/download/dotnet/8.0) if not already installed
2. Download `GTAngel-X.Y.Z-portable.zip`
3. Extract and run `GTAngel.exe`

---

## Verifying Your Download

Each release includes `SHA256SUMS.txt`. Verify integrity:

```powershell
# PowerShell
(Get-FileHash .\GTAngel-1.84.3-win-x64.zip -Algorithm SHA256).Hash
# Compare with the value in SHA256SUMS.txt
```

---

## Windows SmartScreen

On first run, Windows SmartScreen may show a warning for unsigned builds. Click **More info → Run anyway**. Signed releases will not trigger this warning.

---

## System Requirements

See [System Requirements](../release/SYSTEM_REQUIREMENTS.md) for full details.

**Minimum:** Windows 10 1809+, 4-core CPU, 8 GB RAM, DirectX 11 GPU
**Recommended:** Windows 11, 8+ cores, 16 GB RAM, DirectX 12 GPU with 4+ GB VRAM
