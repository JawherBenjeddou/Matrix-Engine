#include "pch.h"
#include "Logging.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/base_sink.h"

#include <Windows.h>

namespace Matrix::core
{
    namespace
    {
        std::mutex s_HistoryMutex;
        std::deque<LogMessage> s_History;
        std::atomic<uint64_t> s_HistoryVersion{ 0 };
    }

    /// spdlog sink that stores formatted messages in memory for the editor console.
    class HistorySink : public spdlog::sinks::base_sink<std::mutex>
    {
    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            LogMessage entry;
            entry.Level = static_cast<LogLevel>(static_cast<int>(msg.level));
            entry.Logger = std::string(msg.logger_name.data(), msg.logger_name.size());
            entry.Text = std::string(msg.payload.data(), msg.payload.size());

            auto time = std::chrono::system_clock::to_time_t(msg.time);
            std::tm local{};
            localtime_s(&local, &time);
            char buffer[16];
            std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &local);
            entry.Time = buffer;

            Logging::PushHistory(std::move(entry));
        }

        void flush_() override {}
    };

    void Logging::Init(const std::string& logFilePath)
    {
        if (s_Initialized)
            return;

        std::vector<spdlog::sink_ptr> sinks;

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_pattern("%^[%T] %n: %v%$");
        sinks.push_back(consoleSink);

        sinks.push_back(std::make_shared<HistorySink>());

        if (!logFilePath.empty())
        {
            try
            {
                std::filesystem::path path(logFilePath);
                if (path.has_parent_path())
                    std::filesystem::create_directories(path.parent_path());
                auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath, true);
                fileSink->set_pattern("[%T] [%l] %n: %v");
                sinks.push_back(fileSink);
            }
            catch (const std::exception& e)
            {
                std::cerr << "Could not create log file '" << logFilePath << "': " << e.what() << "\n";
            }
        }

        s_CoreLogger = std::make_shared<spdlog::logger>("ENGINE", sinks.begin(), sinks.end());
        s_CoreLogger->set_level(spdlog::level::trace);
        s_CoreLogger->flush_on(spdlog::level::warn);
        spdlog::register_logger(s_CoreLogger);

        s_ClientLogger = std::make_shared<spdlog::logger>("APP", sinks.begin(), sinks.end());
        s_ClientLogger->set_level(spdlog::level::trace);
        s_ClientLogger->flush_on(spdlog::level::warn);
        spdlog::register_logger(s_ClientLogger);

        s_Initialized = true;
    }

    void Logging::Shutdown()
    {
        if (!s_Initialized)
            return;
        s_CoreLogger->flush();
        s_ClientLogger->flush();
        spdlog::drop_all();
        s_CoreLogger.reset();
        s_ClientLogger.reset();
        s_Initialized = false;
    }

    std::shared_ptr<spdlog::logger>& Logging::GetCoreLogger()
    {
        if (!s_Initialized)
            Init();
        return s_CoreLogger;
    }

    std::shared_ptr<spdlog::logger>& Logging::GetClientLogger()
    {
        if (!s_Initialized)
            Init();
        return s_ClientLogger;
    }

    void Logging::PushHistory(LogMessage&& message)
    {
        std::lock_guard<std::mutex> lock(s_HistoryMutex);
        s_History.push_back(std::move(message));
        while (s_History.size() > MaxHistory)
            s_History.pop_front();
        ++s_HistoryVersion;
    }

    std::vector<LogMessage> Logging::GetHistory()
    {
        std::lock_guard<std::mutex> lock(s_HistoryMutex);
        return std::vector<LogMessage>(s_History.begin(), s_History.end());
    }

    uint64_t Logging::GetHistoryVersion()
    {
        return s_HistoryVersion.load();
    }

    void Logging::ClearHistory()
    {
        std::lock_guard<std::mutex> lock(s_HistoryMutex);
        s_History.clear();
        ++s_HistoryVersion;
    }

    void ReportAssertionFailure(const char* expression, const char* file, int line, const std::string& message, bool fatal)
    {
        std::string text = fmt::format("{} failed: {} ({}:{}) {}", fatal ? "Assertion" : "Verification", expression, file, line, message);
        if (Logging::IsInitialized())
            MX_CORE_CRITICAL("{}", text);
        else
            std::cerr << text << std::endl;

        if (IsDebuggerPresent())
            __debugbreak();

        if (fatal)
        {
            Logging::Shutdown();
            std::abort();
        }
    }
}
