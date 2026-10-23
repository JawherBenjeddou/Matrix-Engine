# Changelog

All notable changes to Matrix Engine are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and the project uses
[Semantic Versioning](https://semver.org/).

## [0.1.0] - 2026-10-23

First stable release. The project was revived from the unfinished 2024 prototype ("Version 0.0") and rebuilt into a
complete, tested 2D engine while keeping its architecture (engine DLL + applications, `Matrix` namespaces, Premake build).

### Added
- **Build system**: `build.bat` / `scripts/setup.py` bootstrap (fetches submodules and Premake, generates the solution,
  builds), Visual Studio 2022 and 2026 support, Debug and Release configurations with no compiler warnings,
  `Sandbox` and `Tests` projects.
- **Core**: `Application` lifecycle hooks (`OnStart`, `OnUpdate`, `OnImGuiRender`, `OnPostRender`, `OnShutdown`),
  command-line argument helpers, project-root aware `FileSystem`, 64-bit `UUID`, `Timer` with FPS tracking and delta
  clamping, log file + in-memory log history, assertion/verification macros that report through the logger.
- **Input**: per-frame pressed/released edges, scroll wheel, mouse delta, modifier helpers, full key code set.
- **Window**: resize handling, VSync toggle, window icon, screenshot capture to PNG, OpenGL 4.5 core context with
  debug output in Debug builds.
- **Renderer2D**: batched quads (10k per draw call, 16 texture slots), anti-aliased circles, lines and rectangles,
  entity-id output for picking, statistics. `Framebuffer`, `BufferLayout`/`VertexArray` abstraction, `RenderCommand`.
- **Textures**: `TextureLibrary` cache, white and "missing texture" placeholders, wrap/filter/mipmap options.
- **Cameras**: `OrthoCamera` with correct inverse view matrix, zoom, aspect and screen/world conversions;
  `EditorCamera` (pan, zoom to cursor, keyboard) and `SceneCamera` for camera components.
- **World / ECS**: `Entity` value handles, `IDComponent`, `TagComponent`, `TransformComponent`,
  `RelationshipComponent` (hierarchy with world transforms), `SpriteComponent` (per-axis tiling),
  `CircleRendererComponent`, `CameraComponent`, `RigidBody2DComponent`, `BoxCollider2DComponent`,
  `CircleCollider2DComponent`, `NativeScriptComponent` (with float parameters), `AudioSourceComponent`;
  duplication, world copy, runtime start/stop/pause/step, primary camera rendering.
- **Serialization**: JSON `.mxscene` files via `WorldSerializer` with tolerant loading of partial or hand-edited files.
- **Physics2D**: built-in fixed-timestep simulation with static/dynamic/kinematic bodies, box and circle colliders,
  triggers, restitution, friction, damping, grounded detection and script collision callbacks.
- **Scripting**: `ScriptableEntity` base class, `ScriptRegistry` with `MX_REGISTER_SCRIPT`, built-in `Rotator`,
  `Oscillator`, `PlatformerController2D` and `CameraFollow2D` scripts.
- **Audio**: miniaudio based `AudioEngine` (WAV/MP3/FLAC, volume, pitch, looping, master volume).
- **Editor**: dockable layout with Scene Hierarchy, Viewport, Inspector, Content Browser, Console, Statistics and
  Toolbar panels; mouse picking; move/rotate/scale tools with snapping; drag-and-drop re-parenting and asset drops;
  play/pause/step/stop with scene snapshots; collider, camera and selection overlays; scene settings; native file
  dialogs; screenshot capture; About and Controls windows; default dock layout and `Reset Layout`.
- **Sandbox**: standalone runtime that loads and plays a scene.
- **Tests**: self-contained test runner with 45 tests (core, world, hierarchy, serialization, physics, cameras).
- **Demo content**: `assets/scenes/Demo.mxscene` plus procedurally generated textures and sounds
  (`scripts/generate_sample_assets.py`, `scripts/generate_demo_scene.py`).
- **Documentation**: rewritten README with real screenshots, `docs/ARCHITECTURE.md`, this changelog, Doxygen config
  pointing at the current sources.

### Changed
- Dear ImGui is compiled from the `Dependencies/imgui` submodule instead of a copied snapshot inside the engine sources.
- Dependencies are proper git submodules (`entt`, `glm`, `imgui`, `spdlog`, `stb`); `spdlog` updated to 1.15.3 for
  current MSVC compatibility. `nlohmann/json` and `miniaudio` are vendored as single headers.
- All binaries are built into `build/bin/<Config>/` so the engine DLL sits next to the executables (no copy steps).
- `PSRComponent` is now `TransformComponent` (the old name remains as an alias); the transform matrix is composed in
  the correct translate * rotate * scale order.
- `World::SpawnEntity` returns a lightweight `Entity` handle instead of a shared pointer (the method still exists).
- Shaders moved to the batched `Renderer2D_*` programs; the old `default.vert/frag` were removed.

### Fixed
- Precompiled header path in the Premake script (the project did not build at all).
- Double destruction of the window on shutdown and logging after the logger was shut down.
- Delta time was reported in milliseconds and reset every frame; it is now seconds, clamped, and measured correctly.
- Texture channel detection never ran (`m_NrComponents` was unset); all textures are now uploaded as RGBA8 correctly.
- Camera view matrix was not inverted, so moving the camera moved the world the wrong way; zoom had no effect.
- The vertex array object was never created, which fails in a core-profile context.
- Missing window icon file; the icon is now generated from the existing branding.
- Static `World::FindEntityByName` returned nothing on the "not found" path (undefined behaviour).

### Removed
- Generated Doxygen HTML (`docs/html`) and the copied ImGui sources from the repository.
- `__init__.py` dependency script (replaced by `scripts/setup.py`).

## [0.0] - 2024-02-05

Original prototype: window, logging, ImGui demo window, a textured quad, early ECS experiments and a Premake build.
