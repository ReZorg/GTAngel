using System.IO;
using System.Text.Json;
using GTAngel.Models;
using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

[Collection("AppConfiguration file system")]
public sealed class AppConfigurationCoverageTests : IDisposable
{
    private const string AssetsDirectoryName = "Assets";
    private const string ConfigDirectoryName = "Config";
    private const int MaxRetryAttempts = 50;
    private const int RetryDelayMilliseconds = 50;

    private readonly string _configDirectory = Path.Combine(
        AppDomain.CurrentDomain.BaseDirectory,
        AssetsDirectoryName,
        ConfigDirectoryName);
    private readonly string _configPath;
    private readonly string _settingsDirectory = Path.Combine(
        Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
        "GTAngel");
    private readonly string _settingsPath;
    private readonly string? _originalConfigContents;
    private readonly string? _originalSettingsContents;

    public AppConfigurationCoverageTests()
    {
        _configPath = Path.Combine(_configDirectory, "SDK.config");
        _settingsPath = Path.Combine(_settingsDirectory, "gtangel_settings.json");

        _originalConfigContents = File.Exists(_configPath)
            ? File.ReadAllText(_configPath)
            : null;
        _originalSettingsContents = File.Exists(_settingsPath)
            ? File.ReadAllText(_settingsPath)
            : null;
    }

    [Fact]
    public async Task LoadAsync_WhenSdkConfigExists_LoadsConfiguredSections()
    {
        Directory.CreateDirectory(_configDirectory);
        await File.WriteAllTextAsync(_configPath, """
        {
          "general": {
            "name": "Vice City",
            "short_name": "VC",
            "slug": "gtavcde"
          },
          "games": {
            "gta3": {
              "id": "gta3",
              "name": "Grand Theft Auto III"
            },
            "gtasa": {
              "id": "gtasa",
              "name": "Grand Theft Auto: San Andreas"
            }
          }
        }
        """);

        var service = new AppConfiguration(NullLogger<AppConfiguration>.Instance);

        await service.LoadAsync();

        Assert.NotNull(service.SdkConfig);
        Assert.NotNull(service.General);
        Assert.Equal("Vice City", service.General!.Name);
        Assert.Equal("VC", service.General.ShortName);
        Assert.Equal("gtavcde", service.General.Slug);
        Assert.NotNull(service.SdkConfig!.Games);
        Assert.Equal(2, service.SdkConfig.Games!.Count);
        Assert.Equal("Grand Theft Auto III", service.SdkConfig.Games["gta3"].Name);
    }

    [Fact]
    public async Task LoadAsync_WhenSdkConfigIsInvalid_UsesEmptySdkConfig()
    {
        Directory.CreateDirectory(_configDirectory);
        await File.WriteAllTextAsync(_configPath, "{ not valid json");

        var service = new AppConfiguration(NullLogger<AppConfiguration>.Instance);

        await service.LoadAsync();

        Assert.NotNull(service.SdkConfig);
        Assert.Null(service.General);
        Assert.Null(service.SdkConfig!.Games);
    }

    [Fact]
    public async Task LoadUserSettingsAsync_WhenSettingsExist_RestoresPersistedEnginePath()
    {
        Directory.CreateDirectory(_settingsDirectory);
        await File.WriteAllTextAsync(_settingsPath, """
        {
          "Ue5EnginePath": "D:\\UE5\\Engine"
        }
        """);

        var service = new AppConfiguration(NullLogger<AppConfiguration>.Instance);

        await service.LoadUserSettingsAsync();

        Assert.Equal(@"D:\UE5\Engine", service.Ue5EnginePath);
    }

    [Fact]
    public async Task Ue5EnginePath_Setter_PersistsSettingsFile()
    {
        var service = new AppConfiguration(NullLogger<AppConfiguration>.Instance);

        service.Ue5EnginePath = @"C:\Tools\UE5";

        var settings = await WaitForSettingsAsync();

        Assert.Equal(@"C:\Tools\UE5", settings.Ue5EnginePath);
    }

    public void Dispose()
    {
        RestoreFile(_configPath, _originalConfigContents);
        RestoreFile(_settingsPath, _originalSettingsContents);
    }

    private async Task<UserSettings> WaitForSettingsAsync()
    {
        for (var attempt = 0; attempt < MaxRetryAttempts; attempt++)
        {
            if (File.Exists(_settingsPath))
            {
                var json = await File.ReadAllTextAsync(_settingsPath);
                var settings = JsonSerializer.Deserialize<UserSettings>(json);
                if (settings?.Ue5EnginePath is not null)
                {
                    return settings;
                }
            }

            await Task.Delay(RetryDelayMilliseconds);
        }

        throw new TimeoutException("Timed out waiting for user settings to be persisted.");
    }

    private static void RestoreFile(string path, string? originalContents)
    {
        if (originalContents is null)
        {
            if (File.Exists(path))
            {
                File.Delete(path);
            }

            return;
        }

        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllText(path, originalContents);
    }
}

[CollectionDefinition("AppConfiguration file system", DisableParallelization = true)]
public sealed class AppConfigurationFileSystemCollectionDefinition
{
}
