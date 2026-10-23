#pragma once
///@file Math.h  Math types used across the engine (thin wrapper over GLM).

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/epsilon.hpp"

#include "core/Export.h"

namespace Matrix::math
{
    /// Decomposes a transformation matrix into translation, rotation (radians) and scale.
    /// Returns false if the matrix could not be decomposed.
    MATRIX_DLL bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

    /// Builds a 2D transform: translate * rotateZ(degrees) * scale.
    MATRIX_DLL glm::mat4 ComposeTransform2D(const glm::vec3& position, float rotationDegrees, const glm::vec3& scale);

    /// Linear interpolation helper.
    inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

    /// Clamp helper.
    inline float Clamp(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
}
