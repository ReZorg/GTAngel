using System.Windows.Controls;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Frame-based navigation service.
/// Replaces: Android Intent-based navigation (startActivity/startActivityForResult)
///           + FragmentManager transactions
///           + NavHostFragment/NavigationGraph
/// 
/// Android navigation patterns mapped:
///   startActivity(new Intent(this, TargetActivity.class)) → NavigateTo<TargetPage>()
///   finish() → GoBack()
///   setResult(RESULT_OK, data) → NavigationResult event
///   FragmentTransaction.replace() → Frame.Navigate()
///   NavController.navigate(R.id.action) → NavigateTo<Page>()
/// </summary>
public class NavigationService
{
    private readonly ILogger<NavigationService> _logger;
    private Frame? _frame;

    public NavigationService(ILogger<NavigationService> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Register the main Frame for navigation.
    /// Replaces: setting up NavHostFragment in activity layout.
    /// </summary>
    public void RegisterFrame(Frame frame)
    {
        _frame = frame;
        _logger.LogDebug("Navigation frame registered");
    }

    /// <summary>
    /// Navigate to a page by type.
    /// Replaces: startActivity(new Intent(context, ActivityClass))
    /// </summary>
    public void NavigateTo<TPage>() where TPage : Page, new()
    {
        if (_frame == null)
        {
            _logger.LogWarning("Navigation frame not registered");
            return;
        }

        var page = new TPage();
        _frame.Navigate(page);
        _logger.LogInformation("Navigated to {PageType}", typeof(TPage).Name);
    }

    /// <summary>
    /// Navigate to a page instance.
    /// </summary>
    public void NavigateTo(Page page)
    {
        if (_frame == null)
        {
            _logger.LogWarning("Navigation frame not registered");
            return;
        }

        _frame.Navigate(page);
        _logger.LogInformation("Navigated to {PageType}", page.GetType().Name);
    }

    /// <summary>
    /// Go back to previous page.
    /// Replaces: finish() or onBackPressed()
    /// </summary>
    public void GoBack()
    {
        if (_frame?.CanGoBack == true)
        {
            _frame.GoBack();
            _logger.LogDebug("Navigated back");
        }
    }

    /// <summary>
    /// Whether back navigation is possible.
    /// Replaces: checking back stack depth.
    /// </summary>
    public bool CanGoBack => _frame?.CanGoBack ?? false;

    /// <summary>
    /// Clear navigation history.
    /// Replaces: Intent.FLAG_ACTIVITY_CLEAR_TOP | FLAG_ACTIVITY_NEW_TASK
    /// </summary>
    public void ClearHistory()
    {
        if (_frame == null) return;

        while (_frame.CanGoBack)
        {
            _frame.RemoveBackEntry();
        }
        _logger.LogDebug("Navigation history cleared");
    }
}
