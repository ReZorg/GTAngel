using System.Windows;
using GTAngel.Organizer.ViewModels;

namespace GTAngel.Organizer;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        DataContext = new MainWindowViewModel();
    }

    private async void Window_Closed(object? sender, EventArgs e)
    {
        if (DataContext is MainWindowViewModel viewModel)
        {
            await viewModel.DisposeAsync();
        }
    }
}
