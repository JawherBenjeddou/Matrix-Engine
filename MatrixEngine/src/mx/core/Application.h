#pragma once
#include "Common.h"
#include "EngineCore.h"

namespace Matrix {

	namespace core {
	
		class MATRIX_API Application
		{
		public:

			Application();

		private:

			void Run();
		
		private:
			Matrix::core::EngineCore m_Engine;
		};

		Application* CreateApplication();
	
	}
}