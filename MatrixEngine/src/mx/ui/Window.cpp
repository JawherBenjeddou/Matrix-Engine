#include "pch.h"
#include "window.h"


namespace Matrix {

	namespace ui
	{
		Window::Window(WindowSpec config) :
			m_Title(config.m_Title.data()),
			m_Width(config.m_Width),
			m_Height(config.m_Height)
		{
			if (!glfwInit())
				return;
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
			if (!m_Window)
			{
				std::cerr << " Window creation ERROR";
				glfwTerminate();
				return;
			}

			glfwSetKeyCallback(m_Window,Window::keyCallback);
			
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
			glfwDestroyWindow(m_Window);
			glfwTerminate();
		}

		void Window::Clear(glm::vec4 clear_color) const 
		{
			glClearStencil(0);  //sets the clear value for the stencil buffer to zero.
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		void Window::OnUpdate() const
		{
			glfwPollEvents();
			glViewport(0, 0, m_Width, m_Height);
			glfwSwapBuffers(m_Window);
		}

		bool Window::Closed() const
		{
			return glfwWindowShouldClose(m_Window);
		}


		void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Input::SetKeyState(key, action);
		}

	}


}