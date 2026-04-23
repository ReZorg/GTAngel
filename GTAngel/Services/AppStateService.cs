using Microsoft.Extensions.Logging;
using GTAngel.Models;

namespace GTAngel.Services;

/// <summary>
/// Application state service.
/// Translated from: rockstarmobile/GCState (singleton state manager)
/// Manages user authentication state, EULA acceptance, guest mode, and game state.
/// 
/// Original Android state fields:
///   GCState.user() → current RockstarUser
///   GCState.gameTicket() → current GameTicket
///   GCState.isLoggedIn() → auth check
///   GCState.isSubscribed() → subscription check
///   Rockstar.stateUpdated() → state change callback
/// </summary>
public class AppStateService
{
    private readonly ILogger<AppStateService> _logger;

    /// <summary>Current authenticated user (replaces GCState.user())</summary>
    public RockstarUser? CurrentUser { get; private set; }

    /// <summary>Current game ticket (replaces GCState.gameTicket())</summary>
    public GameTicket? CurrentGameTicket { get; private set; }

    /// <summary>Whether EULA has been accepted</summary>
    public bool HasAcceptedEula { get; private set; }

    /// <summary>Whether running in guest/offline mode</summary>
    public bool IsGuestMode { get; private set; }

    /// <summary>Whether user is authenticated (replaces GCState.isLoggedIn())</summary>
    public bool IsLoggedIn => CurrentUser != null;

    /// <summary>Whether user has active subscription (replaces GCState.isSubscribed())</summary>
    public bool IsSubscribed => CurrentUser?.Subscription == RockstarUser.SubscriptionState.Active;

    /// <summary>Whether user owns the full game</summary>
    public bool IsGameOwner => CurrentUser?.IsGameOwner ?? false;

    /// <summary>State change event (replaces Rockstar.stateUpdated() callback)</summary>
    public event Action? StateChanged;

    public AppStateService(ILogger<AppStateService> logger)
    {
        _logger = logger;
    }

    public void SetUser(RockstarUser user)
    {
        CurrentUser = user;
        _logger.LogInformation("User set: {RockstarId}", user.RockstarId);
        StateChanged?.Invoke();
    }

    public void SetGameTicket(GameTicket ticket)
    {
        CurrentGameTicket = ticket;
        _logger.LogInformation("Game ticket set for environment: {Env}", ticket.Environment);
        StateChanged?.Invoke();
    }

    public void AcceptEula()
    {
        HasAcceptedEula = true;
        _logger.LogInformation("EULA accepted");
        StateChanged?.Invoke();
    }

    public void SetGuestMode(bool isGuest)
    {
        IsGuestMode = isGuest;
        _logger.LogInformation("Guest mode: {IsGuest}", isGuest);
        StateChanged?.Invoke();
    }

    public void Logout()
    {
        CurrentUser = null;
        CurrentGameTicket = null;
        IsGuestMode = false;
        _logger.LogInformation("User logged out");
        StateChanged?.Invoke();
    }
}
