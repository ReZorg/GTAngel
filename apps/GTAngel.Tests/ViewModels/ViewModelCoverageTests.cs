using System.IO;
using System.Linq;
using GTAngel.Services;
using GTAngel.ViewModels;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

using static GTAngel.Tests.ViewModels.ViewModelTestPaths;

namespace GTAngel.Tests.ViewModels;

[Collection("AppConfiguration file system")]
public sealed class OtherGamesViewModelTests : IDisposable
{
    private readonly string _configPath = Path.Combine(
        AppDomain.CurrentDomain.BaseDirectory,
        AssetsDirectoryName,
        ConfigDirectoryName,
        "SDK.config");
    private readonly string? _originalConfigContents;

    public OtherGamesViewModelTests()
    {
        _originalConfigContents = File.Exists(_configPath)
            ? File.ReadAllText(_configPath)
            : null;
    }

    [Fact]
    public async Task Constructor_WhenSdkConfigContainsGames_LoadsEachGame()
    {
        var configDirectory = Path.GetDirectoryName(_configPath)!;
        Directory.CreateDirectory(configDirectory);
        await File.WriteAllTextAsync(_configPath, """
        {
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

        var config = new AppConfiguration(NullLogger<AppConfiguration>.Instance);
        await config.LoadAsync();

        var viewModel = new OtherGamesViewModel(
            NullLogger<OtherGamesViewModel>.Instance,
            config);

        Assert.Equal(2, viewModel.Games.Count);
        Assert.Equal(new[] { "gta3", "gtasa" }, viewModel.Games.Select(game => game.Id));
    }

    [Fact]
    public void Constructor_WhenSdkConfigHasNoGames_LeavesCollectionEmpty()
    {
        var viewModel = new OtherGamesViewModel(
            NullLogger<OtherGamesViewModel>.Instance,
            new AppConfiguration(NullLogger<AppConfiguration>.Instance));

        Assert.Empty(viewModel.Games);
    }

    public void Dispose()
    {
        if (_originalConfigContents is null)
        {
            if (File.Exists(_configPath))
            {
                File.Delete(_configPath);
            }

            return;
        }

        Directory.CreateDirectory(Path.GetDirectoryName(_configPath)!);
        File.WriteAllText(_configPath, _originalConfigContents);
    }
}

public sealed class BrowserViewModelTests
{
    private readonly BrowserViewModel _viewModel = new(
        NullLogger<BrowserViewModel>.Instance,
        new NavigationService(NullLogger<NavigationService>.Instance));

    [Fact]
    public void Constructor_InitializesDefaultState()
    {
        Assert.Equal("about:blank", _viewModel.Url);
        Assert.True(_viewModel.IsLoading);
        Assert.False(_viewModel.IsError);
        Assert.Equal(string.Empty, _viewModel.ErrorTitle);
        Assert.Equal(string.Empty, _viewModel.ErrorMessage);
    }

    [Fact]
    public void NavigateToUrl_UpdatesUrlAndResetsErrorState()
    {
        _viewModel.IsError = true;
        _viewModel.IsLoading = false;

        _viewModel.NavigateToUrl("https://example.com");

        Assert.Equal("https://example.com", _viewModel.Url);
        Assert.True(_viewModel.IsLoading);
        Assert.False(_viewModel.IsError);
    }

    [Fact]
    public void ReloadCommand_ResetsLoadingAndErrorFlags()
    {
        _viewModel.IsError = true;
        _viewModel.IsLoading = false;

        _viewModel.ReloadCommand.Execute(null);

        Assert.True(_viewModel.IsLoading);
        Assert.False(_viewModel.IsError);
    }

    [Fact]
    public void RetryCommand_RestoresCurrentUrlAndResetsLoadingState()
    {
        _viewModel.Url = "https://rockstargames.com";
        _viewModel.IsError = true;
        _viewModel.IsLoading = false;

        _viewModel.RetryCommand.Execute(null);

        Assert.Equal("https://rockstargames.com", _viewModel.Url);
        Assert.True(_viewModel.IsLoading);
        Assert.False(_viewModel.IsError);
    }

    [Fact]
    public void NavigationCommands_WithoutRegisteredFrame_DoNotThrow()
    {
        var closeException = Record.Exception(() => _viewModel.CloseCommand.Execute(null));
        var backException = Record.Exception(() => _viewModel.BackCommand.Execute(null));
        var forwardException = Record.Exception(() => _viewModel.ForwardCommand.Execute(null));

        Assert.Null(closeException);
        Assert.Null(backException);
        Assert.Null(forwardException);
    }
}

internal static class ViewModelTestPaths
{
    public const string AssetsDirectoryName = "Assets";
    public const string ConfigDirectoryName = "Config";
}
