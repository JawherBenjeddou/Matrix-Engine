#include "pch.h"
#include "EngineCore.h"

#include "Logging.h"
#include "audio/AudioEngine.h"
#include "graphics/renderer/RenderCommand.h"
#include "graphics/renderer/Renderer2D.h"
#include "graphics/shaders/ShaderFactory.h"
#include "graphics/textures/TextureLibrary.h"
#include "scripting/BuiltinScripts.h"
#include "util/FileSystem.h"

namespace Matrix::core
{
    EngineCore::EngineCore(const EngineSpec& spec)
        : m_Spec(spec)
    {
    }

    EngineCore::~EngineCore()
    {
        Shutdown();
    }

    bool EngineCore::Initialize()
    {
        if (m_Initialized)
            return true;

        util::FileSystem::Init();
        std::string logFile = m_Spec.LogFile.empty() ? std::string() : util::FileSystem::Resolve(m_Spec.LogFile).string();
        Logging::Init(logFile);
        LogSystemInfo();

        m_Window = CreateExcl<ui::Window>(m_Spec.Window);
        if (!m_Window->InitSystem())
        {
            MX_CORE_CRITICAL("Engine initialization failed: no window");
            return false;
        }
        m_Window->GetInfo();
        ui::Input::InitSystem();

        if (m_Spec.EnableGui)
        {
            m_Gui = CreateExcl<gui::GuiManager>(*m_Window, m_Spec.Gui);
            if (!m_Gui->InitSystem())
            {
                MX_CORE_ERROR("GUI could not be initialized; continuing without it");
                m_Gui.reset();
            }
        }

        graphics::Renderer2D::Init();

        if (m_Spec.EnableAudio)
            m_AudioEnabled = audio::AudioEngine::Init();

        scripting::RegisterBuiltinScripts();

        m_Initialized = true;
        MX_CORE_INFO("Engine initialization completed");
        return true;
    }

    void EngineCore::Shutdown()
    {
        if (!m_Initialized)
        {
            // Partial initialization (e.g. window failed): still release what exists.
            m_Gui.reset();
            m_Window.reset();
            return;
        }

        MX_CORE_INFO("Engine shutting down");
        if (m_AudioEnabled)
            audio::AudioEngine::Shutdown();

        // GPU resources must be released while the OpenGL context is alive.
        graphics::Renderer2D::Shutdown();
        graphics::TextureLibrary::Get().Clear();
        graphics::ShaderFactory::GetInstance().Cleanup();

        m_Gui.reset();
        m_Window.reset();
        m_Initialized = false;
    }

    void EngineCore::BeginFrame()
    {
        m_Window->PollEvents();
        graphics::RenderCommand::SetViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
        graphics::RenderCommand::SetClearColor(m_Spec.ClearColor);
        graphics::RenderCommand::Clear();
    }

    void EngineCore::EndFrame()
    {
        if (m_AudioEnabled)
            audio::AudioEngine::Update();
        m_Window->SwapBuffers();
    }

    bool EngineCore::IsWindowClosed() const
    {
        return !m_Window || m_Window->Closed();
    }

    void EngineCore::LogSystemInfo() const
    {
        MX_CORE_INFO("{} v{}", MX_ENGINE_NAME, MX_VERSION_STRING);
#ifdef MX_DEBUG_MODE
        MX_CORE_INFO("Configuration : Debug");
#else
        MX_CORE_INFO("Configuration : Release");
#endif
        MX_CORE_INFO("Project root  : {}", util::FileSystem::GetRootDirectory().string());
    }
}
