using GTAngel.Interop;
using Xunit;

namespace GTAngel.Tests.Interop;

public class UEInteropModelsTests
{
    [Fact]
    public void UE5ProjectInfo_ToString_IncludesNameCategoryAndFlags()
    {
        var project = new UE5ProjectInfo
        {
            Name = "LibertyCity",
            Category = "Training",
            HasBinaries = true,
            HasContent = false,
            HasCookedContent = true,
        };

        Assert.Equal("LibertyCity [Training] (bin=True, content=False, cooked=True)", project.ToString());
    }

    [Fact]
    public void BuildProgressEventArgs_PropertiesRoundTrip()
    {
        var args = new BuildProgressEventArgs
        {
            Stage = "Compile",
            Progress = 0.75,
            Message = "Compiling cognitive modules",
        };

        Assert.Equal("Compile", args.Stage);
        Assert.Equal(0.75, args.Progress);
        Assert.Equal("Compiling cognitive modules", args.Message);
    }

    [Fact]
    public void BuildCompletedEventArgs_DefaultErrors_IsEmpty()
    {
        var args = new BuildCompletedEventArgs();
        Assert.Empty(args.Errors);
    }

    [Fact]
    public void BuildCompletedEventArgs_PropertiesRoundTrip()
    {
        var duration = TimeSpan.FromSeconds(12);
        var args = new BuildCompletedEventArgs
        {
            Success = true,
            OutputPath = @"C:\Build\Output",
            Duration = duration,
            Errors = ["warn-1", "warn-2"],
        };

        Assert.True(args.Success);
        Assert.Equal(@"C:\Build\Output", args.OutputPath);
        Assert.Equal(duration, args.Duration);
        Assert.Collection(
            args.Errors,
            error => Assert.Equal("warn-1", error),
            error => Assert.Equal("warn-2", error));
    }

    [Fact]
    public void UE5IntegrationStatus_DefaultProjects_IsInitialized()
    {
        var status = new UE5IntegrationStatus();
        Assert.NotNull(status.Projects);
        Assert.Empty(status.Projects);
    }

    [Fact]
    public void DiscoveredUEProject_DefaultEngineVersion_IsUnknown()
    {
        var project = new DiscoveredUEProject();
        Assert.Equal("Unknown", project.EngineVersion);
    }
}
