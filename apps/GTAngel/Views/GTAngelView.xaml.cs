using System.Windows.Controls;

namespace GTAngel.Views;

/// <summary>
/// GTAngel Guardian Angel View — UserControl for embedding in TrainingDashboard tab.
///
/// Composition: /dte-ksm-evo-autogenesis ( /gta3-ue5-wpf ) → "GTAngel"
///
/// The DataContext is set via XAML to GTAngelViewModel (direct instantiation pattern,
/// consistent with GameRuntimeView). The ViewModel resolves GTAngelService internally
/// via App.Services DI container.
/// </summary>
public partial class GTAngelView : UserControl
{
    public GTAngelView()
    {
        InitializeComponent();
    }
}
