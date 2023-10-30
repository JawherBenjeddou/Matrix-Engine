#pragma once
/**
    @file GuiManager.h
*/

#include "UI/Window.h"   
#include "Core/logging.h" 

namespace Matrix
{
    namespace gui
    {
        /// A manager responsible for controlling the core and all inits of ImGui.
        class MATRIX_API GuiManager
        {
        public:

          
            GuiManager(Matrix::ui::Window& window);

            ///Initialize the GUI system, including ImGui.
            void InitSystem();

            ///Clean up and release resources used by the GUI.
            void Shutdown();

            ///Renders all the engine gui (gets called every frame).
            void OnRenderGui();

            ///Draws all the custom gui and can be overriden by sub classes.
            virtual void DrawGui();

        private:

            ///Configures the ImGui style with custom colors.
            void SetupImGuiStyleColors();

            /// Begin a new frame for the GUI.
            void BeginFrame();

            /// Render the ImGui-based user interface.
            void RenderGuiFrame();

        private:
            float m_FontSize;
            uint32_t counter = 0;
            Matrix::ui::Window* m_Window;
        };

    }
}
