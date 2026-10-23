#include "pch.h"
#include "Application.h"

#include "Logging.h"

namespace Matrix::core
{
    Application::Application(const ApplicationSpec& spec)
        : m_Spec(spec)
    {
        MX_ASSERT(s_Instance == nullptr, "Only one Application may exist");
        s_Instance = this;
        if (m_Spec.Engine.Window.Title == ui::WindowSpec().Title && !m_Spec.Name.empty())
            m_Spec.Engine.Window.Title = m_Spec.Name;
        m_Engine = CreateExcl<EngineCore>(m_Spec.Engine);
    }

    Application::~Application()
    {
        if (m_Engine)
            m_Engine->Shutdown();
        s_Instance = nullptr;
    }

    Application& Application::Get()
    {
        MX_ASSERT(s_Instance, "No Application instance");
        return *s_Instance;
    }

    void Application::Close()
    {
        m_Running = false;
    }

    int Application::Run()
    {
        if (!m_Engine->Initialize())
        {
            MX_CORE_CRITICAL("'{}' could not start because the engine failed to initialize", m_Spec.Name);
            return 1;
        }

        if (!OnStart())
        {
            MX_CORE_ERROR("'{}' aborted during OnStart()", m_Spec.Name);
            OnShutdown();
            m_Engine->Shutdown();
            return 2;
        }

        m_Running = true;
        m_Timer.Reset();
        MX_CORE_INFO("'{}' is running", m_Spec.Name);

        while (m_Running && !m_Engine->IsWindowClosed())
        {
            m_Timer.Tick();
            m_Engine->BeginFrame();

            if (!m_Engine->GetWindow().IsMinimized())
            {
                OnUpdate(m_Timer.DeltaTime());

                if (auto* gui = m_Engine->GetGui())
                {
                    gui->BeginFrame();
                    OnImGuiRender();
                    gui->EndFrame();
                }
                OnPostRender();
            }

            m_Engine->EndFrame();
        }

        m_Running = false;
        OnShutdown();
        m_Engine->Shutdown();
        return 0;
    }

    void Application::SetCommandLineArgs(int argc, char** argv)
    {
        s_Args.clear();
        for (int i = 0; i < argc; ++i)
            s_Args.emplace_back(argv[i] ? argv[i] : "");
    }

    bool Application::HasArg(const std::string& flag)
    {
        return std::find(s_Args.begin(), s_Args.end(), flag) != s_Args.end();
    }

    std::string Application::GetArgValue(const std::string& flag, const std::string& defaultValue)
    {
        for (size_t i = 0; i + 1 < s_Args.size(); ++i)
        {
            if (s_Args[i] == flag)
                return s_Args[i + 1];
        }
        return defaultValue;
    }
}
