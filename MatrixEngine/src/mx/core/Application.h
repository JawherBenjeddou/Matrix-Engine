#pragma once

#include "Common.h"
#include "ui/Window.h"

namespace Matrix {

	namespace core {
	
		class MATRIX_API Application
		{

		public:

			Application(Matrix::ui::WindowSpec config = Matrix::ui::WindowSpec());

		private:

			void Run();

		private:
			std::unique_ptr<Matrix::ui::Window> m_Window;
			Matrix::ui::Input m_Input;
		};

		//Needs to be called on the Client side
		Application* CreateApplication();
	
	}
}