#include "TestFramework.h"

#include <thread>

using namespace Matrix;

TEST_CASE(Core, UUIDsAreUniqueAndNonZero)
{
    std::unordered_set<uint64_t> seen;
    for (int i = 0; i < 1000; ++i)
    {
        core::UUID id;
        CHECK(id.IsValid());
        CHECK(seen.insert(id).second);
    }
    CHECK(!core::UUID::Invalid().IsValid());
    CHECK(core::UUID(42).Value() == 42);
}

TEST_CASE(Core, TimerMeasuresDeltaInSeconds)
{
    util::Timer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    timer.Tick();
    CHECK(timer.DeltaTime() > 0.01f);
    CHECK(timer.DeltaTime() < 1.0f);
    CHECK(timer.FrameCount() == 1);
    CHECK(timer.TotalTime() >= timer.DeltaTime() * 0.9f);
}

TEST_CASE(Core, TimerClampsHugeFrames)
{
    util::Timer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    timer.Tick();
    CHECK(timer.DeltaTime() <= util::Timer::MaxDeltaTime + 1e-5f);
    CHECK(timer.RawDeltaTime() > util::Timer::MaxDeltaTime);
}

TEST_CASE(Core, EventBusSubscribeBroadcastUnsubscribe)
{
    auto& bus = events::Event::GetInstance();
    bus.Clear();
    int calls = 0;
    auto id = bus.AddFunction("test.event", [&] { ++calls; });
    bus.AddFunction("test.event", [&] { calls += 10; });

    CHECK(bus.SubscriberCount("test.event") == 2);
    CHECK(bus.Broadcast("test.event") == 2);
    CHECK(calls == 11);
    CHECK(bus.Broadcast("missing.event") == 0);

    CHECK(bus.RemoveFunction(id));
    CHECK(!bus.RemoveFunction(id));
    bus.Broadcast("test.event");
    CHECK(calls == 21);
    bus.Clear();
    CHECK(bus.SubscriberCount("test.event") == 0);
}

TEST_CASE(Core, EventHandlersMayUnsubscribeDuringBroadcast)
{
    auto& bus = events::Event::GetInstance();
    bus.Clear();
    events::HandlerId self = 0;
    int calls = 0;
    self = bus.AddFunction("once", [&] { ++calls; bus.RemoveFunction(self); });
    bus.Broadcast("once");
    bus.Broadcast("once");
    CHECK(calls == 1);
    bus.Clear();
}

TEST_CASE(Core, FileSystemFindsProjectRoot)
{
    auto root = util::FileSystem::GetRootDirectory();
    CHECK(std::filesystem::exists(root / "resources" / "shaders"));
    CHECK(util::FileSystem::Resolve("assets/x.png") == (root / "assets" / "x.png").lexically_normal());
    CHECK(util::FileSystem::MakeRelative(root / "assets" / "textures" / "player.png") == "assets/textures/player.png");
    CHECK(util::FileSystem::Exists("resources/shaders/Renderer2D_Quad.vert"));
}

TEST_CASE(Core, FileSystemReadWriteRoundTrip)
{
    auto path = util::FileSystem::GetRootDirectory() / "build" / "tests" / "roundtrip.txt";
    CHECK(util::FileSystem::WriteText(path, "hello matrix"));
    auto text = util::FileSystem::ReadText(path);
    CHECK(text.has_value());
    CHECK(*text == "hello matrix");
    CHECK(!util::FileSystem::ReadText(path / "does-not-exist").has_value());
}

TEST_CASE(Core, LoggingHistoryCapturesMessages)
{
    core::Logging::ClearHistory();
    uint64_t version = core::Logging::GetHistoryVersion();
    MX_WARN("test warning {}", 42);
    CHECK(core::Logging::GetHistoryVersion() > version);
    auto history = core::Logging::GetHistory();
    CHECK(!history.empty());
    CHECK(history.back().Text == "test warning 42");
    CHECK(history.back().Level == core::LogLevel::Warn);
    CHECK(history.back().Logger == "APP");
}

TEST_CASE(Core, InputEdgeDetection)
{
    using ui::Input;
    Input::InitSystem();
    Input::NewFrame();
    Input::SetKeyState(static_cast<int>(Key::SPACE), GLFW_PRESS);
    CHECK(Input::IsKeyDown(Key::SPACE));
    CHECK(Input::IsKeyPressed(Key::SPACE));
    Input::NewFrame();
    CHECK(Input::IsKeyDown(Key::SPACE));
    CHECK(!Input::IsKeyPressed(Key::SPACE));
    Input::SetKeyState(static_cast<int>(Key::SPACE), GLFW_RELEASE);
    CHECK(!Input::IsKeyDown(Key::SPACE));
    CHECK(Input::IsKeyReleased(Key::SPACE));
    // Out-of-range codes are ignored instead of corrupting memory.
    Input::SetKeyState(100000, GLFW_PRESS);
    Input::SetMouseState(-5, GLFW_PRESS);
    CHECK(!Input::IsKeyDown(Key::UNKNOWN));
    Input::InitSystem();
}

TEST_CASE(Core, ScriptRegistryKnowsBuiltins)
{
    CHECK(graphics::ScriptRegistry::Exists("Rotator"));
    CHECK(graphics::ScriptRegistry::Exists("PlatformerController2D"));
    CHECK(!graphics::ScriptRegistry::Exists("DoesNotExist"));
    CHECK(graphics::ScriptRegistry::Instantiate("DoesNotExist") == nullptr);
    auto params = graphics::ScriptRegistry::GetParameters("Rotator");
    CHECK(params.size() == 1);
    CHECK(params[0].Name == "Speed");
    auto* script = graphics::ScriptRegistry::Instantiate("Rotator");
    CHECK(script != nullptr);
    delete script;
}

TEST_CASE(Core, MathComposeAndDecompose)
{
    glm::mat4 transform = math::ComposeTransform2D(glm::vec3(2.0f, -3.0f, 0.5f), 30.0f, glm::vec3(2.0f, 0.5f, 1.0f));
    glm::vec3 translation, rotation, scale;
    CHECK(math::DecomposeTransform(transform, translation, rotation, scale));
    CHECK_APPROX(translation, glm::vec3(2.0f, -3.0f, 0.5f));
    CHECK_APPROX(scale, glm::vec3(2.0f, 0.5f, 1.0f));
    CHECK(tests::Approx(glm::degrees(rotation.z), 30.0f, 1e-3f));
}
