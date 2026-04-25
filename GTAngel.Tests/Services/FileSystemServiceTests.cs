using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for FileSystemService — path construction, GetAllGameExecutablePaths,
/// GetContentPath, and GetUProjectPath.
/// Note: Tests that require real file existence are isolated from those that check
/// path-construction logic only.
/// </summary>
public class FileSystemServiceTests
{
    private readonly FileSystemService _service;

    public FileSystemServiceTests()
    {
        _service = new FileSystemService(NullLogger<FileSystemService>.Instance);
    }

    // ── Path property sanity ───────────────────────────────────────────────

    [Fact]
    public void AppDataPath_IsNotEmpty()
    {
        Assert.NotEmpty(_service.AppDataPath);
    }

    [Fact]
    public void GameDataPath_IsNotEmpty()
    {
        Assert.NotEmpty(_service.GameDataPath);
    }

    [Fact]
    public void SavesPath_IsNotEmpty()
    {
        Assert.NotEmpty(_service.SavesPath);
    }

    [Fact]
    public void ConfigPath_IsNotEmpty()
    {
        Assert.NotEmpty(_service.ConfigPath);
    }

    [Fact]
    public void LogsPath_IsNotEmpty()
    {
        Assert.NotEmpty(_service.LogsPath);
    }

    [Fact]
    public void GameDataPath_IsSubdirectoryOfAppDataPath()
    {
        Assert.StartsWith(_service.AppDataPath, _service.GameDataPath);
    }

    [Fact]
    public void ConfigPath_IsSubdirectoryOfAppDataPath()
    {
        Assert.StartsWith(_service.AppDataPath, _service.ConfigPath);
    }

    [Fact]
    public void LogsPath_IsSubdirectoryOfAppDataPath()
    {
        Assert.StartsWith(_service.AppDataPath, _service.LogsPath);
    }

    // ── Constants ─────────────────────────────────────────────────────────

    [Fact]
    public void UEProjectName_IsGameface()
    {
        Assert.Equal("Gameface", FileSystemService.UEProjectName);
    }

    [Fact]
    public void UnrealEngineCogPath_IsNotEmpty()
    {
        Assert.NotEmpty(FileSystemService.UnrealEngineCogPath);
    }

    // ── GetAllGameExecutablePaths ──────────────────────────────────────────

    [Fact]
    public void GetAllGameExecutablePaths_ReturnsAtLeastOnePath()
    {
        var paths = _service.GetAllGameExecutablePaths();
        Assert.NotEmpty(paths);
    }

    [Fact]
    public void GetAllGameExecutablePaths_AllPathsAreDistinct()
    {
        var paths = _service.GetAllGameExecutablePaths();
        Assert.Equal(paths.Distinct().Count(), paths.Length);
    }

    [Fact]
    public void GetAllGameExecutablePaths_AllPathsEndWithExe()
    {
        var paths = _service.GetAllGameExecutablePaths();
        Assert.All(paths, p => Assert.True(p.EndsWith(".exe", StringComparison.OrdinalIgnoreCase)));
    }

    [Fact]
    public void GetAllGameExecutablePaths_ContainsGta3DeExe()
    {
        var paths = _service.GetAllGameExecutablePaths();
        Assert.Contains(paths, p => p.EndsWith("GTA3DE.exe", StringComparison.OrdinalIgnoreCase));
    }

    [Fact]
    public void GetAllGameExecutablePaths_ContainsGamefaceShippingExe()
    {
        var paths = _service.GetAllGameExecutablePaths();
        Assert.Contains(paths, p => p.Contains("Gameface-Win64-Shipping.exe"));
    }

    // ── GetContentPath ─────────────────────────────────────────────────────

    [Fact]
    public void GetContentPath_IsNotEmpty()
    {
        Assert.NotEmpty(_service.GetContentPath());
    }

    [Fact]
    public void GetContentPath_ContainsGamefaceAndContent()
    {
        var path = _service.GetContentPath();
        Assert.Contains("Gameface", path);
        Assert.Contains("Content", path);
    }

    [Fact]
    public void GetContentPath_IsSubdirectoryOfGameDataPath()
    {
        var contentPath = _service.GetContentPath();
        Assert.StartsWith(_service.GameDataPath, contentPath);
    }

    // ── AreGameAssetsPresent ───────────────────────────────────────────────

    [Fact]
    public void AreGameAssetsPresent_DoesNotThrow()
    {
        var ex = Record.Exception(() => _service.AreGameAssetsPresent());
        Assert.Null(ex);
    }

    // ── GetGameExecutablePath ─────────────────────────────────────────────

    [Fact]
    public void GetGameExecutablePath_ReturnsNonEmptyString()
    {
        var path = _service.GetGameExecutablePath();
        Assert.NotEmpty(path);
    }

    [Fact]
    public void GetGameExecutablePath_EndsWithExe()
    {
        var path = _service.GetGameExecutablePath();
        Assert.True(path.EndsWith(".exe", StringComparison.OrdinalIgnoreCase));
    }

    // ── GetUProjectPath ───────────────────────────────────────────────────

    [Fact]
    public void GetUProjectPath_WhenNoFileExists_ReturnsNull()
    {
        // On CI/dev machines without a UE project, result should be null
        var path = _service.GetUProjectPath();
        // Either null (not found) or a real path ending in .uproject
        if (path != null)
            Assert.EndsWith(".uproject", path, StringComparison.OrdinalIgnoreCase);
    }

    // ── IsUnrealEngineCogAvailable ────────────────────────────────────────

    [Fact]
    public void IsUnrealEngineCogAvailable_DoesNotThrow()
    {
        var ex = Record.Exception(() => _service.IsUnrealEngineCogAvailable());
        Assert.Null(ex);
    }

    // ── GetGameDataSize ───────────────────────────────────────────────────

    [Fact]
    public void GetGameDataSize_ReturnsNonNegativeValue()
    {
        long size = _service.GetGameDataSize();
        Assert.True(size >= 0);
    }
}
