#include "SurvivantCore/Utility/Timer.h"

#include <chrono>

namespace SvCore::Utility
{
    Timer::Timer(const float p_timeScale)
        : m_timeScale(p_timeScale)
    {
    }

    Timer& Timer::GetInstance()
    {
        static Timer timer;
        return timer;
    }

    void Timer::Tick()
    {
        if (m_isFirstUpdate)
        {
            m_currentTime = clock::now();
            m_lastUpdate  = m_currentTime;
            m_deltaTime   = 0;

            m_isFirstUpdate = false;
        }
        else
        {
            m_lastUpdate  = m_currentTime;
            m_currentTime = clock::now();
            m_deltaTime   = std::chrono::duration<float>(m_currentTime - m_lastUpdate).count();
            m_unscaledTime += m_deltaTime;
            m_time += m_deltaTime * m_timeScale;
        }

        m_frameCount++;
    }

    void Timer::Reset(const float p_timeScale)
    {
        m_isFirstUpdate = true;
        m_timeScale     = p_timeScale;
    }

    float Timer::GetTime() const
    {
        return m_time;
    }

    float Timer::GetDeltaTime() const
    {
        return m_deltaTime * m_timeScale;
    }

    float Timer::GetUnscaledTime() const
    {
        return m_unscaledTime;
    }

    float Timer::GetUnscaledDeltaTime() const
    {
        return m_deltaTime;
    }

    float Timer::GetTimeScale() const
    {
        return m_timeScale;
    }

    void Timer::SetTimeScale(const float p_timeScale)
    {
        m_timeScale = p_timeScale;
    }

    uint64_t Timer::GetFrameCount() const
    {
        return m_frameCount;
    }
}
