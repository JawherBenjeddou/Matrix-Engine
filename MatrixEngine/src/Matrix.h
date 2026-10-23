#pragma once
/***************************************************************************
                    Matrix Engine - public umbrella header
    Include this from applications. Add #include "core/EntryPoint.h" in
    exactly one translation unit (or use IMPLEMENT_APPLICATION_CLASS there).
***************************************************************************/

#include "Common.h"
#include "core/Version.h"
#include "core/Logging.h"
#include "core/UUID.h"
#include "core/Event.h"
#include "core/EngineCore.h"
#include "core/Application.h"

#include "math/Math.h"
#include "util/Timer.h"
#include "util/FileSystem.h"
#include "util/PlatformUtils.h"

#include "ui/Window.h"
#include "ui/Input.h"
#include "gui/GuiManager.h"

#include "graphics/renderer/RenderCommand.h"
#include "graphics/renderer/Renderer2D.h"
#include "graphics/buffers/Framebuffer.h"
#include "graphics/shaders/ShaderFactory.h"
#include "graphics/textures/Texture2D.h"
#include "graphics/textures/TextureLibrary.h"
#include "graphics/cameras/OrthoCamera.h"
#include "graphics/cameras/EditorCamera.h"
#include "graphics/cameras/SceneCamera.h"

#include "graphics/world/Components.h"
#include "graphics/world/Entity.h"
#include "graphics/world/World.h"
#include "graphics/world/WorldSerializer.h"
#include "graphics/world/ScriptableEntity.h"
#include "graphics/world/ScriptRegistry.h"

#include "physics/Physics2D.h"
#include "audio/AudioEngine.h"
#include "scripting/BuiltinScripts.h"
