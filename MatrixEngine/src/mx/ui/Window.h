#pragma once

#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h" 
#include "Common.h"


namespace Matrix {
	
    namespace ui
	{

		 class MATRIX_API Window
		{

        public:
            Window(std::string_view title, int width, int height);

            ~Window();

            void Clear();

            void Update();

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