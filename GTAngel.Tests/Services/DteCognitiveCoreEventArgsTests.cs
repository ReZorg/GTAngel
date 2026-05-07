using GTAngel.Services;
using Xunit;

namespace GTAngel.Tests.Services;

public class DteCognitiveCoreEventArgsTests
{
    [Fact]
    public void WoutTrainedEventArgs_ExposesSnapshot()
    {
        var snapshot = new WoutTrainingSnapshot(0.125, 64, 1.75, true);
        var args = new WoutTrainedEventArgs(snapshot);

        Assert.Same(snapshot, args.Snapshot);
        Assert.True(args.Snapshot.IsConverged);
    }

    [Fact]
    public void CognitiveLogEventArgs_StoresMessageLevelAndTimestamp()
    {
        var before = DateTime.UtcNow.AddSeconds(-1);
        var args = new CognitiveLogEventArgs("Attention updated", "DEBUG");
        var after = DateTime.UtcNow.AddSeconds(1);

        Assert.Equal("Attention updated", args.Message);
        Assert.Equal("DEBUG", args.Level);
        Assert.InRange(args.Timestamp, before, after);
    }
}
