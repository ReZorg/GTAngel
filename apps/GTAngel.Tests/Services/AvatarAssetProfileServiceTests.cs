using System.IO;
using System.Text.Json;
using GTAngel.Models;
using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

public sealed class AvatarAssetProfileServiceTests : IDisposable
{
    private readonly string _tempRoot = Path.Combine(
        Path.GetTempPath(), "gtangel-avatar-tests-" + Guid.NewGuid().ToString("N"));

    public AvatarAssetProfileServiceTests() => Directory.CreateDirectory(_tempRoot);

    [Fact]
    public async Task LoadAsync_ValidManifest_ResolvesContainedAssets()
    {
        string manifestPath = await CreatePackageAsync();
        var result = await CreateService().LoadAsync(manifestPath);

        Assert.True(result.Validation.IsValid);
        Assert.NotNull(result.Profile);
        Assert.Equal("test-angel", result.Profile!.Manifest.Id);
        Assert.True(Path.IsPathFullyQualified(result.Profile.GetAssetPath("Mesh")));
        Assert.Equal(5, result.Validation.ValidatedFiles.Count);
    }

    [Fact]
    public async Task LoadAsync_HashMismatch_IsRejected()
    {
        string manifestPath = await CreatePackageAsync(meshHash: new string('0', 64));
        var result = await CreateService().LoadAsync(manifestPath);

        Assert.False(result.Validation.IsValid);
        Assert.Null(result.Profile);
        Assert.Contains(result.Validation.Errors, error => error.Contains("SHA-256 mismatch"));
    }

    [Fact]
    public async Task LoadAsync_PathTraversal_IsRejected()
    {
        string manifestPath = await CreatePackageAsync(meshAssetPath: "../escape.fbx");
        var result = await CreateService().LoadAsync(manifestPath, verifyHashes: false);

        Assert.False(result.Validation.IsValid);
        Assert.Contains(result.Validation.Errors, error => error.Contains("escapes its package root"));
    }

    [Fact]
    public async Task LoadAsync_BlankRequiredPath_IsRejected()
    {
        string manifestPath = await CreatePackageAsync(meshAssetPath: "");
        var result = await CreateService().LoadAsync(manifestPath, verifyHashes: false);

        Assert.False(result.Validation.IsValid);
        Assert.Contains(result.Validation.Errors, error => error.Contains("non-empty path"));
        Assert.Contains(result.Validation.Errors, error => error.Contains("required asset 'Mesh'"));
    }

    [Fact]
    public async Task LoadAsync_MissingMaterialAsset_IsRejected()
    {
        string manifestPath = await CreatePackageAsync(includeRoughness: false);
        var result = await CreateService().LoadAsync(manifestPath);

        Assert.False(result.Validation.IsValid);
        Assert.Contains(result.Validation.Errors, error => error.Contains("required asset 'Roughness'"));
    }

    [Fact]
    public async Task LoadAsync_NullAssetsCollection_IsRejectedWithoutThrowing()
    {
        string manifestPath = await CreatePackageAsync(nullAssets: true);
        var result = await CreateService().LoadAsync(manifestPath);

        Assert.False(result.Validation.IsValid);
        Assert.Null(result.Profile);
        Assert.Contains(result.Validation.Errors, error => error.Contains("assets collection"));
    }

    [Fact]
    public async Task LoadAsync_MissingMeshCapability_IsRejected()
    {
        string manifestPath = await CreatePackageAsync(hasSkinWeights: false);
        var result = await CreateService().LoadAsync(manifestPath);

        Assert.False(result.Validation.IsValid);
        Assert.Contains(result.Validation.Errors, error => error.Contains("skinned body rig"));
    }

    [Fact]
    public async Task LoadAsync_NoFaceRig_EmitsFallbackWarning()
    {
        string manifestPath = await CreatePackageAsync();
        var result = await CreateService().LoadAsync(manifestPath);

        Assert.True(result.Validation.IsValid);
        Assert.Contains(result.Validation.Warnings, warning => warning.Contains("skeletal/aura"));
        Assert.Contains(result.Validation.Warnings, warning => warning.Contains("idle animation"));
    }

    private AvatarAssetProfileService CreateService() =>
        new(NullLogger<AvatarAssetProfileService>.Instance, _tempRoot);

    private async Task<string> CreatePackageAsync(
        string? meshHash = null,
        string meshAssetPath = "source/character.fbx",
        bool hasSkinWeights = true,
        bool includeRoughness = true,
        bool nullAssets = false)
    {
        string package = Path.Combine(_tempRoot, "Assets", "Avatars", "Test", "v1");
        string source = Path.Combine(package, "source");
        Directory.CreateDirectory(source);
        byte[] data = "test-avatar-data"u8.ToArray();

        var files = new Dictionary<string, string>
        {
            ["Mesh"] = "source/character.fbx",
            ["BaseColor"] = "source/base.png",
            ["Normal"] = "source/normal.png",
            ["Metallic"] = "source/metallic.png",
            ["Roughness"] = "source/roughness.png"
        };
        foreach (string relativePath in files.Values)
            await File.WriteAllBytesAsync(Path.Combine(package, relativePath), data);

        string validHash = Convert.ToHexString(
            System.Security.Cryptography.SHA256.HashData(data)).ToLowerInvariant();
        var assets = files
            .Where(pair => includeRoughness || pair.Key != "Roughness")
            .Select(pair => new AvatarAssetFile
            {
                Key = pair.Key,
                Role = pair.Key == "Mesh" ? "SkeletalMesh" : "Texture",
                Path = pair.Key == "Mesh" ? meshAssetPath : pair.Value,
                SizeBytes = data.Length,
                Sha256 = pair.Key == "Mesh" ? meshHash ?? validHash : validHash
            })
            .ToList();

        var manifest = new AvatarAssetManifest
        {
            Id = "test-angel",
            Name = "Test Angel",
            Version = "1.0.0",
            HeightCentimeters = 170,
            Rig = new AvatarRigProfile
            {
                BoneCount = 22,
                HasSkinWeights = hasSkinWeights,
                HasFacialBones = false,
                HasMorphTargets = false,
                TriangleCount = 10
            },
            Assets = nullAssets ? null! : assets
        };

        string manifestPath = Path.Combine(package, "test.avatar.json");
        await File.WriteAllTextAsync(
            manifestPath,
            JsonSerializer.Serialize(manifest, new JsonSerializerOptions { WriteIndented = true }));
        return manifestPath;
    }

    public void Dispose()
    {
        if (Directory.Exists(_tempRoot))
            Directory.Delete(_tempRoot, recursive: true);
    }
}
