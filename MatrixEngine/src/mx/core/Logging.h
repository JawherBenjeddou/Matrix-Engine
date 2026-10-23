#pragma once
#include "Common.h"
///@file Logging.h  spdlog based logging with console, file and in-memory (editor console) sinks.

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Matrix::core
{
    /// Severity levels mirrored from spdlog so consumers do not need spdlog headers.
    enum class LogLevel : int { Trace = 0, Debug = 1, Info = 2, Warn = 3, Error = 4, Critical = 5 };

    /// A single captured log line, used by the editor console.
    struct MATRIX_DLL LogMessage
    {
        LogLevel Level = LogLevel::Info;
        std::string Logger;
        std::string Text;
        std::string Time;
    };

    class MATRIX_DLL Logging
    {
    public:
        /// Initializes the logging system. logFilePath may be empty to disable the file sink.
        static void Init(const std::string& logFilePath = "");

        /// Flushes and shuts the logging system down. Safe to call multiple times.
        static void Shutdown();

        static bool IsInitialized() { return s_Initialized; }

        /// Core (engine) logger. Lazily initializes the system if needed.
        static std::shared_ptr<spdlog::logger>& GetCoreLogger();

        /// Client (application) logger. Lazily initializes the system if needed.
        static std::shared_ptr<spdlog::logger>& GetClientLogger();

        /// Returns a copy of the captured message history (newest last).
        static std::vector<LogMessage> GetHistory();

        /// Increments every time a message is captured; cheap way for UIs to detect changes.
        static uint64_t GetHistoryVersion();

        static void ClearHistory();

        /// Maximum number of messages kept in memory.
        static constexpr size_t MaxHistory = 4000;

    private:
        friend class HistorySink;
        static void PushHistory(LogMessage&& message);

        inline static std::shared_ptr<spdlog::logger> s_CoreLogger;
        inline static std::shared_ptr<spdlog::logger> s_ClientLogger;
        inline static bool s_Initialized = false;
    };
}

/// Core (engine) log macros
#define MX_CORE_TRACE(...)    ::Matrix::core::Logging::GetCoreLogger()->trace(__VA_ARGS__)
#define MX_CORE_INFO(...)     ::Matrix::core::Logging::GetCoreLogger()->info(__VA_ARGS__)
#define MX_CORE_WARN(...)     ::Matrix::core::Logging::GetCoreLogger()->warn(__VA_ARGS__)
#define MX_CORE_ERROR(...)    ::Matrix::core::Logging::GetCoreLogger()->error(__VA_ARGS__)
#define MX_CORE_CRITICAL(...) ::Matrix::core::Logging::GetCoreLogger()->critical(__VA_ARGS__)

/// Client (application) log macros
#define MX_TRACE(...)         ::Matrix::core::Logging::GetClientLogger()->trace(__VA_ARGS__)
#define MX_INFO(...)          ::Matrix::core::Logging::GetClientLogger()->info(__VA_ARGS__)
#define MX_WARN(...)          ::Matrix::core::Logging::GetClientLogger()->warn(__VA_ARGS__)
#define MX_ERROR(...)         ::Matrix::core::Logging::GetClientLogger()->error(__VA_ARGS__)
#define MX_CRITICAL(...)      ::Matrix::core::Logging::GetClientLogger()->critical(__VA_ARGS__)
