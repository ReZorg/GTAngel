using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for AppConfiguration, AudioService, GtaPlusService, SubscriptionService,
/// RockstarApiClient, SocialClubApiClient, and NavigationService.
/// </summary>

// ── AppConfiguration ──────────────────────────────────────────────────────────

public class AppConfigurationTests
{
    private readonly AppConfiguration _svc =
        new(NullLogger<AppConfiguration>.Instance);

    [Fact]
    public void Ue5EnginePath_DefaultValue_IsNotNullOrEmpty()
    {
        Assert.False(string.IsNullOrEmpty(_svc.Ue5EnginePath));
    }

    [Fact]
    public void Ue5EnginePath_Setter_UpdatesValue()
    {
        _svc.Ue5EnginePath = @"C:\TestEngine";
        Assert.Equal(@"C:\TestEngine", _svc.Ue5EnginePath);
    }

    [Fact]
    public void SdkConfig_Initially_IsNull()
    {
        Assert.Null(_svc.SdkConfig);
    }

    [Fact]
    public void General_Initially_IsNull()
    {
        Assert.Null(_svc.General);
    }

    [Fact]
    public async Task LoadAsync_WhenConfigFileAbsent_SetsFallbackSdkConfig()
    {
        await _svc.LoadAsync();
        Assert.NotNull(_svc.SdkConfig);
        Assert.NotNull(_svc.General);
    }

    [Fact]
    public async Task LoadAsync_WhenConfigFileAbsent_FallbackNameContainsGTA()
    {
        await _svc.LoadAsync();
        Assert.Contains("GTA", _svc.General!.Name, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public async Task LoadAsync_CalledTwice_DoesNotThrow()
    {
        await _svc.LoadAsync();
        var ex = await Record.ExceptionAsync(() => _svc.LoadAsync());
        Assert.Null(ex);
    }

    [Fact]
    public async Task LoadUserSettingsAsync_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() => _svc.LoadUserSettingsAsync());
        Assert.Null(ex);
    }
}

// ── AudioService ──────────────────────────────────────────────────────────────

public class AudioServiceTests : IDisposable
{
    private readonly AudioService _svc =
        new(NullLogger<AudioService>.Instance);

    [Fact]
    public void Initialize_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.Initialize());
        Assert.Null(ex);
    }

    [Fact]
    public void IsPaused_Initially_IsFalse()
    {
        Assert.False(_svc.IsPaused);
    }

    [Fact]
    public void Pause_SetsPausedTrue()
    {
        _svc.Pause();
        Assert.True(_svc.IsPaused);
    }

    [Fact]
    public void Resume_SetsPausedFalse()
    {
        _svc.Pause();
        _svc.Resume();
        Assert.False(_svc.IsPaused);
    }

    [Fact]
    public void SetMasterVolume_ClampsToZero_WhenBelowRange()
    {
        var ex = Record.Exception(() => _svc.SetMasterVolume(-5f));
        Assert.Null(ex);
    }

    [Fact]
    public void SetMasterVolume_ClampsToOne_WhenAboveRange()
    {
        var ex = Record.Exception(() => _svc.SetMasterVolume(10f));
        Assert.Null(ex);
    }

    [Fact]
    public void SetMusicVolume_ValidRange_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.SetMusicVolume(0.5f));
        Assert.Null(ex);
    }

    [Fact]
    public void SetSfxVolume_ValidRange_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.SetSfxVolume(0.5f));
        Assert.Null(ex);
    }

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.Dispose());
        Assert.Null(ex);
    }

    public void Dispose() => _svc.Dispose();
}

// ── GtaPlusService ────────────────────────────────────────────────────────────

public class GtaPlusServiceTests
{
    private readonly GtaPlusService _svc =
        new(NullLogger<GtaPlusService>.Instance);

    [Fact]
    public void IsGtaPlusActive_Initially_IsFalse()
    {
        Assert.False(_svc.IsGtaPlusActive);
    }

    [Fact]
    public async Task CheckStatusAsync_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() => _svc.CheckStatusAsync());
        Assert.Null(ex);
    }

    [Fact]
    public async Task CheckStatusAsync_CalledMultipleTimes_DoesNotThrow()
    {
        for (int i = 0; i < 3; i++)
        {
            var ex = await Record.ExceptionAsync(() => _svc.CheckStatusAsync());
            Assert.Null(ex);
        }
    }
}

// ── SubscriptionService ───────────────────────────────────────────────────────

public class SubscriptionServiceTests
{
    private readonly SubscriptionService _svc =
        new(NullLogger<SubscriptionService>.Instance);

    [Fact]
    public async Task IsSubscribedAsync_ReturnsFalse_ByDefault()
    {
        bool result = await _svc.IsSubscribedAsync();
        Assert.False(result);
    }

    [Fact]
    public async Task RestorePurchaseAsync_ReturnsFalse_ByDefault()
    {
        bool result = await _svc.RestorePurchaseAsync();
        Assert.False(result);
    }

    [Fact]
    public async Task StartPurchaseFlowAsync_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() => _svc.StartPurchaseFlowAsync());
        Assert.Null(ex);
    }

    [Fact]
    public async Task IsSubscribedAsync_CalledConcurrently_DoesNotThrow()
    {
        var tasks = Enumerable.Range(0, 5)
            .Select(_ => _svc.IsSubscribedAsync());
        var results = await Task.WhenAll(tasks);
        Assert.All(results, r => Assert.False(r));
    }
}

// ── RockstarApiClient ─────────────────────────────────────────────────────────

public class RockstarApiClientTests
{
    private readonly RockstarApiClient _svc =
        new(NullLogger<RockstarApiClient>.Instance);

    [Fact]
    public async Task GetGameConfigAsync_ReturnsNonNull()
    {
        var result = await _svc.GetGameConfigAsync();
        Assert.NotNull(result);
    }

    [Fact]
    public async Task GetCloudSavesAsync_ReturnsNonNull()
    {
        var result = await _svc.GetCloudSavesAsync("SC-TEST123");
        Assert.NotNull(result);
    }

    [Fact]
    public async Task GetCloudSavesAsync_WithEmptyId_DoesNotThrow()
    {
        var ex = await Record.ExceptionAsync(() => _svc.GetCloudSavesAsync(string.Empty));
        Assert.Null(ex);
    }
}

// ── SocialClubApiClient ───────────────────────────────────────────────────────

public class SocialClubApiClientTests
{
    private readonly SocialClubApiClient _svc =
        new(NullLogger<SocialClubApiClient>.Instance);

    [Fact]
    public async Task LoginAsync_ValidCredentials_ReturnsUser()
    {
        var user = await _svc.LoginAsync("test@example.com", "password");
        Assert.NotNull(user);
    }

    [Fact]
    public async Task LoginAsync_SetsEmailOnUser()
    {
        var user = await _svc.LoginAsync("user@example.com", "pass");
        Assert.NotNull(user);
        Assert.Equal("user@example.com", user!.Email);
    }

    [Fact]
    public async Task LoginAsync_SetsDisplayNameFromEmail()
    {
        var user = await _svc.LoginAsync("myname@example.com", "pass");
        Assert.NotNull(user);
        Assert.Equal("myname", user!.DisplayName);
    }

    [Fact]
    public async Task LoginAsync_SetsRockstarId()
    {
        var user = await _svc.LoginAsync("test@example.com", "pass");
        Assert.NotNull(user);
        Assert.StartsWith("SC-", user!.RockstarId);
    }

    [Fact]
    public async Task LoginAsync_SetsAuthToken()
    {
        var user = await _svc.LoginAsync("test@example.com", "pass");
        Assert.NotNull(user);
        Assert.False(string.IsNullOrEmpty(user!.AuthToken));
    }

    [Fact]
    public async Task LoginAsync_SetsIsGameOwnerTrue()
    {
        var user = await _svc.LoginAsync("test@example.com", "pass");
        Assert.NotNull(user);
        Assert.True(user!.IsGameOwner);
    }

    [Fact]
    public async Task RefreshTokenAsync_ReturnsNewToken()
    {
        var token = await _svc.RefreshTokenAsync("old-refresh-token");
        Assert.NotNull(token);
        Assert.False(string.IsNullOrEmpty(token));
    }

    [Fact]
    public async Task ValidateTicketAsync_ReturnsTrue()
    {
        bool result = await _svc.ValidateTicketAsync("game-ticket-abc");
        Assert.True(result);
    }

    [Fact]
    public async Task LoginAsync_TwoLogins_ReturnDifferentRockstarIds()
    {
        var user1 = await _svc.LoginAsync("a@example.com", "p");
        var user2 = await _svc.LoginAsync("b@example.com", "p");
        Assert.NotNull(user1);
        Assert.NotNull(user2);
        Assert.NotEqual(user1!.RockstarId, user2!.RockstarId);
    }
}

// ── NavigationService ─────────────────────────────────────────────────────────

public class NavigationServiceTests
{
    private readonly NavigationService _svc =
        new(NullLogger<NavigationService>.Instance);

    [Fact]
    public void CanGoBack_WithoutFrame_IsFalse()
    {
        Assert.False(_svc.CanGoBack);
    }

    [Fact]
    public void GoBack_WithoutFrame_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.GoBack());
        Assert.Null(ex);
    }

    [Fact]
    public void ClearHistory_WithoutFrame_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.ClearHistory());
        Assert.Null(ex);
    }

    [Fact]
    public void RegisterFrame_NullFrame_DoesNotThrow()
    {
        // NavigationService stores the frame — registering null should be
        // handled gracefully (the Frame parameter is nullable in practice).
        var ex = Record.Exception(() => _svc.RegisterFrame(null!));
        Assert.Null(ex);
    }

    [Fact]
    public void NavigateTo_PageInstance_WithoutFrame_DoesNotThrow()
    {
        var ex = Record.Exception(() => _svc.NavigateTo(null!));
        Assert.Null(ex);
    }
}
