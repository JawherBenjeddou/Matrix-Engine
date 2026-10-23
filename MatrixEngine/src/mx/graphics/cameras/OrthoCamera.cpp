#include "pch.h"
#include "OrthoCamera.h"

namespace Matrix::graphics
{
    OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
    {
        SetProjection(left, right, bottom, top);
        RecalculateView();
    }

    OrthoCamera::OrthoCamera(float aspectRatio, float zoom)
        : m_Zoom(std::max(zoom, 0.01f)), m_AspectRatio(std::max(aspectRatio, 0.01f))
    {
        RecalculateProjection();
        RecalculateView();
    }

    void OrthoCamera::SetProjection(float left, float right, float bottom, float top)
    {
        m_Bounds = { left, right, bottom, top };
        m_Zoom = top - bottom;
        m_AspectRatio = (top - bottom) != 0.0f ? (right - left) / (top - bottom) : 1.0f;
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -100.0f, 100.0f);
    }

    void OrthoCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;
        SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    }

    void OrthoCamera::SetAspectRatio(float aspectRatio)
    {
        m_AspectRatio = std::max(aspectRatio, 0.01f);
        RecalculateProjection();
    }

    void OrthoCamera::SetZoom(float zoomLevel)
    {
        m_Zoom = std::max(zoomLevel, 0.01f);
        RecalculateProjection();
    }

    void OrthoCamera::RecalculateProjection()
    {
        float halfHeight = m_Zoom * 0.5f;
        float halfWidth = halfHeight * m_AspectRatio;
        m_Bounds = { -halfWidth, halfWidth, -halfHeight, halfHeight };
        m_ProjectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -100.0f, 100.0f);
    }

    void OrthoCamera::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        RecalculateView();
    }

    void OrthoCamera::SetRotation(float angleDegrees)
    {
        m_Rotation = angleDegrees;
        RecalculateView();
    }

    void OrthoCamera::RecalculateView()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)
                            * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        m_ViewMatrix = glm::inverse(transform);
    }

    glm::vec2 OrthoCamera::ScreenToWorld(const glm::vec2& screenPosition, const glm::vec2& viewportSize) const
    {
        if (viewportSize.x <= 0.0f || viewportSize.y <= 0.0f)
            return glm::vec2(m_Position);
        // Normalized device coordinates (y up).
        float ndcX = (screenPosition.x / viewportSize.x) * 2.0f - 1.0f;
        float ndcY = 1.0f - (screenPosition.y / viewportSize.y) * 2.0f;
        glm::vec4 world = glm::inverse(GetViewProjectionMatrix()) * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
        return glm::vec2(world.x, world.y);
    }

    glm::vec2 OrthoCamera::WorldToScreen(const glm::vec2& worldPosition, const glm::vec2& viewportSize) const
    {
        glm::vec4 clip = GetViewProjectionMatrix() * glm::vec4(worldPosition, 0.0f, 1.0f);
        float x = (clip.x * 0.5f + 0.5f) * viewportSize.x;
        float y = (1.0f - (clip.y * 0.5f + 0.5f)) * viewportSize.y;
        return { x, y };
    }
}
