using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using GTAngel.Models;
using GTAngel.Services;
using GTAngel.ViewModels;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.ViewModels;

public class OtherGamesViewModelTests
{
    [Fact]
    public void Constructor_WhenSdkConfigContainsGames_LoadsEachGame()
    {
        var config = new AppConfiguration(NullLogger<AppConfiguration>.Instance);
        typeof(AppConfiguration)
            .GetProperty(
                nameof(AppConfiguration.SdkConfig),
                BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic)!
            .SetValue(config, new SdkConfig
            {
                Games = new Dictionary<string, GameConfig>
                {
                    ["gta3"] = new() { Id = "gta3", Name = "Grand Theft Auto III" },
                    ["gtasa"] = new() { Id = "gtasa", Name = "Grand Theft Auto: San Andreas" }
                }
            });

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
}

public class BrowserViewModelTests
{
    private readonly BrowserViewModel _viewModel = new(
        NullLogger<BrowserViewModel>.Instance,
        new NavigationService(NullLogger<NavigationService>.Instance));

    [Fact]
    public void Defaults_AreInitializedForBlankLoadingPage()
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
