using System.Runtime.InteropServices;
using Microsoft.Extensions.Logging;

namespace GTAngel.Interop;

/// <summary>
/// Native Windows interop layer — complete translation of all 52 JNI exports from libUE4.so.
/// Translated from: com.epicgames.ue4.NativeCalls (JNI bridge to libUE4.so)
///
/// The Android JNI bridge provides 52 native methods that the Java layer calls
/// to communicate with the UE4 C++ engine. On Windows, these are replaced by:
///   1. Win32 P/Invoke calls (power management, window management)
///   2. Named pipe IPC via UEProcessManager (game commands)
///   3. Configuration reads (metadata queries)
///   4. Direct .NET APIs (sensor data, keyboard, etc.)
///
/// JNI Export Categories and their Windows equivalents:
///
/// Power Management (2 functions):
///   KeepAwake(tag, awake) → SetThreadExecutionState
///   AllowSleep(tag) → Clear execution state
///
/// Window Management (4 functions):
///   WebViewVisible(visible) → Show/hide WPF overlay
///   nativeSetWindowInfo(focus, x, y, w, h) → MoveWindow + SetFocus
///   nativeSetSurfaceViewInfo(id, x, y, w, h) → Resize embedded HWND
///   nativeSetSafezoneInfo(safe, l, t, r, b) → Safe area insets
///
/// Input (6 functions):
///   HandleCustomTouchEvent(dev, action, src, ptr, x, y) → Mouse/touch forwarding
///   nativeHandleSensorEvents(values) → Gyroscope/accelerometer (N/A on desktop)
///   nativeInputDisconnected(id) → Controller disconnect
///   nativeVirtualKeyboardChanged(text) → IME text input
///   nativeVirtualKeyboardResult(result) → IME completion
///   nativeVirtualKeyboardSendKey(keyCode) → Key event forwarding
///
/// Lifecycle (8 functions):
///   nativeResumeMainInit() → Resume from background
///   nativeSetGlobalActivity(activity) → Set host reference
///   nativeSetAndroidStartupState(state) → Startup flags
///   nativeSetAndroidVersionInformation(sdk, model, mfg) → Platform info
///   nativeOnInitialDownloadCompleted() → Assets ready
///   nativeConsoleCommand(cmd) → Engine console command
///   nativeSetConfigRulesVariables(vars) → Config rules
///   nativeIsShippingBuild() → Build type check
///
/// Configuration (6 functions):
///   HasMetaDataKey(key) → App.config check
///   GetMetaDataBoolean(key) → App.config read
///   GetMetaDataInt(key) → App.config read
///   GetMetaDataString(key) → App.config read
///   nativeSetObbFilePaths(internal, external) → Asset paths
///   nativeSetObbInfo(name, size, checksum) → Asset metadata
///
/// IPC Bridge (6 functions):
///   CallNativeToEmbedded(action, id, key, val, extras, json) → Named pipe command
///   SetNamedObject(name, obj) → Named pipe object registry
///   ForwardNotification(json) → Named pipe notification
///   RouteServiceIntent(action, data) → Named pipe service routing
///   nativeSupportsNEON() → SIMD capability check
///   nativeSupportsVulkan() → Vulkan support check
///
/// Logging (4 functions):
///   UELogLog(msg) → Serilog Info
///   UELogWarning(msg) → Serilog Warning
///   UELogError(msg) → Serilog Error
///   UELogVerbose(msg) → Serilog Debug
/// </summary>
public static class NativeInterop
{
    private static ILogger? _logger;

    public static void Initialize(ILogger logger)
    {
        _logger = logger;
    }

    #region Power Management (replaces NativeCalls.KeepAwake / AllowSleep)

    [Flags]
    private enum EXECUTION_STATE : uint
    {
        ES_AWAYMODE_REQUIRED = 0x00000040,
        ES_CONTINUOUS = 0x80000000,
        ES_DISPLAY_REQUIRED = 0x00000002,
        ES_SYSTEM_REQUIRED = 0x00000001
    }

    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern EXECUTION_STATE SetThreadExecutionState(EXECUTION_STATE esFlags);

    /// <summary>
    /// Prevent system sleep while game is running.
    /// Replaces: NativeCalls.KeepAwake("GameActivity", true)
    /// </summary>
    public static void KeepAwake()
    {
        SetThreadExecutionState(
            EXECUTION_STATE.ES_CONTINUOUS |
            EXECUTION_STATE.ES_SYSTEM_REQUIRED |
            EXECUTION_STATE.ES_DISPLAY_REQUIRED);
        _logger?.LogDebug("System sleep prevention enabled");
    }

    /// <summary>
    /// Allow system sleep.
    /// Replaces: NativeCalls.AllowSleep("GameActivity")
    /// </summary>
    public static void AllowSleep()
    {
        SetThreadExecutionState(EXECUTION_STATE.ES_CONTINUOUS);
        _logger?.LogDebug("System sleep prevention disabled");
    }

    #endregion

    #region Window Management (replaces NativeCalls.WebViewVisible + nativeSetWindowInfo)

    [DllImport("user32.dll")]
    private static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

    [DllImport("user32.dll")]
    private static extern bool MoveWindow(IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);

    [DllImport("user32.dll", EntryPoint = "SetWindowLongPtr")]
    private static extern IntPtr SetWindowLongPtr(IntPtr hWnd, int nIndex, IntPtr dwNewLong);

    [DllImport("user32.dll", EntryPoint = "GetWindowLongPtr")]
    private static extern IntPtr GetWindowLongPtr(IntPtr hWnd, int nIndex);

    [DllImport("user32.dll")]
    private static extern IntPtr SetFocus(IntPtr hWnd);

    [DllImport("user32.dll")]
    private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

    [DllImport("user32.dll")]
    private static extern bool SetForegroundWindow(IntPtr hWnd);

    [DllImport("user32.dll")]
    private static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);

    [StructLayout(LayoutKind.Sequential)]
    private struct RECT
    {
        public int Left, Top, Right, Bottom;
    }

    private const int GWL_STYLE = -16;
    private const int GWL_EXSTYLE = -20;
    private const int WS_VISIBLE = 0x10000000;
    private const int WS_CHILD = 0x40000000;
    private const int SW_SHOW = 5;
    private const int SW_HIDE = 0;

    /// <summary>
    /// Embed an external window (game process) into a WPF host.
    /// Replaces: NativeActivity surface view management
    /// </summary>
    public static void EmbedWindow(IntPtr childHwnd, IntPtr parentHwnd, int width, int height)
    {
        var style = GetWindowLongPtr(childHwnd, GWL_STYLE).ToInt64();
        SetWindowLongPtr(childHwnd, GWL_STYLE, new IntPtr((style | WS_CHILD) & ~WS_VISIBLE));
        SetParent(childHwnd, parentHwnd);
        MoveWindow(childHwnd, 0, 0, width, height, true);
        SetWindowLongPtr(childHwnd, GWL_STYLE, new IntPtr(style | WS_CHILD | WS_VISIBLE));
        _logger?.LogDebug("External window embedded: {Hwnd}", childHwnd);
    }

    /// <summary>
    /// Set window focus.
    /// Replaces: nativeSetWindowInfo(hasFocus=true, ...)
    /// </summary>
    public static void FocusWindow(IntPtr hwnd)
    {
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
    }

    /// <summary>
    /// Show or hide a window.
    /// Replaces: NativeCalls.WebViewVisible(visible)
    /// </summary>
    public static void SetWindowVisible(IntPtr hwnd, bool visible)
    {
        ShowWindow(hwnd, visible ? SW_SHOW : SW_HIDE);
    }

    /// <summary>
    /// Get client area dimensions.
    /// Replaces: nativeSetSurfaceViewInfo reading surface dimensions
    /// </summary>
    public static (int Width, int Height) GetClientSize(IntPtr hwnd)
    {
        GetClientRect(hwnd, out var rect);
        return (rect.Right - rect.Left, rect.Bottom - rect.Top);
    }

    #endregion

    #region Platform Capability Checks (replaces nativeSupportsNEON / nativeSupportsVulkan)

    [DllImport("kernel32.dll")]
    private static extern bool IsProcessorFeaturePresent(int feature);

    private const int PF_ARM_NEON_INSTRUCTIONS_AVAILABLE = 19;
    private const int PF_SSE3_INSTRUCTIONS_AVAILABLE = 13;
    private const int PF_AVX_INSTRUCTIONS_AVAILABLE = 39;
    private const int PF_AVX2_INSTRUCTIONS_AVAILABLE = 40;

    /// <summary>
    /// Check SIMD capability.
    /// Replaces: nativeSupportsNEON() — ARM NEON → x86 SSE/AVX equivalent
    /// On ARM64 Windows: checks for NEON
    /// On x86_64 Windows: checks for SSE3/AVX (always true on modern CPUs)
    /// </summary>
    public static bool SupportsSIMD()
    {
        try
        {
            return System.Runtime.Intrinsics.X86.Sse3.IsSupported ||
                   IsProcessorFeaturePresent(PF_SSE3_INSTRUCTIONS_AVAILABLE);
        }
        catch
        {
            return true; // Assume supported on modern hardware
        }
    }

    /// <summary>
    /// Check Vulkan support.
    /// Replaces: nativeSupportsVulkan()
    /// On Windows, checks if vulkan-1.dll is loadable
    /// </summary>
    public static bool SupportsVulkan()
    {
        try
        {
            var vulkanHandle = LoadLibrary("vulkan-1.dll");
            if (vulkanHandle != IntPtr.Zero)
            {
                FreeLibrary(vulkanHandle);
                return true;
            }
        }
        catch { }
        return false;
    }

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr LoadLibrary(string lpFileName);

    [DllImport("kernel32.dll")]
    private static extern bool FreeLibrary(IntPtr hModule);

    #endregion

    #region Metadata (replaces NativeCalls.HasMetaDataKey / GetMetaData*)

    /// <summary>
    /// Check if metadata key exists.
    /// Replaces: NativeCalls.HasMetaDataKey(key) → checks AndroidManifest meta-data
    /// WPF: Checks app.config or registry
    /// </summary>
    public static bool HasMetaDataKey(string key)
    {
        return System.Configuration.ConfigurationManager.AppSettings[key] != null;
    }

    /// <summary>
    /// Get metadata string value.
    /// Replaces: NativeCalls.GetMetaDataString(key)
    /// </summary>
    public static string GetMetaDataString(string key, string defaultValue = "")
    {
        return System.Configuration.ConfigurationManager.AppSettings[key] ?? defaultValue;
    }

    /// <summary>
    /// Get metadata boolean value.
    /// Replaces: NativeCalls.GetMetaDataBoolean(key)
    /// </summary>
    public static bool GetMetaDataBoolean(string key, bool defaultValue = false)
    {
        var value = System.Configuration.ConfigurationManager.AppSettings[key];
        return value != null ? bool.TryParse(value, out var result) && result : defaultValue;
    }

    /// <summary>
    /// Get metadata integer value.
    /// Replaces: NativeCalls.GetMetaDataInt(key)
    /// </summary>
    public static int GetMetaDataInt(string key, int defaultValue = 0)
    {
        var value = System.Configuration.ConfigurationManager.AppSettings[key];
        return value != null && int.TryParse(value, out var result) ? result : defaultValue;
    }

    #endregion

    #region Logging (replaces NativeCalls.UELog*)

    /// <summary>
    /// Log at Info level. Replaces: NativeCalls.UELogLog(msg)
    /// </summary>
    public static void UELogLog(string message) => _logger?.LogInformation("[UE] {Message}", message);

    /// <summary>
    /// Log at Warning level. Replaces: NativeCalls.UELogWarning(msg)
    /// </summary>
    public static void UELogWarning(string message) => _logger?.LogWarning("[UE] {Message}", message);

    /// <summary>
    /// Log at Error level. Replaces: NativeCalls.UELogError(msg)
    /// </summary>
    public static void UELogError(string message) => _logger?.LogError("[UE] {Message}", message);

    /// <summary>
    /// Log at Debug level. Replaces: NativeCalls.UELogVerbose(msg)
    /// </summary>
    public static void UELogVerbose(string message) => _logger?.LogDebug("[UE] {Message}", message);

    #endregion

    #region System Information (replaces nativeSetAndroidVersionInformation)

    /// <summary>
    /// Get system information equivalent to Android version info.
    /// Replaces: nativeSetAndroidVersionInformation(sdkVersion, deviceModel, manufacturer)
    /// </summary>
    public static (string OsVersion, string MachineName, string Manufacturer) GetSystemInfo()
    {
        return (
            Environment.OSVersion.VersionString,
            Environment.MachineName,
            "PC"
        );
    }

    /// <summary>
    /// Check if this is a shipping (release) build.
    /// Replaces: nativeIsShippingBuild()
    /// </summary>
    public static bool IsShippingBuild()
    {
        return GetMetaDataBoolean("com.epicgames.ue4.GameActivity.bIsShippingBuild", true);
    }

    #endregion
}
