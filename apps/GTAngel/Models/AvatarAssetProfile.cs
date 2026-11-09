using System.Text.Json.Serialization;

namespace GTAngel.Models;

/// <summary>
/// Portable, data-driven avatar package description. New characters can be added
/// without changing the cognitive or UE5 bridge layers: add a versioned asset
/// directory and one manifest conforming to this model.
/// </summary>
public sealed class AvatarAssetManifest
{
    public string SchemaVersion { get; set; } = "1.0";
    public string Id { get; set; } = string.Empty;
    public string Name { get; set; } = string.Empty;
    public string Version { get; set; } = string.Empty;
    public string Source { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public string License { get; set; } = "User supplied";
    public float HeightCentimeters { get; set; }
    public string CoordinateSystem { get; set; } = "Y-up";
    public List<AvatarAssetFile> Assets { get; set; } = new();
    public AvatarRigProfile Rig { get; set; } = new();
    public AvatarMaterialProfile Material { get; set; } = new();
    public AvatarExpressionProfile Expression { get; set; } = new();
    public AvatarUe5ImportProfile Ue5Import { get; set; } = new();
}

public sealed class AvatarAssetFile
{
    /// <summary>Stable key used by the runtime, for example Mesh, Walk or BaseColor.</summary>
    public string Key { get; set; } = string.Empty;
    public string Role { get; set; } = string.Empty;
    public string Path { get; set; } = string.Empty;
    public string Sha256 { get; set; } = string.Empty;
    public long SizeBytes { get; set; }
    public int? FrameRate { get; set; }
}

public sealed class AvatarRigProfile
{
    public string SkeletonName { get; set; } = string.Empty;
    public int BoneCount { get; set; }
    public int MeshCount { get; set; }
    public int VertexCount { get; set; }
    public int TriangleCount { get; set; }
    public bool HasSkinWeights { get; set; }
    public bool HasFacialBones { get; set; }
    public bool HasMorphTargets { get; set; }
    public bool HasRootMotion { get; set; }
    public List<string> Bones { get; set; } = new();
}

public sealed class AvatarMaterialProfile
{
    public string BaseColorAssetKey { get; set; } = "BaseColor";
    public string NormalAssetKey { get; set; } = "Normal";
    public string MetallicAssetKey { get; set; } = "Metallic";
    public string RoughnessAssetKey { get; set; } = "Roughness";
    public string EmissiveSourceAssetKey { get; set; } = "BaseColor";
    public float EmissiveIntensity { get; set; } = 0.35f;
    public bool TwoSided { get; set; } = true;
}

public sealed class AvatarExpressionProfile
{
    /// <summary>FacialMorphs, FacialBones, Live2DOverlay, or SkeletalAuraFallback.</summary>
    public string Mode { get; set; } = "SkeletalAuraFallback";
    public bool SupportsFacs { get; set; }
    public string HeadBone { get; set; } = "Head";
    public string GazeBone { get; set; } = "headfront";
    public string JawBone { get; set; } = string.Empty;
    public bool DriveEmissiveAura { get; set; } = true;
}

public sealed class AvatarUe5ImportProfile
{
    public string SkeletonPolicy { get; set; } = "CreateNew";
    public bool ImportMesh { get; set; } = true;
    public bool ImportAnimations { get; set; } = true;
    public bool ImportMorphTargets { get; set; }
    public bool PreserveLocalTransform { get; set; } = true;
    public bool UseT0AsReferencePose { get; set; } = true;
    public bool ConvertSceneUnit { get; set; } = true;
    public float UniformScale { get; set; } = 1.0f;
    public string DestinationPath { get; set; } = "/Game/GTAngel/Avatars/ArcAngelEcho";
}

/// <summary>Validated manifest plus normalized absolute paths for local UE5 import.</summary>
public sealed class AvatarRuntimeProfile
{
    private readonly IReadOnlyDictionary<string, string> _assetPaths;

    public AvatarRuntimeProfile(
        AvatarAssetManifest manifest,
        string rootDirectory,
        IReadOnlyDictionary<string, string> assetPaths)
    {
        Manifest = manifest;
        RootDirectory = rootDirectory;
        _assetPaths = assetPaths;
    }

    public AvatarAssetManifest Manifest { get; }
    public string RootDirectory { get; }
    public IReadOnlyDictionary<string, string> AssetPaths => _assetPaths;
    public string GetAssetPath(string key) =>
        _assetPaths.TryGetValue(key, out var path)
            ? path
            : throw new KeyNotFoundException($"Avatar asset key '{key}' is not present in profile '{Manifest.Id}'.");
}

public sealed class AvatarAssetValidationReport
{
    public bool IsValid => Errors.Count == 0;
    public string ProfileId { get; set; } = string.Empty;
    public List<string> ValidatedFiles { get; } = new();
    public List<string> Errors { get; } = new();
    public List<string> Warnings { get; } = new();
}

public sealed class AvatarAssetLoadResult
{
    public AvatarRuntimeProfile? Profile { get; init; }
    public AvatarAssetValidationReport Validation { get; init; } = new();
}

/// <summary>
/// One renderer-side operation in an avatar activation plan. Keeping the
/// payload typed at the envelope level makes plans testable while allowing
/// UE5 modules to evolve their command-specific parameter shapes.
/// </summary>
public sealed record AvatarModuleCommand(
    string Module,
    string Command,
    object Parameters);
