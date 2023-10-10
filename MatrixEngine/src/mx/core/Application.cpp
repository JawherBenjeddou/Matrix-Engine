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
				m_Window->Clear({1.0f,0.5f,0.0f,1.0f});
				m_Window->OnUpdate();
			}
		}


	}
}