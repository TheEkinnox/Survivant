#pragma once
#include "SurvivantAudio/AudioClip.h"
#include "SurvivantAudio/Types.h"

#include <SurvivantCore/Resources/ResourceRef.h>

namespace SvAudio
{
    class AudioHandle
    {
    public:
        /**
         * \brief Creates an empty audio handle
         */
        AudioHandle() = default;

        /**
         * \brief Creates a handle for the given audio clip
         * \param p_audioClip The target audio clip
         * \param p_handle The internal audio handle
         */
        AudioHandle(SvCore::Resources::ResourceRef<AudioClip> p_audioClip, Handle p_handle = 0);

        /**
         * \brief Checks whether the audio handle is valid or not
         * \return True if the audio handle can be played. False otherwise.
         */
        operator bool() const;

        /**
         * \brief Gets the handle's audio clip
         * \return The handle's audio clip
         */
        SvCore::Resources::ResourceRef<AudioClip> GetClip() const;

        /**
         * \brief Plays the handled sound. If the sound is already playing, restarts it
         * \param p_volume The volume of the played sound. Negative to use default
         * \param p_pan The pan of the played sound. -1 is left, 0 is middle, 1 is right
         * \param p_paused Whether the sound should start paused
         */
        void Play(float p_volume = -1.f, float p_pan = 0.f, bool p_paused = false);

        /**
         * \brief Checks whether the sound is currently playing or not
         * \return True if the sound is currently playing. False otherwise
         */
        bool IsPlaying() const;

        /**
         * \brief Sets the sound's paused state
         * \param p_state The sound's new paused state
         */
        void SetPaused(bool p_state);

        /**
         * \brief Stops the sound
         */
        void Stop();

        /**
         * \brief Gets the sound's volume
         * \return The sound's volume
         */
        float GetVolume() const;

        /**
         * \brief Sets the sound's volume
         * \param p_volume The sound's new volume
         */
        void SetVolume(float p_volume);

        /**
         * \brief Gets the sound's pan
         * \return The sound's pan
         */
        float GetPan() const;

        /**
         * \brief Sets the sound's pan
         * \param p_pan The sound's new pan
         */
        void SetPan(float p_pan);

        /**
         * \brief Gets the sound's play speed
         * \return The sound's play speed
         */
        float GetSpeed() const;

        /**
         * \brief Sets the sound's play speed
         * \param p_speed The sound's new play speed
         */
        void SetSpeed(float p_speed);

        /**
         * \brief Checks whether the sound is currently looping
         * \return True if the sound is currently looping. False otherwise
         */
        bool IsLooping() const;

        /**
         * \brief Sets the sound's looping state
         * \param p_state The sound's new looping state
         */
        void SetLooping(bool p_state);

        /**
         * \brief Gets the sound's looping point
         * \return The sound's looping point
         */
        Time GetLoopPoint() const;

        /**
         * \brief Sets the sound's loop point
         * \param p_loopPoint The sound's new loop point
         */
        void SetLoopPoint(Time p_loopPoint) const;

        /**
         * \brief Gets the sound's loop count
         * \return The sound's loop count
         */
        unsigned int GetLoopCount() const;

        /**
         * \brief Sets the sound's loop count
         * \param p_loopCount The sound's new loop count
         */
        void SetLoopCount(unsigned int p_loopCount) const;

    private:
        SvCore::Resources::ResourceRef<AudioClip> m_clip;
        Handle                                    m_handle;

        /**
         * \brief Checks whether the internal handle is valid or not
         * \return True if the internal handle is valid. False otherwise
         */
        bool CheckHandle() const;
    };
}
