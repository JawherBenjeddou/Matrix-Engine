#include "pch.h"
#include "OrthoCamera.h"


namespace Matrix
{
	namespace graphics
	{
		OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
            :m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)),
             m_ViewMatrix(1.0f),
             m_Position(0.0f, 0.0f),
             m_Rotation(0.0f),
             m_Zoom(1.0f) 
        {

        }

        void OrthoCamera::SetPosition(const glm::vec2& pos) {
            m_Position = pos;
            RecalculateViewMatrix();
        }

        void OrthoCamera::SetRotation(float angleDegrees) {
            m_Rotation = angleDegrees;
            RecalculateViewMatrix();
        }

        void OrthoCamera::SetZoom(float zoomLevel) {
            m_Zoom = zoomLevel;
            RecalculateViewMatrix();
        }

        void OrthoCamera::RecalculateViewMatrix() {
            m_ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-m_Position.x, -m_Position.y, 0.0f)) 
                * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f)) 
                * glm::scale(glm::mat4(1.0f), glm::vec3(m_Zoom, m_Zoom, 1.0f));
        }

        glm::mat4 OrthoCamera::GetViewProjectionMatrix() const {
            return m_ProjectionMatrix * m_ViewMatrix;
        }

        glm::vec2 OrthoCamera::GetPosition()
        {
            return m_Position;
        }

        float OrthoCamera::GetRotation()
        {
            return m_Rotation;
        }

	}
}

