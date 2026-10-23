#include "pch.h"
#include "Physics2D.h"

#include "graphics/world/World.h"
#include "core/Logging.h"

namespace Matrix::physics
{
    using namespace graphics;

    namespace
    {
        glm::vec2 ExtractScale(const glm::mat4& transform)
        {
            return { glm::length(glm::vec2(transform[0])), glm::length(glm::vec2(transform[1])) };
        }

        void TranslateWorld(World& world, Entity entity, const glm::vec2& delta)
        {
            if (delta.x == 0.0f && delta.y == 0.0f)
                return;
            auto& transform = entity.GetComponent<TransformComponent>();
            if (!entity.HasParent())
            {
                transform.Position.x += delta.x;
                transform.Position.y += delta.y;
                return;
            }
            glm::mat4 worldTransform = world.GetWorldTransform(entity);
            worldTransform[3].x += delta.x;
            worldTransform[3].y += delta.y;
            world.SetWorldTransform(entity, worldTransform);
        }

        float InverseMass(const RigidBody2DComponent* body)
        {
            if (!body || body->Type != BodyType2D::Dynamic)
                return 0.0f;
            return body->Mass > 0.0f ? 1.0f / body->Mass : 0.0f;
        }

        uint64_t PairKey(Entity a, Entity b)
        {
            uint32_t ia = static_cast<uint32_t>(a), ib = static_cast<uint32_t>(b);
            if (ia > ib) std::swap(ia, ib);
            return (static_cast<uint64_t>(ia) << 32) | ib;
        }
    }

    Physics2D::Physics2D(World& world)
        : m_World(world)
    {
    }

    void Physics2D::Reset()
    {
        m_Accumulator = 0.0f;
        m_LastContactCount = 0;
    }

    void Physics2D::Step(float deltaTime)
    {
        m_Accumulator += std::max(deltaTime, 0.0f);
        int steps = 0;
        while (m_Accumulator >= FixedTimestep && steps < MaxSubSteps)
        {
            Simulate(FixedTimestep);
            m_Accumulator -= FixedTimestep;
            ++steps;
        }
        // Drop time we could not simulate to avoid a spiral of death after long stalls.
        if (steps == MaxSubSteps)
            m_Accumulator = 0.0f;
    }

    // --- Shape helpers -----------------------------------------------------------------------

    AABB Physics2D::ComputeBoxAABB(const glm::mat4& worldTransform, const BoxCollider2DComponent& collider)
    {
        glm::vec2 center = glm::vec2(worldTransform * glm::vec4(collider.Offset, 0.0f, 1.0f));
        glm::vec2 scale = ExtractScale(worldTransform);
        glm::vec2 half = glm::abs(collider.Size * scale);
        return { center - half, center + half };
    }

    void Physics2D::ComputeCircle(const glm::mat4& worldTransform, const CircleCollider2DComponent& collider, glm::vec2& outCenter, float& outRadius)
    {
        outCenter = glm::vec2(worldTransform * glm::vec4(collider.Offset, 0.0f, 1.0f));
        glm::vec2 scale = ExtractScale(worldTransform);
        outRadius = std::abs(collider.Radius) * std::max(scale.x, scale.y);
    }

    bool Physics2D::TestAABB(const AABB& a, const AABB& b, glm::vec2& mtv)
    {
        float overlapX = std::min(a.Max.x, b.Max.x) - std::max(a.Min.x, b.Min.x);
        float overlapY = std::min(a.Max.y, b.Max.y) - std::max(a.Min.y, b.Min.y);
        if (overlapX <= 0.0f || overlapY <= 0.0f)
            return false;

        glm::vec2 delta = a.Center() - b.Center();
        if (overlapX < overlapY)
            mtv = { (delta.x < 0.0f ? -overlapX : overlapX), 0.0f };
        else
            mtv = { 0.0f, (delta.y < 0.0f ? -overlapY : overlapY) };
        return true;
    }

    bool Physics2D::TestCircleCircle(const glm::vec2& centerA, float radiusA, const glm::vec2& centerB, float radiusB, glm::vec2& mtv)
    {
        glm::vec2 delta = centerA - centerB;
        float distanceSq = glm::dot(delta, delta);
        float radii = radiusA + radiusB;
        if (distanceSq >= radii * radii)
            return false;
        float distance = std::sqrt(distanceSq);
        glm::vec2 normal = distance > 1e-6f ? delta / distance : glm::vec2(0.0f, 1.0f);
        mtv = normal * (radii - distance);
        return true;
    }

    bool Physics2D::TestCircleAABB(const glm::vec2& center, float radius, const AABB& box, glm::vec2& mtv)
    {
        glm::vec2 closest = glm::clamp(center, box.Min, box.Max);
        glm::vec2 delta = center - closest;
        float distanceSq = glm::dot(delta, delta);
        if (distanceSq > radius * radius)
            return false;

        if (distanceSq > 1e-8f)
        {
            float distance = std::sqrt(distanceSq);
            mtv = (delta / distance) * (radius - distance);
            return true;
        }

        // Circle centre is inside the box: push out along the axis of least penetration.
        glm::vec2 boxCenter = box.Center();
        glm::vec2 half = box.HalfExtents();
        glm::vec2 local = center - boxCenter;
        float penX = half.x - std::abs(local.x) + radius;
        float penY = half.y - std::abs(local.y) + radius;
        if (penX < penY)
            mtv = { local.x < 0.0f ? -penX : penX, 0.0f };
        else
            mtv = { 0.0f, local.y < 0.0f ? -penY : penY };
        return true;
    }

    // --- Simulation ------------------------------------------------------------------------

    void Physics2D::Integrate(float dt)
    {
        auto view = m_World.View<TransformComponent, RigidBody2DComponent>();
        for (auto handle : view)
        {
            Entity entity(handle, &m_World);
            auto& body = view.get<RigidBody2DComponent>(handle);
            body.IsGrounded = false;
            if (body.Type == BodyType2D::Static)
                continue;

            if (body.Type == BodyType2D::Dynamic)
            {
                body.Velocity += m_World.Gravity * body.GravityScale * dt;
                if (body.LinearDamping > 0.0f)
                    body.Velocity *= 1.0f / (1.0f + body.LinearDamping * dt);
            }
            TranslateWorld(m_World, entity, body.Velocity * dt);
        }
    }

    void Physics2D::CollectColliders(std::vector<Collider>& out)
    {
        out.clear();
        for (auto handle : m_World.View<TransformComponent, BoxCollider2DComponent>())
        {
            Entity entity(handle, &m_World);
            Collider collider;
            collider.Entity = entity;
            collider.Body = entity.TryGetComponent<RigidBody2DComponent>();
            collider.IsCircle = false;
            const auto& box = entity.GetComponent<BoxCollider2DComponent>();
            collider.IsTrigger = box.IsTrigger;
            collider.Box = ComputeBoxAABB(m_World.GetWorldTransform(entity), box);
            out.push_back(collider);
        }
        for (auto handle : m_World.View<TransformComponent, CircleCollider2DComponent>())
        {
            Entity entity(handle, &m_World);
            Collider collider;
            collider.Entity = entity;
            collider.Body = entity.TryGetComponent<RigidBody2DComponent>();
            collider.IsCircle = true;
            const auto& circle = entity.GetComponent<CircleCollider2DComponent>();
            collider.IsTrigger = circle.IsTrigger;
            ComputeCircle(m_World.GetWorldTransform(entity), circle, collider.Center, collider.Radius);
            collider.Box = { collider.Center - glm::vec2(collider.Radius), collider.Center + glm::vec2(collider.Radius) };
            out.push_back(collider);
        }
    }

    bool Physics2D::Overlap(const Collider& a, const Collider& b, glm::vec2& mtv) const
    {
        if (!a.IsCircle && !b.IsCircle)
            return TestAABB(a.Box, b.Box, mtv);
        if (a.IsCircle && b.IsCircle)
            return TestCircleCircle(a.Center, a.Radius, b.Center, b.Radius, mtv);
        if (a.IsCircle)
            return TestCircleAABB(a.Center, a.Radius, b.Box, mtv);
        glm::vec2 inverse;
        if (!TestCircleAABB(b.Center, b.Radius, a.Box, inverse))
            return false;
        mtv = -inverse;
        return true;
    }

    void Physics2D::Resolve(Collider& a, Collider& b, const glm::vec2& mtv)
    {
        float invMassA = InverseMass(a.Body);
        float invMassB = InverseMass(b.Body);
        float totalInvMass = invMassA + invMassB;
        if (totalInvMass <= 0.0f)
            return; // neither body can move

        glm::vec2 normal = glm::normalize(mtv);

        // Positional correction split by inverse mass.
        glm::vec2 correctionA = mtv * (invMassA / totalInvMass);
        glm::vec2 correctionB = -mtv * (invMassB / totalInvMass);
        a.PositionDelta += correctionA;
        b.PositionDelta += correctionB;
        a.Box.Min += correctionA; a.Box.Max += correctionA; a.Center += correctionA;
        b.Box.Min += correctionB; b.Box.Max += correctionB; b.Center += correctionB;

        // Velocity response along the normal.
        glm::vec2 velocityA = a.Body ? a.Body->Velocity : glm::vec2(0.0f);
        glm::vec2 velocityB = b.Body ? b.Body->Velocity : glm::vec2(0.0f);
        glm::vec2 relative = velocityA - velocityB;
        float velocityAlongNormal = glm::dot(relative, normal);
        if (velocityAlongNormal < 0.0f)
        {
            float restitution = std::max(a.Body ? a.Body->Restitution : 0.0f, b.Body ? b.Body->Restitution : 0.0f);
            // Avoid jitter from tiny bounces.
            if (std::abs(velocityAlongNormal) < 1.0f)
                restitution = 0.0f;
            float impulse = -(1.0f + restitution) * velocityAlongNormal / totalInvMass;
            glm::vec2 impulseVector = impulse * normal;
            if (a.Body && invMassA > 0.0f) a.Body->Velocity += impulseVector * invMassA;
            if (b.Body && invMassB > 0.0f) b.Body->Velocity -= impulseVector * invMassB;

            // Friction: damp the tangential velocity of the moving bodies.
            float friction = std::sqrt(std::max(0.0f, (a.Body ? a.Body->Friction : 0.5f) * (b.Body ? b.Body->Friction : 0.5f)));
            float damping = 1.0f - math::Clamp(friction * 0.15f, 0.0f, 1.0f);
            glm::vec2 tangent(-normal.y, normal.x);
            if (a.Body && invMassA > 0.0f)
            {
                float vt = glm::dot(a.Body->Velocity, tangent);
                a.Body->Velocity -= tangent * vt * (1.0f - damping);
            }
            if (b.Body && invMassB > 0.0f)
            {
                float vt = glm::dot(b.Body->Velocity, tangent);
                b.Body->Velocity -= tangent * vt * (1.0f - damping);
            }
        }

        if (a.Body && invMassA > 0.0f && normal.y > 0.5f)
            a.Body->IsGrounded = true;
        if (b.Body && invMassB > 0.0f && normal.y < -0.5f)
            b.Body->IsGrounded = true;
    }

    void Physics2D::ApplyCorrections(std::vector<Collider>& colliders)
    {
        for (auto& collider : colliders)
        {
            if (collider.PositionDelta.x != 0.0f || collider.PositionDelta.y != 0.0f)
                TranslateWorld(m_World, collider.Entity, collider.PositionDelta);
        }
    }

    void Physics2D::Simulate(float fixedDeltaTime)
    {
        Integrate(fixedDeltaTime);

        std::vector<Collider> colliders;
        CollectColliders(colliders);

        struct Contact { Entity A; Entity B; glm::vec2 Normal; bool Trigger; };
        std::vector<Contact> contacts;
        std::unordered_set<uint64_t> reported;

        for (int iteration = 0; iteration < SolverIterations; ++iteration)
        {
            for (size_t i = 0; i < colliders.size(); ++i)
            {
                for (size_t j = i + 1; j < colliders.size(); ++j)
                {
                    Collider& a = colliders[i];
                    Collider& b = colliders[j];
                    if (a.Entity == b.Entity)
                        continue;
                    bool aMovable = InverseMass(a.Body) > 0.0f;
                    bool bMovable = InverseMass(b.Body) > 0.0f;
                    bool anyTrigger = a.IsTrigger || b.IsTrigger;
                    if (!aMovable && !bMovable && !anyTrigger)
                        continue; // static vs static

                    glm::vec2 mtv;
                    if (!Overlap(a, b, mtv))
                        continue;

                    uint64_t key = PairKey(a.Entity, b.Entity);
                    if (reported.insert(key).second)
                        contacts.push_back({ a.Entity, b.Entity, glm::normalize(mtv), anyTrigger });

                    if (anyTrigger)
                        continue;
                    Resolve(a, b, mtv);
                }
            }
        }

        ApplyCorrections(colliders);
        m_LastContactCount = static_cast<uint32_t>(contacts.size());

        for (const auto& contact : contacts)
        {
            if (m_World.IsValid(contact.A))
                m_World.OnCollision(contact.A, Collision2D{ contact.B, contact.Normal, contact.Trigger });
            if (m_World.IsValid(contact.B))
                m_World.OnCollision(contact.B, Collision2D{ contact.A, -contact.Normal, contact.Trigger });
        }
    }
}
