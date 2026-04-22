using System.Windows;
using System.Windows.Input;
using Microsoft.Extensions.DependencyInjection;
using GTA3DE.Wpf.Services;
using GTA3DE.Wpf.ViewModels;
using Serilog;

namespace GTA3DE.Wpf.Views;

/// <summary>
/// Main application window. Replaces GameActivity (NativeActivity) from UE4.
/// Manages frame-based navigation between pages (replaces Android Intent system).
/// Lifecycle: onCreate → OnLoaded, onResume → Activated, onPause → Deactivated,
///            onDestroy → Closing, onSaveInstanceState → (auto via NavigationService)
/// </summary>
public partial class MainWindow : Window
{
    private readonly MainWindowViewModel _viewModel;
    private readonly NavigationService _navigationService;
    private bool _isFullscreen = true;

    public MainWindow()
    {
        InitializeComponent();

        _viewModel = App.Services.GetRequiredService<MainWindowViewModel>();
        _navigationService = App.Services.GetRequiredService<NavigationService>();
        DataContext = _viewModel;

        // Register the Frame with NavigationService (replaces FragmentManager)
        _navigationService.RegisterFrame(MainFrame);

        Loaded += MainWindow_Loaded;
        Closing += MainWindow_Closing;
        StateChanged += MainWindow_StateChanged;
    }

    /// <summary>
    /// Replaces GameActivity.onCreate() + Rockstar.setup()
    /// </summary>
    private async void MainWindow_Loaded(object sender, RoutedEventArgs e)
    {
        Log.Information("MainWindow loaded - navigating to splash");

        // Navigate to splash page first (replaces FlutterGateScreen)
        _navigationService.NavigateTo<SplashPage>();

        // Initialize game services in background
        await _viewModel.InitializeAsync();
    }

    /// <summary>
    /// Replaces GameActivity.onDestroy()
    /// </summary>
    private void MainWindow_Closing(object? sender, System.ComponentModel.CancelEventArgs e)
    {
        Log.Information("MainWindow closing - cleaning up");
        _viewModel.Cleanup();
    }

    /// <summary>
    /// Replaces GameActivity.onWindowFocusChanged()
    /// </summary>
    private void MainWindow_StateChanged(object? sender, EventArgs e)
    {
        if (WindowState == WindowState.Minimized)
        {
            // Replaces onPause - pause game audio
            _viewModel.OnPause();
        }
        else
        {
            // Replaces onResume - resume game audio
            _viewModel.OnResume();
        }
    }

    private void MainFrame_Navigated(object sender, System.Windows.Navigation.NavigationEventArgs e)
    {
        // Update title bar visibility based on current page
        var showTitleBar = e.Content is not SplashPage and not GamePage;
        // GTAngel: Show title bar for GTAngelPage
        if (e.Content is GTAngelPage) showTitleBar = true;
        TitleBar.Visibility = showTitleBar ? Visibility.Visible : Visibility.Collapsed;
    }

    /// <summary>
    /// Replaces Android back button handling (NativeCalls.AllowJavaBackButtonEvent)
    /// </summary>
    private void Window_KeyDown(object sender, KeyEventArgs e)
    {
        if (e.Key == Key.Escape)
        {
            if (_navigationService.CanGoBack)
            {
                _navigationService.GoBack();
            }
            else
            {
                ToggleFullscreen();
            }
            e.Handled = true;
        }
        else if (e.Key == Key.F11)
        {
            ToggleFullscreen();
            e.Handled = true;
        }
    }

    private void ToggleFullscreen()
    {
        if (_isFullscreen)
        {
            WindowStyle = WindowStyle.SingleBorderWindow;
            WindowState = WindowState.Normal;
            TitleBar.Visibility = Visibility.Visible;
        }
        else
        {
            WindowStyle = WindowStyle.None;
            WindowState = WindowState.Maximized;
        }
        _isFullscreen = !_isFullscreen;
    }

    private void MinimizeButton_Click(object sender, RoutedEventArgs e) =>
        WindowState = WindowState.Minimized;

    private void MaximizeButton_Click(object sender, RoutedEventArgs e) =>
        ToggleFullscreen();

    private void CloseButton_Click(object sender, RoutedEventArgs e) =>
        Close();
}
