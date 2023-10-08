#include "window.h"

namespace matrix {

	namespace graphics
	{
		Window::Window(std::string_view title, int width, int height) :
			m_Title(title.data()),
			m_Width(width),
			m_Height(height)
		{
			if (!glfwInit())
				return;
			window = glfwCreateWindow(width, height, title.data(), NULL, NULL);
			if (!window)
			{
				std::cerr << " Window creation ERROR";
				glfwTerminate();
				return;
			}

			InitGL();

		}
		void Window::Update()
		{
		}
		int Window::GetWidth()
		{
			return 0;
		}
		int Window::GetHeight()
		{
			return 0;
		}
		void Window::InitGL()
		{
			glfwMakeContextCurrent(window);
			if (glewInit() != GLEW_OK)
			{
				std::cerr << " Error initializing glew";
			}

			std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);
		}
	}


}