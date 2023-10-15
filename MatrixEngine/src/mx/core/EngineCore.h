#pragma once
#include "Common.h"

#include "ui\Window.h"
#include "Logging.h"

namespace Matrix
{
	namespace core
	{
        class MATRIX_API EngineCore
        {
        public:

            // Initialize the engine and all its subsystems
            void Initialize();

            // Run the main update loop for the engine
            void OnUpdate();

            // Clean up and shut down the engine
            void Shutdown();

            // Check if the engine's window is closed
            bool IsWindowClosed() const;

        private:
            Matrix::core::Logging m_LoggingSystem;
            Matrix::ui::Window m_WindowSystem;
            Matrix::ui::Input m_InputSystem;
        };

	}
}
