#include "TestFramework.h"

using namespace Matrix;
using namespace Matrix::graphics;

TEST_CASE(World, CreateFindDestroyEntities)
{
    World world("Test");
    Entity a = world.CreateEntity("Alpha");
    Entity b = world.CreateEntity("Beta");
    CHECK(world.GetEntityCount() == 2);
    CHECK(a && b);
    CHECK(a != b);
    CHECK(a.HasComponent<TransformComponent>());
    CHECK(a.HasComponent<TagComponent>());
    CHECK(a.HasComponent<IDComponent>());
    CHECK(a.GetName() == "Alpha");
    CHECK(world.FindEntityByName("Beta") == b);
    CHECK(!world.FindEntityByName("Gamma"));
    CHECK(world.GetEntityByUUID(a.GetUUID()) == a);

    core::UUID idOfA = a.GetUUID();
    world.DestroyEntity(a);
    CHECK(!world.IsValid(a));
    CHECK(world.GetEntityCount() == 1);
    CHECK(!world.GetEntityByUUID(idOfA));
    // Destroying twice is harmless.
    world.DestroyEntity(a);
    CHECK(world.GetEntityCount() == 1);
}

TEST_CASE(World, ComponentsAddRemove)
{
    World world;
    Entity e = world.CreateEntity("Sprite");
    auto& sprite = e.AddComponent<SpriteComponent>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    CHECK(e.HasComponent<SpriteComponent>());
    CHECK_APPROX(sprite.Color, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    CHECK(e.TryGetComponent<CameraComponent>() == nullptr);
    e.RemoveComponent<SpriteComponent>();
    CHECK(!e.HasComponent<SpriteComponent>());
    e.RemoveComponent<SpriteComponent>(); // no-op
    CHECK((e.HasAnyComponent<SpriteComponent, TransformComponent>()));
}

TEST_CASE(World, CreationOrderIsStable)
{
    World world;
    world.CreateEntity("1");
    world.CreateEntity("2");
    world.CreateEntity("3");
    auto all = world.GetAllEntities();
    CHECK(all.size() == 3);
    CHECK(all[0].GetName() == "1");
    CHECK(all[2].GetName() == "3");
}

TEST_CASE(World, HierarchyParentingAndWorldTransforms)
{
    World world;
    Entity parent = world.CreateEntity("Parent");
    Entity child = world.CreateEntity("Child");
    parent.GetComponent<TransformComponent>().Position = glm::vec3(10.0f, 0.0f, 0.0f);
    parent.GetComponent<TransformComponent>().Rotation = 90.0f;
    child.GetComponent<TransformComponent>().Position = glm::vec3(1.0f, 0.0f, 0.0f);

    world.SetParent(child, parent, false);
    CHECK(child.GetParent() == parent);
    CHECK(parent.GetChildren().size() == 1);
    CHECK(world.IsDescendantOf(child, parent));
    CHECK(!world.IsDescendantOf(parent, child));
    CHECK(world.GetRootEntities().size() == 1);

    // Child local (1,0) rotated by 90 degrees around the parent at (10,0) ends up at (10,1).
    CHECK(tests::Approx(child.GetWorldPosition(), glm::vec2(10.0f, 1.0f), 1e-4f));

    // Re-parenting with keepWorldTransform preserves the world position.
    world.RemoveParent(child, true);
    CHECK(!child.HasParent());
    CHECK(tests::Approx(child.GetWorldPosition(), glm::vec2(10.0f, 1.0f), 1e-4f));
    CHECK(tests::Approx(child.GetComponent<TransformComponent>().Rotation, 90.0f, 1e-3f));

    world.SetParent(child, parent, true);
    CHECK(tests::Approx(child.GetWorldPosition(), glm::vec2(10.0f, 1.0f), 1e-4f));
    CHECK(tests::Approx(child.GetComponent<TransformComponent>().Position, glm::vec3(1.0f, 0.0f, 0.0f), 1e-4f));
}

TEST_CASE(World, HierarchyRejectsCycles)
{
    World world;
    Entity a = world.CreateEntity("A");
    Entity b = world.CreateEntity("B");
    Entity c = world.CreateEntity("C");
    world.SetParent(b, a);
    world.SetParent(c, b);
    world.SetParent(a, c); // would create a cycle: ignored
    CHECK(!a.HasParent());
    world.SetParent(a, a); // self parenting ignored
    CHECK(!a.HasParent());
}

TEST_CASE(World, DestroyingParentDestroysChildren)
{
    World world;
    Entity a = world.CreateEntity("A");
    Entity b = world.CreateEntity("B");
    Entity c = world.CreateEntity("C");
    world.SetParent(b, a);
    world.SetParent(c, b);
    world.DestroyEntity(a);
    CHECK(world.GetEntityCount() == 0);
    CHECK(!world.IsValid(b));
    CHECK(!world.IsValid(c));
}

TEST_CASE(World, DuplicateEntityCopiesComponentsAndChildren)
{
    World world;
    Entity parent = world.CreateEntity("Parent");
    parent.AddComponent<SpriteComponent>(glm::vec4(0.5f));
    auto& script = parent.AddComponent<NativeScriptComponent>("Rotator");
    script.Parameters["Speed"] = 123.0f;
    Entity child = world.CreateEntity("Child");
    world.SetParent(child, parent);

    Entity copy = world.DuplicateEntity(parent);
    CHECK(copy && copy != parent);
    CHECK(copy.GetUUID() != parent.GetUUID());
    CHECK(copy.GetName() == "Parent (copy)");
    CHECK(copy.HasComponent<SpriteComponent>());
    CHECK(copy.GetComponent<NativeScriptComponent>().GetParameter("Speed", 0.0f) == 123.0f);
    CHECK(copy.GetChildren().size() == 1);
    CHECK(copy.GetChildren()[0] != child);
    CHECK(!copy.HasParent());
    CHECK(world.GetEntityCount() == 4);
}

TEST_CASE(World, CopyProducesIndependentWorld)
{
    auto world = CreateShared<World>("Original");
    world->Gravity = glm::vec2(0.0f, -5.0f);
    Entity a = world->CreateEntity("A");
    Entity b = world->CreateEntity("B");
    world->SetParent(b, a);
    a.AddComponent<RigidBody2DComponent>().Type = BodyType2D::Dynamic;

    auto copy = World::Copy(world);
    CHECK(copy->GetName() == "Original");
    CHECK(copy->GetEntityCount() == 2);
    CHECK_APPROX(copy->Gravity, glm::vec2(0.0f, -5.0f));
    Entity a2 = copy->GetEntityByUUID(a.GetUUID());
    Entity b2 = copy->GetEntityByUUID(b.GetUUID());
    CHECK(a2 && b2);
    CHECK(b2.GetParent() == a2);
    CHECK(a2.GetComponent<RigidBody2DComponent>().Type == BodyType2D::Dynamic);

    // Mutating the copy does not touch the original.
    a2.GetComponent<TransformComponent>().Position.x = 50.0f;
    CHECK(a.GetComponent<TransformComponent>().Position.x == 0.0f);
}

TEST_CASE(World, RuntimeRunsScripts)
{
    World world;
    Entity spinner = world.CreateEntity("Spinner");
    auto& script = spinner.AddComponent<NativeScriptComponent>("Rotator");
    script.Parameters["Speed"] = 90.0f;

    world.OnRuntimeStart();
    CHECK(world.IsRunning());
    CHECK(script.Instance != nullptr);
    world.OnUpdateRuntime(0.5f);
    CHECK_APPROX(spinner.GetComponent<TransformComponent>().Rotation, 45.0f);

    world.SetPaused(true);
    world.OnUpdateRuntime(0.5f);
    CHECK_APPROX(spinner.GetComponent<TransformComponent>().Rotation, 45.0f);
    world.Step(1);
    world.OnUpdateRuntime(0.5f);
    CHECK_APPROX(spinner.GetComponent<TransformComponent>().Rotation, 90.0f);

    world.OnRuntimeStop();
    CHECK(!world.IsRunning());
    CHECK(script.Instance == nullptr);
}

TEST_CASE(World, UnknownScriptDoesNotCrash)
{
    World world;
    Entity e = world.CreateEntity("Broken");
    e.AddComponent<NativeScriptComponent>("ScriptThatDoesNotExist");
    world.OnRuntimeStart();
    world.OnUpdateRuntime(0.016f);
    world.OnRuntimeStop();
    CHECK(e.GetComponent<NativeScriptComponent>().Instance == nullptr);
}

TEST_CASE(World, PrimaryCameraAndViewportResize)
{
    World world;
    world.OnViewportResize(800, 400);
    Entity cam = world.CreateEntity("Camera");
    auto& component = cam.AddComponent<CameraComponent>();
    component.Camera.SetOrthographic(10.0f);
    CHECK(world.GetPrimaryCameraEntity() == cam);
    CHECK_APPROX(component.Camera.GetAspectRatio(), 2.0f);
    world.OnViewportResize(400, 400);
    CHECK_APPROX(component.Camera.GetAspectRatio(), 1.0f);
    component.Primary = false;
    CHECK(!world.GetPrimaryCameraEntity());
}
