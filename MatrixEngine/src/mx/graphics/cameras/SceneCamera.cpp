#include "pch.h"
#include "SceneCamera.h"

namespace Matrix::graphics
{
    SceneCamera::SceneCamera()
    {
        RecalculateProjection();
    }

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_OrthographicSize = std::max(size, 0.01f);
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;
        RecalculateProjection();
    }

    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;
        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
        RecalculateProjection();
    }

    void SceneCamera::RecalculateProjection()
    {
        float halfHeight = m_OrthographicSize * 0.5f;
        float halfWidth = halfHeight * m_AspectRatio;
        m_Projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_OrthographicNear, m_OrthographicFar);
    }
}
