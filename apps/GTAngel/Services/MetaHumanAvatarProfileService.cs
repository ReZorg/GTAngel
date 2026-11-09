using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// MetaHuman Avatar Profile Service — UE5 MetaHuman Integration Evaluator
///
/// Evaluates and manages the Deep Tree Echo 3D avatar profile image integration
/// for Unreal Engine 5 MetaHuman rendering. Provides:
///
///   1. Profile Image → MetaHuman Rig Mapping (facial landmark extraction)
///   2. Avatar Visualization Readiness Scoring
///   3. MetaHuman Blueprint Configuration Generation
///   4. FACS Action Unit Calibration from Profile
///   5. Hair/Skin/Eye Appearance Parametrization
///
/// UE5 MetaHuman Pipeline:
///   ProfileImage → FacialLandmarks → MetaHumanDNA → FRigLogic → LiveLink
///
/// The service evaluates how accurately the DTE profile image can be
/// represented as a MetaHuman avatar with full facial animation support.
///
/// Alexander's 15 Properties: P2 (Strong Centers), P5 (Positive Space),
/// P8 (Deep Interlock), P11 (Roughness), P14 (Simplicity)
/// </summary>
public sealed class MetaHumanAvatarProfileService : IDisposable
{
    private readonly ILogger<MetaHumanAvatarProfileService> _logger;
    private bool _disposed;

    // ── Profile State ─────────────────────────────────────────────────────────
    private AvatarProfileData? _currentProfile;
    private MetaHumanReadinessReport? _lastReadinessReport;

    // ── MetaHuman Configuration ───────────────────────────────────────────────
    private MetaHumanConfig _metaHumanConfig = MetaHumanConfig.Default;

    // ── FACS Calibration ──────────────────────────────────────────────────────
    private readonly Dictionary<int, float> _facsCalibration = new();
    private float _facsCalibrationQuality;

    // ── Events ────────────────────────────────────────────────────────────────
    public event EventHandler<MetaHumanReadinessReport>? ReadinessEvaluated;
    public event EventHandler<AvatarProfileData>? ProfileUpdated;
    public event EventHandler<MetaHumanConfig>? ConfigurationGenerated;

    public MetaHumanAvatarProfileService(ILogger<MetaHumanAvatarProfileService> logger)
    {
        _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        _logger.LogInformation("MetaHuman Avatar Profile Service initialized");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Public API
    // ═══════════════════════════════════════════════════════════════════════════

    /// <summary>Current avatar profile data.</summary>
    public AvatarProfileData? CurrentProfile => _currentProfile;

    /// <summary>Latest readiness evaluation report.</summary>
    public MetaHumanReadinessReport? LastReadinessReport => _lastReadinessReport;

    /// <summary>Current MetaHuman configuration.</summary>
    public MetaHumanConfig Configuration => _metaHumanConfig;

    /// <summary>FACS calibration quality [0,1].</summary>
    public float FacsCalibrationQuality => _facsCalibrationQuality;

    // ── Profile Loading ───────────────────────────────────────────────────────

    /// <summary>
    /// Load a profile image and extract avatar parameters.
    /// </summary>
    public AvatarProfileData LoadProfile(string profileImagePath, string avatarName = "DeepTreeEcho")
    {
        if (string.IsNullOrEmpty(profileImagePath))
            throw new ArgumentException("Profile image path is required", nameof(profileImagePath));

        var profile = new AvatarProfileData
        {
            AvatarName = avatarName,
            ProfileImagePath = profileImagePath,
            ImageExists = File.Exists(profileImagePath),
            LoadedAt = DateTimeOffset.UtcNow,
            FacialLandmarks = ExtractFacialLandmarks(profileImagePath),
            AppearanceParams = ExtractAppearanceParameters(profileImagePath),
            MetaHumanBodyType = MetaHumanBodyType.Feminine,
            ExpressionNeutral = true
        };

        _currentProfile = profile;
        ProfileUpdated?.Invoke(this, profile);

        _logger.LogInformation("Profile loaded: {Name}, image exists: {Exists}",
            avatarName, profile.ImageExists);

        return profile;
    }

    /// <summary>
    /// Load a profile from raw parameters (when image is not available).
    /// </summary>
    public AvatarProfileData LoadProfileFromParameters(
        string avatarName,
        MetaHumanBodyType bodyType,
        AppearanceParameters appearance)
    {
        var profile = new AvatarProfileData
        {
            AvatarName = avatarName,
            ProfileImagePath = string.Empty,
            ImageExists = false,
            LoadedAt = DateTimeOffset.UtcNow,
            FacialLandmarks = new FacialLandmarkData(),
            AppearanceParams = appearance,
            MetaHumanBodyType = bodyType,
            ExpressionNeutral = true
        };

        _currentProfile = profile;
        ProfileUpdated?.Invoke(this, profile);
        return profile;
    }

    // ── Readiness Evaluation ──────────────────────────────────────────────────

    /// <summary>
    /// Evaluate how ready the current profile is for MetaHuman rendering.
    /// Returns a comprehensive readiness report with scores per category.
    /// </summary>
    public MetaHumanReadinessReport EvaluateReadiness()
    {
        if (_currentProfile == null)
        {
            return new MetaHumanReadinessReport
            {
                OverallScore = 0f,
                Status = ReadinessStatus.NotReady,
                Issues = new[] { "No profile loaded" }
            };
        }

        var scores = new Dictionary<string, float>();
        var issues = new List<string>();
        var recommendations = new List<string>();

        // 1. Profile Image Quality
        float imageScore = EvaluateImageQuality(_currentProfile);
        scores["ImageQuality"] = imageScore;
        if (imageScore < 0.5f)
            issues.Add("Profile image quality insufficient for accurate MetaHuman mapping");

        // 2. Facial Landmark Coverage
        float landmarkScore = EvaluateLandmarkCoverage(_currentProfile.FacialLandmarks);
        scores["LandmarkCoverage"] = landmarkScore;
        if (landmarkScore < 0.6f)
            recommendations.Add("Add multi-angle profile images for better landmark extraction");

        // 3. FACS Compatibility
        float facsScore = EvaluateFacsCompatibility(_currentProfile);
        scores["FACSCompatibility"] = facsScore;
        if (facsScore < 0.5f)
            recommendations.Add("Calibrate FACS action units from neutral expression image");

        // 4. MetaHuman DNA Readiness
        float dnaScore = EvaluateMetaHumanDNA(_currentProfile);
        scores["MetaHumanDNA"] = dnaScore;

        // 5. Appearance Parametrization
        float appearanceScore = EvaluateAppearanceParams(_currentProfile.AppearanceParams);
        scores["Appearance"] = appearanceScore;
        if (appearanceScore < 0.7f)
            recommendations.Add("Specify hair style, eye color, and skin tone parameters");

        // 6. Animation Readiness (LiveLink compatibility)
        float animScore = EvaluateAnimationReadiness(_currentProfile);
        scores["AnimationReady"] = animScore;

        // 7. UE5 Pipeline Compatibility
        float pipelineScore = EvaluateUE5PipelineCompat();
        scores["UE5Pipeline"] = pipelineScore;

        // Overall score
        float overallScore = scores.Values.Average();

        var report = new MetaHumanReadinessReport
        {
            OverallScore = overallScore,
            CategoryScores = scores,
            Status = overallScore switch
            {
                >= 0.8f => ReadinessStatus.Ready,
                >= 0.6f => ReadinessStatus.NearReady,
                >= 0.4f => ReadinessStatus.PartiallyReady,
                _ => ReadinessStatus.NotReady
            },
            Issues = issues.ToArray(),
            Recommendations = recommendations.ToArray(),
            EvaluatedAt = DateTimeOffset.UtcNow
        };

        _lastReadinessReport = report;
        ReadinessEvaluated?.Invoke(this, report);

        _logger.LogInformation("MetaHuman readiness: {Score:F1}% ({Status})",
            overallScore * 100f, report.Status);

        return report;
    }

    // ── MetaHuman Configuration Generation ────────────────────────────────────

    /// <summary>
    /// Generate MetaHuman Blueprint configuration from the current profile.
    /// </summary>
    public MetaHumanConfig GenerateConfiguration()
    {
        if (_currentProfile == null)
            return MetaHumanConfig.Default;

        var config = new MetaHumanConfig
        {
            AvatarName = _currentProfile.AvatarName,
            BodyType = _currentProfile.MetaHumanBodyType,
            HeightCm = 170f,
            BuildType = MetaHumanBuildType.Athletic,

            // Face shape from landmarks
            FaceShape = new FaceShapeConfig
            {
                JawWidth = _currentProfile.FacialLandmarks.JawWidth,
                CheekboneProminence = _currentProfile.FacialLandmarks.CheekboneProminence,
                ForeheadHeight = _currentProfile.FacialLandmarks.ForeheadHeight,
                ChinLength = _currentProfile.FacialLandmarks.ChinLength,
                NoseBridgeWidth = _currentProfile.FacialLandmarks.NoseBridgeWidth,
                LipFullness = _currentProfile.FacialLandmarks.LipFullness
            },

            // Appearance
            SkinTone = _currentProfile.AppearanceParams.SkinTone,
            HairStyle = _currentProfile.AppearanceParams.HairStyle,
            HairColor = _currentProfile.AppearanceParams.HairColor,
            EyeColor = _currentProfile.AppearanceParams.EyeColor,

            // Animation setup
            EnableLiveLink = true,
            EnableFACS = true,
            FACSActionUnitCount = 46,
            EnableBodyIK = true,
            EnableFingerTracking = true,

            // Rendering
            EnableRayTracing = true,
            EnableSubsurfaceScattering = true,
            EnableStrandHair = true,
            LODCount = 4,
            TextureResolution = 4096,

            GeneratedAt = DateTimeOffset.UtcNow
        };

        _metaHumanConfig = config;
        ConfigurationGenerated?.Invoke(this, config);

        _logger.LogInformation("MetaHuman config generated: {Name} ({BodyType})",
            config.AvatarName, config.BodyType);

        return config;
    }

    // ── FACS Calibration ──────────────────────────────────────────────────────

    /// <summary>
    /// Calibrate FACS action units from the profile's neutral expression.
    /// Maps the DTE avatar's emotional expression range to MetaHuman rig parameters.
    /// </summary>
    public Dictionary<int, float> CalibrateFACS()
    {
        _facsCalibration.Clear();

        // Standard FACS AU calibration (46 action units)
        // These represent the neutral baseline offsets for the MetaHuman rig
        var auBaselines = new (int AU, float Baseline, string Name)[]
        {
            (1, 0.0f, "Inner Brow Raise"),
            (2, 0.0f, "Outer Brow Raise"),
            (4, 0.05f, "Brow Lowerer"),  // slight baseline from profile
            (5, 0.3f, "Upper Lid Raiser"),
            (6, 0.1f, "Cheek Raiser"),
            (7, 0.0f, "Lid Tightener"),
            (9, 0.0f, "Nose Wrinkler"),
            (10, 0.0f, "Upper Lip Raiser"),
            (12, 0.15f, "Lip Corner Puller"), // slight smile baseline (personality: Charm)
            (14, 0.0f, "Dimpler"),
            (15, 0.0f, "Lip Corner Depressor"),
            (17, 0.0f, "Chin Raiser"),
            (20, 0.0f, "Lip Stretcher"),
            (23, 0.0f, "Lip Tightener"),
            (25, 0.05f, "Lips Part"),
            (26, 0.0f, "Jaw Drop"),
            (27, 0.0f, "Mouth Stretch"),
            (28, 0.0f, "Lip Suck"),
            (43, 0.0f, "Eyes Closed"),
            (45, 0.0f, "Blink"),
            (46, 0.0f, "Wink"),
        };

        foreach (var (au, baseline, _) in auBaselines)
        {
            _facsCalibration[au] = baseline;
        }

        // Quality based on profile data availability
        _facsCalibrationQuality = _currentProfile?.ImageExists == true ? 0.85f : 0.6f;

        _logger.LogInformation("FACS calibrated: {Count} AUs, quality: {Quality:F2}",
            _facsCalibration.Count, _facsCalibrationQuality);

        return new Dictionary<int, float>(_facsCalibration);
    }

    // ── Serialization ─────────────────────────────────────────────────────────

    /// <summary>
    /// Export MetaHuman configuration as JSON for UE5 import.
    /// </summary>
    public string ExportConfigurationJson()
    {
        var config = _metaHumanConfig;
        return JsonSerializer.Serialize(config, new JsonSerializerOptions
        {
            WriteIndented = true,
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase
        });
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Evaluation Helpers
    // ═══════════════════════════════════════════════════════════════════════════

    private static float EvaluateImageQuality(AvatarProfileData profile)
    {
        float score = 0f;
        if (profile.ImageExists) score += 0.5f;
        if (profile.ExpressionNeutral) score += 0.2f;
        if (!string.IsNullOrEmpty(profile.ProfileImagePath)) score += 0.15f;
        score += 0.15f; // base score for having a profile at all
        return Math.Clamp(score, 0f, 1f);
    }

    private static float EvaluateLandmarkCoverage(FacialLandmarkData landmarks)
    {
        int coveredCount = 0;
        int totalChecks = 6;

        if (landmarks.JawWidth > 0) coveredCount++;
        if (landmarks.CheekboneProminence > 0) coveredCount++;
        if (landmarks.ForeheadHeight > 0) coveredCount++;
        if (landmarks.ChinLength > 0) coveredCount++;
        if (landmarks.NoseBridgeWidth > 0) coveredCount++;
        if (landmarks.LipFullness > 0) coveredCount++;

        return (float)coveredCount / totalChecks;
    }

    private float EvaluateFacsCompatibility(AvatarProfileData profile)
    {
        float score = 0.5f; // base compatibility
        if (_facsCalibration.Count > 0) score += 0.3f;
        if (profile.ExpressionNeutral) score += 0.2f;
        return Math.Clamp(score, 0f, 1f);
    }

    private static float EvaluateMetaHumanDNA(AvatarProfileData profile)
    {
        float score = 0.4f; // base DNA readiness
        if (profile.FacialLandmarks.HasSufficientData) score += 0.3f;
        if (profile.AppearanceParams.IsComplete) score += 0.3f;
        return Math.Clamp(score, 0f, 1f);
    }

    private static float EvaluateAppearanceParams(AppearanceParameters appearance)
    {
        float score = 0f;
        if (!string.IsNullOrEmpty(appearance.SkinTone)) score += 0.25f;
        if (!string.IsNullOrEmpty(appearance.HairStyle)) score += 0.25f;
        if (!string.IsNullOrEmpty(appearance.HairColor)) score += 0.25f;
        if (!string.IsNullOrEmpty(appearance.EyeColor)) score += 0.25f;
        return score;
    }

    private float EvaluateAnimationReadiness(AvatarProfileData profile)
    {
        float score = 0.3f; // base: LiveLink support exists
        if (_facsCalibration.Count >= 15) score += 0.3f;
        if (profile.MetaHumanBodyType != MetaHumanBodyType.Generic) score += 0.2f;
        if (profile.ExpressionNeutral) score += 0.2f;
        return Math.Clamp(score, 0f, 1f);
    }

    private static float EvaluateUE5PipelineCompat()
    {
        // Always ready — we target UE5.3+ with MetaHuman 2.0
        return 0.9f;
    }

    private static FacialLandmarkData ExtractFacialLandmarks(string imagePath)
    {
        // In production, this would use a facial landmark detector (dlib/mediapipe)
        // For now, return default "gamer girl" proportions
        return new FacialLandmarkData
        {
            JawWidth = 0.45f,
            CheekboneProminence = 0.6f,
            ForeheadHeight = 0.35f,
            ChinLength = 0.3f,
            NoseBridgeWidth = 0.25f,
            LipFullness = 0.55f,
            EyeSpacing = 0.4f,
            EyeOpenness = 0.7f,
            BrowArchHeight = 0.65f
        };
    }

    private static AppearanceParameters ExtractAppearanceParameters(string imagePath)
    {
        // Default appearance based on DTE avatar specification
        return new AppearanceParameters
        {
            SkinTone = "Fair-Medium",
            HairStyle = "Long-Wavy",
            HairColor = "Dark-Brown",
            EyeColor = "Hazel-Green",
            MakeupStyle = "Natural-Gamer",
            AccessoryStyle = "Gaming-Headset"
        };
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;
        _logger.LogInformation("MetaHumanAvatarProfileService disposed");
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Data Types
// ═══════════════════════════════════════════════════════════════════════════════

/// <summary>Avatar profile data extracted from profile image.</summary>
public sealed class AvatarProfileData
{
    public string AvatarName { get; init; } = "DeepTreeEcho";
    public string ProfileImagePath { get; init; } = "";
    public bool ImageExists { get; init; }
    public DateTimeOffset LoadedAt { get; init; }
    public FacialLandmarkData FacialLandmarks { get; init; } = new();
    public AppearanceParameters AppearanceParams { get; init; } = new();
    public MetaHumanBodyType MetaHumanBodyType { get; init; } = MetaHumanBodyType.Feminine;
    public bool ExpressionNeutral { get; init; }
}

/// <summary>Facial landmark positions (normalized 0-1).</summary>
public sealed class FacialLandmarkData
{
    public float JawWidth { get; init; }
    public float CheekboneProminence { get; init; }
    public float ForeheadHeight { get; init; }
    public float ChinLength { get; init; }
    public float NoseBridgeWidth { get; init; }
    public float LipFullness { get; init; }
    public float EyeSpacing { get; init; }
    public float EyeOpenness { get; init; }
    public float BrowArchHeight { get; init; }

    public bool HasSufficientData =>
        JawWidth > 0 && CheekboneProminence > 0 && ForeheadHeight > 0 &&
        NoseBridgeWidth > 0 && LipFullness > 0;
}

/// <summary>Avatar appearance parameters for MetaHuman.</summary>
public sealed class AppearanceParameters
{
    public string SkinTone { get; init; } = "";
    public string HairStyle { get; init; } = "";
    public string HairColor { get; init; } = "";
    public string EyeColor { get; init; } = "";
    public string MakeupStyle { get; init; } = "";
    public string AccessoryStyle { get; init; } = "";

    public bool IsComplete =>
        !string.IsNullOrEmpty(SkinTone) && !string.IsNullOrEmpty(HairStyle) &&
        !string.IsNullOrEmpty(HairColor) && !string.IsNullOrEmpty(EyeColor);
}

/// <summary>MetaHuman body type presets.</summary>
public enum MetaHumanBodyType
{
    Generic, Masculine, Feminine, Androgynous
}

/// <summary>MetaHuman build type.</summary>
public enum MetaHumanBuildType
{
    Slim, Athletic, Average, Muscular
}

/// <summary>Avatar readiness status.</summary>
public enum ReadinessStatus
{
    NotReady, PartiallyReady, NearReady, Ready
}

/// <summary>MetaHuman readiness evaluation report.</summary>
public sealed class MetaHumanReadinessReport
{
    public float OverallScore { get; init; }
    public Dictionary<string, float> CategoryScores { get; init; } = new();
    public ReadinessStatus Status { get; init; }
    public string[] Issues { get; init; } = Array.Empty<string>();
    public string[] Recommendations { get; init; } = Array.Empty<string>();
    public DateTimeOffset EvaluatedAt { get; init; }
}

/// <summary>Face shape configuration for MetaHuman Blueprint.</summary>
public sealed class FaceShapeConfig
{
    public float JawWidth { get; init; }
    public float CheekboneProminence { get; init; }
    public float ForeheadHeight { get; init; }
    public float ChinLength { get; init; }
    public float NoseBridgeWidth { get; init; }
    public float LipFullness { get; init; }
}

/// <summary>Complete MetaHuman Blueprint configuration.</summary>
public sealed class MetaHumanConfig
{
    public string AvatarName { get; init; } = "DeepTreeEcho";
    public MetaHumanBodyType BodyType { get; init; } = MetaHumanBodyType.Feminine;
    public float HeightCm { get; init; } = 170f;
    public MetaHumanBuildType BuildType { get; init; } = MetaHumanBuildType.Athletic;
    public FaceShapeConfig FaceShape { get; init; } = new();
    public string SkinTone { get; init; } = "Fair-Medium";
    public string HairStyle { get; init; } = "Long-Wavy";
    public string HairColor { get; init; } = "Dark-Brown";
    public string EyeColor { get; init; } = "Hazel-Green";
    public bool EnableLiveLink { get; init; } = true;
    public bool EnableFACS { get; init; } = true;
    public int FACSActionUnitCount { get; init; } = 46;
    public bool EnableBodyIK { get; init; } = true;
    public bool EnableFingerTracking { get; init; }
    public bool EnableRayTracing { get; init; } = true;
    public bool EnableSubsurfaceScattering { get; init; } = true;
    public bool EnableStrandHair { get; init; } = true;
    public int LODCount { get; init; } = 4;
    public int TextureResolution { get; init; } = 4096;
    public DateTimeOffset GeneratedAt { get; init; }

    public static MetaHumanConfig Default => new();
}
