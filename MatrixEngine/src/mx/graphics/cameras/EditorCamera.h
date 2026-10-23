#pragma once
#include "Common.h"
///@file EditorCamera.h  Mouse/keyboard driven orthographic camera used by the editor viewport.

#include "OrthoCamera.h"

namespace Matrix::graphics
{
    class MATRIX_DLL EditorCamera
    {
    public:
        EditorCamera();

        /// Processes input. hovered/focused describe the viewport the camera renders into;
        /// mouseInViewport is the cursor position relative to the viewport's top-left corner.
        void OnUpdate(float deltaTime, bool viewportHovered, bool viewportFocused, const glm::vec2& mouseInViewport);

        void SetViewportSize(uint32_t width, uint32_t height);
        glm::vec2 GetViewportSize() const { return m_ViewportSize; }

        /// Centres the camera on a world position.
        void Focus(const glm::vec2& worldPosition);

        /// Resets position and zoom to the defaults.
        void Reset();

        /// Multiplies the zoom (visible height). factor > 1 zooms out.
        void Zoom(float factor, const glm::vec2* zoomTowardsWorld = nullptr);

        OrthoCamera& GetCamera() { return m_Camera; }
        const OrthoCamera& GetCamera() const { return m_Camera; }
        glm::mat4 GetViewProjection() const { return m_Camera.GetViewProjectionMatrix(); }
        glm::vec2 GetPosition() const { return glm::vec2(m_Camera.GetPosition()); }
        void SetPosition(const glm::vec2& position);
        float GetZoom() const { return m_Camera.GetZoom(); }
        void SetZoom(float zoom);

        glm::vec2 ScreenToWorld(const glm::vec2& mouseInViewport) const { return m_Camera.ScreenToWorld(mouseInViewport, m_ViewportSize); }

        float PanSpeed = 1.0f;
        float KeyboardSpeed = 1.0f;   ///< In screen-heights per second.
        float MinZoom = 0.5f;
        float MaxZoom = 500.0f;

    private:
        OrthoCamera m_Camera;
        glm::vec2 m_ViewportSize{ 1280.0f, 720.0f };
        bool m_Panning = false;
        glm::vec2 m_LastMouse{ 0.0f };
    };
}
