#pragma once
#include "Common.h"
///@file Input.h  Keyboard and mouse state polled from GLFW callbacks.

#include "GLFW/glfw3.h"

/// Keyboard key codes (values match GLFW).
enum class MATRIX_DLL Key : int32_t
{
    UNKNOWN = GLFW_KEY_UNKNOWN,
    SPACE = GLFW_KEY_SPACE,
    APOSTROPHE = GLFW_KEY_APOSTROPHE,
    COMMA = GLFW_KEY_COMMA,
    MINUS = GLFW_KEY_MINUS,
    PERIOD = GLFW_KEY_PERIOD,
    SLASH = GLFW_KEY_SLASH,
    ZERO = GLFW_KEY_0, ONE = GLFW_KEY_1, TWO = GLFW_KEY_2, THREE = GLFW_KEY_3, FOUR = GLFW_KEY_4,
    FIVE = GLFW_KEY_5, SIX = GLFW_KEY_6, SEVEN = GLFW_KEY_7, EIGHT = GLFW_KEY_8, NINE = GLFW_KEY_9,
    SEMICOLON = GLFW_KEY_SEMICOLON,
    EQUAL = GLFW_KEY_EQUAL,
    A = GLFW_KEY_A, B = GLFW_KEY_B, C = GLFW_KEY_C, D = GLFW_KEY_D, E = GLFW_KEY_E, F = GLFW_KEY_F,
    G = GLFW_KEY_G, H = GLFW_KEY_H, I = GLFW_KEY_I, J = GLFW_KEY_J, K = GLFW_KEY_K, L = GLFW_KEY_L,
    M = GLFW_KEY_M, N = GLFW_KEY_N, O = GLFW_KEY_O, P = GLFW_KEY_P, Q = GLFW_KEY_Q, R = GLFW_KEY_R,
    S = GLFW_KEY_S, T = GLFW_KEY_T, U = GLFW_KEY_U, V = GLFW_KEY_V, W = GLFW_KEY_W, X = GLFW_KEY_X,
    Y = GLFW_KEY_Y, Z = GLFW_KEY_Z,
    LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,
    BACKSLASH = GLFW_KEY_BACKSLASH,
    RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
    GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,
    ESCAPE = GLFW_KEY_ESCAPE,
    ENTER = GLFW_KEY_ENTER,
    TAB = GLFW_KEY_TAB,
    BACKSPACE = GLFW_KEY_BACKSPACE,
    INSERT = GLFW_KEY_INSERT,
    DEL = GLFW_KEY_DELETE,
    RIGHT = GLFW_KEY_RIGHT,
    LEFT = GLFW_KEY_LEFT,
    DOWN = GLFW_KEY_DOWN,
    UP = GLFW_KEY_UP,
    PAGE_UP = GLFW_KEY_PAGE_UP,
    PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
    HOME = GLFW_KEY_HOME,
    END = GLFW_KEY_END,
    CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
    F1 = GLFW_KEY_F1, F2 = GLFW_KEY_F2, F3 = GLFW_KEY_F3, F4 = GLFW_KEY_F4, F5 = GLFW_KEY_F5, F6 = GLFW_KEY_F6,
    F7 = GLFW_KEY_F7, F8 = GLFW_KEY_F8, F9 = GLFW_KEY_F9, F10 = GLFW_KEY_F10, F11 = GLFW_KEY_F11, F12 = GLFW_KEY_F12,
    KP_0 = GLFW_KEY_KP_0, KP_1 = GLFW_KEY_KP_1, KP_2 = GLFW_KEY_KP_2, KP_3 = GLFW_KEY_KP_3, KP_4 = GLFW_KEY_KP_4,
    KP_5 = GLFW_KEY_KP_5, KP_6 = GLFW_KEY_KP_6, KP_7 = GLFW_KEY_KP_7, KP_8 = GLFW_KEY_KP_8, KP_9 = GLFW_KEY_KP_9,
    KP_ADD = GLFW_KEY_KP_ADD,
    KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT,
    LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
    LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
    LEFT_ALT = GLFW_KEY_LEFT_ALT,
    LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
    RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
    RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
    RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
    RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
};

/// Mouse button codes (values match GLFW).
enum class MATRIX_DLL Mouse : int32_t
{
    UNKNOWN = -1,
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

namespace Matrix::ui
{
    constexpr uint16_t MAX_BUTTONS = 16;
    constexpr uint16_t MAX_KEYS = 512;

    /// Static input state. Updated by the Window; readable from anywhere.
    class MATRIX_DLL Input
    {
    public:
        /// Resets all state. Called once by the engine on start-up.
        static void InitSystem();

        /// Clears per-frame transient state (pressed/released edges, scroll, mouse delta).
        /// The Window calls this right before polling OS events.
        static void NewFrame();

        // --- Keyboard ---------------------------------------------------------------------
        /// True while the key is held down.
        static bool IsKeyDown(Key key);
        /// True only during the frame the key went down.
        static bool IsKeyPressed(Key key);
        /// True only during the frame the key was released.
        static bool IsKeyReleased(Key key);
        /// True if either shift / control / alt key is down.
        static bool IsShiftDown();
        static bool IsControlDown();
        static bool IsAltDown();

        // --- Mouse ------------------------------------------------------------------------
        static bool IsMouseButtonDown(Mouse button);
        static bool IsMouseButtonPressed(Mouse button);
        static bool IsMouseButtonReleased(Mouse button);

        /// Cursor position in window pixel coordinates (origin top-left).
        static glm::vec2 GetMousePosition();
        /// Cursor movement since the previous frame in pixels.
        static glm::vec2 GetMouseDelta();
        /// Scroll wheel movement accumulated during the current frame.
        static glm::vec2 GetScrollDelta();
        /// True while the cursor is inside the window client area.
        static bool IsCursorInWindow();

        // --- Backwards compatible instance API --------------------------------------------
        bool IsPressed(const Key& keycode) const { return IsKeyDown(keycode); }
        bool IsPressed(const Mouse& button) const { return IsMouseButtonDown(button); }
        bool IsCursorInViewport() const { return IsCursorInWindow(); }

        // --- Fed by the Window callbacks ---------------------------------------------------
        static void SetKeyState(int32_t key, int32_t action);
        static void SetMouseState(int32_t button, int32_t action);
        static void SetCursorPosition(const glm::vec2& position);
        static void AddScroll(const glm::vec2& offset);
        static void CursorFocus(int32_t entered);

    private:
        inline static bool s_KeyDown[MAX_KEYS]{};
        inline static bool s_KeyPressed[MAX_KEYS]{};
        inline static bool s_KeyReleased[MAX_KEYS]{};
        inline static bool s_MouseDown[MAX_BUTTONS]{};
        inline static bool s_MousePressed[MAX_BUTTONS]{};
        inline static bool s_MouseReleased[MAX_BUTTONS]{};
        inline static glm::vec2 s_MousePos{ 0.0f };
        inline static glm::vec2 s_LastMousePos{ 0.0f };
        inline static glm::vec2 s_MouseDelta{ 0.0f };
        inline static glm::vec2 s_Scroll{ 0.0f };
        inline static bool s_CursorInWindow = false;
        inline static bool s_FirstMouseSample = true;
    };
}
