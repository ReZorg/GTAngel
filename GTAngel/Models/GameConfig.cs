using System.Text.Json.Serialization;

namespace GTA3DE.Wpf.Models;

/// <summary>
/// Game configuration model.
/// Translated from: rockstarmobile/GCConfig.GamesConfig.GameConfig
/// Loaded from SDK.config JSON file.
/// </summary>
public class GameConfig
{
    [JsonPropertyName("id")]
    public string Id { get; set; } = string.Empty;

    [JsonPropertyName("slug")]
    public string Slug { get; set; } = string.Empty;

    [JsonPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    [JsonPropertyName("subtitle")]
    public string Subtitle { get; set; } = string.Empty;

    [JsonPropertyName("short_description")]
    public string ShortDescription { get; set; } = string.Empty;

    [JsonPropertyName("description")]
    public string Description { get; set; } = string.Empty;

    [JsonPropertyName("url_scheme")]
    public string UrlScheme { get; set; } = string.Empty;

    [JsonPropertyName("tags")]
    public string[] Tags { get; set; } = Array.Empty<string>();

    [JsonPropertyName("genres")]
    public string[] Genres { get; set; } = Array.Empty<string>();

    [JsonPropertyName("icon_url")]
    public string IconUrl { get; set; } = string.Empty;

    [JsonPropertyName("cover_url")]
    public string CoverUrl { get; set; } = string.Empty;

    [JsonPropertyName("trailer_url")]
    public string TrailerUrl { get; set; } = string.Empty;

    [JsonPropertyName("background_image_url")]
    public string BackgroundImageUrl { get; set; } = string.Empty;

    [JsonPropertyName("background_video_stream_url")]
    public string BackgroundVideoStreamUrl { get; set; } = string.Empty;

    [JsonPropertyName("android_package_name")]
    public string AndroidPackageName { get; set; } = string.Empty;
}

/// <summary>
/// SDK configuration root model.
/// Translated from: rockstarmobile/GCConfig
/// </summary>
public class SdkConfig
{
    [JsonPropertyName("general")]
    public GeneralConfig? General { get; set; }

    [JsonPropertyName("games")]
    public Dictionary<string, GameConfig>? Games { get; set; }

    [JsonPropertyName("gates")]
    public GateConfig? Gates { get; set; }

    [JsonPropertyName("googleplay")]
    public GooglePlayConfig? GooglePlay { get; set; }
}

public class GeneralConfig
{
    [JsonPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    [JsonPropertyName("short_name")]
    public string ShortName { get; set; } = string.Empty;

    [JsonPropertyName("slug")]
    public string Slug { get; set; } = string.Empty;
}

public class GateConfig
{
    [JsonPropertyName("type")]
    public string Type { get; set; } = string.Empty;
}

public class GooglePlayConfig
{
    [JsonPropertyName("product_id")]
    public string ProductId { get; set; } = string.Empty;
}

/// <summary>
/// Game ticket model.
/// Translated from: rockstarmobile/GCState.GameTicket
/// </summary>
public class GameTicket
{
    public string Ticket { get; set; } = string.Empty;
    public string Environment { get; set; } = string.Empty;

    public GameTicket() { }

    public GameTicket(string ticket, string environment)
    {
        Ticket = ticket;
        Environment = environment;
    }
}

/// <summary>
/// OBB data configuration.
/// Translated from: com.rockstargames.gta3.p011de.OBBData
/// </summary>
public class ObbData
{
    public string FileName { get; set; } = string.Empty;
    public long FileSize { get; set; }
    public string Checksum { get; set; } = string.Empty;
}

/// <summary>
/// Build configuration constants.
/// Translated from: com.rockstargames.gta3.p011de.BuildConfig
/// </summary>
public static class BuildConfig
{
    public const string ApplicationId = "com.rockstargames.gta3.de";
    public const string VersionName = "1.84.3";
    public const int VersionCode = 54543439;
    public const bool Debug = false;
}
