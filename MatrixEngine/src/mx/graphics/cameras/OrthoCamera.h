#pragma once
#include "Common.h"
///@file OrthoCamera.h  2D orthographic camera.

namespace Matrix::graphics
{
    /// World-space rectangle visible to a camera.
    struct MATRIX_DLL CameraBounds
    {
        float Left = -1.0f, Right = 1.0f, Bottom = -1.0f, Top = 1.0f;
        float GetWidth() const { return Right - Left; }
        float GetHeight() const { return Top - Bottom; }
    };

    class MATRIX_DLL OrthoCamera
    {
    public:
        /// Explicit projection bounds.
        OrthoCamera(float left, float right, float bottom, float top);

        /// Aspect-ratio based projection; zoom is the visible height in world units.
        explicit OrthoCamera(float aspectRatio = 16.0f / 9.0f, float zoom = 10.0f);

        void SetProjection(float left, float right, float bottom, float top);

        /// Updates the aspect ratio from a viewport size (keeps the current zoom).
        void SetViewportSize(uint32_t width, uint32_t height);
        void SetAspectRatio(float aspectRatio);

        /// Visible height in world units.
        void SetZoom(float zoomLevel);
        float GetZoom() const { return m_Zoom; }
        float GetAspectRatio() const { return m_AspectRatio; }

        void SetPosition(const glm::vec3& position);
        void SetRotation(float angleDegrees);
        const glm::vec3& GetPosition() const { return m_Position; }
        float GetRotation() const { return m_Rotation; }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

        /// Converts a pixel position (origin top-left) within a viewport of the given size to world coordinates.
        glm::vec2 ScreenToWorld(const glm::vec2& screenPosition, const glm::vec2& viewportSize) const;

        /// Converts a world position to a pixel position (origin top-left) within a viewport of the given size.
        glm::vec2 WorldToScreen(const glm::vec2& worldPosition, const glm::vec2& viewportSize) const;

        /// Visible world rectangle (ignores rotation).
        CameraBounds GetBounds() const { return m_Bounds; }

    private:
        void RecalculateView();
        void RecalculateProjection();

        glm::mat4 m_ProjectionMatrix{ 1.0f };
        glm::mat4 m_ViewMatrix{ 1.0f };
        glm::vec3 m_Position{ 0.0f };
        float m_Rotation = 0.0f;
        float m_Zoom = 10.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        CameraBounds m_Bounds;
    };
}
