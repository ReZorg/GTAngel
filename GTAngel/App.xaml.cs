using System.Windows;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Serilog;
using GTAngel.Interop;
using GTAngel.Services;
using GTAngel.ViewModels;

namespace GTAngel;

/// <summary>
/// Application entry point. Configures dependency injection, logging, and services.
/// Replaces: com.pairip.application.Application + Rockstar.setup()
///
/// GTAngel composition: /dte-ksm-evo-autogenesis ( /gta3-ue5-wpf ) → "GTAngel"
/// </summary>
public partial class App : Application
{
    private ServiceProvider? _serviceProvider;

    public static IServiceProvider Services { get; private set; } = null!;

    protected override void OnStartup(StartupEventArgs e)
    {
        base.OnStartup(e);

        // Determine environment (DOTNET_ENVIRONMENT or default to Production)
        var environment = Environment.GetEnvironmentVariable("DOTNET_ENVIRONMENT") ?? "Production";

        // Build configuration from appsettings files
        var configuration = new ConfigurationBuilder()
            .SetBasePath(AppDomain.CurrentDomain.BaseDirectory)
            .AddJsonFile("appsettings.json", optional: true, reloadOnChange: false)
            .AddJsonFile($"appsettings.{environment}.json", optional: true, reloadOnChange: false)
            .Build();

        // Configure Serilog from appsettings (environment-aware logging)
        Log.Logger = new LoggerConfiguration()
            .ReadFrom.Configuration(configuration)
            .Enrich.WithProperty("Environment", environment)
            .CreateLogger();

        // Build DI container (replaces manual singleton wiring in Rockstar.setup)
        var services = new ServiceCollection();
        ConfigureServices(services);
        _serviceProvider = services.BuildServiceProvider();
        Services = _serviceProvider;

        // Initialize NativeInterop with logger
        var loggerFactory = _serviceProvider.GetRequiredService<ILoggerFactory>();
        NativeInterop.Initialize(loggerFactory.CreateLogger("NativeInterop"));

        // Initialize core services
        var config = _serviceProvider.GetRequiredService<AppConfiguration>();
        config.LoadAsync().Wait();

        var localization = _serviceProvider.GetRequiredService<LocalizationService>();
        localization.LoadLanguage("en-US");

        // KSM Cycle 5: Wire DteCognitiveCoreService into ESN pipeline and training loop
        var cogCore = _serviceProvider.GetRequiredService<DteCognitiveCoreService>();
        var esnPipeline = _serviceProvider.GetRequiredService<EsnReservoirPipeline>();
        var trainingLoop = _serviceProvider.GetRequiredService<DteTrainingLoop>();
        esnPipeline.SetCognitiveCoreService(cogCore);
        trainingLoop.SetCognitiveCoreService(cogCore);
        // The same singleton is now driven by both DteTrainingLoop and
        // DTE4EAvatarService at ~4 Hz each. Enable the rate-limit gate so
        // STI/LTI don't decay twice per step and patterns aren't mined at
        // double rate when both loops run simultaneously. The 200ms window
        // is just below the 250ms step interval so a single caller running
        // alone is never throttled.
        cogCore.MinUpdateIntervalMs = 200;

        // Phase 1.3 wiring: route POI arrivals and new-cell discoveries from
        // GameWorldNavigationService into the training loop's RewardShaper so
        // RewardShaper.NavigationBonus is actually nonzero — without this,
        // Weights.Navigation × NavigationBonus is identically 0 and the
        // autogenesis Navigation-weight mutation has no effect.
        var navigation = _serviceProvider.GetRequiredService<GameWorldNavigationService>();
        trainingLoop.SetNavigationService(navigation);

        // Phase 6.1: Wire full DTE pipeline into GTAngelService for KSM orchestration
        var gtAngel = _serviceProvider.GetRequiredService<GTAngelService>();
        gtAngel.SetDtePipelineServices(trainingLoop, esnPipeline, cogCore);
        
        // KSM Cycle 6: Initialize bridge service early if needed (it gets wired in AvatarViewModel)
        var bridge = _serviceProvider.GetRequiredService<Ue5PlayerAiBridgeService>();

        Log.Information("GTAngel — Guardian Angel Cognitive Orchestrator — Initialized");
        Log.Information("Composition: /dte-ksm-evo-autogenesis ( /gta3-ue5-wpf ) → GTAngel");
        Log.Information("KSM Cycle 5: DteCognitiveCoreService wired into ESN + TrainingLoop");

        // Production services: model integrity and auto-updates
        var modelIntegrity = _serviceProvider.GetRequiredService<ModelIntegrityService>();
        _ = modelIntegrity.ValidateAllAsync(); // Fire-and-forget; logs warnings if issues found

        var updateService = _serviceProvider.GetRequiredService<UpdateService>();
        updateService.Initialize();
        _ = updateService.CheckForUpdatesAsync(); // Background update check
    }

    private static void ConfigureServices(IServiceCollection services)
    {
        // Logging
        services.AddLogging(builder =>
        {
            builder.AddSerilog();
        });

        // Core Services (replaces Rockstar singleton fields)
        services.AddSingleton<AppConfiguration>();
        services.AddSingleton<AppStateService>();
        services.AddSingleton<LocalizationService>();
        services.AddSingleton<NavigationService>();
        services.AddSingleton<AudioService>();
        services.AddSingleton<TelemetryService>();
        services.AddSingleton<FileSystemService>();
        services.AddSingleton<NotificationService>();
        services.AddSingleton<AssetDownloadService>();

        // UE Engine Integration (replaces NativeActivity + JNI bridge)
        services.AddSingleton<UEProcessManager>();

        // AngelClaw DTE Training Services
        services.AddSingleton<DxgiFrameCaptureService>();
        services.AddSingleton<VigemControllerService>();
        services.AddSingleton<OpenRwEngineBridge>();
        services.AddSingleton<EsnReservoirPipeline>();
        services.AddSingleton<ExperienceReplayBuffer>();
        services.AddSingleton<OnnxCnnFeatureExtractor>();
        services.AddSingleton<DteTrainingLoop>();
        services.AddSingleton<MultiAgentTrainer>();

        // ── GTAngel: Guardian Angel Cognitive Orchestrator ──────────────────────
        // Composition: /dte-ksm-evo-autogenesis ( /gta3-ue5-wpf ) → "GTAngel"
        services.AddSingleton<TrainingEngine>();
        services.AddSingleton<GTAngelService>();
        services.AddSingleton<GTAngelViewModel>();
        // KSM Cycle 1 — ML Vision Pipeline (768×768) — structure-preserving transformation
        // /echo ( /gta3-ue5-wpf ) → /ksm-evolve → weakest centre: ML Vision Pipeline
        services.AddSingleton<MlVisionCaptureService>();
        // KSM Cycle 2 — UE5 Build & Asset Integration — 4-stage launch orchestrator
        // /echo-wpf-ksm-evolve → weakest centre: UE5 Build & Asset Integration
        services.AddSingleton<UE5LaunchOrchestrator>();
        // KSM Cycle 3 — UE5 Avatar Embodiment — FACS+IK+Neuro+Personality pipeline
        // /echo-wpf-ksm-evolve → weakest centre: UE5 Avatar Embodiment
        services.AddSingleton<AvatarEmbodimentService>();
        // KSM Cycle 4 — Game World Navigation — A* pathfinding + POI curiosity + district coverage
        // /echo-wpf-ksm-evolve → weakest centre: Game World Navigation
        services.AddSingleton<GameWorldNavigationService>();
        // KSM Cycle 5 — DTE Cognitive Core — ECAN attention, MOSES pattern mining, Wout ridge regression, Thompson sampling
        // /echo-wpf-ksm-evolve → weakest centre: DTE Cognitive Core
        services.AddSingleton<DteCognitiveCoreService>();
        // KSM Cycle 6 — UE5 Player↔AI Bridge — Input arbitration, mode toggling, observation fusion
        // /echo-wpf-ksm-evolve → weakest centre: UE5 Player↔AI Bridge
        services.AddSingleton<Ue5PlayerAiBridgeService>();

        // API Services (replaces SocialClubAPI, RockstarMobileAPI)
        services.AddSingleton<SocialClubApiClient>();
        services.AddSingleton<RockstarApiClient>();
        services.AddSingleton<SubscriptionService>();
        services.AddSingleton<GtaPlusService>();
        services.AddSingleton<LicenseService>();

        // Production Services
        services.AddSingleton<UpdateService>();
        services.AddSingleton<ModelIntegrityService>();

        // ViewModels
        services.AddTransient<MainWindowViewModel>();
        services.AddTransient<SplashViewModel>();
        services.AddTransient<LoginViewModel>();
        services.AddTransient<DownloadViewModel>();
        services.AddTransient<SettingsViewModel>();
        services.AddTransient<BrowserViewModel>();
        services.AddTransient<GameViewModel>();
        services.AddTransient<TrialBannerViewModel>();
        services.AddTransient<OtherGamesViewModel>();
        services.AddTransient<LegalViewModel>();
    }

    protected override void OnExit(ExitEventArgs e)
    {
        Log.Information("GTAngel — Shutting down");

        // Gracefully stop GTAngel autogenesis loop
        _serviceProvider?.GetService<GTAngelService>()?.StopAsync().Wait(TimeSpan.FromSeconds(3));
        _serviceProvider?.GetService<GTAngelService>()?.Dispose();

        // Gracefully stop the engine if running
        var processManager = _serviceProvider?.GetService<UEProcessManager>();
        processManager?.Dispose();

        // Dispose DTE training services
        _serviceProvider?.GetService<DxgiFrameCaptureService>()?.Dispose();
        _serviceProvider?.GetService<VigemControllerService>()?.Dispose();
        _serviceProvider?.GetService<OpenRwEngineBridge>()?.Dispose();
        _serviceProvider?.GetService<EsnReservoirPipeline>()?.Dispose();
        _serviceProvider?.GetService<ExperienceReplayBuffer>()?.Dispose();
        _serviceProvider?.GetService<OnnxCnnFeatureExtractor>()?.Dispose();
        _serviceProvider?.GetService<DteTrainingLoop>()?.Dispose();
        _serviceProvider?.GetService<MultiAgentTrainer>()?.Dispose();

        Log.CloseAndFlush();
        _serviceProvider?.Dispose();
        base.OnExit(e);
    }
}
