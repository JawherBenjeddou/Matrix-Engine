#pragma once

#include "ui/Window.h"
#include "core/Logging.h"

namespace Matrix
{
	namespace gui
	{
        class GuiManager {

        public:
            GuiManager(Matrix::ui::Window& window);

            // Initialize ImGui and perform any necessary setup.
            void InitSystem();

            // Shutdown and clean up ImGui resources.
            void Shutdown();

            // Start a new ImGui frame.
            void BeginFrame();

            // Render the ImGui UI.
            void RenderGuiFrame();

        private:
            Matrix::ui::Window* m_Window;
            uint32_t counter = 0;
        };
	}
}






