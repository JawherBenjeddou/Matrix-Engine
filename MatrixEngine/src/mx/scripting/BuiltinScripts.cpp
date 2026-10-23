#include "pch.h"
#include "BuiltinScripts.h"

#include "graphics/world/ScriptableEntity.h"
#include "graphics/world/ScriptRegistry.h"
#include "audio/AudioEngine.h"
#include "ui/Input.h"
#include "core/Logging.h"

namespace Matrix::scripting
{
    using namespace graphics;
    using ui::Input;

    /// Spins the entity around Z.
    class Rotator : public ScriptableEntity
    {
    protected:
        void OnUpdate(float deltaTime) override
        {
            GetComponent<TransformComponent>().Rotation += GetParameter("Speed", 90.0f) * deltaTime;
        }
    };

    /// Moves the entity back and forth around its starting position.
    class Oscillator : public ScriptableEntity
    {
    protected:
        void OnCreate() override
        {
            m_Origin = GetComponent<TransformComponent>().Position;
            m_Time = 0.0f;
        }

        void OnUpdate(float deltaTime) override
        {
            m_Time += deltaTime;
            float amplitude = GetParameter("Amplitude", 1.0f);
            float frequency = GetParameter("Frequency", 0.5f);
            bool horizontal = GetParameter("Horizontal", 0.0f) > 0.5f;
            float offset = std::sin(m_Time * glm::two_pi<float>() * frequency) * amplitude;
            auto& transform = GetComponent<TransformComponent>();
            transform.Position = m_Origin + (horizontal ? glm::vec3(offset, 0.0f, 0.0f) : glm::vec3(0.0f, offset, 0.0f));
        }

    private:
        glm::vec3 m_Origin{ 0.0f };
        float m_Time = 0.0f;
    };

    /// Keyboard controlled platformer character (A/D or arrows to move, Space/W/Up to jump).
    class PlatformerController2D : public ScriptableEntity
    {
    protected:
        void OnCreate() override
        {
            m_SpawnPosition = GetComponent<TransformComponent>().Position;
            if (!HasComponent<RigidBody2DComponent>())
                MX_CORE_WARN("PlatformerController2D on '{}' needs a RigidBody2DComponent", GetEntity().GetName());
        }

        void OnUpdate(float deltaTime) override
        {
            (void)deltaTime;
            auto* body = TryGetComponent<RigidBody2DComponent>();
            if (!body)
                return;

            float speed = GetParameter("Speed", 6.0f);
            float jumpForce = GetParameter("JumpForce", 9.0f);
            float respawnY = GetParameter("RespawnY", -20.0f);

            float move = 0.0f;
            if (Input::IsKeyDown(Key::A) || Input::IsKeyDown(Key::LEFT)) move -= 1.0f;
            if (Input::IsKeyDown(Key::D) || Input::IsKeyDown(Key::RIGHT)) move += 1.0f;
            body->Velocity.x = move * speed;

            // Face the movement direction by mirroring the sprite.
            auto& transform = GetComponent<TransformComponent>();
            if (move != 0.0f)
                transform.Scale.x = std::abs(transform.Scale.x) * (move < 0.0f ? -1.0f : 1.0f);

            bool jumpPressed = Input::IsKeyPressed(Key::SPACE) || Input::IsKeyPressed(Key::W) || Input::IsKeyPressed(Key::UP);
            if (jumpPressed && body->IsGrounded)
            {
                body->Velocity.y = jumpForce;
                body->IsGrounded = false;
                if (auto* audio = TryGetComponent<AudioSourceComponent>(); audio && !audio->ClipPath.empty())
                    audio::AudioEngine::Play(audio->ClipPath, audio->Volume, audio->Pitch, false);
            }

            if (transform.Position.y < respawnY)
            {
                transform.Position = m_SpawnPosition;
                body->Velocity = glm::vec2(0.0f);
            }
        }

    private:
        glm::vec3 m_SpawnPosition{ 0.0f };
    };

    /// Keeps the camera centred on the player (an entity running PlatformerController2D, or named "Player").
    class CameraFollow2D : public ScriptableEntity
    {
    protected:
        void OnCreate() override
        {
            m_Target = Entity();
            for (Entity entity : GetWorld().GetAllEntities())
            {
                auto* script = entity.TryGetComponent<NativeScriptComponent>();
                if (script && script->ScriptName == "PlatformerController2D")
                {
                    m_Target = entity;
                    break;
                }
            }
            if (!m_Target)
                m_Target = FindEntity("Player");
        }

        void OnUpdate(float deltaTime) override
        {
            if (!m_Target || !GetWorld().IsValid(m_Target))
                return;
            float smoothing = GetParameter("Smoothing", 5.0f);
            glm::vec2 offset(GetParameter("OffsetX", 0.0f), GetParameter("OffsetY", 1.0f));
            glm::vec2 target = m_Target.GetWorldPosition() + offset;

            auto& transform = GetComponent<TransformComponent>();
            glm::vec2 current(transform.Position);
            float t = smoothing <= 0.0f ? 1.0f : math::Clamp(smoothing * deltaTime, 0.0f, 1.0f);
            glm::vec2 next = current + (target - current) * t;
            transform.Position.x = next.x;
            transform.Position.y = next.y;
        }

    private:
        Entity m_Target;
    };

    void RegisterBuiltinScripts()
    {
        MX_REGISTER_SCRIPT(Rotator, { "Speed", 90.0f });
        MX_REGISTER_SCRIPT(Oscillator, { "Amplitude", 1.0f }, { "Frequency", 0.5f }, { "Horizontal", 0.0f });
        MX_REGISTER_SCRIPT(PlatformerController2D, { "Speed", 6.0f }, { "JumpForce", 9.0f }, { "RespawnY", -20.0f });
        MX_REGISTER_SCRIPT(CameraFollow2D, { "Smoothing", 5.0f }, { "OffsetX", 0.0f }, { "OffsetY", 1.0f });
        MX_CORE_INFO("Registered {} built-in scripts", ScriptRegistry::GetNames().size());
    }
}
