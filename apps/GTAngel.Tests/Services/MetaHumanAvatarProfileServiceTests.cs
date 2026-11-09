using System;
using System.Collections.Generic;
using System.IO;
using GTAngel.Services;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Comprehensive tests for MetaHumanAvatarProfileService — UE5 MetaHuman integration evaluator.
/// </summary>
public class MetaHumanAvatarProfileServiceTests
{
    private readonly MetaHumanAvatarProfileService _sut;

    public MetaHumanAvatarProfileServiceTests()
    {
        _sut = new MetaHumanAvatarProfileService(NullLogger<MetaHumanAvatarProfileService>.Instance);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Construction & Initialization
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void Constructor_InitializesDefaultState()
    {
        Assert.Null(_sut.CurrentProfile);
        Assert.Null(_sut.LastReadinessReport);
        Assert.NotNull(_sut.Configuration);
        Assert.Equal(0f, _sut.FacsCalibrationQuality);
    }

    [Fact]
    public void Constructor_ThrowsOnNullLogger()
    {
        Assert.Throws<ArgumentNullException>(() =>
            new MetaHumanAvatarProfileService(null!));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Profile Loading
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void LoadProfile_ReturnsProfileData()
    {
        var profile = _sut.LoadProfile("test_image.png", "TestAvatar");

        Assert.NotNull(profile);
        Assert.Equal("TestAvatar", profile.AvatarName);
        Assert.Equal("test_image.png", profile.ProfileImagePath);
        Assert.False(profile.ImageExists); // file doesn't actually exist
        Assert.NotNull(profile.FacialLandmarks);
        Assert.NotNull(profile.AppearanceParams);
    }

    [Fact]
    public void LoadProfile_SetsCurrentProfile()
    {
        _sut.LoadProfile("avatar.png", "DTE");
        Assert.NotNull(_sut.CurrentProfile);
        Assert.Equal("DTE", _sut.CurrentProfile!.AvatarName);
    }

    [Fact]
    public void LoadProfile_RaisesProfileUpdatedEvent()
    {
        AvatarProfileData? received = null;
        _sut.ProfileUpdated += (_, p) => received = p;

        _sut.LoadProfile("test.png");

        Assert.NotNull(received);
    }

    [Fact]
    public void LoadProfile_ThrowsOnEmptyPath()
    {
        Assert.Throws<ArgumentException>(() => _sut.LoadProfile(""));
        Assert.Throws<ArgumentException>(() => _sut.LoadProfile(null!));
    }

    [Fact]
    public void LoadProfile_DefaultAvatarNameIsDeepTreeEcho()
    {
        var profile = _sut.LoadProfile("image.png");
        Assert.Equal("DeepTreeEcho", profile.AvatarName);
    }

    [Fact]
    public void LoadProfileFromParameters_ReturnsProfile()
    {
        var appearance = new AppearanceParameters
        {
            SkinTone = "Medium",
            HairStyle = "Pixie-Cut",
            HairColor = "Platinum",
            EyeColor = "Blue"
        };

        var profile = _sut.LoadProfileFromParameters("GamerGirl", MetaHumanBodyType.Feminine, appearance);

        Assert.Equal("GamerGirl", profile.AvatarName);
        Assert.Equal(MetaHumanBodyType.Feminine, profile.MetaHumanBodyType);
        Assert.Equal("Medium", profile.AppearanceParams.SkinTone);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Readiness Evaluation
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void EvaluateReadiness_NoProfile_ReturnsNotReady()
    {
        var report = _sut.EvaluateReadiness();

        Assert.Equal(0f, report.OverallScore);
        Assert.Equal(ReadinessStatus.NotReady, report.Status);
        Assert.Contains("No profile loaded", report.Issues);
    }

    [Fact]
    public void EvaluateReadiness_WithProfile_ReturnsScores()
    {
        _sut.LoadProfile("avatar.png", "DTE");
        var report = _sut.EvaluateReadiness();

        Assert.True(report.OverallScore > 0f);
        Assert.True(report.OverallScore <= 1f);
        Assert.NotEmpty(report.CategoryScores);
        Assert.Contains("ImageQuality", report.CategoryScores.Keys);
        Assert.Contains("FACSCompatibility", report.CategoryScores.Keys);
        Assert.Contains("MetaHumanDNA", report.CategoryScores.Keys);
        Assert.Contains("UE5Pipeline", report.CategoryScores.Keys);
    }

    [Fact]
    public void EvaluateReadiness_WithFullProfile_ScoresHigher()
    {
        var appearance = new AppearanceParameters
        {
            SkinTone = "Fair-Medium",
            HairStyle = "Long-Wavy",
            HairColor = "Dark-Brown",
            EyeColor = "Hazel-Green"
        };
        _sut.LoadProfileFromParameters("DTE", MetaHumanBodyType.Feminine, appearance);
        _sut.CalibrateFACS();

        var report = _sut.EvaluateReadiness();

        // With calibration and full appearance, score should be decent
        Assert.True(report.OverallScore >= 0.4f);
    }

    [Fact]
    public void EvaluateReadiness_RaisesEvent()
    {
        MetaHumanReadinessReport? received = null;
        _sut.ReadinessEvaluated += (_, r) => received = r;

        _sut.LoadProfile("test.png");
        _sut.EvaluateReadiness();

        Assert.NotNull(received);
    }

    [Fact]
    public void EvaluateReadiness_SetsLastReport()
    {
        _sut.LoadProfile("test.png");
        _sut.EvaluateReadiness();
        Assert.NotNull(_sut.LastReadinessReport);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // MetaHuman Configuration
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void GenerateConfiguration_NoProfile_ReturnsDefault()
    {
        var config = _sut.GenerateConfiguration();
        Assert.Equal("DeepTreeEcho", config.AvatarName);
    }

    [Fact]
    public void GenerateConfiguration_WithProfile_UsesProfileData()
    {
        _sut.LoadProfile("avatar.png", "MyAvatar");
        var config = _sut.GenerateConfiguration();

        Assert.Equal("MyAvatar", config.AvatarName);
        Assert.True(config.EnableLiveLink);
        Assert.True(config.EnableFACS);
        Assert.Equal(46, config.FACSActionUnitCount);
        Assert.True(config.EnableBodyIK);
        Assert.True(config.EnableRayTracing);
        Assert.True(config.EnableSubsurfaceScattering);
        Assert.Equal(4096, config.TextureResolution);
    }

    [Fact]
    public void GenerateConfiguration_RaisesEvent()
    {
        MetaHumanConfig? received = null;
        _sut.ConfigurationGenerated += (_, c) => received = c;

        _sut.LoadProfile("test.png");
        _sut.GenerateConfiguration();

        Assert.NotNull(received);
    }

    [Fact]
    public void GenerateConfiguration_SetsConfigurationProperty()
    {
        _sut.LoadProfile("test.png", "TestAvatar");
        _sut.GenerateConfiguration();

        Assert.Equal("TestAvatar", _sut.Configuration.AvatarName);
    }

    [Fact]
    public void GenerateConfiguration_IncludesFaceShape()
    {
        _sut.LoadProfile("test.png");
        var config = _sut.GenerateConfiguration();

        Assert.NotNull(config.FaceShape);
        Assert.True(config.FaceShape.JawWidth > 0);
        Assert.True(config.FaceShape.CheekboneProminence > 0);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // FACS Calibration
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void CalibrateFACS_ReturnsActionUnits()
    {
        _sut.LoadProfile("test.png");
        var facs = _sut.CalibrateFACS();

        Assert.NotEmpty(facs);
        Assert.True(facs.Count >= 15); // at least standard AUs
    }

    [Fact]
    public void CalibrateFACS_SetsQuality()
    {
        _sut.LoadProfile("test.png");
        _sut.CalibrateFACS();

        Assert.True(_sut.FacsCalibrationQuality > 0f);
    }

    [Fact]
    public void CalibrateFACS_IncludesSmileBaseline()
    {
        _sut.LoadProfile("test.png");
        var facs = _sut.CalibrateFACS();

        // AU12 (Lip Corner Puller / Smile) should have a slight baseline
        Assert.True(facs.ContainsKey(12));
        Assert.True(facs[12] > 0f); // personality charm baseline
    }

    [Fact]
    public void CalibrateFACS_WithoutImage_LowerQuality()
    {
        _sut.LoadProfileFromParameters("Test", MetaHumanBodyType.Feminine, new AppearanceParameters());
        _sut.CalibrateFACS();

        Assert.True(_sut.FacsCalibrationQuality <= 0.7f);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Configuration Export
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void ExportConfigurationJson_ReturnsValidJson()
    {
        _sut.LoadProfile("test.png", "ExportTest");
        _sut.GenerateConfiguration();

        var json = _sut.ExportConfigurationJson();

        Assert.NotEmpty(json);
        Assert.Contains("avatarName", json);
        Assert.Contains("ExportTest", json);
        Assert.Contains("enableLiveLink", json);
    }

    [Fact]
    public void ExportConfigurationJson_DefaultConfig_IsValidJson()
    {
        var json = _sut.ExportConfigurationJson();
        Assert.NotEmpty(json);
        Assert.StartsWith("{", json);
        Assert.EndsWith("}", json);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Data Type Validation
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void FacialLandmarkData_HasSufficientData_WhenPopulated()
    {
        var landmarks = new FacialLandmarkData
        {
            JawWidth = 0.5f,
            CheekboneProminence = 0.6f,
            ForeheadHeight = 0.4f,
            NoseBridgeWidth = 0.3f,
            LipFullness = 0.5f
        };
        Assert.True(landmarks.HasSufficientData);
    }

    [Fact]
    public void FacialLandmarkData_InsufficientData_WhenEmpty()
    {
        var landmarks = new FacialLandmarkData();
        Assert.False(landmarks.HasSufficientData);
    }

    [Fact]
    public void AppearanceParameters_IsComplete_WhenAllPopulated()
    {
        var appearance = new AppearanceParameters
        {
            SkinTone = "Fair",
            HairStyle = "Long",
            HairColor = "Brown",
            EyeColor = "Green"
        };
        Assert.True(appearance.IsComplete);
    }

    [Fact]
    public void AppearanceParameters_NotComplete_WhenPartial()
    {
        var appearance = new AppearanceParameters
        {
            SkinTone = "Fair"
            // missing other fields
        };
        Assert.False(appearance.IsComplete);
    }

    [Fact]
    public void MetaHumanConfig_Default_HasSensibleValues()
    {
        var config = MetaHumanConfig.Default;
        Assert.Equal("DeepTreeEcho", config.AvatarName);
        Assert.Equal(MetaHumanBodyType.Feminine, config.BodyType);
        Assert.Equal(170f, config.HeightCm);
        Assert.True(config.EnableLiveLink);
        Assert.True(config.EnableFACS);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Dispose
    // ═══════════════════════════════════════════════════════════════════════════

    [Fact]
    public void Dispose_DoesNotThrow()
    {
        var sut = new MetaHumanAvatarProfileService(
            NullLogger<MetaHumanAvatarProfileService>.Instance);
        sut.Dispose();
        sut.Dispose(); // double dispose safe
    }
}

/// <summary>
/// E2E integration test for the full DTE avatar pipeline.
/// </summary>
[Trait("Category", "E2E")]
public class MetaHumanAvatarE2ETests
{
    [Fact]
    public void FullPipeline_LoadCalibratEvaluateExport()
    {
        var service = new MetaHumanAvatarProfileService(
            NullLogger<MetaHumanAvatarProfileService>.Instance);

        // Step 1: Load profile
        var profile = service.LoadProfile("deep_tree_echo_avatar.png", "DeepTreeEcho");
        Assert.NotNull(profile);

        // Step 2: Calibrate FACS
        var facs = service.CalibrateFACS();
        Assert.True(facs.Count > 0);

        // Step 3: Generate configuration
        var config = service.GenerateConfiguration();
        Assert.Equal("DeepTreeEcho", config.AvatarName);
        Assert.True(config.EnableFACS);

        // Step 4: Evaluate readiness
        var report = service.EvaluateReadiness();
        Assert.True(report.OverallScore > 0f);

        // Step 5: Export
        var json = service.ExportConfigurationJson();
        Assert.Contains("DeepTreeEcho", json);
        Assert.Contains("enableFACS", json);

        service.Dispose();
    }

    [Fact]
    public void GamerGirlProfile_ReadinessEvaluation()
    {
        var service = new MetaHumanAvatarProfileService(
            NullLogger<MetaHumanAvatarProfileService>.Instance);

        var appearance = new AppearanceParameters
        {
            SkinTone = "Fair-Medium",
            HairStyle = "Long-Wavy",
            HairColor = "Dark-Brown",
            EyeColor = "Hazel-Green",
            MakeupStyle = "Natural-Gamer",
            AccessoryStyle = "Gaming-Headset"
        };

        service.LoadProfileFromParameters("GamerGirlDTE", MetaHumanBodyType.Feminine, appearance);
        service.CalibrateFACS();

        var report = service.EvaluateReadiness();

        Assert.True(report.OverallScore >= 0.4f);
        Assert.NotEqual(ReadinessStatus.NotReady, report.Status);

        service.Dispose();
    }
}
