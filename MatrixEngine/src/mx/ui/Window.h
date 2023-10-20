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

            void InitSystem();

            void OnUpdate() const;

            bool Closed() const;
            
            void GetInfo();

            inline uint32_t GetWidth() const
            {
                return m_Width;
            }

            inline uint32_t GetHeight() const
            {
                return m_Height;
            }


            GLFWwindow* GetWindow() 
            {
                return m_Window;
            }
         
         private:

            void SetWindowIcon();
            
            void Clear(glm::vec4 clear_color = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)) const;
            
            void InitGL();

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
           //bool m_Closed;
            GLFWwindow* m_Window = nullptr;
		};

	}

}