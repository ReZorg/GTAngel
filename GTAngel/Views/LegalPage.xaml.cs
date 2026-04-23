using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTAngel.ViewModels;

namespace GTAngel.Views;

/// <summary>
/// Legal/EULA acceptance page.
/// Replaces: FlutterLegalScreen
/// </summary>
public partial class LegalPage : Page
{
    public LegalPage()
    {
        InitializeComponent();
        DataContext = App.Services.GetRequiredService<LegalViewModel>();
    }
}
