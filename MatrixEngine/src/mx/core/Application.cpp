#include "Application.h"

namespace Matrix {
	namespace core
	{
		void Application::Run()
		{
			Matrix::graphics::Window window("Matrix", 900, 600);
			while (!glfwWindowShouldClose(window.GetWindow()))
			{
				glfwPollEvents();
			}
		}
	}
}