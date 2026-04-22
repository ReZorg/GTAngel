namespace GTA3DE.Wpf.Models;

/// <summary>
/// Rockstar Games Social Club user model.
/// Translated from: rockstarmobile/RockstarUser.java
/// </summary>
public class RockstarUser
{
    /// <summary>
    /// Subscription states (from RockstarUser.SUBSCRIPTION_STATE enum)
    /// </summary>
    public enum SubscriptionState
    {
        None,
        Active,
        Expired,
        Cancelled
    }

    /// <summary>Rockstar Social Club user ID (replaces rockstarId field)</summary>
    public string RockstarId { get; set; } = string.Empty;

    /// <summary>Social Club services ticket (replaces scServicesTicket)</summary>
    public string ScServicesTicket { get; set; } = string.Empty;

    /// <summary>Current subscription state (replaces subscriptionState)</summary>
    public SubscriptionState Subscription { get; set; } = SubscriptionState.None;

    /// <summary>Display name</summary>
    public string DisplayName { get; set; } = string.Empty;

    /// <summary>Email address</summary>
    public string Email { get; set; } = string.Empty;

    /// <summary>Whether the user owns the full game</summary>
    public bool IsGameOwner { get; set; }

    /// <summary>Auth token for API calls</summary>
    public string AuthToken { get; set; } = string.Empty;

    /// <summary>Refresh token</summary>
    public string RefreshToken { get; set; } = string.Empty;
}
