using GTAngel.Models;
using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for AppStateService — SetUser, SetGameTicket, AcceptEula,
/// SetGuestMode, Logout, computed properties, and StateChanged events.
/// </summary>
public class AppStateServiceTests
{
    private readonly AppStateService _service;

    public AppStateServiceTests()
    {
        _service = new AppStateService(NullLogger<AppStateService>.Instance);
    }

    // ── Initial state ──────────────────────────────────────────────────────

    [Fact]
    public void IsLoggedIn_Initially_IsFalse()
    {
        Assert.False(_service.IsLoggedIn);
    }

    [Fact]
    public void IsSubscribed_Initially_IsFalse()
    {
        Assert.False(_service.IsSubscribed);
    }

    [Fact]
    public void IsGameOwner_Initially_IsFalse()
    {
        Assert.False(_service.IsGameOwner);
    }

    [Fact]
    public void HasAcceptedEula_Initially_IsFalse()
    {
        Assert.False(_service.HasAcceptedEula);
    }

    [Fact]
    public void IsGuestMode_Initially_IsFalse()
    {
        Assert.False(_service.IsGuestMode);
    }

    [Fact]
    public void CurrentUser_Initially_IsNull()
    {
        Assert.Null(_service.CurrentUser);
    }

    [Fact]
    public void CurrentGameTicket_Initially_IsNull()
    {
        Assert.Null(_service.CurrentGameTicket);
    }

    // ── SetUser ────────────────────────────────────────────────────────────

    [Fact]
    public void SetUser_SetsCurrentUser()
    {
        var user = new RockstarUser { RockstarId = "r123", DisplayName = "Player1" };
        _service.SetUser(user);
        Assert.Equal(user, _service.CurrentUser);
    }

    [Fact]
    public void SetUser_MakesIsLoggedInTrue()
    {
        _service.SetUser(new RockstarUser { RockstarId = "r123" });
        Assert.True(_service.IsLoggedIn);
    }

    [Fact]
    public void SetUser_WithActiveSubscription_MakesIsSubscribedTrue()
    {
        var user = new RockstarUser
        {
            RockstarId = "r123",
            Subscription = RockstarUser.SubscriptionState.Active
        };
        _service.SetUser(user);
        Assert.True(_service.IsSubscribed);
    }

    [Fact]
    public void SetUser_WithExpiredSubscription_IsSubscribedIsFalse()
    {
        var user = new RockstarUser
        {
            RockstarId = "r123",
            Subscription = RockstarUser.SubscriptionState.Expired
        };
        _service.SetUser(user);
        Assert.False(_service.IsSubscribed);
    }

    [Fact]
    public void SetUser_WithGameOwner_IsGameOwnerTrue()
    {
        var user = new RockstarUser { RockstarId = "r123", IsGameOwner = true };
        _service.SetUser(user);
        Assert.True(_service.IsGameOwner);
    }

    [Fact]
    public void SetUser_RaisesStateChanged()
    {
        bool raised = false;
        _service.StateChanged += () => raised = true;
        _service.SetUser(new RockstarUser { RockstarId = "r1" });
        Assert.True(raised);
    }

    // ── SetGameTicket ──────────────────────────────────────────────────────

    [Fact]
    public void SetGameTicket_SetsCurrentGameTicket()
    {
        var ticket = new GameTicket("abc123", "production");
        _service.SetGameTicket(ticket);
        Assert.Equal(ticket, _service.CurrentGameTicket);
    }

    [Fact]
    public void SetGameTicket_RaisesStateChanged()
    {
        bool raised = false;
        _service.StateChanged += () => raised = true;
        _service.SetGameTicket(new GameTicket("t", "env"));
        Assert.True(raised);
    }

    // ── AcceptEula ─────────────────────────────────────────────────────────

    [Fact]
    public void AcceptEula_SetsHasAcceptedEulaTrue()
    {
        _service.AcceptEula();
        Assert.True(_service.HasAcceptedEula);
    }

    [Fact]
    public void AcceptEula_RaisesStateChanged()
    {
        bool raised = false;
        _service.StateChanged += () => raised = true;
        _service.AcceptEula();
        Assert.True(raised);
    }

    [Fact]
    public void AcceptEula_CalledTwice_StillTrue()
    {
        _service.AcceptEula();
        _service.AcceptEula();
        Assert.True(_service.HasAcceptedEula);
    }

    // ── SetGuestMode ───────────────────────────────────────────────────────

    [Fact]
    public void SetGuestMode_True_SetsIsGuestModeTrue()
    {
        _service.SetGuestMode(true);
        Assert.True(_service.IsGuestMode);
    }

    [Fact]
    public void SetGuestMode_False_SetsIsGuestModeFalse()
    {
        _service.SetGuestMode(true);
        _service.SetGuestMode(false);
        Assert.False(_service.IsGuestMode);
    }

    [Fact]
    public void SetGuestMode_RaisesStateChanged()
    {
        bool raised = false;
        _service.StateChanged += () => raised = true;
        _service.SetGuestMode(true);
        Assert.True(raised);
    }

    // ── Logout ─────────────────────────────────────────────────────────────

    [Fact]
    public void Logout_ClearsCurrentUser()
    {
        _service.SetUser(new RockstarUser { RockstarId = "r123" });
        _service.Logout();
        Assert.Null(_service.CurrentUser);
    }

    [Fact]
    public void Logout_ClearsCurrentGameTicket()
    {
        _service.SetGameTicket(new GameTicket("t", "env"));
        _service.Logout();
        Assert.Null(_service.CurrentGameTicket);
    }

    [Fact]
    public void Logout_ClearsGuestMode()
    {
        _service.SetGuestMode(true);
        _service.Logout();
        Assert.False(_service.IsGuestMode);
    }

    [Fact]
    public void Logout_MakesIsLoggedInFalse()
    {
        _service.SetUser(new RockstarUser { RockstarId = "r123" });
        _service.Logout();
        Assert.False(_service.IsLoggedIn);
    }

    [Fact]
    public void Logout_RaisesStateChanged()
    {
        bool raised = false;
        _service.StateChanged += () => raised = true;
        _service.Logout();
        Assert.True(raised);
    }

    [Fact]
    public void Logout_WhenAlreadyLoggedOut_DoesNotThrow()
    {
        var ex = Record.Exception(() => _service.Logout());
        Assert.Null(ex);
    }

    // ── StateChanged event count ───────────────────────────────────────────

    [Fact]
    public void StateChanged_CountsEachOperation()
    {
        int count = 0;
        _service.StateChanged += () => count++;
        _service.SetUser(new RockstarUser { RockstarId = "r1" });
        _service.SetGameTicket(new GameTicket("t", "e"));
        _service.AcceptEula();
        _service.SetGuestMode(true);
        _service.Logout();
        Assert.Equal(5, count);
    }
}
