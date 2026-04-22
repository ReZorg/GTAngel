using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTA3DE.Wpf.ViewModels;

namespace GTA3DE.Wpf.Views;

/// <summary>
/// Other Rockstar games showcase page.
/// Replaces: FlutterOtherGamesScreen
/// </summary>
public partial class OtherGamesPage : Page
{
    public OtherGamesPage()
    {
        InitializeComponent();
        DataContext = App.Services.GetRequiredService<OtherGamesViewModel>();
    }
}
