#include "pch.h"
#include "Input.h"
#include "core/Logging.h"

namespace Matrix::ui
{
    namespace
    {
        bool ValidKey(int32_t key) { return key >= 0 && key < MAX_KEYS; }
        bool ValidButton(int32_t button) { return button >= 0 && button < MAX_BUTTONS; }
    }

    void Input::InitSystem()
    {
        std::fill(std::begin(s_KeyDown), std::end(s_KeyDown), false);
        std::fill(std::begin(s_KeyPressed), std::end(s_KeyPressed), false);
        std::fill(std::begin(s_KeyReleased), std::end(s_KeyReleased), false);
        std::fill(std::begin(s_MouseDown), std::end(s_MouseDown), false);
        std::fill(std::begin(s_MousePressed), std::end(s_MousePressed), false);
        std::fill(std::begin(s_MouseReleased), std::end(s_MouseReleased), false);
        s_MousePos = s_LastMousePos = s_MouseDelta = s_Scroll = glm::vec2(0.0f);
        s_CursorInWindow = false;
        s_FirstMouseSample = true;
        MX_CORE_INFO("Input system initialized");
    }

    void Input::NewFrame()
    {
        std::fill(std::begin(s_KeyPressed), std::end(s_KeyPressed), false);
        std::fill(std::begin(s_KeyReleased), std::end(s_KeyReleased), false);
        std::fill(std::begin(s_MousePressed), std::end(s_MousePressed), false);
        std::fill(std::begin(s_MouseReleased), std::end(s_MouseReleased), false);
        s_Scroll = glm::vec2(0.0f);
        s_MouseDelta = glm::vec2(0.0f);
        s_LastMousePos = s_MousePos;
    }

    bool Input::IsKeyDown(Key key)
    {
        int32_t code = static_cast<int32_t>(key);
        return ValidKey(code) && s_KeyDown[code];
    }

    bool Input::IsKeyPressed(Key key)
    {
        int32_t code = static_cast<int32_t>(key);
        return ValidKey(code) && s_KeyPressed[code];
    }

    bool Input::IsKeyReleased(Key key)
    {
        int32_t code = static_cast<int32_t>(key);
        return ValidKey(code) && s_KeyReleased[code];
    }

    bool Input::IsShiftDown() { return IsKeyDown(Key::LEFT_SHIFT) || IsKeyDown(Key::RIGHT_SHIFT); }
    bool Input::IsControlDown() { return IsKeyDown(Key::LEFT_CONTROL) || IsKeyDown(Key::RIGHT_CONTROL); }
    bool Input::IsAltDown() { return IsKeyDown(Key::LEFT_ALT) || IsKeyDown(Key::RIGHT_ALT); }

    bool Input::IsMouseButtonDown(Mouse button)
    {
        int32_t code = static_cast<int32_t>(button);
        return ValidButton(code) && s_MouseDown[code];
    }

    bool Input::IsMouseButtonPressed(Mouse button)
    {
        int32_t code = static_cast<int32_t>(button);
        return ValidButton(code) && s_MousePressed[code];
    }

    bool Input::IsMouseButtonReleased(Mouse button)
    {
        int32_t code = static_cast<int32_t>(button);
        return ValidButton(code) && s_MouseReleased[code];
    }

    glm::vec2 Input::GetMousePosition() { return s_MousePos; }
    glm::vec2 Input::GetMouseDelta() { return s_MouseDelta; }
    glm::vec2 Input::GetScrollDelta() { return s_Scroll; }
    bool Input::IsCursorInWindow() { return s_CursorInWindow; }

    void Input::SetKeyState(int32_t key, int32_t action)
    {
        if (!ValidKey(key))
            return;
        if (action == GLFW_PRESS)
        {
            s_KeyDown[key] = true;
            s_KeyPressed[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            s_KeyDown[key] = false;
            s_KeyReleased[key] = true;
        }
        // GLFW_REPEAT keeps the key down; nothing to do.
    }

    void Input::SetMouseState(int32_t button, int32_t action)
    {
        if (!ValidButton(button))
            return;
        if (action == GLFW_PRESS)
        {
            s_MouseDown[button] = true;
            s_MousePressed[button] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            s_MouseDown[button] = false;
            s_MouseReleased[button] = true;
        }
    }

    void Input::SetCursorPosition(const glm::vec2& position)
    {
        if (s_FirstMouseSample)
        {
            s_LastMousePos = position;
            s_FirstMouseSample = false;
        }
        s_MousePos = position;
        s_MouseDelta = s_MousePos - s_LastMousePos;
    }

    void Input::AddScroll(const glm::vec2& offset)
    {
        s_Scroll += offset;
    }

    void Input::CursorFocus(int32_t entered)
    {
        s_CursorInWindow = entered != 0;
    }
}
