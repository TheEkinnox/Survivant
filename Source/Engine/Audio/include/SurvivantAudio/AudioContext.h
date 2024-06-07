#pragma once
#include "SurvivantAudio/AudioHandle.h"
#include "SurvivantAudio/Types.h"

namespace SoLoud
{
    class Soloud;
}

namespace SvAudio
{
    class AudioContext
    {
    public:
        /**
         * \brief Creates a default audio context
         */
        AudioContext() = default;

        /**
         * \brief Disable audio context copying
         */
        AudioContext(const AudioContext& p_other) = delete;

        /**
         * \brief Creates a move copy of the given audio context
         * \param p_other The audio context to move
         */
        AudioContext(AudioContext&& p_other) noexcept = default;

        /**
         * \brief Destroys the audio context
         */
        ~AudioContext();

        /**
         * \brief Disable audio context copying
         */
        AudioContext& operator=(const AudioContext& p_other) = delete;

        /**
         * \brief Moves the given audio context into this one
         * \param p_other The audio context to move
         * \return A reference to the modified audio context
         */
        AudioContext& operator=(AudioContext&& p_other) noexcept = default;

        /**
         * \brief Gets a reference to the current audio context
         * \return A reference to the current audio context
         */
        static AudioContext& GetInstance();

        /**
         * \brief Gets the string representation of the given operation result code
         * \param p_result The operation result code
         * \return The result code's string representation
         */
        static const char* GetResultString(Result p_result);

        /**
         * \brief Initializes the audio context
         * \return True on success. False otherwise
         */
        bool Init();

        /**
         * \brief Clears the audio context
         */
        void Reset();

        /**
         * \brief Plays the given audio clip
         * \param p_clip The audio clip to play
         * \param p_volume The volume of the played sound. Negative to use default
         * \param p_pan The pan of the played sound. -1 is left, 0 is middle, 1 is right
         * \param p_paused Whether the sound should start paused
         * \return A handle to the playing sound on success. An invalid handle otherwise
         */
        AudioHandle Play(const SvCore::Resources::ResourceRef<AudioClip>& p_clip,
                         float p_volume = -1.f, float p_pan = 0.f, bool p_paused = false) const;

        /**
         * \brief Stops all the currently playing songs
         */
        void StopAll() const;

    private:
        friend class AudioHandle;

        std::unique_ptr<SoLoud::Soloud> m_soLoud;
    };
}
