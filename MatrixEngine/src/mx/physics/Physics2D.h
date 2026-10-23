#pragma once
#include "Common.h"
///@file Physics2D.h  Built-in 2D physics: gravity, velocity integration and box/circle collision resolution.

#include "graphics/world/Entity.h"
#include "graphics/world/Components.h"

namespace Matrix::graphics { class World; }

namespace Matrix::physics
{
    struct MATRIX_DLL AABB
    {
        glm::vec2 Min = glm::vec2(0.0f);
        glm::vec2 Max = glm::vec2(0.0f);
        glm::vec2 Center() const { return (Min + Max) * 0.5f; }
        glm::vec2 HalfExtents() const { return (Max - Min) * 0.5f; }
        bool Contains(const glm::vec2& point) const { return point.x >= Min.x && point.x <= Max.x && point.y >= Min.y && point.y <= Max.y; }
    };

    /// Simulates every entity in a World that has RigidBody2D / collider components.
    /// Limitations: colliders are axis aligned (rotation is ignored) and there is no rotational dynamics.
    class MATRIX_DLL Physics2D
    {
    public:
        explicit Physics2D(graphics::World& world);

        /// Advances the simulation by deltaTime using fixed sub-steps.
        void Step(float deltaTime);

        /// Runs exactly one fixed simulation step (exposed for tests and the editor's "step" button).
        void Simulate(float fixedDeltaTime);

        /// Clears accumulated time and runtime flags.
        void Reset();

        float FixedTimestep = 1.0f / 60.0f;
        int MaxSubSteps = 8;
        int SolverIterations = 4;

        /// Number of contacts resolved during the last Simulate call.
        uint32_t GetLastContactCount() const { return m_LastContactCount; }

        // --- Shape helpers (pure, used by tests and the editor) ----------------------------
        static AABB ComputeBoxAABB(const glm::mat4& worldTransform, const graphics::BoxCollider2DComponent& collider);
        static void ComputeCircle(const glm::mat4& worldTransform, const graphics::CircleCollider2DComponent& collider, glm::vec2& outCenter, float& outRadius);

        /// Returns true on overlap. mtv is the minimum translation that moves `a` out of `b`.
        static bool TestAABB(const AABB& a, const AABB& b, glm::vec2& mtv);
        static bool TestCircleCircle(const glm::vec2& centerA, float radiusA, const glm::vec2& centerB, float radiusB, glm::vec2& mtv);
        /// mtv moves the circle out of the box.
        static bool TestCircleAABB(const glm::vec2& center, float radius, const AABB& box, glm::vec2& mtv);

    private:
        struct Collider
        {
            graphics::Entity Entity;
            graphics::RigidBody2DComponent* Body = nullptr; ///< nullptr for static geometry without a rigid body
            bool IsCircle = false;
            bool IsTrigger = false;
            AABB Box;
            glm::vec2 Center = glm::vec2(0.0f);
            float Radius = 0.0f;
            glm::vec2 PositionDelta = glm::vec2(0.0f); ///< accumulated correction this step
        };

        void Integrate(float dt);
        void CollectColliders(std::vector<Collider>& out);
        bool Overlap(const Collider& a, const Collider& b, glm::vec2& mtv) const;
        void Resolve(Collider& a, Collider& b, const glm::vec2& mtv);
        void ApplyCorrections(std::vector<Collider>& colliders);

        graphics::World& m_World;
        float m_Accumulator = 0.0f;
        uint32_t m_LastContactCount = 0;
    };
}
