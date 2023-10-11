#pragma once

#include <iostream>

#include "math/Math.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h" 
#include "Common.h"
#include "Input.h"

namespace Matrix {
	
    namespace ui
	{
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
            Window(WindowSpec config);

            ~Window();

            //Default clear color is 1.0f, 1.0f, 0.0f, 1.0f
            void Clear(glm::vec4 clear_color = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)) const;

            void OnUpdate() const;

            bool Closed() const;


            uint32_t GetWidth() const
            {
                return m_Width;
            }

            uint32_t GetHeight() const
            {
                return m_Height;
            }


            GLFWwindow* GetWindow() 
            {
                return m_Window;
            }

            static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        private:

            void InitGL();

        private:
            const char* m_Title;
            uint32_t m_Width;
            uint32_t m_Height;
           //bool m_Closed;
            GLFWwindow* m_Window = nullptr;
		};

	}

}