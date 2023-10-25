#include "pch.h"
#include "EngineCore.h"

namespace Matrix
{
	namespace core
	{
        EngineCore::EngineCore()
            :m_GuiSystem(m_WindowSystem)
        {
   
        }
        void EngineCore::Initialize()
        {
            //Starts the GameTimer
            m_Timer.Start();

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

            // Initialize Gui System
            auto startTimeGui = std::chrono::high_resolution_clock::now();
            m_GuiSystem.InitSystem();
            auto endTimeGui = std::chrono::high_resolution_clock::now();
            auto elapsedTimeGui = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeInput - startTimeInput);

            MX_CORE_WARN("Gui System Initialized in {} ms", elapsedTimeGui.count());




            MX_CORE_INFO("Initialization Completed");
        }

		void EngineCore::OnUpdate()
		{
            //m_GuiSystem.BeginFrame(); //Called first than gui code batween this and update
            m_GuiSystem.OnRenderGui();
			m_WindowSystem.OnUpdate();
            m_Timer.Tick();
		}

        //Usually in reverse order 
        void EngineCore::Shutdown()
        {
            MX_CORE_TRACE("Engine shutting down...");
            m_WindowSystem.~Window();
            Logging::ShutDown();
            m_GuiSystem.Shutdown();
            MX_CORE_TRACE("Engine SHUTDOWN!");
        }

		bool EngineCore::IsWindowClosed() const
		{
			return m_WindowSystem.Closed();
		}

        void EngineCore::GetInfo()
        {
            MX_CORE_INFO("MATRIX ENGINE v{}.{}", 0, 1);
            std::cout << ("-------------------- \n");
            std::cout << ("SYSTEM INFORMATION \n");
            std::cout << ("-------------------- \n");

#ifdef  MX_DEBUG_MODE 

            std::cout << ("Configuration : DEBUG \n");
#endif  
#ifdef  MX_RELEASE_MODE

            std::cout << ("Configuration : RELEASE \n");

#endif 
        }

	}
}