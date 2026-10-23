#include "pch.h"
#include "Math.h"

namespace Matrix::math
{
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
    {
        glm::mat4 localMatrix(transform);

        // Normalize the matrix.
        if (glm::epsilonEqual(localMatrix[3][3], 0.0f, glm::epsilon<float>()))
            return false;

        // Isolate perspective (we assume there is none).
        localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = 0.0f;
        localMatrix[3][3] = 1.0f;

        translation = glm::vec3(localMatrix[3]);
        localMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, localMatrix[3].w);

        glm::vec3 row[3];
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                row[i][j] = localMatrix[i][j];

        scale.x = glm::length(row[0]);
        row[0] = glm::normalize(row[0]);
        scale.y = glm::length(row[1]);
        row[1] = glm::normalize(row[1]);
        scale.z = glm::length(row[2]);
        row[2] = glm::normalize(row[2]);

        rotation.y = std::asin(-row[0][2]);
        if (std::cos(rotation.y) != 0.0f)
        {
            rotation.x = std::atan2(row[1][2], row[2][2]);
            rotation.z = std::atan2(row[0][1], row[0][0]);
        }
        else
        {
            rotation.x = std::atan2(-row[2][0], row[1][1]);
            rotation.z = 0.0f;
        }
        return true;
    }

    glm::mat4 ComposeTransform2D(const glm::vec3& position, float rotationDegrees, const glm::vec3& scale)
    {
        return glm::translate(glm::mat4(1.0f), position)
             * glm::rotate(glm::mat4(1.0f), glm::radians(rotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f))
             * glm::scale(glm::mat4(1.0f), scale);
    }
}
