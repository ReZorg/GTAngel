using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using GTAngel.Interop;
using GTAngel.ViewModels;

namespace GTAngel.Views;

/// <summary>
/// Main game viewport page with UE engine window hosting.
/// Replaces: GameActivity UE4 NativeActivity surface rendering.
///
/// On Android:
///   GameActivity extends NativeActivity
///   NativeActivity creates a SurfaceView (ANativeWindow)
///   libUE4.so renders to the SurfaceView via EGL/Vulkan
///   Touch events are forwarded via NativeCalls.HandleCustomTouchEvent
///
/// On Windows (WPF):
///   GamePage hosts a UEWindowHost (HwndHost)
///   UEProcessManager launches the game .exe
///   The game's HWND is embedded via Win32 SetParent
///   Input events are forwarded via named pipe IPC
///   Resize events trigger nativeSetWindowInfo equivalent
/// </summary>
public partial class GamePage : Page
{
    private readonly GameViewModel _viewModel;
    private readonly ILogger<GamePage> _logger;
    private UEWindowHost? _windowHost;

    public GamePage()
    {
        InitializeComponent();

        _viewModel = App.Services.GetRequiredService<GameViewModel>();
        _logger = App.Services.GetRequiredService<ILogger<GamePage>>();
        DataContext = _viewModel;

        // Subscribe to engine events
        _viewModel.EngineWindowReady += OnEngineWindowReady;
        _viewModel.EngineDetached += OnEngineDetached;
    }

    /// <summary>
    /// Page loaded — initialize the viewport.
    /// Replaces: GameActivity.onCreate() → setContentView + SurfaceView setup
    /// </summary>
    private void GamePage_Loaded(object sender, RoutedEventArgs e)
    {
        _logger.LogInformation("GamePage loaded — viewport ready for engine");

        // Discover local UE projects
        _viewModel.DiscoverProjects();

        // Register keyboard handler for input forwarding
        // Replaces: GameActivity.dispatchKeyEvent → nativeVirtualKeyboardSendKey
        var window = Window.GetWindow(this);
        if (window != null)
        {
            window.PreviewKeyDown += OnPreviewKeyDown;
            window.PreviewKeyUp += OnPreviewKeyUp;
        }
    }

    /// <summary>
    /// Page unloaded — cleanup engine hosting.
    /// Replaces: GameActivity.onDestroy()
    /// </summary>
    private async void GamePage_Unloaded(object sender, RoutedEventArgs e)
    {
        _logger.LogInformation("GamePage unloaded — cleaning up engine");

        var window = Window.GetWindow(this);
        if (window != null)
        {
            window.PreviewKeyDown -= OnPreviewKeyDown;
            window.PreviewKeyUp -= OnPreviewKeyUp;
        }

        // Detach the engine window before the page is destroyed
        _windowHost?.DetachEngineWindow();

        if (EngineHostContainer.Child is UEWindowHost host)
        {
            EngineHostContainer.Child = null;
            host.Dispose();
        }

        await _viewModel.StopEngineAsync();
    }

    /// <summary>
    /// Called when the UE engine window is ready to be embedded.
    /// Replaces: SurfaceView.surfaceCreated() callback
    /// </summary>
    private void OnEngineWindowReady(object? sender, IntPtr engineHwnd)
    {
        Dispatcher.Invoke(() =>
        {
            _logger.LogInformation("Embedding engine window: HWND {Hwnd}", engineHwnd);

            // Create the HwndHost and add it to the visual tree
            _windowHost = new UEWindowHost(
                App.Services.GetRequiredService<ILogger<UEWindowHost>>());

            _windowHost.WindowEmbedded += (s, e) =>
            {
                _logger.LogInformation("Engine window successfully embedded");
                PreLaunchOverlay.Visibility = Visibility.Collapsed;
                HudOverlay.Visibility = Visibility.Visible;
            };

            _windowHost.WindowDetached += (s, e) =>
            {
                _logger.LogInformation("Engine window detached");
                PreLaunchOverlay.Visibility = Visibility.Visible;
                HudOverlay.Visibility = Visibility.Collapsed;
            };

            // Add the host to the container
            EngineHostContainer.Child = _windowHost;

            // Embed the engine window after the host is in the visual tree
            Dispatcher.BeginInvoke(new Action(() =>
            {
                _windowHost.EmbedEngineWindow(engineHwnd);
                _windowHost.FocusEngineWindow();
            }), System.Windows.Threading.DispatcherPriority.Loaded);
        });
    }

    /// <summary>
    /// Called when the engine window is detached (process exited).
    /// Replaces: SurfaceView.surfaceDestroyed()
    /// </summary>
    private void OnEngineDetached(object? sender, EventArgs e)
    {
        Dispatcher.Invoke(() =>
        {
            _windowHost?.DetachEngineWindow();
            PreLaunchOverlay.Visibility = Visibility.Visible;
            HudOverlay.Visibility = Visibility.Collapsed;
        });
    }

    /// <summary>
    /// Forward keyboard input to the engine.
    /// Replaces: GameActivity.dispatchKeyEvent → nativeVirtualKeyboardSendKey
    /// </summary>
    private async void OnPreviewKeyDown(object sender, KeyEventArgs e)
    {
        if (_windowHost?.IsEmbedded == true)
        {
            // Let the engine handle the input directly via its embedded HWND
            // Only intercept special keys
            if (e.Key == Key.F11)
            {
                // Toggle fullscreen (handled by MainWindow)
                return;
            }

            if (e.Key == Key.Escape)
            {
                // Show/hide the overlay
                if (HudOverlay.Visibility == Visibility.Visible)
                {
                    PreLaunchOverlay.Visibility = Visibility.Visible;
                    HudOverlay.Visibility = Visibility.Collapsed;
                }
                else
                {
                    PreLaunchOverlay.Visibility = Visibility.Collapsed;
                    HudOverlay.Visibility = Visibility.Visible;
                    _windowHost.FocusEngineWindow();
                }
                e.Handled = true;
            }
        }
    }

    private void OnPreviewKeyUp(object sender, KeyEventArgs e)
    {
        // Key up events are forwarded directly to the embedded HWND
    }
}
