# Matrix Engine architecture

This document is a tour of the engine for people who want to extend it. Paths are relative to `MatrixEngine/src/mx/`
unless noted otherwise.

## Big picture

```
+--------------------------------------------------------------+
|  Applications: Editor, Sandbox, Tests, your game              |
|  (derive from core::Application, link Matrix.dll)             |
+--------------------------------------------------------------+
|  core::Application  ->  core::EngineCore                      |
|     frame loop          owns Window, Input, GuiManager,       |
|     OnStart/OnUpdate/   Renderer2D, AudioEngine, script       |
|     OnImGuiRender/...   registry; sequences init/shutdown     |
+---------------+----------------------+-----------------------+
| graphics::World (scene)              | physics::Physics2D    |
|  entt registry + hierarchy           |  reads/writes         |
|  components, scripts, serialization  |  components           |
+---------------+----------------------+-----------------------+
| graphics::Renderer2D  | audio::AudioEngine | ui::Input/Window |
+--------------------------------------------------------------+
| OpenGL 4.5 / GLFW / GLEW / miniaudio / ImGui / EnTT / spdlog  |
+--------------------------------------------------------------+
```

The engine is a DLL. Every public class is marked with `MATRIX_DLL` (see `core/Export.h`). Applications include the
umbrella header `Matrix.h` and, in exactly one translation unit, `core/EntryPoint.h`, which defines `main()`.

## Frame loop (`core/Application.cpp`)

```
Timer.Tick()
EngineCore.BeginFrame()      // Input::NewFrame, glfwPollEvents, clear back buffer
Application::OnUpdate(dt)    // game / editor logic, scene rendering
Gui.BeginFrame(); Application::OnImGuiRender(); Gui.EndFrame()
Application::OnPostRender()  // e.g. screenshots of the finished frame
EngineCore.EndFrame()        // AudioEngine::Update, swap buffers
```

Minimised windows skip the update and render steps. `Application::Close()` ends the loop; `EngineCore::Shutdown()`
releases subsystems in reverse order (audio, renderer and GPU resources, GUI, window) while the OpenGL context is alive.

## Core (`core/`, `util/`, `ui/`)

- `Logging` wraps spdlog with three sinks: coloured console, a log file (`logs/*.log` under the project root) and an
  in-memory ring buffer that the editor console reads through `Logging::GetHistory()`.
- `FileSystem::Init()` walks up from the executable directory and the working directory until it finds a folder
  containing `resources/shaders`; that folder is the project root. All asset paths stored in scenes are relative to it
  (`FileSystem::MakeRelative`, `FileSystem::Resolve`).
- `Window` owns the GLFW window and GL context and forwards callbacks to the static `Input` state. `Input` keeps
  "down" state plus per-frame "pressed"/"released" edges which are cleared in `Input::NewFrame()` before events are pumped.
- `Event` is a small string-keyed publish/subscribe bus (`BIND_EVENT_FUNCTION`, `BROADCAST_EVENT`).
- `Timer` provides a clamped delta time (max 0.1 s) so simulations do not explode after stalls.

## Rendering (`graphics/`)

`Renderer2D` is a static batch renderer. Draw calls between `BeginScene(viewProjection)` and `EndScene()` append
vertices to CPU-side arrays; `Flush()` uploads them to dynamic vertex buffers and issues one draw call per primitive
type (quads, circles, lines). Textures are bound to up to 16 slots per batch; a new batch starts when the slots or the
10,000-quad limit are exhausted. Every vertex carries an integer entity id which the quad/circle/line shaders write to
a second colour attachment, enabling pixel-exact mouse picking in the editor (`Framebuffer::ReadPixel`).

Shaders live in `resources/shaders/Renderer2D_*.{vert,frag}` and are compiled by `ShaderFactory` (which also supports
hot reload). `TextureLibrary` caches textures by root-relative path and hands out a magenta checkerboard for files that
fail to load so broken references are visible rather than fatal.

Cameras: `OrthoCamera` (view = inverse of the camera transform, projection from zoom = visible height and aspect ratio),
`EditorCamera` (input handling on top of `OrthoCamera`), `SceneCamera` (projection only; the view comes from the
owning entity's world transform).

## World / ECS (`graphics/world/`)

`World` owns an `entt::registry`, a UUID -> entity map and a creation-order list. `Entity` is a copyable value type
(handle + world pointer); its component helpers are templates defined at the bottom of `World.h`.

Components (`Components.h`) are plain structs. `AllComponents` lists the ones copied during duplication and world
copy, so adding a component usually means:

1. add the struct to `Components.h` and to `AllComponents`,
2. (de)serialize it in `WorldSerializer.cpp`,
3. draw it in `Editor/src/Panels/InspectorPanel.cpp` and add it to the "Add Component" menu,
4. consume it in the relevant system (renderer, physics, audio...).

Hierarchy is stored in `RelationshipComponent` by UUID. `World::GetWorldTransform` multiplies the local transforms up
the parent chain; `SetWorldTransform` converts a world matrix back into local position/rotation/scale, which is how
re-parenting keeps entities in place and how physics moves child bodies.

Runtime: `OnRuntimeStart()` instantiates scripts (`ScriptRegistry::Instantiate` by name), starts `PlayOnStart` audio
sources and resets physics. `OnUpdateRuntime(dt)` runs `ScriptableEntity::OnUpdate` for every script, then
`Physics2D::Step(dt)`. `OnRuntimeStop()` destroys script instances and stops audio. The editor never runs the edited
world: it plays a `World::Copy` and throws it away on stop.

Rendering a world (`RenderScene`) sorts sprites back-to-front by world Z, resolves `SpriteComponent::TexturePath` to a
texture lazily and submits everything to `Renderer2D` with the entity handle as id.

## Physics (`physics/Physics2D.cpp`)

A deliberately small solver:

1. **Integrate**: dynamic bodies accumulate gravity (scaled per body) and damping, then move by velocity; kinematic
   bodies move by velocity only.
2. **Collect** colliders in world space (box colliders become axis-aligned bounding boxes; rotation is ignored).
3. **Resolve** overlapping pairs for a few iterations: the minimum translation vector is split by inverse mass, an
   impulse with restitution cancels the approaching velocity, tangential velocity is damped by friction, and
   `IsGrounded` is set when the contact normal points up.
4. **Apply** accumulated position corrections (through the hierarchy-aware `TranslateWorld`) and deliver
   `OnCollision` callbacks to scripts once per pair.

`Step(dt)` uses a fixed 1/60 s timestep with an accumulator and at most 8 sub-steps per frame.

## Scripting (`graphics/world/ScriptableEntity.h`, `ScriptRegistry.h`, `scripting/`)

Scripts are C++ classes deriving from `ScriptableEntity`, registered by name with `MX_REGISTER_SCRIPT(Class, {"Param",
default}, ...)`. A `NativeScriptComponent` stores the script name and a map of float parameters; the inspector shows the
registered defaults plus any extra values. Built-in scripts are registered by `EngineCore::Initialize()` through
`scripting::RegisterBuiltinScripts()`.

## Audio (`audio/AudioEngine.cpp`)

A static facade over a single `ma_engine`. `Play()` decodes the file (WAV/MP3/FLAC) into an `ma_sound`, returns a
handle, and `Update()` releases finished one-shots every frame. The implementation of miniaudio is compiled once in
`thirdparty/miniaudio_impl.cpp` with only the Windows back-ends enabled.

## Editor (`Editor/src/`)

`EditorApp` hosts a single `EditorLayer`, which owns an `EditorState` (worlds, selection, camera, panel flags) shared by
the panels. Each frame the layer renders the active world plus overlays (grid, colliders, camera frustums, selection) into
a `Framebuffer` with an entity-id attachment, reads the id under the cursor for hover/selection, handles viewport
dragging (move/rotate/scale), applies keyboard shortcuts, and finally draws the dockspace, toolbar, viewport image and
panels with Dear ImGui. The default layout is built with the ImGui dock builder on first run and stored in
`build/editor_layout.ini` afterwards.

Panels talk to the rest of the editor through `EditorState` and a couple of callbacks (open scene, focus entity), so
new panels only need a reference to the state.

## Build (`premake5.lua`, `scripts/setup.py`)

Premake generates one workspace with four projects. `UseMatrixEngine()` in the root script applies the include paths,
defines and link settings every consumer of the DLL needs. ImGui is compiled into the engine straight from the submodule
with `IMGUI_API` set to export, and applications import it, so a single ImGui context is shared. Third-party
translation units are compiled without the engine precompiled header.
