using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTA3DE.Wpf.Services;
using GTA3DE.Wpf.ViewModels;
using Serilog;

namespace GTA3DE.Wpf.Views;

/// <summary>
/// Splash/gate screen shown during initialization.
/// Replaces: FlutterGateScreen + initial Rockstar SDK setup flow.
/// After initialization completes, navigates to LoginPage or GamePage.
/// </summary>
public partial class SplashPage : Page
{
    private readonly SplashViewModel _viewModel;

    public SplashPage()
    {
        InitializeComponent();
        _viewModel = App.Services.GetRequiredService<SplashViewModel>();
        DataContext = _viewModel;

        Loaded += SplashPage_Loaded;
    }

    private async void SplashPage_Loaded(object sender, System.Windows.RoutedEventArgs e)
    {
        Log.Information("SplashPage loaded - checking game state");
        await _viewModel.CheckAndNavigateAsync();
    }
}
