using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTA3DE.Wpf.ViewModels;

namespace GTA3DE.Wpf.Views;

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
