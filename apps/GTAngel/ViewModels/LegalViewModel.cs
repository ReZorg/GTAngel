using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.Logging;
using GTAngel.Services;
using GTAngel.Views;

namespace GTAngel.ViewModels;

/// <summary>
/// Legal/EULA page view model.
/// Translated from: FlutterLegalScreen
/// Handles EULA acceptance flow before game launch.
/// </summary>
public partial class LegalViewModel : ObservableObject
{
    private readonly ILogger<LegalViewModel> _logger;
    private readonly AppStateService _state;
    private readonly NavigationService _navigation;

    [ObservableProperty]
    private string _legalText = @"END USER LICENSE AGREEMENT

ROCKSTAR GAMES

Grand Theft Auto III - The Definitive Edition

This End User License Agreement (""EULA"") is a legal agreement between you and Rockstar Games, Inc. (""Rockstar Games"") for the use of Grand Theft Auto III - The Definitive Edition software product.

By installing, copying, or otherwise using this software, you agree to be bound by the terms of this EULA. If you do not agree to the terms of this EULA, do not install or use the software.

1. GRANT OF LICENSE
Rockstar Games grants you a non-exclusive, non-transferable license to use the software for personal, non-commercial purposes.

2. RESTRICTIONS
You may not: (a) copy, modify, or distribute the software; (b) reverse engineer, decompile, or disassemble the software; (c) rent, lease, or lend the software; (d) use the software for commercial purposes.

3. INTELLECTUAL PROPERTY
The software and all copies thereof are proprietary to Rockstar Games and title thereto remains in Rockstar Games. All applicable rights to patents, copyrights, trademarks, and trade secrets in the software are and shall remain in Rockstar Games.

4. DISCLAIMER OF WARRANTIES
THE SOFTWARE IS PROVIDED ""AS IS"" WITHOUT WARRANTY OF ANY KIND.

5. LIMITATION OF LIABILITY
IN NO EVENT SHALL ROCKSTAR GAMES BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES.

6. TERMINATION
This EULA is effective until terminated. It will terminate automatically if you fail to comply with any term of this EULA.

© Rockstar Games, Inc. All rights reserved.";

    public LegalViewModel(
        ILogger<LegalViewModel> logger,
        AppStateService state,
        NavigationService navigation)
    {
        _logger = logger;
        _state = state;
        _navigation = navigation;
    }

    [RelayCommand]
    private void Accept()
    {
        _logger.LogInformation("EULA accepted");
        _state.AcceptEula();
        _navigation.NavigateTo<GamePage>();
    }

    [RelayCommand]
    private void Decline()
    {
        _logger.LogInformation("EULA declined - closing application");
        System.Windows.Application.Current.Shutdown();
    }
}
