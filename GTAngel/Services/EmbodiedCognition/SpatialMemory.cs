using System;
using System.Collections.Generic;
using System.Linq;
using GTAngel.Models.EmbodiedCognition;

namespace GTAngel.Services.EmbodiedCognition;

/// <summary>
/// One remembered observation of a perceived object. Confidence decays over
/// time so the avatar gradually forgets things it has not re-perceived.
/// </summary>
public sealed class SpatialMemoryEntry
{
    /// <summary>Stable identifier (forwarded from the original VisualPercept tag).</summary>
    public string Tag { get; init; } = string.Empty;

    /// <summary>Last known world position.</summary>
    public float[] WorldLocation { get; set; } = new float[3];

    /// <summary>Engine-time when this entry was last seen (seconds).</summary>
    public double LastSeen { get; set; }

    /// <summary>Confidence in [0, 1]. 1 = just-perceived, 0 = forgotten.</summary>
    public float Confidence { get; set; }

    /// <summary>How many times this object has been perceived in total.</summary>
    public int Hits { get; set; }
}

/// <summary>
/// A perception-grounded landmark / object memory.
///
/// The avatar only remembers what it has perceived, and confidence decays
/// exponentially with elapsed engine-time since the last observation. This
/// is the cognitive substrate for "limited world knowledge": the policy may
/// consult the memory, but may not consult the raw <c>AvatarObservation</c>.
///
/// Cells are bucketed onto a coarse XY grid keyed by integer coordinates so
/// repeated sightings of the same object near the same location merge into
/// a single entry.
/// </summary>
public sealed class SpatialMemory
{
    private readonly Dictionary<(string tag, int cx, int cy), SpatialMemoryEntry> _entries = new();

    /// <summary>XY cell size in unreal units. Default 100 (= 1 m).</summary>
    public float CellSize { get; set; } = 100f;

    /// <summary>Confidence decay constant (per second). Default 0.05 → ~13 s half-life.</summary>
    public float DecayPerSecond { get; set; } = 0.05f;

    /// <summary>Entries with confidence below this are pruned during decay.</summary>
    public float MinConfidence { get; set; } = 0.02f;

    /// <summary>Total entries currently held in memory.</summary>
    public int Count => _entries.Count;

    /// <summary>
    /// Update memory with the latest perceptual field. Existing entries get
    /// their position/confidence refreshed; new entries are inserted; nothing
    /// is forgotten in this call (use <see cref="Decay"/> for that).
    /// </summary>
    public void Update(PerceptualField field)
    {
        if (field == null) return;

        foreach (var v in field.Visuals)
        {
            UpsertOne(v.Tag, v.WorldLocation, field.Timestamp, v.SignalStrength);
        }

        // Auditory percepts also leave a trace, but at lower confidence —
        // hearing tells you something is roughly over there, not exactly where.
        foreach (var s in field.Sounds)
        {
            float audConf = MathF.Min(0.5f, s.Loudness * 0.5f);
            UpsertOne(s.Tag, s.WorldLocation, field.Timestamp, audConf);
        }
    }

    /// <summary>Apply exponential decay since the previous tick and prune dim entries.</summary>
    public void Decay(double currentTimestampSeconds)
    {
        if (_entries.Count == 0) return;

        var dead = new List<(string, int, int)>();
        foreach (var kv in _entries)
        {
            var entry = kv.Value;
            float elapsed = (float)Math.Max(0, currentTimestampSeconds - entry.LastSeen);
            if (elapsed > 0f)
            {
                entry.Confidence *= MathF.Exp(-DecayPerSecond * elapsed);
                entry.LastSeen = currentTimestampSeconds;
            }
            if (entry.Confidence < MinConfidence) dead.Add(kv.Key);
        }
        foreach (var k in dead) _entries.Remove(k);
    }

    /// <summary>
    /// Recall the strongest known location of an object with the given tag,
    /// or <c>null</c> if no surviving memory exists.
    /// </summary>
    public SpatialMemoryEntry? RecallByTag(string tag)
    {
        SpatialMemoryEntry? best = null;
        foreach (var kv in _entries)
        {
            if (!string.Equals(kv.Key.tag, tag, StringComparison.OrdinalIgnoreCase)) continue;
            if (best == null || kv.Value.Confidence > best.Confidence) best = kv.Value;
        }
        return best;
    }

    /// <summary>Snapshot of all entries currently held in memory (defensively cloned).</summary>
    public IReadOnlyList<SpatialMemoryEntry> Snapshot()
        => _entries.Values
            .Select(e => new SpatialMemoryEntry
            {
                Tag = e.Tag,
                WorldLocation = (float[])e.WorldLocation.Clone(),
                LastSeen = e.LastSeen,
                Confidence = e.Confidence,
                Hits = e.Hits
            })
            .ToList();

    /// <summary>Forget everything.</summary>
    public void Clear() => _entries.Clear();

    // ── Internals ─────────────────────────────────────────────────────────

    private void UpsertOne(string tag, float[] worldLocation, double timestamp, float observedStrength)
    {
        if (string.IsNullOrEmpty(tag) || worldLocation == null || worldLocation.Length < 2) return;

        int cx = (int)MathF.Floor(worldLocation[0] / CellSize);
        int cy = (int)MathF.Floor(worldLocation[1] / CellSize);
        var key = (tag, cx, cy);

        if (!_entries.TryGetValue(key, out var entry))
        {
            entry = new SpatialMemoryEntry { Tag = tag };
            _entries[key] = entry;
        }
        entry.WorldLocation = (float[])worldLocation.Clone();
        entry.LastSeen = timestamp;
        // Bump confidence toward the observed strength, never down.
        entry.Confidence = MathF.Max(entry.Confidence, Clamp01(observedStrength));
        entry.Hits++;
    }

    private static float Clamp01(float v) => v < 0f ? 0f : (v > 1f ? 1f : v);
}
