#pragma once
#include "Common.h"

///@file EngineCore.h


//Systems
#include "ui/Window.h"
#include "Logging.h"
#include "gui/GuiManager.h"


#include "graphics/shaders/ShaderFactory.h"
#include "graphics/textures/Texture2D.h"
#include "graphics/renderer/SpriteRenderer.h"
#include "graphics/cameras/OrthoCamera.h"
#include "graphics/world/World.h"

#include "graphics/buffers/ElementBuffer.h"
#include "graphics/buffers/VertexArray.h"
#include "graphics/buffers/VertexBuffer.h"

namespace Matrix
{
	namespace core
	{
        class MATRIX_DLL EngineCore
        {
        public:
            EngineCore();

            /// Initialize the engine and all its subsystems.
            void Initialize();  

            /// Run the main update loop for the engine.
            void OnUpdate(float deltatime);
            
            /// Clean up and shut down the engine.
            void Shutdown();

            /// Check if the engine's window is closed.
            bool IsWindowClosed() const;

            ///Checks build config and prints an info message at the start.
            void GetInfo();

        private:
             using ShaderFactory = Matrix::graphics::ShaderFactory;
             Matrix::core::Logging m_LoggingSystem;
             SharedObj<Matrix::ui::Window>m_WindowSystem;
             SharedObj<Matrix::ui::Input>m_InputSystem;
             SharedObj<Matrix::gui::GuiManager>m_GuiSystem;
             SharedObj<Matrix::graphics::OrthoCamera>m_CameraSystem2D;
             SharedObj<Matrix::graphics::World>m_WorldSystem;
             SharedObj<Matrix::graphics::SpriteRenderer>m_SpriteRendererSystem;
        };

	}
}
