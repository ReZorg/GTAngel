# GTAngel System Requirements

## Minimum Requirements

| Component | Requirement |
|-----------|-------------|
| **Operating System** | Windows 10 version 1809 (October 2018 Update) or later |
| **Architecture** | x64 (AMD64) or ARM64 |
| **Processor** | 4 cores, 2.0 GHz |
| **Memory (RAM)** | 8 GB |
| **Storage** | 2 GB available space |
| **Graphics** | DirectX 11 compatible GPU |
| **Display** | 1280×720 resolution |

## Recommended Requirements

| Component | Requirement |
|-----------|-------------|
| **Operating System** | Windows 11 |
| **Architecture** | x64 (AMD64) |
| **Processor** | 8+ cores, 3.0+ GHz (Intel 12th gen / AMD Ryzen 5000+) |
| **Memory (RAM)** | 16 GB or more |
| **Storage** | SSD with 10+ GB available |
| **Graphics** | DirectX 12 compatible GPU with 4+ GB VRAM |
| **Display** | 1920×1080 or higher |

## UE5 Integration Requirements

For full Unreal Engine 5 cognitive integration:

| Component | Requirement |
|-----------|-------------|
| **GPU** | NVIDIA RTX 2060 / AMD RX 6600 or better |
| **VRAM** | 6 GB minimum, 8+ GB recommended |
| **DirectX** | DirectX 12 Ultimate |
| **Additional Storage** | 50+ GB for UE5 assets |

## .NET Runtime

### Self-Contained Builds (Recommended)
- No additional runtime required
- .NET 8 is bundled with the application

### Portable Build
- Requires [.NET 8 Desktop Runtime](https://dotnet.microsoft.com/download/dotnet/8.0)
- Download: `dotnet-runtime-8.x.x-win-x64.exe`

## Network Requirements

| Feature | Requirement |
|---------|-------------|
| Initial setup | Internet connection for optional updates |
| Core functionality | No internet required (fully offline capable) |
| Update checking | HTTPS access to GitHub |

## Supported Windows Versions

| Windows Version | Support Status |
|-----------------|----------------|
| Windows 11 (all versions) | ✅ Fully supported |
| Windows 10 22H2 | ✅ Fully supported |
| Windows 10 21H2 | ✅ Supported |
| Windows 10 1809-21H1 | ⚠️ Limited support |
| Windows 10 < 1809 | ❌ Not supported |
| Windows 8.1 | ❌ Not supported |
| Windows 7 | ❌ Not supported |

## ARM64 Support

GTAngel provides native ARM64 builds for:
- Microsoft Surface Pro X
- Windows Dev Kit 2023
- Snapdragon-based Windows laptops
- Windows on ARM virtual machines

Note: UE5 integration on ARM64 requires x64 emulation for some components.

## Antivirus Considerations

Some antivirus software may flag new applications. If you encounter issues:

1. **Add exception** for GTAngel installation folder
2. **Verify download** using SHA256 checksums
3. **Report false positive** to your antivirus vendor

GTAngel is signed with a valid code signing certificate and scanned for malware before release.

## Troubleshooting

### Application won't start

1. Verify Windows version meets minimum requirements
2. Check Event Viewer for .NET runtime errors
3. For portable build: Install .NET 8 Desktop Runtime
4. Try running as Administrator

### Poor performance

1. Close other resource-intensive applications
2. Update graphics drivers
3. Ensure SSD has free space
4. Check Task Manager for resource usage

### UE5 integration issues

1. Verify GPU meets DirectX 12 requirements
2. Update to latest GPU drivers
3. Check UE5 engine path configuration
4. Review logs in `%LOCALAPPDATA%\ReZorg\GTAngel\logs\`
