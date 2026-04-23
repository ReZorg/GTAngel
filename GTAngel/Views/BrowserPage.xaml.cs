using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTAngel.ViewModels;

namespace GTAngel.Views;

/// <summary>
/// In-app browser page with WebView2.
/// Replaces: BrowserScreen.java + res/layout/browser.xml
/// </summary>
public partial class BrowserPage : Page
{
    public BrowserPage()
    {
        InitializeComponent();
        DataContext = App.Services.GetRequiredService<BrowserViewModel>();
    }
}
