#include "pch.h"
#include "AudioEngine.h"

#include "core/Logging.h"
#include "util/FileSystem.h"

#include "miniaudio.h"

namespace Matrix::audio
{
    namespace
    {
        struct ActiveSound
        {
            std::unique_ptr<ma_sound> Sound;
            bool Loop = false;
        };

        ma_engine s_Engine{};
        bool s_Initialized = false;
        SoundHandle s_NextHandle = 1;
        std::unordered_map<SoundHandle, ActiveSound> s_Sounds;
        float s_MasterVolume = 1.0f;

        ActiveSound* Find(SoundHandle handle)
        {
            auto it = s_Sounds.find(handle);
            return it == s_Sounds.end() ? nullptr : &it->second;
        }
    }

    bool AudioEngine::Init()
    {
        if (s_Initialized)
            return true;
        ma_engine_config config = ma_engine_config_init();
        ma_result result = ma_engine_init(&config, &s_Engine);
        if (result != MA_SUCCESS)
        {
            MX_CORE_WARN("Audio engine could not be initialized (miniaudio error {}); audio is disabled", static_cast<int>(result));
            return false;
        }
        s_Initialized = true;
        ma_engine_set_volume(&s_Engine, s_MasterVolume);
        MX_CORE_INFO("Audio engine initialized ({} Hz, {} channels)", ma_engine_get_sample_rate(&s_Engine), ma_engine_get_channels(&s_Engine));
        return true;
    }

    void AudioEngine::Shutdown()
    {
        if (!s_Initialized)
            return;
        StopAll();
        ma_engine_uninit(&s_Engine);
        s_Initialized = false;
        MX_CORE_INFO("Audio engine shut down");
    }

    bool AudioEngine::IsInitialized()
    {
        return s_Initialized;
    }

    SoundHandle AudioEngine::Play(const std::filesystem::path& clipPath, float volume, float pitch, bool loop)
    {
        if (!s_Initialized || clipPath.empty())
            return InvalidSound;

        std::string path = util::FileSystem::Resolve(clipPath).string();
        auto sound = std::make_unique<ma_sound>();
        ma_result result = ma_sound_init_from_file(&s_Engine, path.c_str(), MA_SOUND_FLAG_DECODE, nullptr, nullptr, sound.get());
        if (result != MA_SUCCESS)
        {
            MX_CORE_ERROR("Could not load audio clip '{}' (miniaudio error {})", path, static_cast<int>(result));
            return InvalidSound;
        }

        ma_sound_set_volume(sound.get(), std::max(volume, 0.0f));
        ma_sound_set_pitch(sound.get(), std::max(pitch, 0.01f));
        ma_sound_set_looping(sound.get(), loop ? MA_TRUE : MA_FALSE);
        if (ma_sound_start(sound.get()) != MA_SUCCESS)
        {
            ma_sound_uninit(sound.get());
            return InvalidSound;
        }

        SoundHandle handle = s_NextHandle++;
        s_Sounds[handle] = ActiveSound{ std::move(sound), loop };
        return handle;
    }

    void AudioEngine::Stop(SoundHandle handle)
    {
        auto it = s_Sounds.find(handle);
        if (it == s_Sounds.end())
            return;
        ma_sound_stop(it->second.Sound.get());
        ma_sound_uninit(it->second.Sound.get());
        s_Sounds.erase(it);
    }

    void AudioEngine::StopAll()
    {
        for (auto& [handle, active] : s_Sounds)
        {
            ma_sound_stop(active.Sound.get());
            ma_sound_uninit(active.Sound.get());
        }
        s_Sounds.clear();
    }

    bool AudioEngine::IsPlaying(SoundHandle handle)
    {
        ActiveSound* active = Find(handle);
        return active && ma_sound_is_playing(active->Sound.get()) == MA_TRUE;
    }

    void AudioEngine::SetVolume(SoundHandle handle, float volume)
    {
        if (ActiveSound* active = Find(handle))
            ma_sound_set_volume(active->Sound.get(), std::max(volume, 0.0f));
    }

    void AudioEngine::SetPitch(SoundHandle handle, float pitch)
    {
        if (ActiveSound* active = Find(handle))
            ma_sound_set_pitch(active->Sound.get(), std::max(pitch, 0.01f));
    }

    void AudioEngine::SetMasterVolume(float volume)
    {
        s_MasterVolume = math::Clamp(volume, 0.0f, 1.0f);
        if (s_Initialized)
            ma_engine_set_volume(&s_Engine, s_MasterVolume);
    }

    float AudioEngine::GetMasterVolume()
    {
        return s_MasterVolume;
    }

    void AudioEngine::Update()
    {
        for (auto it = s_Sounds.begin(); it != s_Sounds.end();)
        {
            ma_sound* sound = it->second.Sound.get();
            if (!it->second.Loop && ma_sound_at_end(sound) == MA_TRUE)
            {
                ma_sound_uninit(sound);
                it = s_Sounds.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    size_t AudioEngine::GetActiveSoundCount()
    {
        return s_Sounds.size();
    }
}
