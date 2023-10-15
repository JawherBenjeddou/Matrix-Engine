#pragma once

#include "Common.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Matrix
{
	namespace core
	{
		class MATRIX_API Logging
		{
		public:
			void InitLogging();

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

		private:
			inline static std::shared_ptr<spdlog::logger> s_CoreLogger;
			inline static std::shared_ptr<spdlog::logger> s_ClientLogger;
		};
	}
}

// Core-log macros
#define MX_CORE_TRACE(...)	  Matrix::core::Logging::GetCoreLogger()->trace(__VA_ARGS__)
#define MX_CORE_INFO(...)     Matrix::core::Logging::GetCoreLogger()->info(__VA_ARGS__)
#define MX_CORE_WARN(...)     Matrix::core::Logging::GetCoreLogger()->warn(__VA_ARGS__)
#define MX_CORE_ERROR(...)    Matrix::core::Logging::GetCoreLogger()->error(__VA_ARGS__)
#define MX_CORE_CRITICAL(...) Matrix::core::Logging::GetCoreLogger()->critical(__VA_ARGS__)
										
// Client-log macros					
#define MX_TRACE(...)         Matrix::core::Logging::GetClientLogger()->trace(__VA_ARGS__)
#define MX_INFO(...)          Matrix::core::Logging::GetClientLogger()->info(__VA_ARGS__)
#define MX_WARN(...)          Matrix::core::Logging::GetClientLogger()->warn(__VA_ARGS__)
#define MX_ERROR(...)         Matrix::core::Logging::GetClientLogger()->error(__VA_ARGS__)
#define MX_CRITICAL(...)      Matrix::core::Logging::GetClientLogger()->critical(__VA_ARGS__)