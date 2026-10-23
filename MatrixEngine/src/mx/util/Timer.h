#pragma once
#include "Common.h"
///@file Timer.h  Frame timing utilities.

namespace Matrix::util
{
    /// Tracks per-frame timing. Call Tick() once at the start of every frame.
    class MATRIX_DLL Timer
    {
    public:
        Timer();

        /// Advances the timer by one frame and updates DeltaTime / FPS.
        void Tick();

        /// Resets total time and delta time.
        void Reset();

        /// Seconds elapsed between the last two Tick() calls (clamped to MaxDeltaTime).
        float DeltaTime() const { return m_DeltaTime; }

        /// Unclamped delta time of the last frame in seconds.
        float RawDeltaTime() const { return m_RawDeltaTime; }

        /// Seconds since the timer was created or reset.
        float TotalTime() const { return m_TotalTime; }

        /// Smoothed frames-per-second estimate.
        float FPS() const { return m_FPS; }

        /// Number of Tick() calls so far.
        uint64_t FrameCount() const { return m_FrameCount; }

        /// Largest delta time that will be reported; protects simulations from huge steps after stalls.
        static constexpr float MaxDeltaTime = 0.1f;

    private:
        using Clock = std::chrono::steady_clock;
        Clock::time_point m_StartTime;
        Clock::time_point m_LastTick;
        float m_DeltaTime = 0.0f;
        float m_RawDeltaTime = 0.0f;
        float m_TotalTime = 0.0f;
        float m_FPS = 0.0f;
        float m_FPSAccumulator = 0.0f;
        uint32_t m_FPSFrames = 0;
        uint64_t m_FrameCount = 0;
    };

    /// Measures wall-clock time between construction and Elapsed() calls, in milliseconds.
    class MATRIX_DLL Stopwatch
    {
    public:
        Stopwatch() : m_Start(std::chrono::steady_clock::now()) {}
        void Restart() { m_Start = std::chrono::steady_clock::now(); }
        float ElapsedMilliseconds() const
        {
            return std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - m_Start).count();
        }
    private:
        std::chrono::steady_clock::time_point m_Start;
    };
}
