using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTAngel.ViewModels;

namespace GTAngel.Views;

/// <summary>
/// Settings page with graphics, audio, controls, language, and account tabs.
/// Replaces: FlutterOptionsWithProductBaseScreen
/// </summary>
public partial class SettingsPage : Page
{
    public SettingsPage()
    {
        InitializeComponent();
        DataContext = App.Services.GetRequiredService<SettingsViewModel>();
    }
}
