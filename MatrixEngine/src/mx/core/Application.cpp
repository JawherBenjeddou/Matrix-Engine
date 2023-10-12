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
				if (Matrix::ui::Input::IsKeyPressed(Key::D));
				{
					std::cout << "D is just got pressed ! \n" ;
				}
				m_Window->OnUpdate();
			}
		}


	}
}