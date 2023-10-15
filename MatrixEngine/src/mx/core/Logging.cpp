#include "pch.h"
#include "Logging.h"


namespace Matrix
{
	namespace core
	{
		void Logging::InitLogging()
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			s_CoreLogger = spdlog::stdout_color_mt("ENGINE");
			s_ClientLogger = spdlog::stdout_color_mt("APPLICATION");
		}

	}
}