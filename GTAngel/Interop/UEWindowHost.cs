using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;
using Microsoft.Extensions.Logging;

namespace GTA3DE.Wpf.Interop;

/// <summary>
/// WPF HwndHost that embeds the UE4/UE5 game engine window as a child window.
/// Translated from: Android SurfaceView inside GameActivity's FrameLayout.
///
/// On Android:
///   GameActivity (NativeActivity) creates a SurfaceView
///   UE4 renders to the SurfaceView's ANativeWindow via EGL
///   The SurfaceView is managed by the Activity's window hierarchy
///
/// On Windows (WPF):
///   The UE4/UE5 game creates its own HWND window
///   We use Win32 SetParent() to reparent it as a child of our WPF control
///   We use SetWindowLong() to change its style to WS_CHILD
///   We forward resize events to keep the game window filling our host
///
/// This is the standard pattern for embedding DirectX/OpenGL/Vulkan windows
/// inside WPF applications (used by game launchers, level editors, etc.)
/// </summary>
public class UEWindowHost : HwndHost
{
    private readonly ILogger _logger;
    private IntPtr _engineHwnd;
    private IntPtr _hostHwnd;
    private bool _isEmbedded;

    /// <summary>The embedded engine window handle</summary>
    public IntPtr EngineHandle => _engineHwnd;

    /// <summary>Whether the engine window is currently embedded</summary>
    public bool IsEmbedded => _isEmbedded;

    /// <summary>Fires when the engine window has been successfully embedded</summary>
    public event EventHandler? WindowEmbedded;

    /// <summary>Fires when the engine window has been detached</summary>
    public event EventHandler? WindowDetached;

    public UEWindowHost(ILogger logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Embed the UE engine window into this host control.
    /// Replaces: SurfaceView.surfaceCreated() → UE4 attaches to ANativeWindow
    /// </summary>
    public bool EmbedEngineWindow(IntPtr engineHwnd)
    {
        if (engineHwnd == IntPtr.Zero)
        {
            _logger.LogError("Cannot embed: invalid engine window handle");
            return false;
        }

        if (_hostHwnd == IntPtr.Zero)
        {
            _logger.LogError("Cannot embed: host window not yet created");
            return false;
        }

        _engineHwnd = engineHwnd;

        try
        {
            // Remove WS_POPUP and WS_OVERLAPPEDWINDOW, add WS_CHILD
            // This is equivalent to Android's SurfaceView being a child of the Activity's DecorView
            var style = GetWindowLongPtr(_engineHwnd, GWL_STYLE).ToInt64();
            style &= ~(WS_POPUP | WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION);
            style |= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
            SetWindowLongPtr(_engineHwnd, GWL_STYLE, new IntPtr(style));

            // Remove WS_EX_APPWINDOW so it doesn't show in taskbar
            var exStyle = GetWindowLongPtr(_engineHwnd, GWL_EXSTYLE).ToInt64();
            exStyle &= ~WS_EX_APPWINDOW;
            exStyle |= WS_EX_TOOLWINDOW;
            SetWindowLongPtr(_engineHwnd, GWL_EXSTYLE, new IntPtr(exStyle));

            // Reparent the engine window into our host
            // Replaces: SurfaceView being added to GameActivity's content view
            SetParent(_engineHwnd, _hostHwnd);

            // Resize to fill the host
            ResizeEngineWindow();

            _isEmbedded = true;
            WindowEmbedded?.Invoke(this, EventArgs.Empty);

            _logger.LogInformation("Engine window embedded: HWND {Hwnd} → Host {Host}",
                _engineHwnd, _hostHwnd);
            return true;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to embed engine window");
            return false;
        }
    }

    /// <summary>
    /// Detach the engine window from this host.
    /// Replaces: SurfaceView.surfaceDestroyed()
    /// </summary>
    public void DetachEngineWindow()
    {
        if (!_isEmbedded || _engineHwnd == IntPtr.Zero) return;

        try
        {
            // Restore the window to a top-level window
            SetParent(_engineHwnd, IntPtr.Zero);

            var style = GetWindowLongPtr(_engineHwnd, GWL_STYLE).ToInt64();
            style &= ~WS_CHILD;
            style |= WS_OVERLAPPEDWINDOW | WS_VISIBLE;
            SetWindowLongPtr(_engineHwnd, GWL_STYLE, new IntPtr(style));

            _isEmbedded = false;
            WindowDetached?.Invoke(this, EventArgs.Empty);

            _logger.LogInformation("Engine window detached");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to detach engine window");
        }
    }

    /// <summary>
    /// Resize the embedded engine window to fill the host.
    /// Replaces: SurfaceView.surfaceChanged(holder, format, width, height)
    /// which triggers nativeSetSurfaceViewInfo and nativeSetWindowInfo
    /// </summary>
    public void ResizeEngineWindow()
    {
        if (!_isEmbedded || _engineHwnd == IntPtr.Zero) return;

        var width = (int)ActualWidth;
        var height = (int)ActualHeight;

        if (width > 0 && height > 0)
        {
            MoveWindow(_engineHwnd, 0, 0, width, height, true);
            _logger.LogDebug("Engine window resized: {W}x{H}", width, height);
        }
    }

    /// <summary>
    /// Send focus to the engine window.
    /// Replaces: GameActivity.onWindowFocusChanged(true)
    /// </summary>
    public void FocusEngineWindow()
    {
        if (_isEmbedded && _engineHwnd != IntPtr.Zero)
        {
            SetFocus(_engineHwnd);
        }
    }

    #region HwndHost Overrides

    protected override HandleRef BuildWindowCore(HandleRef hwndParent)
    {
        // Create a simple host window that will contain the engine window
        _hostHwnd = CreateWindowEx(
            0,
            "static",
            "",
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
            0, 0,
            (int)ActualWidth, (int)ActualHeight,
            hwndParent.Handle,
            IntPtr.Zero,
            IntPtr.Zero,
            IntPtr.Zero);

        _logger.LogDebug("Host window created: HWND {Hwnd}", _hostHwnd);
        return new HandleRef(this, _hostHwnd);
    }

    protected override void DestroyWindowCore(HandleRef hwnd)
    {
        if (_isEmbedded)
        {
            DetachEngineWindow();
        }
        DestroyWindow(hwnd.Handle);
        _hostHwnd = IntPtr.Zero;
    }

    protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
    {
        base.OnRenderSizeChanged(sizeInfo);
        ResizeEngineWindow();
    }

    #endregion

    #region Win32 P/Invoke

    private const int GWL_STYLE = -16;
    private const int GWL_EXSTYLE = -20;

    private const long WS_CHILD = 0x40000000L;
    private const long WS_VISIBLE = 0x10000000L;
    private const long WS_POPUP = 0x80000000L;
    private const long WS_OVERLAPPEDWINDOW = 0x00CF0000L;
    private const long WS_THICKFRAME = 0x00040000L;
    private const long WS_CAPTION = 0x00C00000L;
    private const long WS_CLIPCHILDREN = 0x02000000L;
    private const long WS_CLIPSIBLINGS = 0x04000000L;
    private const long WS_EX_APPWINDOW = 0x00040000L;
    private const long WS_EX_TOOLWINDOW = 0x00000080L;

    [DllImport("user32.dll", SetLastError = true)]
    private static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

    [DllImport("user32.dll", SetLastError = true)]
    private static extern bool MoveWindow(IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);

    [DllImport("user32.dll", SetLastError = true)]
    private static extern IntPtr SetFocus(IntPtr hWnd);

    [DllImport("user32.dll", SetLastError = true)]
    private static extern bool DestroyWindow(IntPtr hWnd);

    [DllImport("user32.dll", SetLastError = true)]
    private static extern IntPtr CreateWindowEx(
        int dwExStyle, string lpClassName, string lpWindowName,
        long dwStyle, int x, int y, int nWidth, int nHeight,
        IntPtr hWndParent, IntPtr hMenu, IntPtr hInstance, IntPtr lpParam);

    // Use IntPtr-sized versions for 64-bit compatibility
    [DllImport("user32.dll", EntryPoint = "GetWindowLongPtr", SetLastError = true)]
    private static extern IntPtr GetWindowLongPtr(IntPtr hWnd, int nIndex);

    [DllImport("user32.dll", EntryPoint = "SetWindowLongPtr", SetLastError = true)]
    private static extern IntPtr SetWindowLongPtr(IntPtr hWnd, int nIndex, IntPtr dwNewLong);

    #endregion
}
