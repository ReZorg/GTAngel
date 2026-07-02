# GTAngel Release Process

This document describes the release process for GTAngel.

## Release Types

| Type | Version Pattern | Trigger | Example |
|------|-----------------|---------|---------|
| Major | X.0.0 | Breaking changes | 2.0.0 |
| Minor | X.Y.0 | New features | 1.85.0 |
| Patch | X.Y.Z | Bug fixes | 1.84.4 |

## Pre-Release Checklist

Before creating a release:

- [ ] All tests pass (`dotnet test GTAngel.Tests`)
- [ ] Code coverage meets threshold (≥42%)
- [ ] No critical security vulnerabilities
- [ ] CHANGELOG.md is updated
- [ ] Version numbers updated in:
  - [ ] `GTAngel/GTAngel.csproj` (Version, FileVersion, AssemblyVersion)
  - [ ] `GTAngel/appsettings.json` (App.Version)
  - [ ] `GTAngel/appsettings.Production.json` (App.Version)
- [ ] Documentation is current
- [ ] PR approved and merged to main

## Creating a Release

### Option 1: Tag-Based Release (Recommended)

```bash
# Ensure you're on main with latest changes
git checkout main
git pull origin main

# Create and push a version tag
git tag -a v1.84.3 -m "Release v1.84.3"
git push origin v1.84.3
```

The `release.yml` workflow will automatically:
1. Build all platform variants (win-x64, win-arm64, portable)
2. Run tests
3. Create MSI installer
4. Generate SHA256 checksums
5. Create GitHub Release with all artifacts

### Option 2: Manual Workflow Dispatch

1. Go to **Actions** → **Release** workflow
2. Click **Run workflow**
3. Enter version number (e.g., `1.84.3`)
4. Check "Pre-release" if applicable
5. Click **Run workflow**

## Build Outputs

| Artifact | Description | Size (approx) |
|----------|-------------|---------------|
| `GTAngel-X.Y.Z-win-x64.zip` | Self-contained x64 | ~150 MB |
| `GTAngel-X.Y.Z-win-arm64.zip` | Self-contained ARM64 | ~150 MB |
| `GTAngel-X.Y.Z-portable.zip` | Framework-dependent | ~50 MB |
| `GTAngel-X.Y.Z-setup.msi` | MSI installer | ~100 MB |
| `SHA256SUMS.txt` | Checksums for all files | <1 KB |

## Post-Release Steps

1. **Verify Release**
   - Download and test each artifact
   - Verify SHA256 checksums
   - Test installation on clean VM

2. **Announce Release**
   - Update project website/README
   - Post to relevant channels
   - Update documentation links

3. **Monitor**
   - Watch for crash reports
   - Monitor GitHub Issues
   - Track download statistics

## Hotfix Process

For critical bugs in production:

```bash
# Create hotfix branch from release tag
git checkout -b hotfix/1.84.4 v1.84.3

# Make minimal fix
git commit -m "fix: critical bug description"

# Create PR to main
gh pr create --title "Hotfix: Critical bug" --base main

# After merge, tag new version
git checkout main
git pull
git tag -a v1.84.4 -m "Hotfix v1.84.4"
git push origin v1.84.4
```

## Rollback Process

If a release has critical issues:

1. **Mark as Pre-release**
   - Edit release on GitHub
   - Check "This is a pre-release"

2. **Communicate**
   - Post issue in GitHub Discussions
   - Direct users to previous version

3. **Prepare Hotfix**
   - Follow hotfix process above
   - Include rollback instructions

## Version Numbering

GTAngel follows [Semantic Versioning](https://semver.org/):

- **MAJOR** (X.0.0): Incompatible API/config changes
- **MINOR** (X.Y.0): New features, backwards compatible
- **PATCH** (X.Y.Z): Bug fixes, backwards compatible

### Pre-release Labels

- `alpha` - Early development, unstable
- `beta` - Feature complete, testing
- `rc` - Release candidate

Example: `v2.0.0-beta.1`

## Code Signing

For production releases, executables should be signed:

1. **Certificate Setup** (one-time)
   - Obtain code signing certificate
   - Store in Azure Key Vault
   - Configure GitHub Secrets

2. **Signing Process**
   - Workflow automatically signs during release
   - Timestamp server ensures long-term validity

See [Code Signing Setup](./code-signing.md) for details.

## Troubleshooting

### Build Fails

- Check GitHub Actions logs
- Verify .NET 8 SDK version
- Check for transient network issues

### Tests Fail

- Review test output in Actions
- Run locally: `dotnet test GTAngel.Tests`
- Check for environment-specific issues

### MSI Build Fails

- Verify WiX Toolset installation
- Check for missing files in publish output
- Review WiX error messages

### Release Not Created

- Verify tag format: `v*.*.*`
- Check workflow permissions
- Review `GITHUB_TOKEN` permissions
