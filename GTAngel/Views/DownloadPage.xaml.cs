using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using GTA3DE.Wpf.ViewModels;

namespace GTA3DE.Wpf.Views;

/// <summary>
/// Asset download progress page.
/// Replaces: DownloaderActivity (com.rockstargames.gta3.p011de.DownloaderActivity)
/// Layout: downloader_progress.xml
/// </summary>
public partial class DownloadPage : Page
{
    public DownloadPage()
    {
        InitializeComponent();
        DataContext = App.Services.GetRequiredService<DownloadViewModel>();
    }
}
