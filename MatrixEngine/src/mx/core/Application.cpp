#include "pch.h"
#include "Application.h"


namespace Matrix {
	namespace core
	{

		Application::Application()	
		{
			Run();
		}		
		
		void Application::Run()
		{
			m_Engine.Initialize();
			while (!m_Engine.IsWindowClosed())
			{

				m_Engine.OnUpdate();
			}

			m_Engine.Shutdown();
		}

	}
}