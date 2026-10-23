#include "pch.h"
#include "Timer.h"

namespace Matrix::util
{
    Timer::Timer()
    {
        Reset();
    }

    void Timer::Reset()
    {
        m_StartTime = Clock::now();
        m_LastTick = m_StartTime;
        m_DeltaTime = 0.0f;
        m_RawDeltaTime = 0.0f;
        m_TotalTime = 0.0f;
        m_FPS = 0.0f;
        m_FPSAccumulator = 0.0f;
        m_FPSFrames = 0;
        m_FrameCount = 0;
    }

    void Timer::Tick()
    {
        const auto now = Clock::now();
        m_RawDeltaTime = std::chrono::duration<float>(now - m_LastTick).count();
        m_LastTick = now;
        m_DeltaTime = std::min(m_RawDeltaTime, MaxDeltaTime);
        m_TotalTime = std::chrono::duration<float>(now - m_StartTime).count();
        ++m_FrameCount;

        // Smooth FPS over ~0.25 second windows.
        m_FPSAccumulator += m_RawDeltaTime;
        ++m_FPSFrames;
        if (m_FPSAccumulator >= 0.25f)
        {
            m_FPS = static_cast<float>(m_FPSFrames) / m_FPSAccumulator;
            m_FPSAccumulator = 0.0f;
            m_FPSFrames = 0;
        }
    }
}
