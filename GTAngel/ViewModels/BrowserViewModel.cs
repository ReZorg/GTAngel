using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.Logging;
using GTA3DE.Wpf.Services;

namespace GTA3DE.Wpf.ViewModels;

/// <summary>
/// In-app browser view model.
/// Translated from: rockstarmobile/p018ui/BrowserScreen.java
/// Manages WebView2 navigation, loading state, and error handling.
/// </summary>
public partial class BrowserViewModel : ObservableObject
{
    private readonly ILogger<BrowserViewModel> _logger;
    private readonly NavigationService _navigation;

    [ObservableProperty]
    private string _url = "about:blank";

    [ObservableProperty]
    private bool _isLoading = true;

    [ObservableProperty]
    private bool _isError;

    [ObservableProperty]
    private string _errorTitle = string.Empty;

    [ObservableProperty]
    private string _errorMessage = string.Empty;

    public BrowserViewModel(
        ILogger<BrowserViewModel> logger,
        NavigationService navigation)
    {
        _logger = logger;
        _navigation = navigation;
    }

    public void NavigateToUrl(string url)
    {
        Url = url;
        IsError = false;
        IsLoading = true;
    }

    [RelayCommand]
    private void Close() => _navigation.GoBack();

    [RelayCommand]
    private void Back()
    {
        // WebView2 back navigation handled in code-behind
        _logger.LogDebug("Browser back");
    }

    [RelayCommand]
    private void Forward()
    {
        _logger.LogDebug("Browser forward");
    }

    [RelayCommand]
    private void Reload()
    {
        IsError = false;
        IsLoading = true;
        _logger.LogDebug("Browser reload");
    }

    [RelayCommand]
    private void Retry()
    {
        IsError = false;
        IsLoading = true;
        // Re-navigate to current URL
        var currentUrl = Url;
        Url = "about:blank";
        Url = currentUrl;
    }
}
