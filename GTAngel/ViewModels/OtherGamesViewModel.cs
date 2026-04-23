using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using Microsoft.Extensions.Logging;
using GTAngel.Models;
using GTAngel.Services;

namespace GTAngel.ViewModels;

/// <summary>
/// Other games page view model.
/// Translated from: FlutterOtherGamesScreen
/// Loads game catalog from SDK.config (GCConfig.GamesConfig) and displays game cards.
/// </summary>
public partial class OtherGamesViewModel : ObservableObject
{
    private readonly ILogger<OtherGamesViewModel> _logger;
    private readonly AppConfiguration _config;

    public ObservableCollection<GameConfig> Games { get; } = new();

    public OtherGamesViewModel(
        ILogger<OtherGamesViewModel> logger,
        AppConfiguration config)
    {
        _logger = logger;
        _config = config;

        LoadGames();
    }

    private void LoadGames()
    {
        if (_config.SdkConfig?.Games != null)
        {
            foreach (var game in _config.SdkConfig.Games.Values)
            {
                Games.Add(game);
            }
        }

        _logger.LogInformation("Loaded {Count} games from SDK config", Games.Count);
    }
}
