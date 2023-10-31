#pragma once

///@file OrthoCamera.h

#include "Common.h"
#include "math/Math.h"

namespace Matrix
{
	namespace graphics
	{
        class MATRIX_DLL OrthoCamera {

        public:
            /// <summary>
            /// Initializes a new Orthographic Camera with the specified projection matrix parameters.
            /// </summary>
            /// <param name="left"> : The left coordinate of the projection plane.</param>
            /// <param name="right"> : The right coordinate of the projection plane.</param>
            /// <param name="bottom"> : The bottom coordinate of the projection plane.</param>
            /// <param name="top"> : The top coordinate of the projection plane.</param>
            OrthoCamera(float left, float right, float bottom, float top);


            /// Set the camera's position
            void SetPosition(const glm::vec2& pos);

            /// Set the camera's rotation in degrees
            void SetRotation(float angleDegrees);

            /// Set the camera's zoom level
            void SetZoom(float zoomLevel);

            /// Get the view projection matrix
            glm::mat4 GetViewProjectionMatrix() const;

            /// Return's the camera current positions (x,y)
            glm::vec2 GetPosition();

            /// Return's the camera current rotation in degrees
            float GetRotation();

        private:

            /// Recalculates the view matrix everytime a setter is called
            void RecalculateViewMatrix();

        private:
            glm::mat4 m_ProjectionMatrix;
            glm::mat4 m_ViewMatrix;
            glm::vec2 m_Position;
            float m_Rotation;
            float m_Zoom;
        };

	}
}

