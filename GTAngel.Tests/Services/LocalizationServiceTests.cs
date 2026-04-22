using System.IO;
using GTA3DE.Wpf.Services;
using Microsoft.Extensions.Logging.Abstractions;
using Xunit;

namespace GTAngel.Tests.Services;

/// <summary>
/// Tests for LocalizationService — key lookup, fallback, missing key,
/// and locale change behaviour (without real file I/O where possible).
/// </summary>
public class LocalizationServiceTests
{
    private readonly LocalizationService _service;

    public LocalizationServiceTests()
    {
        _service = new LocalizationService(NullLogger<LocalizationService>.Instance);
    }

    // ── Default state ──────────────────────────────────────────────────────

    [Fact]
    public void CurrentLocale_Default_IsEnUS()
    {
        Assert.Equal("en-US", _service.CurrentLocale);
    }

    // ── Get — no strings loaded ────────────────────────────────────────────

    [Fact]
    public void Get_WithNoStringsLoaded_ReturnsKeyAsValue()
    {
        Assert.Equal("some.key", _service.Get("some.key"));
    }

    [Fact]
    public void Get_EmptyKey_ReturnsEmptyString()
    {
        Assert.Equal(string.Empty, _service.Get(string.Empty));
    }

    // ── GetWithDefault ─────────────────────────────────────────────────────

    [Fact]
    public void GetWithDefault_WithNoStringsLoaded_ReturnsDefault()
    {
        Assert.Equal("fallback", _service.GetWithDefault("missing.key", "fallback"));
    }

    [Fact]
    public void GetWithDefault_EmptyDefault_ReturnsFallback()
    {
        Assert.Equal(string.Empty, _service.GetWithDefault("any.key", string.Empty));
    }

    // ── LoadLanguage — missing file ────────────────────────────────────────

    [Fact]
    public void LoadLanguage_MissingFile_DoesNotThrow()
    {
        var ex = Record.Exception(() => _service.LoadLanguage("xx-XX")); // non-existent locale
        Assert.Null(ex);
    }

    [Fact]
    public void LoadLanguage_MissingFile_DoesNotChangeLocale()
    {
        _service.LoadLanguage("zz-ZZ"); // no such file
        // Locale should remain unchanged
        Assert.Equal("en-US", _service.CurrentLocale);
    }

    [Fact]
    public void Get_AfterFailedLoad_StillFallsBackToKey()
    {
        _service.LoadLanguage("zz-ZZ");
        Assert.Equal("app.title", _service.Get("app.title"));
    }

    // ── LoadLanguage — via temp JSON file ──────────────────────────────────

    [Fact]
    public void Get_AfterLoadFromTempFile_ReturnsTranslatedValue()
    {
        // Create a temporary lang file that the service can discover
        var tempDir = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Assets", "Lang");
        Directory.CreateDirectory(tempDir);
        var filePath = Path.Combine(tempDir, "lang_test-LANG.json");

        try
        {
            File.WriteAllText(filePath, """{"greeting": "Hello World","farewell": "Goodbye"}""");
            _service.LoadLanguage("test-LANG");

            Assert.Equal("Hello World", _service.Get("greeting"));
            Assert.Equal("Goodbye", _service.Get("farewell"));
        }
        finally
        {
            if (File.Exists(filePath)) File.Delete(filePath);
        }
    }

    [Fact]
    public void GetWithDefault_AfterLoad_ReturnsTranslatedValue()
    {
        var tempDir = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Assets", "Lang");
        Directory.CreateDirectory(tempDir);
        var filePath = Path.Combine(tempDir, "lang_test2-LANG.json");

        try
        {
            File.WriteAllText(filePath, """{"key1": "Value One"}""");
            _service.LoadLanguage("test2-LANG");

            Assert.Equal("Value One", _service.GetWithDefault("key1", "MISS"));
        }
        finally
        {
            if (File.Exists(filePath)) File.Delete(filePath);
        }
    }

    [Fact]
    public void GetWithDefault_AfterLoad_MissingKeyReturnsFallback()
    {
        var tempDir = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Assets", "Lang");
        Directory.CreateDirectory(tempDir);
        var filePath = Path.Combine(tempDir, "lang_test3-LANG.json");

        try
        {
            File.WriteAllText(filePath, """{"existing": "exists"}""");
            _service.LoadLanguage("test3-LANG");

            Assert.Equal("default_val", _service.GetWithDefault("nonexistent", "default_val"));
        }
        finally
        {
            if (File.Exists(filePath)) File.Delete(filePath);
        }
    }

    [Fact]
    public void CurrentLocale_ChangesAfterSuccessfulLoad()
    {
        var tempDir = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Assets", "Lang");
        Directory.CreateDirectory(tempDir);
        var filePath = Path.Combine(tempDir, "lang_fr-TEST.json");

        try
        {
            File.WriteAllText(filePath, """{}""");
            _service.LoadLanguage("fr-TEST");
            Assert.Equal("fr-TEST", _service.CurrentLocale);
        }
        finally
        {
            if (File.Exists(filePath)) File.Delete(filePath);
        }
    }

    [Fact]
    public void LoadLanguage_InvalidJson_DoesNotThrow()
    {
        var tempDir = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Assets", "Lang");
        Directory.CreateDirectory(tempDir);
        var filePath = Path.Combine(tempDir, "lang_bad-JSON.json");

        try
        {
            File.WriteAllText(filePath, "NOT VALID JSON {{{");
            var ex = Record.Exception(() => _service.LoadLanguage("bad-JSON"));
            Assert.Null(ex);
        }
        finally
        {
            if (File.Exists(filePath)) File.Delete(filePath);
        }
    }
}
