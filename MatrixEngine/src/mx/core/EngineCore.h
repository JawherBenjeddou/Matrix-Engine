#pragma once
#include "Common.h"

//Systems
#include "ui\Window.h"
#include "Logging.h"
#include "gui/GuiManager.h"

namespace Matrix
{
	namespace core
	{
        class MATRIX_API EngineCore
        {
        public:
            EngineCore();

            // Initialize the engine and all its subsystems
            void Initialize();

            // Run the main update loop for the engine
            void OnUpdate();

            // Clean up and shut down the engine
            void Shutdown();

            // Check if the engine's window is closed
            bool IsWindowClosed() const;

            void GetInfo();

        private:
            Matrix::core::Logging m_LoggingSystem;
            Matrix::ui::Window m_WindowSystem;
            Matrix::ui::Input m_InputSystem;
            Matrix::gui::GuiManager m_GuiSystem;
        };

	}
}
