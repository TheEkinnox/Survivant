#pragma once
#include <SurvivantCore/Resources/IResource.h>

namespace SoLoud
{
    class AudioSource;
}

namespace SvAudio
{
    class AudioClip final : public SvCore::Resources::IResource
    {
        REGISTERED_RESOURCE_BODY()

    public:
        /**
         * \brief Creates an empty audio clip
         */
        AudioClip() = default;

        /**
         * \brief Creates a copy of the given audio clip
         * \param p_other The audio clip to copy
         */
        AudioClip(const AudioClip& p_other) = default;

        /**
         * \brief Creates a move copy of the given audio clip
         * \param p_other The audio clip to move
         */
        AudioClip(AudioClip&& p_other) noexcept = default;

        /**
         * \brief Destroys the audio clip
         */
        ~AudioClip() override = default;

        /**
         * \brief Assigns a copy of the given audio clip to this one
         * \param p_other The audio clip to copy
         * \return A reference to the modified audio clip
         */
        AudioClip& operator=(const AudioClip& p_other) = default;

        /**
         * \brief Moves the given audio clip into this one
         * \param p_other The audio clip to move
         * \return A reference to the modified audio clip
         */
        AudioClip& operator=(AudioClip&& p_other) noexcept = default;

        /**
         * \brief Loads the audio clip from the given file
         * \param p_fileName The path of the audio clip to load
         * \return True on success. False otherwise
         */
        bool Load(const std::string& p_fileName) override;

        bool Init() override
        {
            return true;
        }

        bool Save(const std::string&, bool) override
        {
            return true;
        }

    private:
        friend class AudioContext;

        std::shared_ptr<SoLoud::AudioSource> m_sound;
    };
}
