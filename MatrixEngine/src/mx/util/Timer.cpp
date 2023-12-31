#include"pch.h"
#include "Timer.h"

namespace Matrix
{
    namespace util
    {
        Timer::Timer()
            :m_Running(false),
             m_ElapsedTime(0.0f)
        {
            // Initialize the timer without starting it.
        }

        void Timer::StartFrame()
        {
            m_StartTime = std::chrono::high_resolution_clock::now();
            m_LastFrameTime = m_StartTime;
            m_Running = true;
        }

        void Timer::Stop()
        {
            m_Running = false;
        }

        void Timer::Reset()
        {
            m_StartTime = std::chrono::high_resolution_clock::now();
            m_LastFrameTime = m_StartTime;
            m_ElapsedTime = 0.0f;
        }

        float Timer::ElapsedTime() const
        {
            if (m_Running)
            {
                auto now = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float> duration = now - m_StartTime;
                return duration.count();
            }
            return m_ElapsedTime;
        }

        float Timer::DeltaTime() const
        {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> duration = now - m_LastFrameTime;
            return duration.count() * 1000.0f;
        }

        void Timer::EndFrame()
        {
            if (m_Running)
            {
                auto now = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float> frameDuration = now - m_LastFrameTime;
                m_ElapsedTime += frameDuration.count();
                m_LastFrameTime = now;
            }
        }
    }
}
