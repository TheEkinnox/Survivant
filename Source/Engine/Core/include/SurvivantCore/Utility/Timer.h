#pragma once
#include <chrono>

namespace SvCore::Utility
{
    class Timer
    {
    public:
        /**
         * \brief Creates a default timer
         */
        Timer() = default;

        /**
         * \brief Creates a timer with the given time scale
         * \param p_timeScale The timer's initial time scale
         */
        explicit Timer(float p_timeScale);

        /**
         * \brief Creates a copy of the given timer
         * \param p_other The timer to copy
         */
        Timer(const Timer& p_other) = default;

        /**
         * \brief Creates a move copy of the given timer
         * \param p_other The timer to move
         */
        Timer(Timer&& p_other) noexcept = default;

        /**
         * \brief Destroys the timer
         */
        ~Timer() = default;

        /**
         * \brief Assigns a copy of the given timer to this one
         * \param p_other The timer to copy
         * \return A reference to the modified timer
         */
        Timer& operator=(const Timer& p_other) = default;

        /**
         * \brief Moves the given timer into this one
         * \param p_other The timer to move
         * \return A reference to the modified timer
         */
        Timer& operator=(Timer&& p_other) noexcept = default;

        /**
         * \brief Gets the shared timer instance
         * \return The shared timer instance
         */
        static Timer& GetInstance();

        /**
         * \brief Updates the timer's info
         */
        void Tick();

        /**
         * \brief Resets the timer with the given time scale
         * \param p_timeScale The timer's new time scale
         */
        void Reset(float p_timeScale = 1.f);

        /**
         * \brief Gets the scaled time since the creation of this timer
         * \return The scaled time elapsed since the creation of this timer
         */
        float GetTime() const;

        /**
         * \brief Gets the scaled time between the previous and last ticks
         * \return The scaled time between the previous and last ticks
         */
        float GetDeltaTime() const;

        /**
         * \brief Gets the unscaled time since the creation of this timer
         * \return The unscaled time elapsed since the creation of this timer
         */
        float GetUnscaledTime() const;

        /**
         * \brief Gets the unscaled time between the previous and last ticks
         * \return The unscaled time between the previous and last ticks
         */
        float GetUnscaledDeltaTime() const;

        /**
         * \brief Gets the current time scale
         * \return The current time scale
         */
        float GetTimeScale() const;

        /**
         * \brief Sets the timer's time scale
         */
        void SetTimeScale(float p_timeScale);

        /**
         * \brief Gets the number of time the "Tick" function has been called
         * \return The elapsed number of frames since the timer's creation
         */
        uint64_t GetFrameCount() const;

    private:
        using clock = std::chrono::steady_clock;

        clock::time_point m_currentTime = clock::now();
        clock::time_point m_lastUpdate  = clock::now();

        uint64_t m_frameCount = 0;

        float m_time          = 0.f;
        float m_unscaledTime  = 0.f;
        float m_deltaTime     = 0.f;
        float m_timeScale     = 1.f;
        float m_nextTimeScale = 1.f;

        bool m_isFirstUpdate = true;
    };
}
