#include "pch.h"
#include "EngineCore.h"

namespace Matrix
{
	namespace core
	{

        void EngineCore::Initialize()
        {
            m_LoggingSystem.InitLogging();
            GetInfo();

            // Initialize Window System
            auto startTimeWindow = std::chrono::high_resolution_clock::now();
            m_WindowSystem.InitSystem();
            m_WindowSystem.GetInfo();
            auto endTimeWindow = std::chrono::high_resolution_clock::now();
            auto elapsedTimeWindow = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWindow - startTimeWindow);

            MX_CORE_INFO("Initialization Started ... ");
            MX_CORE_WARN("Window System Initialized in {} ms", elapsedTimeWindow.count());

            // Initialize Input System
            auto startTimeInput = std::chrono::high_resolution_clock::now();
            m_InputSystem.InitSystem();
            auto endTimeInput = std::chrono::high_resolution_clock::now();
            auto elapsedTimeInput = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeInput - startTimeInput);

            MX_CORE_WARN("Input System Initialized in {} ms", elapsedTimeInput.count());

            MX_CORE_INFO("Initialization Completed");
        }

		void EngineCore::OnUpdate()
		{
			m_WindowSystem.OnUpdate();
		}

        //Usually in reverse order 
        void EngineCore::Shutdown()
        {
            m_WindowSystem.~Window();
            Logging::ShutDown();
        }

		bool EngineCore::IsWindowClosed() const
		{
			return m_WindowSystem.Closed();
		}

        void EngineCore::GetInfo()
        {
            MX_CORE_INFO("MATRIX ENGINE v{}.{}", 0, 1);

#ifdef  MX_RELEASE_MODE 

            std::cout << "Configuration : RELEASE \n";

#endif
#ifdef MX_DEBUG_MODE

            std::cout << "Configuration : DEBUG \n";

#endif 
        }

	}
}