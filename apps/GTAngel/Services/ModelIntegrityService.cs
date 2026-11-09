using System.IO;
using System.Security.Cryptography;
using System.Text.Json;
using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Validates ONNX model integrity using SHA-256 checksums.
/// Prevents loading tampered or corrupted models at startup.
/// 
/// Checksums are stored in Assets/Models/checksums.json:
/// { "feature_extractor.onnx": "abc123...", ... }
/// </summary>
public sealed class ModelIntegrityService
{
    private readonly ILogger<ModelIntegrityService> _logger;
    private readonly string _modelsPath;
    private readonly string _checksumsPath;

    public ModelIntegrityService(ILogger<ModelIntegrityService> logger)
    {
        _logger = logger;
        _modelsPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Assets", "Models");
        _checksumsPath = Path.Combine(_modelsPath, "checksums.json");
    }

    /// <summary>
    /// Verify all models listed in checksums.json match their expected hashes.
    /// Returns true if all models pass validation (or if checksums.json doesn't exist).
    /// </summary>
    public async Task<ModelIntegrityResult> ValidateAllAsync()
    {
        var result = new ModelIntegrityResult();

        if (!File.Exists(_checksumsPath))
        {
            _logger.LogWarning("checksums.json not found at {Path}; skipping model integrity check", _checksumsPath);
            result.Skipped = true;
            return result;
        }

        try
        {
            var json = await File.ReadAllTextAsync(_checksumsPath);
            var checksums = JsonSerializer.Deserialize<Dictionary<string, string>>(json);

            if (checksums == null || checksums.Count == 0)
            {
                _logger.LogWarning("checksums.json is empty; skipping model integrity check");
                result.Skipped = true;
                return result;
            }

            foreach (var (filename, expectedHash) in checksums)
            {
                var modelPath = Path.Combine(_modelsPath, filename);

                if (!File.Exists(modelPath))
                {
                    _logger.LogError("Model file missing: {File}", filename);
                    result.MissingFiles.Add(filename);
                    continue;
                }

                var actualHash = await ComputeSha256Async(modelPath);

                if (string.Equals(actualHash, expectedHash, StringComparison.OrdinalIgnoreCase))
                {
                    _logger.LogDebug("Model integrity OK: {File}", filename);
                    result.ValidatedFiles.Add(filename);
                }
                else
                {
                    _logger.LogError("Model integrity FAILED: {File} (expected={Expected}, actual={Actual})",
                        filename, expectedHash, actualHash);
                    result.CorruptedFiles.Add(filename);
                }
            }

            result.IsValid = result.MissingFiles.Count == 0 && result.CorruptedFiles.Count == 0;

            if (result.IsValid)
                _logger.LogInformation("Model integrity check passed ({Count} models verified)", result.ValidatedFiles.Count);
            else
                _logger.LogError("Model integrity check FAILED: {Missing} missing, {Corrupt} corrupted",
                    result.MissingFiles.Count, result.CorruptedFiles.Count);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error during model integrity verification");
            result.Error = ex.Message;
        }

        return result;
    }

    /// <summary>
    /// Generate checksums.json for all .onnx files in the models directory.
    /// Used during build/release to create the manifest.
    /// </summary>
    public async Task GenerateChecksumsAsync()
    {
        if (!Directory.Exists(_modelsPath))
        {
            _logger.LogWarning("Models directory not found: {Path}", _modelsPath);
            return;
        }

        var modelFiles = Directory.GetFiles(_modelsPath, "*.onnx");
        var checksums = new Dictionary<string, string>();

        foreach (var modelPath in modelFiles)
        {
            var filename = Path.GetFileName(modelPath);
            var hash = await ComputeSha256Async(modelPath);
            checksums[filename] = hash;
            _logger.LogInformation("Generated checksum for {File}: {Hash}", filename, hash);
        }

        var json = JsonSerializer.Serialize(checksums, new JsonSerializerOptions { WriteIndented = true });
        await File.WriteAllTextAsync(_checksumsPath, json);
        _logger.LogInformation("Wrote checksums.json with {Count} entries", checksums.Count);
    }

    private static async Task<string> ComputeSha256Async(string filePath)
    {
        using var stream = File.OpenRead(filePath);
        var hashBytes = await SHA256.HashDataAsync(stream);
        return Convert.ToHexString(hashBytes).ToLowerInvariant();
    }
}

public sealed class ModelIntegrityResult
{
    public bool IsValid { get; set; }
    public bool Skipped { get; set; }
    public string? Error { get; set; }
    public List<string> ValidatedFiles { get; } = new();
    public List<string> MissingFiles { get; } = new();
    public List<string> CorruptedFiles { get; } = new();
}
