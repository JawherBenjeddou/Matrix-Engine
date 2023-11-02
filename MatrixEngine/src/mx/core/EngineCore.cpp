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

            
            MX_CORE_WARN("Shader compilation completed successfully");

            //TODO: figure out how to draw the rect and than add the texture later on..
            float vertices[] = {
                // positions // colors // texture coords
                0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
                0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
                -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
            };
            using namespace Matrix::graphics;
            
            // Generate VAO and bind it
            VertexArray vao;

            // Generate VBO, bind it, and fill it with data
            VertexBuffer vbo;
            vbo.BufferData(sizeof(vertices), vertices);

            // Specify the vertex attributes
            // Position attribute (x and y only)
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Texture coordinate attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            // Unbind VBO and VAO
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            //vao.UnBind();
            //Texture2D sprite;
            //sprite.LoadTexture2D("../resources/branding/logo2_white.png");

            MX_CORE_INFO("Engine Initialization Completed");
        }

		void EngineCore::OnUpdate()
		{
            m_Timer.Tick();
            glUseProgram(ShaderFactory::GetInstance().GetShader("defaultshader")->GetId());

            glDrawArrays(GL_TRIANGLES, 0, 6);
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