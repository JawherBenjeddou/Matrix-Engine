#pragma once
#include "Common.h"
///@file SceneCamera.h  Camera stored inside a CameraComponent (orthographic, viewport aware).

namespace Matrix::graphics
{
    class MATRIX_DLL SceneCamera
    {
    public:
        SceneCamera();

        /// size: visible height in world units.
        void SetOrthographic(float size, float nearClip = -100.0f, float farClip = 100.0f);
        void SetViewportSize(uint32_t width, uint32_t height);

        float GetOrthographicSize() const { return m_OrthographicSize; }
        void SetOrthographicSize(float size) { m_OrthographicSize = std::max(size, 0.01f); RecalculateProjection(); }
        float GetOrthographicNearClip() const { return m_OrthographicNear; }
        void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
        float GetOrthographicFarClip() const { return m_OrthographicFar; }
        void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }
        float GetAspectRatio() const { return m_AspectRatio; }

        const glm::mat4& GetProjection() const { return m_Projection; }

    private:
        void RecalculateProjection();

        glm::mat4 m_Projection{ 1.0f };
        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -100.0f;
        float m_OrthographicFar = 100.0f;
        float m_AspectRatio = 16.0f / 9.0f;
    };
}
