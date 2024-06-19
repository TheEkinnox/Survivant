#include "SurvivantAudio/AudioHandle.h"

#include "SurvivantAudio/AudioContext.h"

#include <soloud.h>

namespace SvAudio
{
    AudioHandle::AudioHandle(SvCore::Resources::ResourceRef<AudioClip> p_audioClip, const Handle p_handle)
        : m_clip(std::move(p_audioClip)), m_handle(p_handle)
    {
    }

    AudioHandle::operator bool() const
    {
        return CheckHandle() || m_clip;
    }

    SvCore::Resources::ResourceRef<AudioClip> AudioHandle::GetClip() const
    {
        return m_clip;
    }

    void AudioHandle::Play(const float p_volume, const float p_pan, const bool p_paused)
    {
        const auto& context = SV_SERVICE(AudioContext);

        if (CheckHandle())
            context.m_soLoud->stop(m_handle);

        *this = context.Play(m_clip, p_volume, p_pan, p_paused);
    }

    bool AudioHandle::IsPlaying() const
    {
        return CheckHandle() && !SV_SERVICE(AudioContext).m_soLoud->getPause(m_handle);
    }

    void AudioHandle::SetPaused(const bool p_state)
    {
        const auto& context = SV_SERVICE(AudioContext);

        if (CHECK(CheckHandle(), "Attempted to set pause state of invalid audio handle"))
            context.m_soLoud->setPause(m_handle, p_state);
    }

    void AudioHandle::Stop()
    {
        if (CheckHandle())
        {
            SV_SERVICE(AudioContext).m_soLoud->stop(m_handle);
            m_handle = 0;
        }
    }

    float AudioHandle::GetVolume() const
    {
        if (CHECK(CheckHandle(), "Attempted to get volume of invalid audio handle"))
            return SV_SERVICE(AudioContext).m_soLoud->getVolume(m_handle);

        return 0.f;
    }

    void AudioHandle::SetVolume(const float p_volume)
    {
        if (CHECK(CheckHandle(), "Attempted to set volume of invalid audio handle"))
            SV_SERVICE(AudioContext).m_soLoud->setVolume(m_handle, p_volume);
    }

    float AudioHandle::GetPan() const
    {
        if (CHECK(CheckHandle(), "Attempted to get pan of invalid audio handle"))
            return SV_SERVICE(AudioContext).m_soLoud->getPan(m_handle);

        return 0.f;
    }

    void AudioHandle::SetPan(const float p_pan)
    {
        if (CHECK(CheckHandle(), "Attempted to set pan of invalid audio handle"))
            SV_SERVICE(AudioContext).m_soLoud->setPan(m_handle, p_pan);
    }

    float AudioHandle::GetSpeed() const
    {
        if (CHECK(CheckHandle(), "Attempted to get speed of invalid audio handle"))
            return SV_SERVICE(AudioContext).m_soLoud->getRelativePlaySpeed(m_handle);

        return 0.f;
    }

    void AudioHandle::SetSpeed(const float p_speed)
    {
        if (CHECK(CheckHandle(), "Attempted to set speed of invalid audio handle"))
            SV_SERVICE(AudioContext).m_soLoud->setPan(m_handle, p_speed);
    }

    bool AudioHandle::IsLooping() const
    {
        return CheckHandle() && !SV_SERVICE(AudioContext).m_soLoud->getLooping(m_handle);
    }

    void AudioHandle::SetLooping(bool p_state)
    {
        if (CHECK(CheckHandle(), "Attempted to set loop state of invalid audio handle"))
            SV_SERVICE(AudioContext).m_soLoud->setLooping(m_handle, p_state);
    }

    Time AudioHandle::GetLoopPoint() const
    {
        if (CHECK(CheckHandle(), "Attempted to get loop point of invalid audio handle"))
            return SV_SERVICE(AudioContext).m_soLoud->getLoopPoint(m_handle);

        return 0.f;
    }

    void AudioHandle::SetLoopPoint(Time p_loopPoint) const
    {
        if (CHECK(CheckHandle(), "Attempted to set loop point of invalid audio handle"))
            SV_SERVICE(AudioContext).m_soLoud->setLoopPoint(m_handle, p_loopPoint);
    }

    unsigned int AudioHandle::GetLoopCount() const
    {
        if (CHECK(CheckHandle(), "Attempted to get loop count of invalid audio handle"))
            return SV_SERVICE(AudioContext).m_soLoud->getLoopCount(m_handle);

        return 0;
    }

    void AudioHandle::SetLoopCount(unsigned int p_loopCount) const
    {
        if (CHECK(CheckHandle(), "Attempted to set loop count of invalid audio handle"))
            SV_SERVICE(AudioContext).m_soLoud->setLoopPoint(m_handle, p_loopCount);
    }

    bool AudioHandle::CheckHandle() const
    {
        return SV_SERVICE(AudioContext).m_soLoud->isValidVoiceHandle(m_handle);
    }
}
