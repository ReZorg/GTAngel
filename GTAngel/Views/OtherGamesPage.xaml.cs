using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTAngel.ViewModels;

namespace GTAngel.Views;

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
