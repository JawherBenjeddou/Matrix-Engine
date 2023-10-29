#pragma once
///@file Timer.h

#include "GLFW/glfw3.h"
#include <chrono>

namespace Matrix
{
    namespace util
    {
        class Timer
        {
        public:
            Timer();

            /// Start or restart the timer.
            void Start();

            /// Stop the timer.
            void Stop();

            /// Reset the timer.
            void Reset();

            /// Get the elapsed time in seconds.
            float ElapsedTime() const;

            /// Get the time in seconds since the last frame.
            float DeltaTime() const;

            /// Called at the beginning of each frame.
            void Tick();

        private:
            std::chrono::high_resolution_clock::time_point m_StartTime;
            std::chrono::high_resolution_clock::time_point m_LastFrameTime;
            float m_ElapsedTime;
            bool m_Running;
        };
    }
}
