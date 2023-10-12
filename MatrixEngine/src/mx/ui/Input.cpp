
#include "pch.h"
#include "Input.h"
namespace Matrix
{
	namespace ui
	{
		Input::Input()
		{
			this->m_Mousex = 0.0;
			this->m_Mousey = 0.0;
			this->m_Scrollx = 0.0;
			this->m_Scrolly = 0.0;
			this->m_Lastx = 0.0;
			this->m_Lasty = 0.0;

			for (size_t count = 0; count < MAX_KEYS; count++) {
				m_KeyState[count] = GLFW_RELEASE;
			}
			for (size_t mcount = 0; mcount < MAX_BUTTONS; mcount++) {
				m_MouseButtonPressed[mcount] = GLFW_RELEASE;
			}
		}

		void Input::SetKeyState(int32_t key, int32_t action)
		{
			m_KeyState[key] = action;
		}

		bool Input::IsKeyPressed(Key keycode)
		{
			if (static_cast<int>(keycode) > MAX_KEYS)
			{
				return false;
			}
			return m_KeyState[static_cast<int>(keycode)] == GLFW_PRESS;
		}
	
	}
}

