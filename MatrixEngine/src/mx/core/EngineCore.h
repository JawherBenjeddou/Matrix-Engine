#pragma once

///@file EngineCore.h

#include "Common.h"

//Systems
#include "ui/Window.h"
#include "Logging.h"
#include "gui/GuiManager.h"
#include "util/Timer.h"

#include "graphics/shaders/ShaderFactory.h"
#include "graphics/textures/Texture2D.h"

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
            void OnUpdate();
            
            /// Clean up and shut down the engine.
            void Shutdown();

            /// Check if the engine's window is closed.
            bool IsWindowClosed() const;

            ///Checks build config and prints an info message at the start.
            void GetInfo();

        private:
            using ShaderFactory = Matrix::graphics::ShaderFactory;

            Matrix::core::Logging m_LoggingSystem;
            Matrix::ui::Window m_WindowSystem;
            Matrix::ui::Input m_InputSystem;
            Matrix::gui::GuiManager m_GuiSystem;
            Matrix::util::Timer m_Timer;
        };

	}
}
