#include "pch.h"
#include "window.h"


namespace Matrix {

	namespace ui
	{
		Window::Window(std::string_view title, int width, int height) :
			m_Title(title.data()),
			m_Width(width),
			m_Height(height)
		{
			if (!glfwInit())
				return;
			m_Window = glfwCreateWindow(width, height, title.data(), NULL, NULL);
			if (!m_Window)
			{
				std::cerr << " Window creation ERROR";
				glfwTerminate();
				return;
			}

			InitGL();
			
			
		}
		void Window::InitGL()
		{
			glfwMakeContextCurrent(m_Window);
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

		Window::~Window()
		{

		}

		void Window::Clear()
		{
			glClearStencil(0);  //sets the clear value for the stencil buffer to zero.
			glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		void Window::Update()
		{
			glfwSwapBuffers(m_Window);
		}


	}


}