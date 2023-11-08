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
				m_Timer.StartFrame();
				m_Engine.OnUpdate(m_Timer.DeltaTime());
				m_Timer.EndFrame();
			}

			m_Engine.Shutdown();
		}

	}
}