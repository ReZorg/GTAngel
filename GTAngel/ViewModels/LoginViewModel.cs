using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.Logging;
using GTA3DE.Wpf.Models;
using GTA3DE.Wpf.Services;
using GTA3DE.Wpf.Views;

namespace GTA3DE.Wpf.ViewModels;

/// <summary>
/// Login page view model.
/// Translated from: FlutterSocialClubLoginScreen + Rockstar.checkAuthTokenValidity()
/// Handles Social Club authentication flow.
/// </summary>
public partial class LoginViewModel : ObservableObject
{
    private readonly ILogger<LoginViewModel> _logger;
    private readonly SocialClubApiClient _socialClubApi;
    private readonly AppStateService _state;
    private readonly NavigationService _navigation;

    [ObservableProperty]
    private string _email = string.Empty;

    [ObservableProperty]
    private string _password = string.Empty;

    [ObservableProperty]
    private bool _isLoading;

    [ObservableProperty]
    private string _errorMessage = string.Empty;

    public LoginViewModel(
        ILogger<LoginViewModel> logger,
        SocialClubApiClient socialClubApi,
        AppStateService state,
        NavigationService navigation)
    {
        _logger = logger;
        _socialClubApi = socialClubApi;
        _state = state;
        _navigation = navigation;
    }

    /// <summary>
    /// Login command.
    /// Replaces: Rockstar.login() → SocialClubAPI authentication flow.
    /// </summary>
    [RelayCommand]
    private async Task LoginAsync()
    {
        if (string.IsNullOrWhiteSpace(Email))
        {
            ErrorMessage = "Please enter your email address.";
            return;
        }

        IsLoading = true;
        ErrorMessage = string.Empty;

        try
        {
            _logger.LogInformation("Attempting Social Club login for {Email}", Email);

            var user = await _socialClubApi.LoginAsync(Email, Password);
            if (user != null)
            {
                _state.SetUser(user);
                _logger.LogInformation("Login successful for {RockstarId}", user.RockstarId);

                // Navigate to game (replaces Rockstar.stateUpdated callback)
                _navigation.NavigateTo<GamePage>();
            }
            else
            {
                ErrorMessage = "Login failed. Please check your credentials.";
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Login failed");
            ErrorMessage = $"Login error: {ex.Message}";
        }
        finally
        {
            IsLoading = false;
        }
    }

    /// <summary>
    /// Skip login and continue without signing in.
    /// Replaces: offline/guest mode flow.
    /// </summary>
    [RelayCommand]
    private void SkipLogin()
    {
        _logger.LogInformation("User skipped login - continuing as guest");
        _state.SetGuestMode(true);
        _navigation.NavigateTo<GamePage>();
    }
}
