#pragma once
#include "Common.h"

#include "math.h"

#include "graphics/textures/Texture2D.h"

namespace Matrix
{
    namespace graphics
    {
        struct MATRIX_DLL PSRComponent
        {
            glm::vec3 Position = glm::vec3(0.0f);  // Initialize with zero vector
            glm::vec3 Scale = glm::vec3(1.0f);  // Initialize with 1.0f scale
            float Rotation = 0.0f;
            glm::mat4 Transform = glm::mat4(1.0f);

            PSRComponent() = default;
            PSRComponent(glm::vec3 position, glm::vec3 scale, float rotation)
                : Position(position),
                Scale(scale),
                Rotation(rotation)
            {
                updateTransform();
            }
            PSRComponent(const PSRComponent&) = default;

            operator glm::mat4& () { return Transform; }

            operator const glm::mat4& () const { return Transform; }  // Added const

            void updateTransform()
            {
                glm::mat4 PositionMatrix = glm::translate(glm::mat4(1.0f), Position);
                glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), Scale);
                glm::mat4 RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0.0f, 1.0f, 0.0f));

                Transform = PositionMatrix * RotationMatrix * ScaleMatrix;  // Fixed the order
            }
        };

        struct MATRIX_DLL SpriteComponent
        {
            //TODO : make this a ExclObj
            SharedObj<Texture2D> Texture;
            glm::vec3 Color = glm::vec3(1.0f);

            SpriteComponent() = default;
            SpriteComponent(const SpriteComponent&) = default;
            SpriteComponent(const glm::vec3& color, SharedObj<Texture2D> texture)
                : Color(color),
                  Texture(texture)
            {};

        };
    }
}
