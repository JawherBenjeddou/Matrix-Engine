#include "pch.h"
#include "EngineCore.h"


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
            m_CameraSystem2D = std::make_shared<Matrix::graphics::OrthoCamera>(0.0f, static_cast<float>(m_WindowSystem->GetHeight()), 0.0f, static_cast<float>(m_WindowSystem->GetWidth()));
            m_WorldSystem = std::make_shared<Matrix::graphics::World>(m_CameraSystem2D);
            m_SpriteRendererSystem = std::make_shared<Matrix::graphics::SpriteRenderer>();
        }
        void EngineCore::Initialize()
        {
            //Init logging
            m_LoggingSystem.InitLogging();

            //Log system info
            GetInfo();

            // Initialize Window System
            m_WindowSystem->InitSystem();    
            m_WindowSystem->GetInfo();
            MX_CORE_INFO("Initialization Started ... ");
            m_WindowSystem->SetWindowIcon();
            MX_CORE_WARN("Window System Initialized");

            // Initialize Input System
            m_InputSystem->InitSystem();
            MX_CORE_WARN("Input System Initialized");
            // Initialize Gui System
            m_GuiSystem->InitSystem();
            MX_CORE_WARN("Gui System Initialized");

            //Initializing Shaders
            MX_CORE_WARN("Compiling Shaders...");
            ShaderFactory::GetInstance().CreateShader("defaultshader","../MatrixEngine/src/mx/graphics/shaders/default/default.frag","../MatrixEngine/src/mx/graphics/shaders/default/default.vert");
            MX_CORE_WARN("Shader compilation completed successfully");

            m_SpriteRendererSystem->Init();
            
            
            
            MX_CORE_INFO("Engine Initialization Completed");

        }

		void EngineCore::OnUpdate(float deltatime)
		{
            m_GuiSystem->OnRenderGui();
            m_WorldSystem->OnUpdate();
			m_WindowSystem->OnUpdate();
		}

        //Usually in reverse order 
        void EngineCore::Shutdown()
        {
            ShaderFactory::GetInstance().Cleanup();
            m_GuiSystem->Shutdown();
            m_WindowSystem->~Window();
            Logging::ShutDown();
            MX_CORE_INFO("Engine SHUTDOWN!");
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