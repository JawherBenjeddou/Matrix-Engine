#pragma once

#include "Common.h"
#include "ui/Window.h"

namespace Matrix {

	namespace core {
	
		class MATRIX_API Application
		{

		public:

			Application();

		private:

			void Run();

		};

		//Needs to be called on the Client side
		Application* CreateApplication();
	
	}
}