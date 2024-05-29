#include "SurvivantAudio/AudioClip.h"

#include "SurvivantAudio/AudioContext.h"

#include <soloud_wav.h>

namespace SvAudio
{
    bool AudioClip::Load(const std::string& p_fileName)
    {
        auto                 sound  = std::make_unique<SoLoud::Wav>();
        const SoLoud::result result = sound->load(p_fileName.c_str());

        if (!CHECK(result == SoLoud::SO_NO_ERROR, "Failed to load audio clip from \"%s\" - %s",
                p_fileName.c_str(), AudioContext::GetResultString(result)))
            return false;

        m_sound.reset(sound.release());
        return true;
    }
}
