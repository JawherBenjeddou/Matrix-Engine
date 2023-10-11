#pragma once

#include "Common.h"
#include "math/Math.h"
#include "GLFW/glfw3.h"

namespace Matrix
{
	namespace ui
	{
		
		#define MAX_BUTTONS 24
		#define MAX_KEYS 1024

		class MATRIX_API Input
		{
		public:
			Input();
			 
			~Input() {}

		
			static bool IsKeyPressed(uint32_t keycode);

			static void SetKeyState(int32_t key, int32_t action);
		
		private:
			static bool m_KeyState[MAX_KEYS];
			static bool m_MouseButtonPressed[MAX_BUTTONS];
			double m_Scrollx, m_Scrolly;
			static double m_Mousex;
			static double m_Mousey;
			static double m_Lastx;
			static double m_Lasty;
			bool m_IsDragging;
		};

	}
}
