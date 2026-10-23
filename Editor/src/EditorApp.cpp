#include "Matrix.h"
#include "EditorLayer.h"

/// The Matrix Editor application: hosts a single EditorLayer.
class EditorApp : public Matrix::core::Application
{
public:
    EditorApp()
        : Application(MakeSpec())
    {
    }

protected:
    bool OnStart() override
    {
        m_Layer = Matrix::CreateExcl<Matrix::editor::EditorLayer>();
        return m_Layer->OnAttach();
    }

    void OnUpdate(float deltaTime) override
    {
        m_Layer->OnUpdate(deltaTime);
    }

    void OnImGuiRender() override
    {
        m_Layer->OnImGuiRender();
    }

    void OnPostRender() override
    {
        m_Layer->OnPostRender();
    }

    void OnShutdown() override
    {
        if (m_Layer)
            m_Layer->OnDetach();
        m_Layer.reset();
    }

private:
    static Matrix::core::ApplicationSpec MakeSpec()
    {
        Matrix::core::ApplicationSpec spec;
        spec.Name = "Matrix Editor";
        spec.Engine.Window.Title = "Matrix Editor";
        spec.Engine.Window.Width = 1600;
        spec.Engine.Window.Height = 900;
        spec.Engine.Window.Maximized = !Matrix::core::Application::HasArg("--windowed");
        spec.Engine.Window.VSync = true;
        spec.Engine.Gui.EnableDocking = true;
        spec.Engine.Gui.EnableViewports = Matrix::core::Application::HasArg("--viewports");
        spec.Engine.Gui.IniPath = "build/editor_layout.ini";
        spec.Engine.LogFile = "logs/MatrixEditor.log";
        return spec;
    }

    Matrix::ExclObj<Matrix::editor::EditorLayer> m_Layer;
};

IMPLEMENT_APPLICATION_CLASS(EditorApp)

#include "core/EntryPoint.h"
