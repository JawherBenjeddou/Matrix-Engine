#pragma once
#include "Common.h"
///@file AudioEngine.h  Sound playback built on miniaudio.

namespace Matrix::audio
{
    using SoundHandle = uint32_t;
    constexpr SoundHandle InvalidSound = 0;

    class MATRIX_DLL AudioEngine
    {
    public:
        /// Initializes the audio device. Returns false if no device is available (playback calls then become no-ops).
        static bool Init();
        static void Shutdown();
        static bool IsInitialized();

        /// Starts playing a clip (WAV, MP3, FLAC). Returns InvalidSound on failure.
        static SoundHandle Play(const std::filesystem::path& clipPath, float volume = 1.0f, float pitch = 1.0f, bool loop = false);

        static void Stop(SoundHandle handle);
        static void StopAll();
        static bool IsPlaying(SoundHandle handle);
        static void SetVolume(SoundHandle handle, float volume);
        static void SetPitch(SoundHandle handle, float pitch);

        static void SetMasterVolume(float volume);
        static float GetMasterVolume();

        /// Releases finished one-shot sounds. Call once per frame.
        static void Update();

        static size_t GetActiveSoundCount();
    };
}
