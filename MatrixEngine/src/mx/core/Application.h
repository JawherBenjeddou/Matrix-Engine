#pragma once

#include "Common.h"
#include "graphics/Window.h"

namespace Matrix {

	namespace core {
	
		class MATRIX_API Application
		{
		public:
				void Run();
		};

		//Needs to be called on the Client side
		Application* CreateApplication();
	}
}