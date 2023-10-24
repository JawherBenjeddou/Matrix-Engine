#pragma once

#include <iostream>

#include "math/Math.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h" 
#include "Common.h"
#include "Input.h"
#include "core/Logging.h"
#include "stb_image.h"

namespace Matrix {
	
    namespace ui
	{
        // Structure for specifying window properties aka (title,widh,heigh).
        struct MATRIX_API WindowSpec
        {
            std::string_view m_Title;
            uint32_t m_Width;
            uint32_t m_Height;
            WindowSpec(std::string_view title = "Matrix Engine", uint32_t width = 1280, uint32_t height = 720)
                : m_Title(title), m_Width(width), m_Height(height) {};
        };
         
        class MATRIX_API Window
        {
        public:
            Window(WindowSpec config = WindowSpec());

            ~Window();

            // Initialize the window and input system.
            void InitSystem();

            // Update the window and handle input events.
            void OnUpdate() const;

            /**
             * @brief Check if window is closed.
             * @return True if window is closed, false otherwise.
             */
            bool Closed() const;

            // Get window information.
            void GetInfo();

            //Get the width of the window.
            uint32_t GetWidth() const { return m_Width; }

            // Get the height of the window.
            uint32_t GetHeight() const { return m_Height; }

            // Get a pointer to the GLFW window.
            auto* GetWindow()
            {
                return m_Window;
            }

        private:
            // Set the window icon (temporary, should be set by the user).
            void SetWindowIcon(const char* path = "../resources/branding/iconpsd256.png");

            // Clear the window with a specified clear color.
            void Clear(glm::vec4 clear_color = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)) const;

            // Initialize OpenGL.
            void InitGL();

            // Static callback functions for handling input events.
            static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);

            static void MouseCallBack(GLFWwindow* window, int button, int action, int mods);

            static void CursorPositionCallBack(GLFWwindow* window, double xpos, double ypos);

            static void CursorEnterCallBack(GLFWwindow* window, int entered);

        private:
            int m_IconWidth;
            int m_IconHeight;
            int m_IconChannels;
            const char* m_Title;
            uint32_t m_Width;
            uint32_t m_Height;
            GLFWwindow* m_Window = nullptr;
        };

	}

}