using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTAngel.ViewModels;

namespace GTAngel.Controls;

/// <summary>
/// Trial mode banner overlay control.
/// Replaces: TrialBanner.java + res/layout/trialbanner.xml
/// Shows remaining trial time and unlock button.
/// </summary>
public partial class TrialBannerControl : UserControl
{
    public TrialBannerControl()
    {
        InitializeComponent();
        DataContext = App.Services.GetRequiredService<TrialBannerViewModel>();
    }
}
