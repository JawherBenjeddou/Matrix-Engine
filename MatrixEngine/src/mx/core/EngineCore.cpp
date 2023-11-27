#include "pch.h"
#include "EngineCore.h"

#include "Event.h"

namespace Matrix
{
	namespace core
	{
        EngineCore::EngineCore()
        {
            m_WindowSystem = std::make_shared<Matrix::ui::Window>();
            m_InputSystem = std::make_shared<Matrix::ui::Input>();
            m_GuiSystem = std::make_shared < Matrix::gui::GuiManager>(m_WindowSystem);
            //TODO : is the camera gonna be updated every frame? check for that 
            //it actually fucking worked damn (understand why we pass parameters like this)
            //keep playing around with this
            m_CameraSystem2D = std::make_shared<Matrix::graphics::OrthoCamera>(-5.0f,5.0f, -5.0f, 5.0f);
            m_WorldSystem = std::make_shared<Matrix::graphics::World>();
        }
        void EngineCore::Initialize()
        {
            //Init logging
            m_LoggingSystem.InitLogging();

            //Log system info
            GetInfo();
            
            //TODO : maybe change the systems log infos in the functions itself??
            // Initialize Window System
            m_WindowSystem->InitSystem();    
            m_WindowSystem->GetInfo();
            MX_CORE_INFO("Initialization Started ... ");
            m_WindowSystem->SetWindowIcon();
            MX_CORE_WARN("Window System Initialized");

            // Initialize Input System
            m_InputSystem->InitSystem();
            // Initialize Gui System
            m_GuiSystem->InitSystem();
            //Initializing Shaders
            MX_CORE_WARN("Compiling Shaders...");
            ShaderFactory::GetInstance().CreateShader("defaultshader","../resources/shaders/default.frag","../resources/shaders/default.vert");
            MX_CORE_WARN("Shader compilation completed successfully");
            m_WorldSystem->InitWorld();
            MX_CORE_INFO("Engine Initialization Completed");

        }

		void EngineCore::OnUpdate(float deltatime)
		{
            ShaderFactory::GetInstance().GetShader("defaultshader")->EnableShaderProgram();
            ShaderFactory::GetInstance().GetShader("defaultshader")->SetUniformValue<glm::mat4>("u_ViewProjection", m_CameraSystem2D->GetViewProjectionMatrix());
            test += 1;
            if (test == 100)
            {
                BROADCAST_EVENT("move");
            }
            m_WorldSystem->OnUpdate();
            m_GuiSystem->OnRenderGui();
			m_WindowSystem->OnUpdate();
       
		}

        //Usually in reverse order 
        void EngineCore::Shutdown()
        {
            ShaderFactory::GetInstance().Cleanup();
            m_GuiSystem->Shutdown();
            m_WindowSystem->~Window();
            m_WindowSystem.reset();
            Logging::ShutDown();
            MX_CORE_ERROR("Engine SHUTDOWN!");
        }

		bool EngineCore::IsWindowClosed() const
		{
			return m_WindowSystem->Closed();
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