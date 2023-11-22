#pragma once

///@file Timer.h

#include <chrono>
#include "Common.h"

namespace Matrix
{
    namespace util
    {
        class MATRIX_DLL Timer
        {
        public:
            Timer();

            /// Called at the beginning of each frame.
            void StartFrame();

            /// Stop the timer.
            void Stop();

            /// Reset the timer.
            void Reset();

            /// Get the elapsed time in seconds.
            float ElapsedTime() const;

            /// Get the time in seconds since the last frame.
            float DeltaTime() const;

            /// Called at the end of each frame.
            void EndFrame();

        private:
            std::chrono::high_resolution_clock::time_point m_StartTime;
            std::chrono::high_resolution_clock::time_point m_LastFrameTime;
            float m_ElapsedTime;
            bool m_Running;
        };
    }
}
