#include "TestFramework.h"

using namespace Matrix;
using namespace Matrix::graphics;

namespace
{
    SharedObj<World> BuildSampleWorld()
    {
        auto world = CreateShared<World>("Sample");
        world->Gravity = glm::vec2(1.0f, -3.0f);
        world->BackgroundColor = glm::vec4(0.1f, 0.2f, 0.3f, 1.0f);

        Entity player = world->CreateEntity("Player");
        player.GetComponent<TransformComponent>().Position = glm::vec3(1.0f, 2.0f, 3.0f);
        player.GetComponent<TransformComponent>().Rotation = 45.0f;
        player.AddComponent<SpriteComponent>(glm::vec4(1.0f, 0.5f, 0.25f, 1.0f), "assets/textures/player.png").Tiling = glm::vec2(2.0f, 3.0f);
        auto& body = player.AddComponent<RigidBody2DComponent>();
        body.Type = BodyType2D::Dynamic;
        body.Mass = 2.5f;
        body.Restitution = 0.3f;
        player.AddComponent<BoxCollider2DComponent>().Size = glm::vec2(0.25f, 0.75f);
        auto& script = player.AddComponent<NativeScriptComponent>("PlatformerController2D");
        script.Parameters["Speed"] = 7.5f;
        auto& audio = player.AddComponent<AudioSourceComponent>();
        audio.ClipPath = "assets/audio/jump.wav";
        audio.Loop = true;
        audio.PlayOnStart = false;

        Entity camera = world->CreateEntity("Camera");
        camera.AddComponent<CameraComponent>().Camera.SetOrthographic(20.0f, -5.0f, 5.0f);

        Entity ball = world->CreateEntity("Ball");
        ball.AddComponent<CircleRendererComponent>().Thickness = 0.5f;
        ball.AddComponent<CircleCollider2DComponent>().Radius = 0.75f;
        world->SetParent(ball, player, false);
        return world;
    }
}

TEST_CASE(Serialization, RoundTripPreservesEntitiesAndComponents)
{
    auto original = BuildSampleWorld();
    WorldSerializer writer(original);
    std::string json = writer.SerializeToString();
    CHECK(json.find("\"Scene\": \"Sample\"") != std::string::npos);

    auto loaded = CreateShared<World>();
    WorldSerializer reader(loaded);
    CHECK(reader.DeserializeFromString(json));

    CHECK(loaded->GetName() == "Sample");
    CHECK(loaded->GetEntityCount() == 3);
    CHECK_APPROX(loaded->Gravity, glm::vec2(1.0f, -3.0f));
    CHECK_APPROX(loaded->BackgroundColor, glm::vec4(0.1f, 0.2f, 0.3f, 1.0f));

    Entity player = loaded->FindEntityByName("Player");
    CHECK(player);
    CHECK(player.GetUUID() == original->FindEntityByName("Player").GetUUID());
    CHECK_APPROX(player.GetComponent<TransformComponent>().Position, glm::vec3(1.0f, 2.0f, 3.0f));
    CHECK_APPROX(player.GetComponent<TransformComponent>().Rotation, 45.0f);
    CHECK(player.GetComponent<SpriteComponent>().TexturePath == "assets/textures/player.png");
    CHECK_APPROX(player.GetComponent<SpriteComponent>().Tiling, glm::vec2(2.0f, 3.0f));
    CHECK(player.GetComponent<RigidBody2DComponent>().Type == BodyType2D::Dynamic);
    CHECK_APPROX(player.GetComponent<RigidBody2DComponent>().Mass, 2.5f);
    CHECK_APPROX(player.GetComponent<RigidBody2DComponent>().Restitution, 0.3f);
    CHECK_APPROX(player.GetComponent<BoxCollider2DComponent>().Size, glm::vec2(0.25f, 0.75f));
    CHECK(player.GetComponent<NativeScriptComponent>().ScriptName == "PlatformerController2D");
    CHECK_APPROX(player.GetComponent<NativeScriptComponent>().GetParameter("Speed", 0.0f), 7.5f);
    CHECK(player.GetComponent<AudioSourceComponent>().Loop);
    CHECK(!player.GetComponent<AudioSourceComponent>().PlayOnStart);

    Entity camera = loaded->FindEntityByName("Camera");
    CHECK_APPROX(camera.GetComponent<CameraComponent>().Camera.GetOrthographicSize(), 20.0f);
    CHECK_APPROX(camera.GetComponent<CameraComponent>().Camera.GetOrthographicNearClip(), -5.0f);

    Entity ball = loaded->FindEntityByName("Ball");
    CHECK(ball.GetParent() == player);
    CHECK(player.GetChildren().size() == 1);
    CHECK_APPROX(ball.GetComponent<CircleCollider2DComponent>().Radius, 0.75f);
    CHECK_APPROX(ball.GetComponent<CircleRendererComponent>().Thickness, 0.5f);
}

TEST_CASE(Serialization, FileRoundTrip)
{
    auto original = BuildSampleWorld();
    auto path = util::FileSystem::GetRootDirectory() / "build" / "tests" / "roundtrip.mxscene";
    WorldSerializer writer(original);
    CHECK(writer.Serialize(path));
    CHECK(original->GetFilePath() == "build/tests/roundtrip.mxscene");

    auto loaded = CreateShared<World>();
    WorldSerializer reader(loaded);
    CHECK(reader.Deserialize(path));
    CHECK(loaded->GetEntityCount() == 3);
    CHECK(loaded->GetFilePath() == "build/tests/roundtrip.mxscene");
}

TEST_CASE(Serialization, RejectsInvalidInput)
{
    auto world = CreateShared<World>();
    WorldSerializer reader(world);
    CHECK(!reader.DeserializeFromString("this is not json"));
    CHECK(!reader.DeserializeFromString("{\"Scene\": \"x\"}"));
    CHECK(!reader.DeserializeFromString("[1,2,3]"));
    CHECK(!reader.Deserialize("build/tests/does-not-exist.mxscene"));
    CHECK(world->GetEntityCount() == 0);
}

TEST_CASE(Serialization, ToleratesMissingFieldsAndBrokenReferences)
{
    const char* json = R"({
        "Scene": "Partial",
        "Entities": [
            { "ID": 1, "Tag": "A", "RelationshipComponent": { "Parent": 999, "Children": [2, 777] } },
            { "ID": 2, "Tag": "B", "SpriteComponent": {}, "RigidBody2DComponent": { "Type": "Weird" } },
            "not an object",
            { "ID": 2, "Tag": "DuplicateId" }
        ]
    })";
    auto world = CreateShared<World>();
    WorldSerializer reader(world);
    CHECK(reader.DeserializeFromString(json));
    CHECK(world->GetEntityCount() == 3);

    Entity a = world->FindEntityByName("A");
    CHECK(!a.HasParent());                 // dangling parent dropped
    CHECK(a.GetChildren().size() == 1);    // dangling child dropped
    Entity b = world->FindEntityByName("B");
    CHECK_APPROX(b.GetComponent<SpriteComponent>().Color, glm::vec4(1.0f));
    CHECK(b.GetComponent<RigidBody2DComponent>().Type == BodyType2D::Static);
    Entity dup = world->FindEntityByName("DuplicateId");
    CHECK(dup && dup.GetUUID() != b.GetUUID());
}

TEST_CASE(Serialization, DemoSceneLoads)
{
    auto world = CreateShared<World>();
    WorldSerializer reader(world);
    CHECK(reader.Deserialize("assets/scenes/Demo.mxscene"));
    CHECK(world->GetEntityCount() > 10);
    CHECK(world->GetPrimaryCameraEntity());
    Entity player = world->FindEntityByName("Player");
    CHECK(player);
    CHECK(player.HasComponent<RigidBody2DComponent>());
    CHECK(player.GetComponent<NativeScriptComponent>().ScriptName == "PlatformerController2D");
    for (Entity e : world->GetAllEntities())
    {
        if (auto* sprite = e.TryGetComponent<SpriteComponent>(); sprite && !sprite->TexturePath.empty())
            CHECK_MSG(util::FileSystem::Exists(sprite->TexturePath), sprite->TexturePath);
        if (auto* audio = e.TryGetComponent<AudioSourceComponent>(); audio && !audio->ClipPath.empty())
            CHECK_MSG(util::FileSystem::Exists(audio->ClipPath), audio->ClipPath);
        if (auto* script = e.TryGetComponent<NativeScriptComponent>(); script && !script->ScriptName.empty())
            CHECK_MSG(ScriptRegistry::Exists(script->ScriptName), script->ScriptName);
    }
}
