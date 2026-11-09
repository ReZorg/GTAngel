using System.IO;
using System.Security.Cryptography;
using System.Text.Json;
using GTAngel.Models;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Loads and validates versioned avatar packages beneath Assets/Avatars.
/// The service is intentionally character-agnostic so future angels can reuse
/// the same manifest, integrity, import and expression-capability pipeline.
/// </summary>
public sealed class AvatarAssetProfileService
{
    public const string DefaultManifestRelativePath =
        "Assets/Avatars/ArcAngelEcho/v1/arc-angel-echo.avatar.json";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
        ReadCommentHandling = JsonCommentHandling.Skip,
        AllowTrailingCommas = true
    };

    private readonly ILogger<AvatarAssetProfileService> _logger;
    private readonly string _applicationBaseDirectory;

    public AvatarAssetProfileService(
        ILogger<AvatarAssetProfileService> logger,
        string? applicationBaseDirectory = null)
    {
        _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        _applicationBaseDirectory = Path.GetFullPath(
            applicationBaseDirectory ?? AppDomain.CurrentDomain.BaseDirectory);
    }

    public string DefaultManifestPath =>
        Path.Combine(_applicationBaseDirectory, DefaultManifestRelativePath.Replace('/', Path.DirectorySeparatorChar));

    public Task<AvatarAssetLoadResult> LoadDefaultAsync(
        bool verifyHashes = true,
        CancellationToken cancellationToken = default) =>
        LoadAsync(DefaultManifestPath, verifyHashes, cancellationToken);

    public async Task<AvatarAssetLoadResult> LoadAsync(
        string manifestPath,
        bool verifyHashes = true,
        CancellationToken cancellationToken = default)
    {
        var report = new AvatarAssetValidationReport();
        string fullManifestPath = Path.GetFullPath(manifestPath);

        if (!File.Exists(fullManifestPath))
        {
            report.Errors.Add($"Avatar manifest not found: {fullManifestPath}");
            return new AvatarAssetLoadResult { Validation = report };
        }

        AvatarAssetManifest? manifest;
        try
        {
            await using var manifestStream = File.OpenRead(fullManifestPath);
            manifest = await JsonSerializer.DeserializeAsync<AvatarAssetManifest>(
                manifestStream, JsonOptions, cancellationToken).ConfigureAwait(false);
        }
        catch (Exception ex) when (ex is JsonException or IOException or UnauthorizedAccessException)
        {
            report.Errors.Add($"Avatar manifest could not be read: {ex.Message}");
            return new AvatarAssetLoadResult { Validation = report };
        }

        if (manifest == null)
        {
            report.Errors.Add("Avatar manifest deserialized to null.");
            return new AvatarAssetLoadResult { Validation = report };
        }

        report.ProfileId = manifest.Id;
        ValidateManifestShape(manifest, report);
        if (manifest.Assets == null)
            return new AvatarAssetLoadResult { Validation = report };

        string rootDirectory = Path.GetDirectoryName(fullManifestPath)!;
        var resolvedPaths = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        foreach (var asset in manifest.Assets)
        {
            cancellationToken.ThrowIfCancellationRequested();

            if (asset == null)
            {
                report.Errors.Add("Avatar manifest contains a null asset entry.");
                continue;
            }
            if (string.IsNullOrWhiteSpace(asset.Key))
            {
                report.Errors.Add("Every avatar asset must define a non-empty key.");
                continue;
            }
            if (string.IsNullOrWhiteSpace(asset.Path))
            {
                report.Errors.Add($"Avatar asset '{asset.Key}' must define a non-empty path.");
                continue;
            }
            if (resolvedPaths.ContainsKey(asset.Key))
            {
                report.Errors.Add($"Duplicate avatar asset key: {asset.Key}");
                continue;
            }

            string resolvedPath;
            try
            {
                resolvedPath = ResolveContainedPath(rootDirectory, asset.Path);
            }
            catch (InvalidOperationException ex)
            {
                report.Errors.Add(ex.Message);
                continue;
            }

            resolvedPaths[asset.Key] = resolvedPath;
            if (!File.Exists(resolvedPath))
            {
                report.Errors.Add($"Missing avatar asset '{asset.Key}': {asset.Path}");
                continue;
            }

            var info = new FileInfo(resolvedPath);
            if (asset.SizeBytes > 0 && info.Length != asset.SizeBytes)
            {
                report.Errors.Add(
                    $"Size mismatch for '{asset.Key}': expected {asset.SizeBytes}, found {info.Length} bytes.");
                continue;
            }

            if (verifyHashes && !string.IsNullOrWhiteSpace(asset.Sha256))
            {
                string actualHash = await ComputeSha256Async(resolvedPath, cancellationToken).ConfigureAwait(false);
                if (!string.Equals(actualHash, asset.Sha256, StringComparison.OrdinalIgnoreCase))
                {
                    report.Errors.Add(
                        $"SHA-256 mismatch for '{asset.Key}': expected {asset.Sha256}, found {actualHash}.");
                    continue;
                }
            }

            report.ValidatedFiles.Add(asset.Key);
        }

        foreach (string requiredKey in GetRequiredAssetKeys(manifest))
        {
            if (!resolvedPaths.ContainsKey(requiredKey))
                report.Errors.Add($"Avatar manifest must resolve required asset '{requiredKey}'.");
        }

        AddCapabilityWarnings(manifest, report);
        if (!report.IsValid)
        {
            _logger.LogError(
                "Avatar profile {ProfileId} failed validation with {ErrorCount} errors",
                manifest.Id, report.Errors.Count);
            return new AvatarAssetLoadResult { Validation = report };
        }

        var profile = new AvatarRuntimeProfile(manifest, rootDirectory, resolvedPaths);
        _logger.LogInformation(
            "Avatar profile {ProfileId} v{Version} validated ({FileCount} files, expression={ExpressionMode})",
            manifest.Id, manifest.Version, report.ValidatedFiles.Count, manifest.Expression.Mode);

        return new AvatarAssetLoadResult { Profile = profile, Validation = report };
    }

    private static void ValidateManifestShape(
        AvatarAssetManifest manifest,
        AvatarAssetValidationReport report)
    {
        if (string.IsNullOrWhiteSpace(manifest.Id))
            report.Errors.Add("Avatar manifest id is required.");
        if (string.IsNullOrWhiteSpace(manifest.Name))
            report.Errors.Add("Avatar manifest name is required.");
        if (string.IsNullOrWhiteSpace(manifest.Version))
            report.Errors.Add("Avatar manifest version is required.");

        if (manifest.Assets == null)
        {
            report.Errors.Add("Avatar manifest assets collection is required.");
        }
        else
        {
            if (manifest.Assets.Count == 0)
                report.Errors.Add("Avatar manifest must contain at least one asset.");
            if (!manifest.Assets.Any(asset => asset != null &&
                    string.Equals(asset.Key, "Mesh", StringComparison.OrdinalIgnoreCase)))
                report.Errors.Add("Avatar manifest must define a Mesh asset.");
        }

        if (manifest.HeightCentimeters <= 0)
            report.Errors.Add("Avatar heightCentimeters must be greater than zero.");
        if (manifest.Rig == null)
            report.Errors.Add("Avatar rig profile is required.");
        else if (!manifest.Rig.HasSkinWeights)
            report.Errors.Add("GTAngel avatars must provide a skinned body rig.");
        if (manifest.Material == null)
            report.Errors.Add("Avatar material profile is required.");
        if (manifest.Expression == null)
            report.Errors.Add("Avatar expression profile is required.");
        if (manifest.Ue5Import == null)
            report.Errors.Add("Avatar UE5 import profile is required.");
    }

    private static void AddCapabilityWarnings(
        AvatarAssetManifest manifest,
        AvatarAssetValidationReport report)
    {
        if (manifest.Rig == null || manifest.Assets == null)
            return;

        if (!manifest.Rig.HasMorphTargets && !manifest.Rig.HasFacialBones)
        {
            report.Warnings.Add(
                "No facial morph targets or facial bones detected; skeletal/aura expression fallback will be used.");
        }

        if (!manifest.Assets.Any(asset => asset != null &&
                string.Equals(asset.Key, "Idle", StringComparison.OrdinalIgnoreCase)))
            report.Warnings.Add("No idle animation supplied; UE5 should use reference-pose breathing fallback.");

        if (manifest.Rig.TriangleCount > 200_000)
            report.Warnings.Add("High-poly avatar: generate production LODs before shipping in dense scenes.");
    }

    private static IEnumerable<string> GetRequiredAssetKeys(AvatarAssetManifest manifest)
    {
        yield return "Mesh";
        if (manifest.Material == null) yield break;

        foreach (string key in new[]
        {
            manifest.Material.BaseColorAssetKey,
            manifest.Material.NormalAssetKey,
            manifest.Material.MetallicAssetKey,
            manifest.Material.RoughnessAssetKey,
            manifest.Material.EmissiveSourceAssetKey
        }.Where(key => !string.IsNullOrWhiteSpace(key)).Distinct(StringComparer.OrdinalIgnoreCase))
        {
            yield return key;
        }
    }

    private static string ResolveContainedPath(string rootDirectory, string relativePath)
    {
        if (Path.IsPathRooted(relativePath))
            throw new InvalidOperationException($"Avatar asset path must be relative: {relativePath}");

        string root = Path.GetFullPath(rootDirectory);
        string candidate = Path.GetFullPath(Path.Combine(
            root, relativePath.Replace('/', Path.DirectorySeparatorChar)));
        string rootPrefix = root.TrimEnd(Path.DirectorySeparatorChar) + Path.DirectorySeparatorChar;

        if (!candidate.StartsWith(rootPrefix, StringComparison.OrdinalIgnoreCase))
            throw new InvalidOperationException($"Avatar asset path escapes its package root: {relativePath}");

        return candidate;
    }

    private static async Task<string> ComputeSha256Async(
        string filePath,
        CancellationToken cancellationToken)
    {
        await using var stream = new FileStream(
            filePath, FileMode.Open, FileAccess.Read, FileShare.Read,
            bufferSize: 1024 * 1024, useAsync: true);
        byte[] hash = await SHA256.HashDataAsync(stream, cancellationToken).ConfigureAwait(false);
        return Convert.ToHexString(hash).ToLowerInvariant();
    }
}
