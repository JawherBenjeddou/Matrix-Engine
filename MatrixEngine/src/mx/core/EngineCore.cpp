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
            MX_CORE_INFO("Initialization Started ... ");
            auto endTimeWindow = std::chrono::high_resolution_clock::now();
            auto elapsedTimeWindow = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWindow - startTimeWindow);

            m_WindowSystem.SetWindowIcon();
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

            ShaderFactory::GetInstance().CreateShader("defaultshader","../MatrixEngine/src/mx/graphics/shaders/default/default.frag","../MatrixEngine/src/mx/graphics/shaders/default/default.vert");


            float vertices[] = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                0.0f, 0.5f, 0.0f
            };

            unsigned int VBO;
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),(void*)0);
            glEnableVertexAttribArray(0);
            
            MX_CORE_INFO("Shader compilation completed successfully in {} ms");
           
            
            MX_CORE_INFO("Initialization Completed");
        }

		void EngineCore::OnUpdate()
		{
            glUseProgram(ShaderFactory::GetInstance().GetShader("defaultshader")->GetId());
            glDrawArrays(GL_TRIANGLES, 0, 3);
            m_GuiSystem.OnRenderGui();
			m_WindowSystem.OnUpdate();
            m_Timer.Tick();
		}

        //Usually in reverse order 
        void EngineCore::Shutdown()
        {
            ShaderFactory::GetInstance().Cleanup();
            m_GuiSystem.Shutdown();
            m_WindowSystem.~Window();
            Logging::ShutDown();
            MX_CORE_INFO("Engine SHUTDOWN!");
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