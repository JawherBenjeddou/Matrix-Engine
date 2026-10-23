#include "Matrix.h"

/// Sandbox: runs a scene without the editor, the way a shipped game would.
///
///   Sandbox.exe [--scene assets/scenes/Demo.mxscene] [--screenshot path.png --frames N]
///
/// Controls: A/D or arrows move, Space jumps (PlatformerController2D), R restarts the scene,
/// F12 saves a screenshot to screenshots/, Escape quits.
class SandboxApp : public Matrix::core::Application
{
public:
    SandboxApp()
        : Application(MakeSpec())
    {
    }

protected:
    bool OnStart() override
    {
        m_ScenePath = GetArgValue("--scene", "assets/scenes/Demo.mxscene");
        m_ScreenshotPath = GetArgValue("--screenshot");
        m_ScreenshotFrame = std::max(5, std::atoi(GetArgValue("--frames", "90").c_str()));

        GetWindow().SetResizeCallback([this](uint32_t width, uint32_t height)
        {
            if (m_World)
                m_World->OnViewportResize(width, height);
        });
        return LoadScene();
    }

    void OnUpdate(float deltaTime) override
    {
        using Matrix::ui::Input;
        if (Input::IsKeyPressed(Key::ESCAPE))
            Close();
        if (Input::IsKeyPressed(Key::R))
            LoadScene();
        if (Input::IsKeyPressed(Key::F12))
            m_PendingScreenshot = (Matrix::util::FileSystem::GetRootDirectory() / "screenshots" / "Sandbox.png").string();

        Matrix::graphics::Renderer2D::ResetStats();
        m_World->OnUpdateRuntime(deltaTime);

        Matrix::graphics::RenderCommand::SetClearColor(m_World->BackgroundColor);
        Matrix::graphics::RenderCommand::Clear();
        if (!m_World->OnRenderRuntime())
        {
            // No camera in the scene: fall back to a fixed orthographic view so something is visible.
            m_World->OnRenderEditor(m_FallbackCamera.GetViewProjectionMatrix());
        }

        if (!m_ScreenshotPath.empty() && ++m_FrameCounter == m_ScreenshotFrame)
        {
            m_PendingScreenshot = Matrix::util::FileSystem::Resolve(m_ScreenshotPath).string();
            m_CloseAfterScreenshot = true;
        }
    }

    void OnPostRender() override
    {
        if (m_PendingScreenshot.empty())
            return;
        GetWindow().CaptureScreenshot(m_PendingScreenshot);
        m_PendingScreenshot.clear();
        if (m_CloseAfterScreenshot)
            Close();
    }

    void OnShutdown() override
    {
        if (m_World)
            m_World->OnRuntimeStop();
        m_World.reset();
    }

private:
    bool LoadScene()
    {
        if (m_World)
            m_World->OnRuntimeStop();

        m_World = Matrix::CreateShared<Matrix::graphics::World>();
        Matrix::graphics::WorldSerializer serializer(m_World);
        if (!serializer.Deserialize(m_ScenePath))
        {
            MX_ERROR("Sandbox could not load '{}'", m_ScenePath);
            return false;
        }
        m_World->OnViewportResize(GetWindow().GetWidth(), GetWindow().GetHeight());
        m_FallbackCamera.SetViewportSize(GetWindow().GetWidth(), GetWindow().GetHeight());
        m_World->OnRuntimeStart();
        GetWindow().SetTitle("Matrix Sandbox - " + m_World->GetName());
        return true;
    }

    static Matrix::core::ApplicationSpec MakeSpec()
    {
        Matrix::core::ApplicationSpec spec;
        spec.Name = "Matrix Sandbox";
        spec.Engine.Window.Title = "Matrix Sandbox";
        spec.Engine.Window.Width = 1280;
        spec.Engine.Window.Height = 720;
        spec.Engine.EnableGui = false;
        spec.Engine.LogFile = "logs/MatrixSandbox.log";
        return spec;
    }

    Matrix::SharedObj<Matrix::graphics::World> m_World;
    Matrix::graphics::OrthoCamera m_FallbackCamera{ 16.0f / 9.0f, 14.0f };
    std::string m_ScenePath;
    std::string m_ScreenshotPath;
    std::string m_PendingScreenshot;
    bool m_CloseAfterScreenshot = false;
    int m_ScreenshotFrame = 0;
    int m_FrameCounter = 0;
};

IMPLEMENT_APPLICATION_CLASS(SandboxApp)

#include "core/EntryPoint.h"
