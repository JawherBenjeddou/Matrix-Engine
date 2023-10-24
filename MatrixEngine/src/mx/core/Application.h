#pragma once
#include "Common.h"
#include "EngineCore.h"

namespace Matrix {

	namespace core {
		
		//application class for the Matrix framework.
		class MATRIX_API Application
		{
		public:

			Application();

		private:

			//Main execution loop for the application.
			void Run();
		
		private:
			Matrix::core::EngineCore m_Engine;
		};

		/**
		* @brief Factory function for creating an Application object.
		* @return A pointer to the created Application instance.
		*/
		Application* CreateApplication();
	
	}
}