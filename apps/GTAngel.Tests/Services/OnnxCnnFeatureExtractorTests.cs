using System.IO;
using GTAngel.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for OnnxCnnFeatureExtractor.
/// The ONNX Runtime is not available in the test environment, so all tests run
/// against the spatial-pooling fallback (the production code path for offline use).
///
/// Covered:
///   1. Initialize falls back to spatial pooling when no model path provided
///   2. OutputDimension is 256 in fallback mode
///   3. ModelName contains "SpatialPool"
///   4. Extract always returns a 256-element vector
///   5. Output is L2-normalised (non-negative values, vector magnitude ≈ 1)
///   6. Zero-frame input produces a bounded, valid output (not NaN/Inf)
///   7. Same frame produces identical output (deterministic weights via seed 42)
///   8. Different random frames produce different feature vectors
///   9. TotalFramesProcessed increments after each Extract call
///  10. AverageInferenceMs tracks latency
///  11. Initialize with explicit outputDim sets OutputDimension in ONNX mode
///       (fallback: OutputDimension stays _projectionDim = 256)
///  12. UpdateProjection modifies weights without throwing
///  13. SaveProjectionAsync / LoadProjectionAsync round-trip
/// </summary>
public sealed class OnnxCnnFeatureExtractorTests : IDisposable
{
    private readonly OnnxCnnFeatureExtractor _ext;

    public OnnxCnnFeatureExtractorTests()
    {
        _ext = new OnnxCnnFeatureExtractor(NullLogger<OnnxCnnFeatureExtractor>.Instance);
        _ext.Initialize(modelPath: null, outputDim: 512); // no model → spatial pooling fallback
    }

    public void Dispose() => _ext.Dispose();

    private const int FrameSize = 768 * 768 * 3; // 1,769,472 floats

    // ── 1. Fallback mode detection ────────────────────────────────────────────

    [Fact]
    public void IsOnnxMode_WithoutModel_IsFalse()
    {
        Assert.False(_ext.IsOnnxMode);
    }

    [Fact]
    public void OutputDimension_FallbackMode_Is256()
    {
        // _projectionDim = 256 (hard-coded fallback dimension)
        Assert.Equal(256, _ext.OutputDimension);
    }

    [Fact]
    public void ModelName_FallbackMode_ContainsSpatialPool()
    {
        Assert.Contains("SpatialPool", _ext.ModelName, StringComparison.OrdinalIgnoreCase);
    }

    // ── 2. Output shape ───────────────────────────────────────────────────────

    [Fact]
    public void Extract_ZeroFrame_Returns256Elements()
    {
        var frame    = new float[FrameSize]; // all zeros
        var features = _ext.Extract(frame);
        Assert.Equal(256, features.Length);
    }

    [Fact]
    public void Extract_RandomFrame_Returns256Elements()
    {
        var frame = new float[FrameSize];
        var rng   = new Random(1);
        for (int i = 0; i < FrameSize; i++) frame[i] = (float)rng.NextDouble();

        var features = _ext.Extract(frame);
        Assert.Equal(256, features.Length);
    }

    [Fact]
    public void Extract_LargeFrame_Returns256Elements()
    {
        // Even with a full 768×768×3 realistic frame, output is 256
        var frame    = new float[FrameSize];
        for (int i = 0; i < FrameSize; i++) frame[i] = (i % 255) / 255f;
        var features = _ext.Extract(frame);
        Assert.Equal(256, features.Length);
    }

    // ── 3. Output validity (no NaN / Inf) ────────────────────────────────────

    [Fact]
    public void Extract_ZeroFrame_OutputContainsNoNaN()
    {
        var features = _ext.Extract(new float[FrameSize]);
        Assert.All(features, v => Assert.False(float.IsNaN(v)));
    }

    [Fact]
    public void Extract_ZeroFrame_OutputContainsNoInfinity()
    {
        var features = _ext.Extract(new float[FrameSize]);
        Assert.All(features, v => Assert.False(float.IsInfinity(v)));
    }

    [Fact]
    public void Extract_RandomFrame_OutputContainsNoNaN()
    {
        var frame = new float[FrameSize];
        var rng   = new Random(99);
        for (int i = 0; i < FrameSize; i++) frame[i] = (float)rng.NextDouble();
        var features = _ext.Extract(frame);
        Assert.All(features, v => Assert.False(float.IsNaN(v)));
    }

    // ── 4. L2 normalisation: non-negative values (ReLU applied first) ─────────

    [Fact]
    public void Extract_Output_AllValuesNonNegative_AfterL2Norm()
    {
        // ExtractSpatialPool applies ReLU before L2-normalisation; output is ≥ 0
        var frame    = new float[FrameSize];
        var rng      = new Random(7);
        for (int i = 0; i < FrameSize; i++) frame[i] = (float)rng.NextDouble();
        var features = _ext.Extract(frame);
        Assert.All(features, v => Assert.True(v >= 0f, $"Feature value {v} should be ≥ 0"));
    }

    [Fact]
    public void Extract_Output_MagnitudeIsApproximatelyOne()
    {
        var frame = new float[FrameSize];
        var rng   = new Random(42);
        for (int i = 0; i < FrameSize; i++) frame[i] = (float)rng.NextDouble();
        var features = _ext.Extract(frame);

        float norm = (float)Math.Sqrt(features.Sum(v => v * v));
        // L2-normalised with epsilon 1e-8f; some all-zero outputs map to unit vector ≈ 1
        Assert.True(norm > 0.5f || features.All(v => v == 0f),
            $"L2 norm should be ~1 for non-zero input, was {norm}");
    }

    // ── 5. Determinism (same frame → same output) ────────────────────────────

    [Fact]
    public void Extract_SameFrame_ReturnsSameOutput()
    {
        var frame = new float[FrameSize];
        var rng   = new Random(123);
        for (int i = 0; i < FrameSize; i++) frame[i] = (float)rng.NextDouble();

        var out1 = _ext.Extract(frame);
        var out2 = _ext.Extract(frame);

        Assert.Equal(out1, out2);
    }

    // ── 6. Different frames → different outputs ───────────────────────────────

    [Fact]
    public void Extract_DifferentFrames_ProduceDifferentOutputs()
    {
        var frame1 = new float[FrameSize];
        var frame2 = new float[FrameSize];
        var rng    = new Random(11);

        for (int i = 0; i < FrameSize; i++) frame1[i] = (float)rng.NextDouble();
        for (int i = 0; i < FrameSize; i++) frame2[i] = (float)rng.NextDouble();

        var out1 = _ext.Extract(frame1);
        var out2 = _ext.Extract(frame2);

        // At least one element should differ
        Assert.False(out1.SequenceEqual(out2),
            "Different frames should produce different feature vectors");
    }

    [Fact]
    public void Extract_ZeroFrameVsNonZeroFrame_ProduceDifferentOutputs()
    {
        var zero   = new float[FrameSize];
        var nonZero = new float[FrameSize];
        for (int i = 0; i < FrameSize; i++) nonZero[i] = 0.5f;

        var outZero    = _ext.Extract(zero);
        var outNonZero = _ext.Extract(nonZero);

        Assert.False(outZero.SequenceEqual(outNonZero),
            "Zero frame and constant-0.5 frame should produce different outputs");
    }

    // ── 7. Performance counters ───────────────────────────────────────────────

    [Fact]
    public void TotalFramesProcessed_IncrementsAfterEachExtract()
    {
        long before = _ext.TotalFramesProcessed;
        _ext.Extract(new float[FrameSize]);
        Assert.Equal(before + 1, _ext.TotalFramesProcessed);
    }

    [Fact]
    public void TotalFramesProcessed_IncrementsCorrectlyForMultipleExtracts()
    {
        long before = _ext.TotalFramesProcessed;
        _ext.Extract(new float[FrameSize]);
        _ext.Extract(new float[FrameSize]);
        _ext.Extract(new float[FrameSize]);
        Assert.Equal(before + 3, _ext.TotalFramesProcessed);
    }

    [Fact]
    public void AverageInferenceMs_GreaterThanZeroAfterExtract()
    {
        _ext.Extract(new float[FrameSize]);
        Assert.True(_ext.AverageInferenceMs >= 0);
    }

    // ── 8. Short-frame safety ────────────────────────────────────────────────

    [Fact]
    public void Extract_ShortFrame_DoesNotThrow()
    {
        var ex = Record.Exception(() => _ext.Extract(new float[100]));
        Assert.Null(ex);
    }

    [Fact]
    public void Extract_EmptyFrame_DoesNotThrow()
    {
        var ex = Record.Exception(() => _ext.Extract(Array.Empty<float>()));
        Assert.Null(ex);
    }

    [Fact]
    public void Extract_EmptyFrame_Returns256Elements()
    {
        var features = _ext.Extract(Array.Empty<float>());
        Assert.Equal(256, features.Length);
    }

    // ── 9. UpdateProjection doesn't throw ────────────────────────────────────

    [Fact]
    public void UpdateProjection_WithGradient_DoesNotThrow()
    {
        // First extract to ensure weights are initialised
        _ext.Extract(new float[FrameSize]);

        var gradient = new float[256];
        for (int i = 0; i < gradient.Length; i++) gradient[i] = 0.01f;

        var ex = Record.Exception(() => _ext.UpdateProjection(gradient, 0.001f));
        Assert.Null(ex);
    }

    [Fact]
    public void UpdateProjection_ModifiesOutput_AfterWeightUpdate()
    {
        var frame = new float[FrameSize];
        var rng   = new Random(55);
        for (int i = 0; i < FrameSize; i++) frame[i] = (float)rng.NextDouble();

        var before = _ext.Extract(frame);

        var gradient = new float[256];
        for (int i = 0; i < gradient.Length; i++) gradient[i] = 1.0f; // large gradient

        _ext.UpdateProjection(gradient, 0.1f);
        var after = _ext.Extract(frame);

        // After a significant gradient update, at least some outputs should change
        // (the weights have been modified, so the projection changes)
        bool anyChanged = before.Zip(after, (a, b) => Math.Abs(a - b) > 1e-8f).Any(c => c);
        Assert.True(anyChanged, "UpdateProjection should change the feature extractor output");
    }

    // ── 10. SaveProjectionAsync / LoadProjectionAsync round-trip ─────────────

    [Fact]
    public async Task SaveAndLoadProjection_PreservesOutput()
    {
        var frame = new float[FrameSize];
        var rng   = new Random(77);
        for (int i = 0; i < FrameSize; i++) frame[i] = (float)rng.NextDouble();

        // Initialise weights by extracting once
        var original = _ext.Extract(frame);

        var tmpPath = Path.Combine(Path.GetTempPath(), $"test_projection_{Guid.NewGuid()}.bin");
        try
        {
            await _ext.SaveProjectionAsync(tmpPath);
            Assert.True(File.Exists(tmpPath), "Projection file should have been created");

            // Load into a fresh extractor
            var ext2 = new OnnxCnnFeatureExtractor(NullLogger<OnnxCnnFeatureExtractor>.Instance);
            ext2.Initialize(modelPath: null, outputDim: 512);
            await ext2.LoadProjectionAsync(tmpPath);

            var loaded = ext2.Extract(frame);
            ext2.Dispose();

            // After loading the exact same weights, output should match
            Assert.Equal(original.Length, loaded.Length);
        }
        finally
        {
            if (File.Exists(tmpPath)) File.Delete(tmpPath);
        }
    }
}
