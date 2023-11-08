#pragma once
///@file Application.h

#include "Common.h"
#include "util/Timer.h"

#include "EngineCore.h"

namespace Matrix {

	namespace core {

	

		///application class for the Matrix framework.
		class MATRIX_DLL Application
		{
		public:

			Application();

		private:

			///Main execution loop for the application.
			void Run();
		
		private:
			EngineCore m_Engine;
			Matrix::util::Timer m_Timer;
		};

		/**
		* @brief Factory function for creating an Application object.
		* @return A pointer to the created Application instance.
		*/
		Application* CreateApplication();
	
	}
}

/**
 * @brief Macro to create the CreateApplication function for an application class.
 *
 * Use this macro in a derived application class header to auto-generate
 * the CreateApplication function. It simplifies the process of instantiating
 * the class as an application.
 *
 * @return A new instance of the specified application class.
 */
#define IMPLEMENT_APPLICATION_CLASS(appClass) \
    Matrix::core::Application* Matrix::core::CreateApplication() \
    { \
        return new appClass(); \
    }