using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using GTAngel.Services;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for AutonomousTrainingService and the re3 external-engine integration path:
///   - Options are bound from the AutonomousTraining configuration section.
///   - OpenRwEngineBridge preserves a manually configured re3 executable path across DetectEngines().
///   - Starting the service without an engine fails gracefully.
/// </summary>
public sealed class AutonomousTrainingServiceTests
{
    private readonly IConfiguration _configuration = new ConfigurationBuilder()
        .AddInMemoryCollection(new Dictionary<string, string?>
        {
            ["AutonomousTraining:Re3ExecutablePath"] = "C:\\re3\\re3.exe",
            ["AutonomousTraining:Re3GameDataPath"] = "C:\\re3\\gamefiles",
            ["AutonomousTraining:TargetFps"] = "45",
            ["AutonomousTraining:MaxStepsPerEpisode"] = "500",
            ["AutonomousTraining:MaxEpisodes"] = "5",
            ["AutonomousTraining:TrainingMode"] = "Hybrid",
            ["AutonomousTraining:InitializeDtePipeline"] = "false",
        })
        .Build();

    [Fact]
    public void Options_AreBound_FromConfiguration()
    {
        var service = new AutonomousTrainingService(
            NullLogger<AutonomousTrainingService>.Instance,
            new OpenRwEngineBridge(NullLogger<OpenRwEngineBridge>.Instance),
            new VigemControllerService(NullLogger<VigemControllerService>.Instance),
            CreateLoop(),
            _configuration);

        Assert.Equal("C:\\re3\\re3.exe", service.Options.Re3ExecutablePath);
        Assert.Equal("C:\\re3\\gamefiles", service.Options.Re3GameDataPath);
        Assert.Equal(45, service.Options.TargetFps);
        Assert.Equal(500, service.Options.MaxStepsPerEpisode);
        Assert.Equal(5, service.Options.MaxEpisodes);
        Assert.Equal(DteTrainingMode.Hybrid, service.Options.TrainingMode);
        Assert.False(service.Options.InitializeDtePipeline);
    }

    [Fact]
    public async Task StartAsync_WithoutConfiguredOrDetectedEngine_ReturnsFalseAndRecordsError()
    {
        var service = new AutonomousTrainingService(
            NullLogger<AutonomousTrainingService>.Instance,
            new OpenRwEngineBridge(NullLogger<OpenRwEngineBridge>.Instance),
            new VigemControllerService(NullLogger<VigemControllerService>.Instance),
            CreateLoop(),
            _configuration);

        var started = await service.StartAsync();

        Assert.False(started);
        Assert.NotNull(service.State.LastError);
    }

    [Fact]
    public void DetectEngines_PreservesManuallyConfiguredRe3Path()
    {
        var bridge = new OpenRwEngineBridge(NullLogger<OpenRwEngineBridge>.Instance);
        var tempExe = Path.GetTempFileName();

        try
        {
            bridge.SetEnginePath(tempExe, OpenRwEngineBridge.EngineType.Re3);
            var detected = bridge.DetectEngines();

            Assert.Equal(OpenRwEngineBridge.EngineType.Re3, bridge.DetectedEngine);
            Assert.Equal(OpenRwEngineBridge.EngineType.Re3, detected);
            Assert.Equal(tempExe, bridge.EnginePath);
        }
        finally
        {
            try { File.Delete(tempExe); } catch { /* ignore */ }
        }
    }

    private static DteTrainingLoop CreateLoop()
    {
        var engine = new OpenRwEngineBridge(NullLogger<OpenRwEngineBridge>.Instance);
        var controller = new VigemControllerService(NullLogger<VigemControllerService>.Instance);
        var capture = new DxgiFrameCaptureService(NullLogger<DxgiFrameCaptureService>.Instance);
        var reservoir = new EsnReservoirPipeline(NullLogger<EsnReservoirPipeline>.Instance);
        var buffer = new ExperienceReplayBuffer(
            NullLogger<ExperienceReplayBuffer>.Instance,
            capacity: 1000,
            alpha: 0.6f,
            beta: 0.4f,
            nStep: 1);
        var extractor = new OnnxCnnFeatureExtractor(NullLogger<OnnxCnnFeatureExtractor>.Instance);

        return new DteTrainingLoop(
            NullLogger<DteTrainingLoop>.Instance,
            capture, controller, engine,
            reservoir, buffer, extractor);
    }
}
