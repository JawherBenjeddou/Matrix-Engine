#pragma once

#include <iostream>

#include "GLFW/glfw3.h" 
#include "GL/glew.h"


namespace matrix {
	
    namespace graphics
	{

		class Window
		{

        public:
            Window(std::string_view title, int width, int height);

            ~Window();

            void Clear(glm::vec4& color);

            void Update();

            int GetWidth();

            int GetHeight();

            GLFWwindow* GetWindow();

        private:
            void InitGL();
        private:
            const char* m_title;
            uint32_t m_width;
            uint32_t m_height;
            GLFWwindow* m_window = nullptr;
		};

	}

}