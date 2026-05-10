using System;
using System.Linq;
using GTAngel.Models.EmbodiedCognition;
using GTAngel.Services.EmbodiedCognition;
using Xunit;

namespace GTAngel.Tests.Services.EmbodiedCognition;

/// <summary>
/// Tests for <see cref="SpatialMemory"/>: insertion/merging, exponential decay,
/// recall by tag, and pruning of dim entries.
/// </summary>
public sealed class SpatialMemoryTests
{
    private static PerceptualField FieldWith(double timestamp, params VisualPercept[] visuals)
        => new()
        {
            Timestamp = timestamp,
            Self = new EmbodiedSelfState(),
            Visuals = visuals,
            Sounds = Array.Empty<AuditoryPercept>()
        };

    private static VisualPercept VPerc(string tag, float x, float y, float strength = 1f)
        => new()
        {
            Tag = tag,
            WorldLocation = new[] { x, y, 0f },
            SignalStrength = strength,
            Distance = MathF.Sqrt(x * x + y * y)
        };

    [Fact]
    public void Update_AddsNewEntries()
    {
        var mem = new SpatialMemory();
        mem.Update(FieldWith(1.0,
            VPerc("Door", 100f, 200f, 0.8f),
            VPerc("Pickup", -50f, 50f, 0.6f)));

        Assert.Equal(2, mem.Count);
    }

    [Fact]
    public void Update_MergesRepeatedSightingsInSameCell()
    {
        var mem = new SpatialMemory { CellSize = 100f };
        mem.Update(FieldWith(1.0, VPerc("Door", 110f, 220f, 0.5f)));
        // Same logical cell (1, 2) → should merge into the existing entry.
        mem.Update(FieldWith(2.0, VPerc("Door", 150f, 250f, 0.9f)));

        Assert.Equal(1, mem.Count);
        var snap = mem.Snapshot();
        Assert.Equal(2, snap[0].Hits);
        // Confidence should ratchet up to the new, stronger observation.
        Assert.Equal(0.9f, snap[0].Confidence, 2);
    }

    [Fact]
    public void Update_KeepsSeparateEntries_WhenObjectMovesAcrossCells()
    {
        var mem = new SpatialMemory { CellSize = 100f };
        mem.Update(FieldWith(1.0, VPerc("Door", 50f, 50f, 0.8f)));
        mem.Update(FieldWith(2.0, VPerc("Door", 500f, 500f, 0.8f)));

        Assert.Equal(2, mem.Count);
    }

    [Fact]
    public void Decay_DecreasesConfidenceOverTime()
    {
        var mem = new SpatialMemory
        {
            CellSize = 100f,
            DecayPerSecond = 0.5f,
            MinConfidence = 0.001f
        };
        mem.Update(FieldWith(0.0, VPerc("Door", 100f, 100f, 1f)));

        var pre = mem.Snapshot()[0].Confidence;
        mem.Decay(2.0); // 2s elapsed → confidence ≈ exp(-1) ≈ 0.368
        var post = mem.Snapshot()[0].Confidence;

        Assert.True(post < pre);
        Assert.InRange(post, 0.30f, 0.42f);
    }

    [Fact]
    public void Decay_PrunesEntriesBelowMinConfidence()
    {
        var mem = new SpatialMemory
        {
            CellSize = 100f,
            DecayPerSecond = 5f,
            MinConfidence = 0.1f
        };
        mem.Update(FieldWith(0.0, VPerc("Door", 100f, 100f, 1f)));
        mem.Decay(10.0); // ridiculously long time → far below min confidence.

        Assert.Equal(0, mem.Count);
    }

    [Fact]
    public void RecallByTag_ReturnsHighestConfidenceMatch()
    {
        var mem = new SpatialMemory { CellSize = 100f };
        mem.Update(FieldWith(0.0, VPerc("NPC", 100f, 100f, 0.3f)));
        mem.Update(FieldWith(1.0, VPerc("NPC", 1000f, 1000f, 0.9f)));

        var got = mem.RecallByTag("NPC");

        Assert.NotNull(got);
        Assert.Equal(0.9f, got!.Confidence, 2);
        // The 0.9-confidence one is at (1000, 1000).
        Assert.Equal(1000f, got.WorldLocation[0], 1);
    }

    [Fact]
    public void RecallByTag_IgnoresUnknownTags()
    {
        var mem = new SpatialMemory();
        mem.Update(FieldWith(0.0, VPerc("Door", 100f, 100f, 1f)));

        Assert.Null(mem.RecallByTag("Lemur"));
    }

    [Fact]
    public void AuditoryPerceptsLeaveWeakerTrace_ThanVisuals()
    {
        var mem = new SpatialMemory { CellSize = 100f };
        var field = new PerceptualField
        {
            Timestamp = 0.0,
            Self = new EmbodiedSelfState(),
            Visuals = Array.Empty<VisualPercept>(),
            Sounds = new[]
            {
                new AuditoryPercept
                {
                    Tag = "Footstep",
                    WorldLocation = new[] { 200f, 200f, 0f },
                    Loudness = 0.6f
                }
            }
        };

        mem.Update(field);

        var snap = mem.Snapshot();
        Assert.Single(snap);
        // Auditory traces are capped at half loudness (0.5) and at 0.5 absolute.
        Assert.True(snap[0].Confidence <= 0.5f + 1e-3f);
        Assert.True(snap[0].Confidence >= 0.25f);
    }

    [Fact]
    public void Clear_RemovesAllEntries()
    {
        var mem = new SpatialMemory();
        mem.Update(FieldWith(0.0,
            VPerc("A", 100f, 100f, 1f),
            VPerc("B", 200f, 200f, 1f)));
        Assert.Equal(2, mem.Count);

        mem.Clear();
        Assert.Equal(0, mem.Count);
    }
}
