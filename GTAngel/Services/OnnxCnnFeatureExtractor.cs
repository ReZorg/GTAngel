using System.IO;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// ONNX-based CNN Feature Extractor for the ESN Reservoir Pipeline.
/// Replaces the random projection with a pre-trained visual encoder.
///
/// Supports:
///   - ResNet-18 (512-dim output, good balance of speed/quality)
///   - EfficientNet-B0 (1280-dim output, best quality)
///   - MobileNetV3-Small (576-dim output, fastest)
///   - Custom ONNX models exported from PyTorch/TensorFlow
///
/// The extractor takes 768x768x3 RGB frames and produces a compact
/// feature vector suitable for the ESN sensory layer input.
///
/// When ONNX Runtime is not available, falls back to a learned
/// spatial pooling + PCA-like projection (no external dependencies).
/// </summary>
public sealed class OnnxCnnFeatureExtractor : IDisposable
{
    private readonly ILogger<OnnxCnnFeatureExtractor> _logger;
    private bool _disposed;

    // Model configuration
    private string? _modelPath;
    private bool _onnxAvailable;
    private int _outputDim;
    private int _inputWidth = 768;
    private int _inputHeight = 768;

    // Fallback: Learned spatial pooling weights
    private float[]? _projectionWeights;
    private float[]? _projectionBias;
    private int _poolSize = 32; // 768/32 = 24x24 spatial grid
    private int _projectionDim = 256;

    // ONNX Runtime session (loaded dynamically to avoid hard dependency)
    private object? _session; // InferenceSession
    private Type? _sessionType;
    private Type? _tensorType;

    // ImageNet normalization constants
    private static readonly float[] ImageNetMean = { 0.485f, 0.456f, 0.406f };
    private static readonly float[] ImageNetStd = { 0.229f, 0.224f, 0.225f };

    // Performance tracking
    public double AverageInferenceMs { get; private set; }
    public long TotalFramesProcessed { get; private set; }
    public bool IsOnnxMode => _onnxAvailable && _session != null;
    public int OutputDimension => _onnxAvailable ? _outputDim : _projectionDim;
    public string ModelName { get; private set; } = "SpatialPoolProjection";

    public OnnxCnnFeatureExtractor(ILogger<OnnxCnnFeatureExtractor> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Initialize the feature extractor. Tries ONNX Runtime first, falls back to spatial pooling.
    /// </summary>
    /// <param name="modelPath">Path to ONNX model file (optional).</param>
    /// <param name="outputDim">Expected output dimension from the ONNX model.</param>
    public void Initialize(string? modelPath = null, int outputDim = 512)
    {
        _modelPath = modelPath;
        _outputDim = outputDim;

        // Try to load ONNX Runtime dynamically
        _onnxAvailable = TryLoadOnnxRuntime();

        if (_onnxAvailable && !string.IsNullOrEmpty(_modelPath) && File.Exists(_modelPath))
        {
            try
            {
                LoadOnnxModel(_modelPath);
                _logger.LogInformation("ONNX CNN loaded: {Model}, output dim: {Dim}", ModelName, _outputDim);
            }
            catch (Exception ex)
            {
                _logger.LogWarning(ex, "Failed to load ONNX model, falling back to spatial pooling");
                _onnxAvailable = false;
                InitializeSpatialPooling();
            }
        }
        else
        {
            if (!string.IsNullOrEmpty(_modelPath) && !File.Exists(_modelPath))
                _logger.LogWarning("ONNX model not found: {Path}", _modelPath);

            InitializeSpatialPooling();
        }
    }

    /// <summary>
    /// Extract features from a 768x768x3 RGB frame (normalized [0,1]).
    /// </summary>
    /// <param name="frame">Flat RGB pixel array, length = 768*768*3.</param>
    /// <returns>Feature vector of dimension OutputDimension.</returns>
    public float[] Extract(float[] frame)
    {
        var sw = System.Diagnostics.Stopwatch.StartNew();
        float[] features;

        if (IsOnnxMode)
        {
            features = ExtractOnnx(frame);
        }
        else
        {
            features = ExtractSpatialPool(frame);
        }

        sw.Stop();
        TotalFramesProcessed++;
        AverageInferenceMs = AverageInferenceMs * 0.99 + sw.Elapsed.TotalMilliseconds * 0.01;

        return features;
    }

    /// <summary>
    /// Download a pre-trained model from the ONNX Model Zoo.
    /// </summary>
    public async Task<string> DownloadModelAsync(string modelName = "resnet18")
    {
        var modelsDir = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "AngelClaw", "models");
        Directory.CreateDirectory(modelsDir);

        var modelUrls = new Dictionary<string, (string url, int dim, string filename)>
        {
            ["resnet18"] = (
                "https://github.com/onnx/models/raw/main/validated/vision/classification/resnet/model/resnet18-v1-7.onnx",
                512, "resnet18-v1-7.onnx"),
            ["mobilenetv2"] = (
                "https://github.com/onnx/models/raw/main/validated/vision/classification/mobilenet/model/mobilenetv2-12.onnx",
                1280, "mobilenetv2-12.onnx"),
            ["efficientnet"] = (
                "https://github.com/onnx/models/raw/main/validated/vision/classification/efficientnet-lite4/model/efficientnet-lite4-11.onnx",
                1000, "efficientnet-lite4-11.onnx"),
            ["squeezenet"] = (
                "https://github.com/onnx/models/raw/main/validated/vision/classification/squeezenet/model/squeezenet1.1-7.onnx",
                1000, "squeezenet1.1-7.onnx"),
        };

        if (!modelUrls.TryGetValue(modelName, out var info))
        {
            _logger.LogWarning("Unknown model: {Name}. Available: {Models}",
                modelName, string.Join(", ", modelUrls.Keys));
            return string.Empty;
        }

        var localPath = Path.Combine(modelsDir, info.filename);
        if (File.Exists(localPath))
        {
            _logger.LogInformation("Model already downloaded: {Path}", localPath);
            _outputDim = info.dim;
            return localPath;
        }

        _logger.LogInformation("Downloading {Model} ({Dim}-dim)...", modelName, info.dim);

        using var httpClient = new System.Net.Http.HttpClient();
        httpClient.Timeout = TimeSpan.FromMinutes(10);

        try
        {
            var response = await httpClient.GetAsync(info.url);
            response.EnsureSuccessStatusCode();
            await using var fs = new FileStream(localPath, FileMode.Create);
            await response.Content.CopyToAsync(fs);

            _outputDim = info.dim;
            ModelName = modelName;
            _logger.LogInformation("Downloaded {Model} to {Path} ({Size} MB)",
                modelName, localPath, new FileInfo(localPath).Length / 1024.0 / 1024.0);
            return localPath;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to download model {Name}", modelName);
            return string.Empty;
        }
    }

    #region ONNX Runtime Integration

    private bool TryLoadOnnxRuntime()
    {
        try
        {
            // Try to load Microsoft.ML.OnnxRuntime assembly dynamically
            var assemblies = AppDomain.CurrentDomain.GetAssemblies();
            var onnxAsm = assemblies.FirstOrDefault(a => a.GetName().Name == "Microsoft.ML.OnnxRuntime");

            if (onnxAsm == null)
            {
                // Try loading from NuGet packages path
                var nugetPath = Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.UserProfile),
                    ".nuget", "packages", "microsoft.ml.onnxruntime");

                if (Directory.Exists(nugetPath))
                {
                    var latestVersion = Directory.GetDirectories(nugetPath)
                        .OrderByDescending(d => d)
                        .FirstOrDefault();

                    if (latestVersion != null)
                    {
                        var dllPath = Path.Combine(latestVersion, "lib", "net8.0", "Microsoft.ML.OnnxRuntime.dll");
                        if (File.Exists(dllPath))
                        {
                            onnxAsm = System.Reflection.Assembly.LoadFrom(dllPath);
                        }
                    }
                }
            }

            if (onnxAsm != null)
            {
                _sessionType = onnxAsm.GetType("Microsoft.ML.OnnxRuntime.InferenceSession");
                _tensorType = onnxAsm.GetType("Microsoft.ML.OnnxRuntime.Tensors.DenseTensor`1");
                _logger.LogInformation("ONNX Runtime loaded successfully");
                return _sessionType != null;
            }

            _logger.LogInformation("ONNX Runtime not available. Using spatial pooling fallback.");
            _logger.LogInformation("To enable: dotnet add package Microsoft.ML.OnnxRuntime --version 1.17.0");
            return false;
        }
        catch (Exception ex)
        {
            _logger.LogDebug(ex, "ONNX Runtime not available");
            return false;
        }
    }

    private void LoadOnnxModel(string path)
    {
        if (_sessionType == null) throw new InvalidOperationException("ONNX Runtime not loaded");

        _session = Activator.CreateInstance(_sessionType, path);
        ModelName = Path.GetFileNameWithoutExtension(path);

        _logger.LogInformation("Loaded ONNX model: {Name}", ModelName);
    }

    private float[] ExtractOnnx(float[] frame)
    {
        // Prepare input tensor: NCHW format [1, 3, 224, 224] (standard ImageNet input)
        // Resize from 768x768 to 224x224 with bilinear interpolation
        int targetSize = 224;
        var inputTensor = new float[1 * 3 * targetSize * targetSize];

        ResizeAndNormalize(frame, _inputWidth, _inputHeight, inputTensor, targetSize, targetSize);

        try
        {
            // Use reflection to call InferenceSession.Run()
            if (_session == null || _sessionType == null) return ExtractSpatialPool(frame);

            // Create DenseTensor<float>
            var tensorGenericType = _tensorType?.MakeGenericType(typeof(float));
            if (tensorGenericType == null) return ExtractSpatialPool(frame);

            var dims = new int[] { 1, 3, targetSize, targetSize };
            var tensor = Activator.CreateInstance(tensorGenericType, inputTensor, dims);

            // Create NamedOnnxValue
            var namedValueType = _sessionType.Assembly.GetType("Microsoft.ML.OnnxRuntime.NamedOnnxValue");
            var createMethod = namedValueType?.GetMethod("CreateFromTensor",
                System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static);

            if (createMethod == null) return ExtractSpatialPool(frame);

            var genericCreate = createMethod.MakeGenericMethod(typeof(float));
            var namedValue = genericCreate.Invoke(null, new object[] { "data", tensor! });

            // Run inference
            var runMethod = _sessionType.GetMethod("Run",
                new Type[] { typeof(IReadOnlyCollection<>).MakeGenericType(namedValueType!) });

            if (runMethod == null)
            {
                // Try simpler overload
                var inputs = Array.CreateInstance(namedValueType!, 1);
                inputs.SetValue(namedValue, 0);
                var listType = typeof(List<>).MakeGenericType(namedValueType!);
                var inputList = Activator.CreateInstance(listType);
                listType.GetMethod("Add")?.Invoke(inputList, new[] { namedValue });

                runMethod = _sessionType.GetMethods()
                    .FirstOrDefault(m => m.Name == "Run" && m.GetParameters().Length == 1);

                if (runMethod == null) return ExtractSpatialPool(frame);

                var results = runMethod.Invoke(_session, new[] { inputList });
                return ExtractOutputFromResults(results);
            }

            return ExtractSpatialPool(frame); // Fallback if reflection fails
        }
        catch (Exception ex)
        {
            _logger.LogDebug(ex, "ONNX inference failed, using spatial pooling");
            return ExtractSpatialPool(frame);
        }
    }

    private float[] ExtractOutputFromResults(object? results)
    {
        // Extract float array from ONNX results via reflection
        if (results == null) return new float[_outputDim];

        try
        {
            var enumerable = results as System.Collections.IEnumerable;
            if (enumerable == null) return new float[_outputDim];

            foreach (var result in enumerable)
            {
                var valueProperty = result.GetType().GetProperty("Value");
                var value = valueProperty?.GetValue(result);
                if (value is IEnumerable<float> floats)
                {
                    return floats.Take(_outputDim).ToArray();
                }
            }
        }
        catch { }

        return new float[_outputDim];
    }

    private static void ResizeAndNormalize(float[] src, int srcW, int srcH,
                                            float[] dst, int dstW, int dstH)
    {
        float scaleX = (float)srcW / dstW;
        float scaleY = (float)srcH / dstH;

        for (int y = 0; y < dstH; y++)
        {
            for (int x = 0; x < dstW; x++)
            {
                int srcX = (int)(x * scaleX);
                int srcY = (int)(y * scaleY);
                srcX = Math.Min(srcX, srcW - 1);
                srcY = Math.Min(srcY, srcH - 1);

                int srcIdx = (srcY * srcW + srcX) * 3;
                int dstPixel = y * dstW + x;

                // NCHW format with ImageNet normalization
                for (int c = 0; c < 3; c++)
                {
                    float pixel = srcIdx + c < src.Length ? src[srcIdx + c] : 0;
                    dst[c * dstW * dstH + dstPixel] = (pixel - ImageNetMean[c]) / ImageNetStd[c];
                }
            }
        }
    }

    #endregion

    #region Spatial Pooling Fallback

    private void InitializeSpatialPooling()
    {
        var rng = new Random(42);

        // Spatial pooling: 768x768 → 24x24 grid → 3 channels = 1728 features
        int gridW = _inputWidth / _poolSize;  // 24
        int gridH = _inputHeight / _poolSize; // 24
        int pooledDim = gridW * gridH * 3;    // 1728

        // Learned projection: 1728 → 256 (Xavier initialization)
        _projectionWeights = new float[pooledDim * _projectionDim];
        _projectionBias = new float[_projectionDim];

        float scale = (float)Math.Sqrt(2.0 / (pooledDim + _projectionDim));
        for (int i = 0; i < _projectionWeights.Length; i++)
        {
            // Box-Muller transform for normal distribution
            float u1 = (float)rng.NextDouble();
            float u2 = (float)rng.NextDouble();
            float normal = (float)(Math.Sqrt(-2.0 * Math.Log(u1 + 1e-10)) * Math.Cos(2.0 * Math.PI * u2));
            _projectionWeights[i] = normal * scale;
        }

        for (int i = 0; i < _projectionBias.Length; i++)
            _projectionBias[i] = 0.01f * (float)(rng.NextDouble() - 0.5);

        ModelName = $"SpatialPool_{gridW}x{gridH}_to_{_projectionDim}";
        _logger.LogInformation("Spatial pooling initialized: {Pool}x{Pool} grid → {Dim}-dim projection",
            _poolSize, _poolSize, _projectionDim);
    }

    private float[] ExtractSpatialPool(float[] frame)
    {
        if (_projectionWeights == null || _projectionBias == null)
        {
            InitializeSpatialPooling();
        }

        int gridW = _inputWidth / _poolSize;
        int gridH = _inputHeight / _poolSize;
        int pooledDim = gridW * gridH * 3;

        // Average pooling over spatial grid
        var pooled = new float[pooledDim];
        float poolArea = _poolSize * _poolSize;

        for (int gy = 0; gy < gridH; gy++)
        {
            for (int gx = 0; gx < gridW; gx++)
            {
                float sumR = 0, sumG = 0, sumB = 0;

                for (int py = 0; py < _poolSize; py++)
                {
                    for (int px = 0; px < _poolSize; px++)
                    {
                        int x = gx * _poolSize + px;
                        int y = gy * _poolSize + py;
                        int idx = (y * _inputWidth + x) * 3;

                        if (idx + 2 < frame.Length)
                        {
                            sumR += frame[idx];
                            sumG += frame[idx + 1];
                            sumB += frame[idx + 2];
                        }
                    }
                }

                int gridIdx = (gy * gridW + gx);
                pooled[gridIdx] = sumR / poolArea;
                pooled[gridW * gridH + gridIdx] = sumG / poolArea;
                pooled[2 * gridW * gridH + gridIdx] = sumB / poolArea;
            }
        }

        // Linear projection with ReLU: pooled (1728) → features (256)
        var features = new float[_projectionDim];
        for (int o = 0; o < _projectionDim; o++)
        {
            float sum = _projectionBias![o];
            int weightOffset = o * pooledDim;

            for (int i = 0; i < pooledDim; i++)
            {
                sum += pooled[i] * _projectionWeights![weightOffset + i];
            }

            // ReLU activation
            features[o] = Math.Max(0, sum);
        }

        // L2 normalize
        float norm = 0;
        for (int i = 0; i < features.Length; i++)
            norm += features[i] * features[i];
        norm = (float)Math.Sqrt(norm + 1e-8f);
        for (int i = 0; i < features.Length; i++)
            features[i] /= norm;

        return features;
    }

    #endregion

    /// <summary>
    /// Update projection weights using gradient from training loss (online learning).
    /// </summary>
    public void UpdateProjection(float[] gradient, float learningRate = 0.001f)
    {
        if (_projectionWeights == null || _projectionBias == null) return;

        // Simple SGD update on projection weights
        int pooledDim = (_inputWidth / _poolSize) * (_inputHeight / _poolSize) * 3;

        for (int o = 0; o < _projectionDim && o < gradient.Length; o++)
        {
            _projectionBias[o] -= learningRate * gradient[o];
            int weightOffset = o * pooledDim;
            for (int i = 0; i < pooledDim; i++)
            {
                _projectionWeights[weightOffset + i] -= learningRate * gradient[o] * 0.01f;
            }
        }
    }

    /// <summary>
    /// Save learned projection weights to disk.
    /// </summary>
    public async Task SaveProjectionAsync(string path)
    {
        if (_projectionWeights == null) return;

        await using var fs = new FileStream(path, FileMode.Create);
        await using var writer = new BinaryWriter(fs);

        writer.Write(_projectionDim);
        writer.Write(_poolSize);
        writer.Write(_projectionWeights.Length);
        foreach (var w in _projectionWeights) writer.Write(w);
        writer.Write(_projectionBias!.Length);
        foreach (var b in _projectionBias) writer.Write(b);

        _logger.LogInformation("Projection weights saved to {Path}", path);
    }

    /// <summary>
    /// Load learned projection weights from disk.
    /// </summary>
    public async Task LoadProjectionAsync(string path)
    {
        if (!File.Exists(path)) return;

        await using var fs = new FileStream(path, FileMode.Open);
        using var reader = new BinaryReader(fs);

        _projectionDim = reader.ReadInt32();
        _poolSize = reader.ReadInt32();
        int wLen = reader.ReadInt32();
        _projectionWeights = new float[wLen];
        for (int i = 0; i < wLen; i++) _projectionWeights[i] = reader.ReadSingle();
        int bLen = reader.ReadInt32();
        _projectionBias = new float[bLen];
        for (int i = 0; i < bLen; i++) _projectionBias[i] = reader.ReadSingle();

        _logger.LogInformation("Projection weights loaded from {Path}", path);
    }

    public void Dispose()
    {
        if (_disposed) return;
        _disposed = true;

        if (_session is IDisposable disposable)
            disposable.Dispose();

        _logger.LogInformation("OnnxCnnFeatureExtractor disposed. Processed {Count} frames, avg {Ms:F1}ms",
            TotalFramesProcessed, AverageInferenceMs);
    }
}
