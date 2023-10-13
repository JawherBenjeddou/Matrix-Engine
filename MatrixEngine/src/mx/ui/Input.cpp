
#include "pch.h"
#include "Input.h"
namespace Matrix
{
	namespace ui
	{

		Input::Input()
		{
			m_Scrollx = 0.0;
			m_Scrolly = 0.0;
			m_Lastx = 0.0;
			m_Lasty = 0.0;

			for (size_t count = 0; count < MAX_KEYS; count++) {
				m_KeyState[count] = GLFW_RELEASE;
			}
			for (size_t mcount = 0; mcount < MAX_BUTTONS; mcount++) {
				m_MouseButtonPressed[mcount] = GLFW_RELEASE;
			}
		}

		void Input::CursorFocus(int32_t entered)
		{
			m_CursorCheck = entered;
		}

		void Input::SetKeyState(int32_t key, int32_t action)
		{
			m_KeyState[key] = action;
		}

		void Input::SetMouseState(int32_t button, int32_t action)
		{
			m_MouseButtonPressed[button] = action;
		}
		void Input::SetCursorPosition(const glm::vec2& position)
		{
			m_MousePos = position;
		}

		bool Input::IsPressed(const Key& keycode) const
		{
			if (static_cast<int>(keycode) > MAX_KEYS)
			{
				return false;
			}
			return m_KeyState[static_cast<int>(keycode)];
		}

		bool Input::IsPressed(const Mouse& button) const
		{
			if (static_cast<int>(button) > MAX_KEYS)
			{
				return false;
			}
			return m_MouseButtonPressed[static_cast<int>(button)];
		}

		bool Input::IsCursorInViewport() const
		{
			return m_CursorCheck;
		}

		glm::vec2 Input::GetMousePosition() const
		{
			return m_MousePos;
		}
	

	}
}

