#pragma once

#include "Common.h"
#include "math/Math.h"
#include "GLFW/glfw3.h"
enum class MATRIX_API Key
{
	UNKNOWN = GLFW_KEY_UNKNOWN,
	SPACE = GLFW_KEY_SPACE,
	APOSTROPHE = GLFW_KEY_APOSTROPHE,
	COMMA = GLFW_KEY_COMMA,
	MINUS = GLFW_KEY_MINUS,
	PERIOD = GLFW_KEY_PERIOD,
	SLASH = GLFW_KEY_SLASH,
	SEMICOLON = GLFW_KEY_SEMICOLON,
	EQUAL = GLFW_KEY_EQUAL,
	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,
	LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET, /* [ */
	BACKSLASH = GLFW_KEY_BACKSLASH,  /* \ */
	RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,  /* ] */
	GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,  /* ` */
	ZERO = GLFW_KEY_0,
	ONE = GLFW_KEY_1,
	TWO = GLFW_KEY_2,
	THREE = GLFW_KEY_3,
	FOUR = GLFW_KEY_4,
	FIVE = GLFW_KEY_5,
	SIX = GLFW_KEY_6,
	SEVEN = GLFW_KEY_7,
	EIGHT = GLFW_KEY_8,
	NINE = GLFW_KEY_9
};

enum class MATRIX_API Mouse
{
	UNKNOWN = GLFW_KEY_UNKNOWN,
	BUTTON_1 = GLFW_MOUSE_BUTTON_1,
	BUTTON_2 = GLFW_MOUSE_BUTTON_2,
	BUTTON_3 = GLFW_MOUSE_BUTTON_3,
	BUTTON_4 = GLFW_MOUSE_BUTTON_4,
	BUTTON_5 = GLFW_MOUSE_BUTTON_5,
	BUTTON_6 = GLFW_MOUSE_BUTTON_6,
	BUTTON_7 = GLFW_MOUSE_BUTTON_7,
	BUTTON_8 = GLFW_MOUSE_BUTTON_8,
	BUTTON_LAST = GLFW_MOUSE_BUTTON_LAST,
	BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT,
	BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
	BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE
};
namespace Matrix
{
	namespace ui
	{
		//constexpr instead of macro so doesn't leak to user - code
		constexpr uint16_t MAX_BUTTONS = 24;
		constexpr uint16_t MAX_KEYS = 1024;

		class MATRIX_API Input
		{
		public:
			Input();
		public:
			bool IsPressed(const Key& keycode) const;

			bool IsPressed(const Mouse& button) const;

			bool IsCursorInViewport() const;
			
			glm::vec2 GetMousePosition() const;

		private:
			static void SetKeyState(int32_t key, int32_t action);

			static void SetMouseState(int32_t button, int32_t action);
	
			static void SetCursorPosition(const glm::vec2& position);

			static void CursorFocus(int32_t entered);

		private:
			friend class Window;

			inline static bool m_KeyState[MAX_KEYS]{ 0 };
			inline static bool m_MouseButtonPressed[MAX_BUTTONS]{ 0 };
			
			inline static glm::vec2 m_MousePos;
			inline static double m_Lastx;
			inline static double m_Lasty;
			inline static double m_Scrollx;
			inline static double m_Scrolly;
			inline static bool m_IsDragging;
			inline static int m_CursorCheck;
		};
		
	
	}
}

