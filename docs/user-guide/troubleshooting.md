# Troubleshooting

## Application Won't Start

### Symptoms
- Nothing happens when double-clicking GTAngel.exe
- Application crashes immediately on launch

### Solutions

1. **Check Windows version** — Requires Windows 10 1809 or later
2. **Install .NET 8 Runtime** (portable build only):
   ```
   winget install Microsoft.DotNet.DesktopRuntime.8
   ```
3. **Check Event Viewer:** `Windows Logs → Application` for .NET runtime errors
4. **Try running from Command Prompt** to see error messages:
   ```cmd
   cd C:\path\to\GTAngel
   GTAngel.exe
   ```
5. **Antivirus interference:** Add GTAngel folder to exclusions

---

## Windows SmartScreen Warning

**"Windows protected your PC"** appears on unsigned builds.

**Solution:** Click **More info → Run anyway**. Signed release builds will not trigger this warning.

---

## Model Integrity Check Fails

### Symptoms
- Log shows "Model integrity FAILED" errors
- Features appear degraded or non-functional

### Solutions

1. **Re-download** the release — file may be corrupted during download
2. **Verify checksum** of the ZIP against SHA256SUMS.txt
3. **Check antivirus** — some AV quarantines ONNX files

---

## UE5 Integration Issues

### Engine Not Found

```
UE5 engine path not configured or does not exist
```

**Solution:** Set the UE5 engine path:
- Via environment variable: `SET UE5_ENGINE_PATH=C:\UE5\Engine`
- Via Settings panel in the application
- Or ensure the bundled `Engine/` directory is intact

### UE5 Launch Fails

1. Verify GPU meets DirectX 12 requirements
2. Update GPU drivers to latest version
3. Check available disk space (50+ GB for UE5 assets)
4. Review logs: `logs/gtangel-*.log` for detailed error messages

---

## Performance Issues

### Low Frame Rate / Stuttering

1. **Close other GPU-intensive applications**
2. **Reduce capture resolution** in settings (default: 768×768)
3. **Lower frame rate** — edit `appsettings.Production.json`:
   ```json
   "MLVision": { "FrameRate": 15 }
   ```
4. **Update GPU drivers**
5. **Ensure SSD** has adequate free space

### High Memory Usage

The cognitive training services use significant RAM:
- ESN Reservoir: scales with `ESNReservoirSize` (default: 1000)
- Experience Replay Buffer: grows during training
- ONNX models: ~200-500 MB GPU memory

Reduce memory by lowering `ESNReservoirSize` in appsettings.

---

## Logging & Diagnostics

### Finding Logs

Logs are in `<install-dir>/logs/`:
```
logs/gtangel-2026-07-03.log  (Production, daily)
logs/gtangel-dev-2026-07-03-14.log  (Development, hourly)
```

### Enabling Debug Logging

Set environment variable before launch:
```cmd
SET DOTNET_ENVIRONMENT=Development
GTAngel.exe
```

This activates verbose Debug-level logging.

### Crash Dumps

If the application crashes:
1. Check `logs/` for the most recent log file
2. Look for unhandled exception entries
3. File a [GitHub Issue](https://github.com/ReZorg/GTAngel/issues) with the log excerpt

---

## Update Issues

### Update Check Fails

- Verify internet connectivity
- Check if `https://github.com/ReZorg/GTAngel/releases` is accessible
- Disable update checking: set `"CheckOnStartup": false` in appsettings

### Manual Update

1. Download the latest release from GitHub
2. Extract over the existing installation (or install new MSI)
3. Your settings in `%LOCALAPPDATA%\GTAngel\` are preserved

---

## Getting Help

1. **Check logs** in the `logs/` directory for error details
2. **Search [GitHub Issues](https://github.com/ReZorg/GTAngel/issues)** for known problems
3. **File a new issue** with:
   - GTAngel version (from Settings or file properties)
   - Windows version (`winver`)
   - GPU model and driver version
   - Relevant log excerpt
   - Steps to reproduce
