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
            
            //Init logging
            m_LoggingSystem.InitLogging();

            //Log system info
            GetInfo();

            // Initialize Window System
            m_WindowSystem.InitSystem();
            m_WindowSystem.GetInfo();
            MX_CORE_INFO("Initialization Started ... ");
            m_WindowSystem.SetWindowIcon();
            MX_CORE_WARN("Window System Initialized");

            // Initialize Input System
            m_InputSystem.InitSystem();
            MX_CORE_WARN("Input System Initialized");

            // Initialize Gui System
            m_GuiSystem.InitSystem();
            MX_CORE_WARN("Gui System Initialized");

            //Initializing Shaders
            MX_CORE_WARN("Compiling Shaders...");
            ShaderFactory::GetInstance().CreateShader("defaultshader","../MatrixEngine/src/mx/graphics/shaders/default/default.frag","../MatrixEngine/src/mx/graphics/shaders/default/default.vert");


            float vertices[] = {
             -0.5f, -0.5f, 0.0f,
              0.5f, -0.5f, 0.0f,
              0.5f,  0.5f, 0.0f,
              -0.5f,  0.5f, 0.0f
            };

            unsigned int VBO;
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            
            MX_CORE_WARN("Shader compilation completed successfully");
           
            
            MX_CORE_INFO("Engine Initialization Completed");
        }

		void EngineCore::OnUpdate()
		{
            m_Timer.Tick();
            glUseProgram(ShaderFactory::GetInstance().GetShader("defaultshader")->GetId());
            glDrawArrays(GL_TRIANGLES, 0, 3);
            m_GuiSystem.OnRenderGui();
			m_WindowSystem.OnUpdate();
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