#include "pch.h"
#include "Application.h"
namespace Matrix {
	namespace core
	{


		Application::Application(Matrix::ui::WindowSpec config)
			
		{
			m_Window = std::make_unique<Matrix::ui::Window>(config);
			Run();
		}
		
		
		void Application::Run()
		{
			while (!m_Window->Closed())
			{
				m_Window->Clear();
				if (Matrix::ui::Input::IsPressed(Mouse::BUTTON_MIDDLE))
				{
					std::cout << "A pressed" << std::endl;
				}
				m_Window->OnUpdate();
			}
		}


	}
}