#include "SurvivantAudio/AudioContext.h"

#include <soloud.h>

using namespace SvCore::Resources;

namespace SvAudio
{
    const char* AudioContext::GetResultString(const Result p_result)
    {
        switch (static_cast<SoLoud::SOLOUD_ERRORS>(p_result))
        {
        case SoLoud::SO_NO_ERROR:
            return "No error";
        case SoLoud::INVALID_PARAMETER:
            return "Invalid parameter";
        case SoLoud::FILE_NOT_FOUND:
            return "File not found";
        case SoLoud::FILE_LOAD_FAILED:
            return "File load failed";
        case SoLoud::DLL_NOT_FOUND:
            return "DLL not found";
        case SoLoud::OUT_OF_MEMORY:
            return "Out of memory";
        case SoLoud::NOT_IMPLEMENTED:
            return "Not implemented";
        case SoLoud::UNKNOWN_ERROR:
        default:
            return "Unknown error";
        }
    }

    AudioContext::~AudioContext()
    {
        Reset();
    }

    AudioContext& AudioContext::GetInstance()
    {
        static AudioContext instance;
        return instance;
    }

    bool AudioContext::Init()
    {
        if (ASSUME_FALSE(m_soLoud, "Attempted to initialize audio context more than once"))
            return false;

        m_soLoud = std::make_unique<SoLoud::Soloud>();

        const SoLoud::result result = m_soLoud->init();

        if (!CHECK(result == 0, "Unable to initialize soloud - Error %u: %s", result, GetResultString(result)))
            return false;

        return true;
    }

    void AudioContext::Reset()
    {
        if (!m_soLoud)
            return;

        m_soLoud.reset();
    }

    AudioHandle AudioContext::Play(
        const ResourceRef<AudioClip>& p_clip, const float p_volume, const float p_pan, const bool p_paused) const
    {
        if (!CHECK(p_clip, "Attempted to play invalid audio clip"))
            return {};

        auto handle = m_soLoud->play(*p_clip->m_sound, p_volume, p_pan, p_paused);

        if (ASSUME(m_soLoud->isValidVoiceHandle(handle), "Failed to play audio clip at \"%s\"", p_clip.GetFullPath().c_str()))
            return { p_clip, handle };

        return {};
    }

    void AudioContext::StopAll() const
    {
        m_soLoud->stopAll();
    }
}
