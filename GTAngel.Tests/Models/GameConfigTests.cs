using System.Text.Json;
using GTAngel.Models;
using Xunit;

namespace GTAngel.Tests.Models;

/// <summary>
/// Tests for GameConfig, SdkConfig, GameTicket, ObbData, BuildConfig,
/// RockstarUser, and related models.
/// </summary>
public class GameConfigTests
{
    // ── GameConfig defaults ────────────────────────────────────────────────

    [Fact]
    public void GameConfig_Defaults_StringsAreEmpty()
    {
        var config = new GameConfig();
        Assert.Equal(string.Empty, config.Id);
        Assert.Equal(string.Empty, config.Slug);
        Assert.Equal(string.Empty, config.Name);
        Assert.Equal(string.Empty, config.Subtitle);
        Assert.Equal(string.Empty, config.ShortDescription);
        Assert.Equal(string.Empty, config.Description);
        Assert.Equal(string.Empty, config.UrlScheme);
        Assert.Equal(string.Empty, config.IconUrl);
        Assert.Equal(string.Empty, config.CoverUrl);
        Assert.Equal(string.Empty, config.TrailerUrl);
        Assert.Equal(string.Empty, config.BackgroundImageUrl);
        Assert.Equal(string.Empty, config.BackgroundVideoStreamUrl);
        Assert.Equal(string.Empty, config.AndroidPackageName);
    }

    [Fact]
    public void GameConfig_Defaults_ArraysAreEmpty()
    {
        var config = new GameConfig();
        Assert.Empty(config.Tags);
        Assert.Empty(config.Genres);
    }

    // ── GameConfig JSON deserialization ────────────────────────────────────

    [Fact]
    public void GameConfig_Deserialize_NameIsSet()
    {
        const string json = """{"id":"gta3","slug":"gta3de","name":"Grand Theft Auto III"}""";
        var config = JsonSerializer.Deserialize<GameConfig>(json);
        Assert.NotNull(config);
        Assert.Equal("gta3", config!.Id);
        Assert.Equal("gta3de", config.Slug);
        Assert.Equal("Grand Theft Auto III", config.Name);
    }

    [Fact]
    public void GameConfig_Deserialize_TagsAndGenres()
    {
        const string json = """{"tags":["action","open-world"],"genres":["sandbox"]}""";
        var config = JsonSerializer.Deserialize<GameConfig>(json);
        Assert.NotNull(config);
        Assert.Equal(new[] { "action", "open-world" }, config!.Tags);
        Assert.Equal(new[] { "sandbox" }, config.Genres);
    }

    // ── SdkConfig defaults ─────────────────────────────────────────────────

    [Fact]
    public void SdkConfig_Defaults_AllNullable_AreNull()
    {
        var config = new SdkConfig();
        Assert.Null(config.General);
        Assert.Null(config.Games);
        Assert.Null(config.Gates);
        Assert.Null(config.GooglePlay);
    }

    // ── GeneralConfig ──────────────────────────────────────────────────────

    [Fact]
    public void GeneralConfig_Defaults_StringsAreEmpty()
    {
        var config = new GeneralConfig();
        Assert.Equal(string.Empty, config.Name);
        Assert.Equal(string.Empty, config.ShortName);
        Assert.Equal(string.Empty, config.Slug);
    }

    // ── GateConfig ────────────────────────────────────────────────────────

    [Fact]
    public void GateConfig_Default_TypeIsEmpty()
    {
        var config = new GateConfig();
        Assert.Equal(string.Empty, config.Type);
    }

    // ── GooglePlayConfig ───────────────────────────────────────────────────

    [Fact]
    public void GooglePlayConfig_Default_ProductIdIsEmpty()
    {
        var config = new GooglePlayConfig();
        Assert.Equal(string.Empty, config.ProductId);
    }

    // ── GameTicket ────────────────────────────────────────────────────────

    [Fact]
    public void GameTicket_DefaultConstructor_StringsAreEmpty()
    {
        var ticket = new GameTicket();
        Assert.Equal(string.Empty, ticket.Ticket);
        Assert.Equal(string.Empty, ticket.Environment);
    }

    [Fact]
    public void GameTicket_ParameterizedConstructor_SetsValues()
    {
        var ticket = new GameTicket("abc123", "production");
        Assert.Equal("abc123", ticket.Ticket);
        Assert.Equal("production", ticket.Environment);
    }

    [Fact]
    public void GameTicket_ParameterizedConstructor_EmptyStrings_Work()
    {
        var ticket = new GameTicket(string.Empty, string.Empty);
        Assert.Equal(string.Empty, ticket.Ticket);
        Assert.Equal(string.Empty, ticket.Environment);
    }

    // ── ObbData ───────────────────────────────────────────────────────────

    [Fact]
    public void ObbData_Defaults_StringsAreEmpty()
    {
        var obb = new ObbData();
        Assert.Equal(string.Empty, obb.FileName);
        Assert.Equal(0L, obb.FileSize);
        Assert.Equal(string.Empty, obb.Checksum);
    }

    [Fact]
    public void ObbData_CanSetValues()
    {
        var obb = new ObbData
        {
            FileName = "main.obb",
            FileSize = 1024 * 1024,
            Checksum = "abc123"
        };
        Assert.Equal("main.obb", obb.FileName);
        Assert.Equal(1024 * 1024, obb.FileSize);
        Assert.Equal("abc123", obb.Checksum);
    }

    // ── BuildConfig ───────────────────────────────────────────────────────

    [Fact]
    public void BuildConfig_ApplicationId_IsCorrect()
    {
        Assert.Equal("com.rockstargames.gta3.de", BuildConfig.ApplicationId);
    }

    [Fact]
    public void BuildConfig_VersionName_IsCorrect()
    {
        Assert.Equal("1.84.3", BuildConfig.VersionName);
    }

    [Fact]
    public void BuildConfig_VersionCode_IsPositive()
    {
        Assert.True(BuildConfig.VersionCode > 0);
    }

    [Fact]
    public void BuildConfig_Debug_IsFalse()
    {
        Assert.False(BuildConfig.Debug);
    }
}

/// <summary>
/// Tests for RockstarUser model.
/// </summary>
public class RockstarUserTests
{
    // ── Defaults ──────────────────────────────────────────────────────────

    [Fact]
    public void RockstarUser_Defaults_StringsAreEmpty()
    {
        var user = new RockstarUser();
        Assert.Equal(string.Empty, user.RockstarId);
        Assert.Equal(string.Empty, user.ScServicesTicket);
        Assert.Equal(string.Empty, user.DisplayName);
        Assert.Equal(string.Empty, user.Email);
        Assert.Equal(string.Empty, user.AuthToken);
        Assert.Equal(string.Empty, user.RefreshToken);
    }

    [Fact]
    public void RockstarUser_Default_SubscriptionIsNone()
    {
        var user = new RockstarUser();
        Assert.Equal(RockstarUser.SubscriptionState.None, user.Subscription);
    }

    [Fact]
    public void RockstarUser_Default_IsGameOwnerIsFalse()
    {
        var user = new RockstarUser();
        Assert.False(user.IsGameOwner);
    }

    // ── SubscriptionState enum ────────────────────────────────────────────

    [Theory]
    [InlineData(RockstarUser.SubscriptionState.None)]
    [InlineData(RockstarUser.SubscriptionState.Active)]
    [InlineData(RockstarUser.SubscriptionState.Expired)]
    [InlineData(RockstarUser.SubscriptionState.Cancelled)]
    public void RockstarUser_SubscriptionState_CanBeSet(RockstarUser.SubscriptionState state)
    {
        var user = new RockstarUser { Subscription = state };
        Assert.Equal(state, user.Subscription);
    }

    [Fact]
    public void RockstarUser_SetProperties_PersistedCorrectly()
    {
        var user = new RockstarUser
        {
            RockstarId = "u123",
            ScServicesTicket = "ticket_xyz",
            DisplayName = "TestPlayer",
            Email = "test@example.com",
            AuthToken = "auth_abc",
            RefreshToken = "refresh_def",
            Subscription = RockstarUser.SubscriptionState.Active,
            IsGameOwner = true
        };

        Assert.Equal("u123", user.RockstarId);
        Assert.Equal("ticket_xyz", user.ScServicesTicket);
        Assert.Equal("TestPlayer", user.DisplayName);
        Assert.Equal("test@example.com", user.Email);
        Assert.Equal("auth_abc", user.AuthToken);
        Assert.Equal("refresh_def", user.RefreshToken);
        Assert.Equal(RockstarUser.SubscriptionState.Active, user.Subscription);
        Assert.True(user.IsGameOwner);
    }
}
