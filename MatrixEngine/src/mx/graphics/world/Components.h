#pragma once
#include "Common.h"
///@file Components.h  Every component type that can be attached to a World entity.

#include "core/UUID.h"
#include "graphics/textures/Texture2D.h"
#include "graphics/cameras/SceneCamera.h"

namespace Matrix::graphics
{
    class ScriptableEntity;

    /// Stable identifier that survives copies, duplication and serialization.
    struct MATRIX_DLL IDComponent
    {
        core::UUID ID;
        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
        explicit IDComponent(core::UUID id) : ID(id) {}
    };

    /// Human readable name shown in the editor.
    struct MATRIX_DLL TagComponent
    {
        std::string Tag;
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        explicit TagComponent(const std::string& tag) : Tag(tag) {}
    };

    /// Position / rotation (degrees around Z) / scale. Local to the parent when the entity has one.
    struct MATRIX_DLL TransformComponent
    {
        glm::vec3 Position = glm::vec3(0.0f);
        float Rotation = 0.0f;
        glm::vec3 Scale = glm::vec3(1.0f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& position, const glm::vec3& scale = glm::vec3(1.0f), float rotation = 0.0f)
            : Position(position), Rotation(rotation), Scale(scale) {}

        /// Local transform matrix: translate * rotateZ * scale.
        glm::mat4 GetTransform() const
        {
            return glm::translate(glm::mat4(1.0f), Position)
                 * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f))
                 * glm::scale(glm::mat4(1.0f), Scale);
        }

        operator glm::mat4() const { return GetTransform(); }
    };

    /// Legacy name kept for compatibility with earlier engine versions.
    using PSRComponent = TransformComponent;

    /// Parent / children links. Entities without this component are scene roots.
    struct MATRIX_DLL RelationshipComponent
    {
        core::UUID Parent = core::UUID::Invalid();
        std::vector<core::UUID> Children;

        RelationshipComponent() = default;
        RelationshipComponent(const RelationshipComponent&) = default;
    };

    /// Textured (or flat-coloured) quad.
    struct MATRIX_DLL SpriteComponent
    {
        glm::vec4 Color = glm::vec4(1.0f);
        /// Root-relative texture path. Empty = untextured.
        std::string TexturePath;
        /// Runtime texture handle, resolved from TexturePath by the renderer.
        SharedObj<Texture2D> Texture;
        /// How many times the texture repeats across the quad on each axis.
        glm::vec2 Tiling = glm::vec2(1.0f);

        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent&) = default;
        explicit SpriteComponent(const glm::vec4& color) : Color(color) {}
        SpriteComponent(const glm::vec4& color, const std::string& texturePath) : Color(color), TexturePath(texturePath) {}
    };

    /// Anti-aliased circle / ring.
    struct MATRIX_DLL CircleRendererComponent
    {
        glm::vec4 Color = glm::vec4(1.0f);
        float Thickness = 1.0f;
        float Fade = 0.005f;

        CircleRendererComponent() = default;
        CircleRendererComponent(const CircleRendererComponent&) = default;
    };

    /// Camera used when the scene runs. The first "Primary" camera is used for rendering.
    struct MATRIX_DLL CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    enum class BodyType2D : uint8_t
    {
        Static = 0,   ///< Never moves; collides with dynamic bodies.
        Dynamic,      ///< Affected by gravity, velocity and collisions.
        Kinematic     ///< Moves by velocity only; pushes dynamic bodies, ignores gravity.
    };

    /// 2D rigid body simulated by the built-in physics system.
    struct MATRIX_DLL RigidBody2DComponent
    {
        BodyType2D Type = BodyType2D::Static;
        glm::vec2 Velocity = glm::vec2(0.0f);
        float GravityScale = 1.0f;
        float Mass = 1.0f;
        float Restitution = 0.0f;   ///< Bounciness 0..1
        float Friction = 0.4f;      ///< Tangential velocity damping on contact 0..1
        float LinearDamping = 0.0f; ///< Per-second velocity damping
        bool FixedRotation = true;

        /// Runtime: true while the body rests on something below it.
        bool IsGrounded = false;

        RigidBody2DComponent() = default;
        RigidBody2DComponent(const RigidBody2DComponent&) = default;
    };

    /// Axis-aligned box collider (local offset, half extents in local units; scaled by the transform).
    struct MATRIX_DLL BoxCollider2DComponent
    {
        glm::vec2 Offset = glm::vec2(0.0f);
        glm::vec2 Size = glm::vec2(0.5f);
        bool IsTrigger = false;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
    };

    /// Circle collider (radius in local units; scaled by the largest transform scale axis).
    struct MATRIX_DLL CircleCollider2DComponent
    {
        glm::vec2 Offset = glm::vec2(0.0f);
        float Radius = 0.5f;
        bool IsTrigger = false;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
    };

    /// Attaches a C++ script (registered in the ScriptRegistry) to the entity.
    struct MATRIX_DLL NativeScriptComponent
    {
        std::string ScriptName;
        /// Named float parameters editable in the inspector and saved with the scene.
        std::map<std::string, float> Parameters;
        /// Runtime instance (owned by the World while it runs).
        ScriptableEntity* Instance = nullptr;

        NativeScriptComponent() = default;
        NativeScriptComponent(const NativeScriptComponent& other)
            : ScriptName(other.ScriptName), Parameters(other.Parameters), Instance(nullptr) {}
        explicit NativeScriptComponent(const std::string& scriptName) : ScriptName(scriptName) {}

        float GetParameter(const std::string& name, float defaultValue) const
        {
            auto it = Parameters.find(name);
            return it == Parameters.end() ? defaultValue : it->second;
        }
    };

    /// Plays an audio clip (WAV / MP3 / FLAC) through the audio engine.
    struct MATRIX_DLL AudioSourceComponent
    {
        std::string ClipPath;
        float Volume = 1.0f;
        float Pitch = 1.0f;
        bool Loop = false;
        bool PlayOnStart = true;
        /// Runtime sound handle (0 = not playing).
        uint32_t RuntimeHandle = 0;

        AudioSourceComponent() = default;
        AudioSourceComponent(const AudioSourceComponent& other)
            : ClipPath(other.ClipPath), Volume(other.Volume), Pitch(other.Pitch), Loop(other.Loop), PlayOnStart(other.PlayOnStart), RuntimeHandle(0) {}
    };

    template <typename... Component>
    struct ComponentGroup {};

    /// Every component that is copied when a World or entity is duplicated (ID and Tag are handled separately).
    using AllComponents = ComponentGroup<
        TransformComponent, RelationshipComponent, SpriteComponent, CircleRendererComponent, CameraComponent,
        RigidBody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, NativeScriptComponent, AudioSourceComponent>;
}
