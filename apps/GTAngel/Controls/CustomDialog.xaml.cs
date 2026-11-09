using System.Windows;
using System.Windows.Controls;

namespace GTAngel.Controls;

/// <summary>
/// Custom dialog overlay control.
/// Replaces: res/layout/custom_dialog.xml + AlertDialog usage throughout the app.
/// Used for: login errors, purchase confirmations, logout confirmation,
///           delete account confirmation, trial over, etc.
/// </summary>
public partial class CustomDialog : UserControl
{
    public event EventHandler? PositiveClicked;
    public event EventHandler? NegativeClicked;

    public CustomDialog()
    {
        InitializeComponent();
    }

    public static CustomDialog Show(string title, string message,
        string positiveText = "OK", string? negativeText = null)
    {
        var dialog = new CustomDialog();
        dialog.TitleText.Text = title;
        dialog.MessageText.Text = message;
        dialog.PositiveButton.Content = positiveText;

        if (negativeText != null)
        {
            dialog.NegativeButton.Content = negativeText;
            dialog.NegativeButton.Visibility = Visibility.Visible;
        }
        else
        {
            dialog.NegativeButton.Visibility = Visibility.Collapsed;
        }

        return dialog;
    }

    private void PositiveButton_Click(object sender, RoutedEventArgs e)
    {
        PositiveClicked?.Invoke(this, EventArgs.Empty);
        Visibility = Visibility.Collapsed;
    }

    private void NegativeButton_Click(object sender, RoutedEventArgs e)
    {
        NegativeClicked?.Invoke(this, EventArgs.Empty);
        Visibility = Visibility.Collapsed;
    }
}
