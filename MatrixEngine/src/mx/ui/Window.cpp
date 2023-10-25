#include "pch.h"
#include "window.h"
#include "graphics/DebugContext.h"

namespace Matrix {

	namespace ui
	{
		Window::Window(WindowSpec config) :
			m_Title(config.m_Title.data()),
			m_Width(config.m_Width),
			m_Height(config.m_Height)
		{

		}
		void Window::InitSystem()
		{
			if (!glfwInit())
				return;
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#ifdef  MX_DEBUG_MODE
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif //  MX_DEBUG_MODE
			m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
			if (!m_Window)
			{
				MX_CORE_CRITICAL(" Window creation ERROR");
				glfwTerminate();
				return;
			}

			glfwSetKeyCallback(m_Window, Window::KeyCallBack);

			glfwSetMouseButtonCallback(m_Window, Window::MouseCallBack);

			glfwSetCursorPosCallback(m_Window, Window::CursorPositionCallBack);

			glfwSetCursorEnterCallback(m_Window, Window::CursorEnterCallBack);

			InitGL();

		}

		void Window::GetInfo()
		{
			std::cout << "Graphics Card : " << glGetString(GL_RENDERER) << "\n";

			if (glfwExtensionSupported("GL_NVX_gpu_memory_info")) 
			{
				int32_t totalAvailableMemoryMB;
				glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalAvailableMemoryMB);

				// Print the total available VRAM
				std::cout << "Total Available VRAM: " << totalAvailableMemoryMB / 1000 << " GB" << "\n";
			}
			else 
			{
				std::cout << "GL_NVX_gpu_memory_info extension not supported.Cannot query VRAM." << "\n";
			}

			std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
		}

		Window::~Window()
		{
			glfwDestroyWindow(m_Window);
			glfwTerminate();
		}

		void Window::OnUpdate() const
		{
			glfwPollEvents();
			glViewport(0, 0, m_Width, m_Height);
			glfwSwapBuffers(m_Window);
			Clear();
		}

		bool Window::Closed() const
		{
			return glfwWindowShouldClose(m_Window);
		}

		void Window::SetWindowIcon(const char* path)
		{
			
			unsigned char* data = stbi_load(path, &m_IconWidth, &m_IconHeight, &m_IconChannels, STBI_rgb_alpha);
			if (data)
			{
				GLFWimage icon;


				icon.pixels = data;
				icon.width = m_IconWidth;
				icon.height = m_IconHeight;

				// Assuming you have a GLFWwindow* named 'm_Window', you can set the icon like this:
				glfwSetWindowIcon(m_Window, 1, &icon);

				stbi_image_free(data);
			}
			else
			{
				MX_CORE_ERROR("Icon failed to load at path: {0}", path);
				stbi_image_free(data);
			}
		}

		void Window::Clear(glm::vec4 clear_color) const
		{
			glClearStencil(0);  //sets the clear value for the stencil buffer to zero.
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		void Window::InitGL()
		{
			glfwMakeContextCurrent(m_Window);
			if (glewInit() != GLEW_OK)
			{
				MX_CORE_CRITICAL(" Error initializing glew");
			}
			
			//enables the opengl debug context
			Matrix::graphics::EnableDebugContext();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);

		}

		void Window::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Input::SetKeyState(key, action);
		}

		void Window::MouseCallBack(GLFWwindow* window, int button, int action, int mods)
		{
			Input::SetMouseState(button, action);
		}

		void Window::CursorPositionCallBack(GLFWwindow* window, double xpos, double ypos)
		{
			Input::SetCursorPosition(glm::vec2(xpos,ypos));
		}

		void Window::CursorEnterCallBack(GLFWwindow* window, int entered)
		{
			Input::CursorFocus(entered);
		}
	

	}


}

