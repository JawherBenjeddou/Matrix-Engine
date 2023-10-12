#pragma once

#include "Common.h"
#include "math/Math.h"
#include "GLFW/glfw3.h"
enum class Key
{
	UNKNOWN = -1,
	SPACE = 32,
	APOSTROPHE = 39,
	COMMA = 44,
	MINUS = 45,
	PERIOD = 46,
	SLASH = 47,
	ZERO = 48,
	ONE = 49,
	TWO = 50,
	THREE = 51,
	FOUR = 52,
	FIVE = 53,
	SIX = 54,
	SEVEN = 55,
	EIGHT = 56,
	NINE = 57,
	SEMICOLON = 59,
	EQUAL = 61,
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	LEFT_BRACKET = 91, /* [ */
	BACKSLASH = 92,  /* \ */
	RIGHT_BRACKET = 93,  /* ] */
	GRAVE_ACCENT = 96,  /* ` */
	WORLD_1 = 161, /* non-US #1 */
	WORLD_2 = 162, /* non-US #2 */

};
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

			static void SetKeyState(int32_t key, int32_t action);

			static bool IsKeyPressed(Key keycode);
		
		private:
			inline static bool m_KeyState[MAX_KEYS];
			
			inline static bool m_MouseButtonPressed[MAX_BUTTONS];
			inline static double m_Mousex;
			inline static double m_Mousey;
			inline static double m_Lastx;
			inline static double m_Lasty;
			double m_Scrollx, m_Scrolly;

			bool m_IsDragging;
		};
		
	
	}
}

