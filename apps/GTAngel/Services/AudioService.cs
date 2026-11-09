using Microsoft.Extensions.Logging;

namespace GTAngel.Services;

/// <summary>
/// Audio management service.
/// Translated from: rockstarmobile/GCAudioLogic
/// Manages game audio, music, and SFX volume levels.
/// 
/// Original Android flow:
///   GCAudioLogic.setMasterVolume(float) → set master volume
///   GCAudioLogic.setMusicVolume(float) → set music volume
///   GCAudioLogic.setSfxVolume(float) → set SFX volume
///   GCAudioLogic.pause() → pause all audio (onPause)
///   GCAudioLogic.resume() → resume all audio (onResume)
///   Uses Android AudioManager + MediaPlayer
/// </summary>
public class AudioService : IDisposable
{
    private readonly ILogger<AudioService> _logger;
    private float _masterVolume = 0.8f;
    private float _musicVolume = 0.7f;
    private float _sfxVolume = 0.8f;
    public bool IsPaused { get; private set; }

    public AudioService(ILogger<AudioService> logger)
    {
        _logger = logger;
    }

    public void Initialize()
    {
        _logger.LogInformation("Audio service initialized");
        // In production, initialize Windows audio APIs (NAudio, XAudio2, etc.)
    }

    /// <summary>Replaces: GCAudioLogic.setMasterVolume()</summary>
    public void SetMasterVolume(float volume)
    {
        _masterVolume = Math.Clamp(volume, 0f, 1f);
        _logger.LogDebug("Master volume: {Volume}", _masterVolume);
    }

    /// <summary>Replaces: GCAudioLogic.setMusicVolume()</summary>
    public void SetMusicVolume(float volume)
    {
        _musicVolume = Math.Clamp(volume, 0f, 1f);
        _logger.LogDebug("Music volume: {Volume}", _musicVolume);
    }

    /// <summary>Replaces: GCAudioLogic.setSfxVolume()</summary>
    public void SetSfxVolume(float volume)
    {
        _sfxVolume = Math.Clamp(volume, 0f, 1f);
        _logger.LogDebug("SFX volume: {Volume}", _sfxVolume);
    }

    /// <summary>Replaces: GCAudioLogic.pause() called from onPause()</summary>
    public void Pause()
    {
        IsPaused = true;
        _logger.LogDebug("Audio paused");
    }

    /// <summary>Replaces: GCAudioLogic.resume() called from onResume()</summary>
    public void Resume()
    {
        IsPaused = false;
        _logger.LogDebug("Audio resumed");
    }

    public void Dispose()
    {
        _logger.LogDebug("Audio service disposed");
    }
}
