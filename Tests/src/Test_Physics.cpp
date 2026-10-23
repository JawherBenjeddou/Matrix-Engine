#include "TestFramework.h"

using namespace Matrix;
using namespace Matrix::graphics;
using namespace Matrix::physics;

namespace
{
    Entity MakeBox(World& world, const char* name, glm::vec2 position, glm::vec2 scale, BodyType2D type, float restitution = 0.0f)
    {
        Entity e = world.CreateEntity(name);
        auto& t = e.GetComponent<TransformComponent>();
        t.Position = glm::vec3(position, 0.0f);
        t.Scale = glm::vec3(scale, 1.0f);
        auto& body = e.AddComponent<RigidBody2DComponent>();
        body.Type = type;
        body.Restitution = restitution;
        e.AddComponent<BoxCollider2DComponent>();
        return e;
    }

    void Simulate(World& world, int steps)
    {
        for (int i = 0; i < steps; ++i)
            world.GetPhysics().Simulate(1.0f / 60.0f);
    }
}

TEST_CASE(Physics, AABBOverlapAndMTV)
{
    AABB a{ { 0.0f, 0.0f }, { 2.0f, 2.0f } };
    AABB b{ { 1.5f, 0.5f }, { 3.5f, 1.5f } };
    glm::vec2 mtv;
    CHECK(Physics2D::TestAABB(a, b, mtv));
    CHECK_APPROX(mtv, glm::vec2(-0.5f, 0.0f)); // push a left, out of b
    AABB c{ { 5.0f, 5.0f }, { 6.0f, 6.0f } };
    CHECK(!Physics2D::TestAABB(a, c, mtv));
    AABB touching{ { 2.0f, 0.0f }, { 3.0f, 1.0f } };
    CHECK(!Physics2D::TestAABB(a, touching, mtv));
}

TEST_CASE(Physics, CircleTests)
{
    glm::vec2 mtv;
    CHECK(Physics2D::TestCircleCircle({ 0, 0 }, 1.0f, { 1.5f, 0 }, 1.0f, mtv));
    CHECK_APPROX(mtv, glm::vec2(-0.5f, 0.0f));
    CHECK(!Physics2D::TestCircleCircle({ 0, 0 }, 1.0f, { 3.0f, 0 }, 1.0f, mtv));

    AABB box{ { 0.0f, 0.0f }, { 2.0f, 2.0f } };
    CHECK(Physics2D::TestCircleAABB({ 1.0f, 2.5f }, 1.0f, box, mtv));
    CHECK_APPROX(mtv, glm::vec2(0.0f, 0.5f));
    CHECK(!Physics2D::TestCircleAABB({ 1.0f, 4.0f }, 1.0f, box, mtv));
    // Centre inside the box: still resolved along the shortest axis.
    CHECK(Physics2D::TestCircleAABB({ 1.0f, 1.8f }, 0.1f, box, mtv));
    CHECK(mtv.y > 0.0f);
}

TEST_CASE(Physics, ColliderShapesFollowTransforms)
{
    BoxCollider2DComponent box;
    box.Offset = { 1.0f, 0.0f };
    box.Size = { 0.5f, 0.5f };
    glm::mat4 transform = math::ComposeTransform2D(glm::vec3(10.0f, 5.0f, 0.0f), 0.0f, glm::vec3(2.0f, 4.0f, 1.0f));
    AABB aabb = Physics2D::ComputeBoxAABB(transform, box);
    CHECK_APPROX(aabb.Center(), glm::vec2(12.0f, 5.0f));
    CHECK_APPROX(aabb.HalfExtents(), glm::vec2(1.0f, 2.0f));

    CircleCollider2DComponent circle;
    circle.Radius = 0.5f;
    glm::vec2 center; float radius;
    Physics2D::ComputeCircle(transform, circle, center, radius);
    CHECK_APPROX(center, glm::vec2(10.0f, 5.0f));
    CHECK_APPROX(radius, 2.0f);
}

TEST_CASE(Physics, DynamicBodyFallsAndRestsOnStaticGround)
{
    World world;
    world.Gravity = glm::vec2(0.0f, -10.0f);
    Entity ground = MakeBox(world, "Ground", { 0.0f, -1.0f }, { 20.0f, 1.0f }, BodyType2D::Static);
    Entity box = MakeBox(world, "Box", { 0.0f, 3.0f }, { 1.0f, 1.0f }, BodyType2D::Dynamic);

    Simulate(world, 180); // 3 seconds
    auto& transform = box.GetComponent<TransformComponent>();
    auto& body = box.GetComponent<RigidBody2DComponent>();
    // Ground top is at y = -0.5, box half height 0.5 -> rests at y = 0.
    CHECK(tests::Approx(transform.Position.y, 0.0f, 0.02f));
    CHECK(tests::Approx(body.Velocity.y, 0.0f, 0.05f));
    CHECK(body.IsGrounded);
    CHECK(ground.GetComponent<TransformComponent>().Position.y == -1.0f); // static never moves
    CHECK(world.GetPhysics().GetLastContactCount() == 1);
}

TEST_CASE(Physics, StaticBodiesIgnoreGravity)
{
    World world;
    Entity platform = MakeBox(world, "Platform", { 0.0f, 0.0f }, { 2.0f, 1.0f }, BodyType2D::Static);
    Simulate(world, 60);
    CHECK_APPROX(platform.GetComponent<TransformComponent>().Position, glm::vec3(0.0f));
}

TEST_CASE(Physics, KinematicBodyMovesByVelocity)
{
    World world;
    Entity mover = MakeBox(world, "Mover", { 0.0f, 0.0f }, { 1.0f, 1.0f }, BodyType2D::Kinematic);
    mover.GetComponent<RigidBody2DComponent>().Velocity = glm::vec2(6.0f, 0.0f);
    Simulate(world, 60);
    CHECK(tests::Approx(mover.GetComponent<TransformComponent>().Position.x, 6.0f, 1e-3f));
    CHECK(tests::Approx(mover.GetComponent<TransformComponent>().Position.y, 0.0f, 1e-3f)); // no gravity
}

TEST_CASE(Physics, RestitutionBounces)
{
    World world;
    world.Gravity = glm::vec2(0.0f, -10.0f);
    MakeBox(world, "Ground", { 0.0f, -1.0f }, { 20.0f, 1.0f }, BodyType2D::Static);
    Entity ball = MakeBox(world, "Ball", { 0.0f, 4.0f }, { 1.0f, 1.0f }, BodyType2D::Dynamic, 0.9f);

    float maxUpwardVelocity = 0.0f;
    for (int i = 0; i < 180; ++i)
    {
        world.GetPhysics().Simulate(1.0f / 60.0f);
        maxUpwardVelocity = std::max(maxUpwardVelocity, ball.GetComponent<RigidBody2DComponent>().Velocity.y);
    }
    CHECK(maxUpwardVelocity > 3.0f); // it bounced back up
}

TEST_CASE(Physics, DynamicBodiesPushEachOtherApart)
{
    World world;
    world.Gravity = glm::vec2(0.0f);
    Entity a = MakeBox(world, "A", { 0.0f, 0.0f }, { 1.0f, 1.0f }, BodyType2D::Dynamic);
    Entity b = MakeBox(world, "B", { 0.5f, 0.0f }, { 1.0f, 1.0f }, BodyType2D::Dynamic);
    Simulate(world, 10);
    float ax = a.GetComponent<TransformComponent>().Position.x;
    float bx = b.GetComponent<TransformComponent>().Position.x;
    CHECK(bx - ax >= 1.0f - 1e-3f);           // no longer overlapping
    CHECK(tests::Approx(ax, -0.25f, 1e-3f));  // equal masses: split evenly
    CHECK(tests::Approx(bx, 0.75f, 1e-3f));
}

TEST_CASE(Physics, TriggersReportButDoNotResolve)
{
    World world;
    world.Gravity = glm::vec2(0.0f);
    Entity zone = world.CreateEntity("Zone");
    zone.GetComponent<TransformComponent>().Scale = glm::vec3(4.0f, 4.0f, 1.0f);
    zone.AddComponent<BoxCollider2DComponent>().IsTrigger = true;
    Entity mover = MakeBox(world, "Mover", { 0.0f, 0.0f }, { 1.0f, 1.0f }, BodyType2D::Dynamic);
    mover.GetComponent<RigidBody2DComponent>().Velocity = glm::vec2(1.0f, 0.0f);
    Simulate(world, 60);
    CHECK(tests::Approx(mover.GetComponent<TransformComponent>().Position.x, 1.0f, 1e-3f));
    CHECK(world.GetPhysics().GetLastContactCount() == 1);
}

TEST_CASE(Physics, StepUsesFixedTimestepAccumulator)
{
    World world;
    world.Gravity = glm::vec2(0.0f);
    Entity mover = MakeBox(world, "Mover", { 0.0f, 0.0f }, { 1.0f, 1.0f }, BodyType2D::Kinematic);
    mover.GetComponent<RigidBody2DComponent>().Velocity = glm::vec2(60.0f, 0.0f);
    auto& physics = world.GetPhysics();
    physics.Step(1.0f / 120.0f); // half a step: nothing happens yet
    CHECK(mover.GetComponent<TransformComponent>().Position.x == 0.0f);
    physics.Step(1.0f / 120.0f); // accumulator reaches one full step
    CHECK(tests::Approx(mover.GetComponent<TransformComponent>().Position.x, 1.0f, 1e-3f));
    physics.Step(10.0f);         // long stall: capped at MaxSubSteps
    CHECK(mover.GetComponent<TransformComponent>().Position.x <= 1.0f + physics.MaxSubSteps + 1e-3f);
}

TEST_CASE(Physics, ChildBodiesRespectParentTransform)
{
    World world;
    world.Gravity = glm::vec2(0.0f, -10.0f);
    MakeBox(world, "Ground", { 0.0f, -1.0f }, { 20.0f, 1.0f }, BodyType2D::Static);
    Entity group = world.CreateEntity("Group");
    group.GetComponent<TransformComponent>().Position = glm::vec3(5.0f, 0.0f, 0.0f);
    Entity box = MakeBox(world, "Child", { 0.0f, 3.0f }, { 1.0f, 1.0f }, BodyType2D::Dynamic);
    world.SetParent(box, group, false);

    Simulate(world, 180);
    glm::vec2 worldPos = box.GetWorldPosition();
    CHECK(tests::Approx(worldPos.x, 5.0f, 1e-3f));
    CHECK(tests::Approx(worldPos.y, 0.0f, 0.02f));
}

namespace
{
    int s_ListenerHits = 0;

    struct Listener : public ScriptableEntity
    {
        void OnCollision(const Collision2D& collision) override
        {
            if (collision.Other && collision.Normal.y > 0.5f)
                ++s_ListenerHits;
        }
    };
}

TEST_CASE(Physics, ScriptsReceiveCollisionCallbacks)
{
    ScriptRegistry::Register<Listener>("TestListener");
    s_ListenerHits = 0;

    World world;
    world.Gravity = glm::vec2(0.0f, -10.0f);
    MakeBox(world, "Ground", { 0.0f, -1.0f }, { 20.0f, 1.0f }, BodyType2D::Static);
    Entity box = MakeBox(world, "Box", { 0.0f, 1.0f }, { 1.0f, 1.0f }, BodyType2D::Dynamic);
    box.AddComponent<NativeScriptComponent>("TestListener");
    world.OnRuntimeStart();
    for (int i = 0; i < 120; ++i)
        world.OnUpdateRuntime(1.0f / 60.0f);
    world.OnRuntimeStop();
    CHECK(s_ListenerHits > 0);
}
