using System.ComponentModel;
using GTAngel.ViewModels;
using Xunit;

namespace GTAngel.Tests.ViewModels;

public class ViewModelHelperTests
{
    [Fact]
    public void AlexanderPropertyVM_ComputedTextsReflectValues()
    {
        var vm = new AlexanderPropertyVM
        {
            Score = 0.75,
            Delta = 0.125,
        };

        Assert.Contains("75", vm.ScoreText);
        Assert.StartsWith("+", vm.DeltaText);
    }

    [Fact]
    public void AssetCategoryInfo_SizeFormatted_UsesKbAndMbThresholds()
    {
        var kb = new AssetCategoryInfo { SizeBytes = 512 * 1024 };
        var mb = new AssetCategoryInfo { SizeBytes = 3 * 1024 * 1024, Percentage = 0.25 };

        Assert.Equal("512.0 KB", kb.SizeFormatted);
        Assert.Equal("3.0 MB", mb.SizeFormatted);
        Assert.Contains("25", mb.PercentageText);
    }

    [Fact]
    public void UE5ModuleStatus_RaisesPropertyChanged_ForStatusAndReadiness()
    {
        var vm = new UE5ModuleStatus();
        var changed = new List<string>();
        vm.PropertyChanged += (_, e) => changed.Add(e.PropertyName ?? string.Empty);

        vm.Status = "Ready";
        vm.IsReady = true;

        Assert.Contains(nameof(UE5ModuleStatus.Status), changed);
        Assert.Contains(nameof(UE5ModuleStatus.IsReady), changed);
    }
}
