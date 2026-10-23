#include "pch.h"
#include "EditorCamera.h"

#include "ui/Input.h"

namespace Matrix::graphics
{
    EditorCamera::EditorCamera()
        : m_Camera(16.0f / 9.0f, 12.0f)
    {
    }

    void EditorCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;
        m_ViewportSize = { static_cast<float>(width), static_cast<float>(height) };
        m_Camera.SetViewportSize(width, height);
    }

    void EditorCamera::Focus(const glm::vec2& worldPosition)
    {
        m_Camera.SetPosition(glm::vec3(worldPosition, m_Camera.GetPosition().z));
    }

    void EditorCamera::Reset()
    {
        m_Camera.SetPosition(glm::vec3(0.0f));
        m_Camera.SetRotation(0.0f);
        m_Camera.SetZoom(12.0f);
    }

    void EditorCamera::SetPosition(const glm::vec2& position)
    {
        m_Camera.SetPosition(glm::vec3(position, m_Camera.GetPosition().z));
    }

    void EditorCamera::SetZoom(float zoom)
    {
        m_Camera.SetZoom(math::Clamp(zoom, MinZoom, MaxZoom));
    }

    void EditorCamera::Zoom(float factor, const glm::vec2* zoomTowardsWorld)
    {
        glm::vec2 before = zoomTowardsWorld ? *zoomTowardsWorld : GetPosition();
        glm::vec2 offset = before - GetPosition();
        float newZoom = math::Clamp(m_Camera.GetZoom() * factor, MinZoom, MaxZoom);
        float applied = newZoom / m_Camera.GetZoom();
        m_Camera.SetZoom(newZoom);
        // Keep the world point under the cursor fixed.
        if (zoomTowardsWorld)
            SetPosition(before - offset * applied);
    }

    void EditorCamera::OnUpdate(float deltaTime, bool viewportHovered, bool viewportFocused, const glm::vec2& mouseInViewport)
    {
        using ui::Input;

        // Scroll to zoom towards the cursor.
        if (viewportHovered)
        {
            float scroll = Input::GetScrollDelta().y;
            if (scroll != 0.0f)
            {
                glm::vec2 worldUnderCursor = ScreenToWorld(mouseInViewport);
                Zoom(scroll > 0.0f ? 0.9f : 1.1f, &worldUnderCursor);
            }
        }

        // Pan with middle mouse, right mouse, or Alt + left mouse.
        bool panButton = Input::IsMouseButtonDown(Mouse::BUTTON_MIDDLE)
                      || Input::IsMouseButtonDown(Mouse::BUTTON_RIGHT)
                      || (Input::IsAltDown() && Input::IsMouseButtonDown(Mouse::BUTTON_LEFT));
        if (panButton && (viewportHovered || m_Panning))
        {
            if (!m_Panning)
            {
                m_Panning = true;
                m_LastMouse = mouseInViewport;
            }
            glm::vec2 delta = mouseInViewport - m_LastMouse;
            m_LastMouse = mouseInViewport;
            float unitsPerPixel = m_Camera.GetZoom() / m_ViewportSize.y;
            glm::vec2 move = glm::vec2(-delta.x, delta.y) * unitsPerPixel * PanSpeed;
            SetPosition(GetPosition() + move);
        }
        else
        {
            m_Panning = false;
        }

        // Keyboard panning while the viewport has focus.
        if (viewportFocused && !Input::IsControlDown())
        {
            glm::vec2 direction(0.0f);
            if (Input::IsKeyDown(Key::W) || Input::IsKeyDown(Key::UP))    direction.y += 1.0f;
            if (Input::IsKeyDown(Key::S) || Input::IsKeyDown(Key::DOWN))  direction.y -= 1.0f;
            if (Input::IsKeyDown(Key::A) || Input::IsKeyDown(Key::LEFT))  direction.x -= 1.0f;
            if (Input::IsKeyDown(Key::D) || Input::IsKeyDown(Key::RIGHT)) direction.x += 1.0f;
            if (direction.x != 0.0f || direction.y != 0.0f)
            {
                float speed = m_Camera.GetZoom() * KeyboardSpeed * (Input::IsShiftDown() ? 2.0f : 1.0f);
                SetPosition(GetPosition() + glm::normalize(direction) * speed * deltaTime);
            }
            if (Input::IsKeyPressed(Key::KP_ADD)) Zoom(0.8f);
            if (Input::IsKeyPressed(Key::KP_SUBTRACT)) Zoom(1.25f);
        }
    }
}
