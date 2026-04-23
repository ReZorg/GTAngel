using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTAngel.ViewModels;

namespace GTAngel.Views;

/// <summary>
/// GTAngel Guardian Angel Cognitive Orchestrator Page.
///
/// Composition: /dte-ksm-evo-autogenesis ( /gta3-ue5-wpf ) → "GTAngel"
///
/// This page is the primary UI surface for the GTAngel autogenesis loop:
///   - 6-step Autogenesis Loop stepper (Observe → Edit → Run → Assess → Decide → Log)
///   - KSM 12-step Evolution Cycle tracker
///   - Alexander's 15 Properties live coherence scores
///   - Experiment log (results.tsv mirror)
///   - Guardian Angel status header with safety constraint indicators
///   - Live coherence and metric charts
/// </summary>
public partial class GTAngelPage : Page
{
    private readonly GTAngelViewModel _viewModel;

    public GTAngelPage()
    {
        InitializeComponent();
        _viewModel = App.Services.GetRequiredService<GTAngelViewModel>();
        DataContext = _viewModel;
    }
}
