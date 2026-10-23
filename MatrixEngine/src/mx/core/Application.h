#pragma once
#include "Common.h"
///@file Application.h  Base class for programs built on the engine (editor, games, tools).

#include "EngineCore.h"
#include "util/Timer.h"

namespace Matrix::core
{
    struct MATRIX_DLL ApplicationSpec
    {
        std::string Name = "Matrix Application";
        EngineSpec Engine;
    };

    /// Derive from Application, override the hooks and use IMPLEMENT_APPLICATION_CLASS to create the entry point.
    class MATRIX_DLL Application
    {
    public:
        explicit Application(const ApplicationSpec& spec = ApplicationSpec());
        virtual ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        /// Initializes the engine, runs the main loop until Close() or the window closes, then shuts down.
        /// Returns the process exit code.
        int Run();

        /// Requests the main loop to stop after the current frame.
        void Close();

        static Application& Get();
        static bool HasInstance() { return s_Instance != nullptr; }

        const ApplicationSpec& GetSpec() const { return m_Spec; }
        EngineCore& GetEngine() { return *m_Engine; }
        ui::Window& GetWindow() { return m_Engine->GetWindow(); }
        const util::Timer& GetTimer() const { return m_Timer; }
        float GetDeltaTime() const { return m_Timer.DeltaTime(); }
        bool IsRunning() const { return m_Running; }

        // --- Command line ---------------------------------------------------------------
        static void SetCommandLineArgs(int argc, char** argv);
        static const std::vector<std::string>& GetCommandLineArgs() { return s_Args; }
        /// True if "--flag" is present.
        static bool HasArg(const std::string& flag);
        /// Value following "--flag", or the default.
        static std::string GetArgValue(const std::string& flag, const std::string& defaultValue = std::string());

    protected:
        /// Called once after the engine initialized. Return false to abort start-up.
        virtual bool OnStart() { return true; }
        /// Called every frame with the frame delta time in seconds.
        virtual void OnUpdate(float deltaTime) { (void)deltaTime; }
        /// Called every frame between ImGui NewFrame/Render (only when the GUI is enabled).
        virtual void OnImGuiRender() {}
        /// Called after everything (including the GUI) has been drawn, right before the frame is presented.
        virtual void OnPostRender() {}
        /// Called once before the engine shuts down.
        virtual void OnShutdown() {}

    private:
        ApplicationSpec m_Spec;
        ExclObj<EngineCore> m_Engine;
        util::Timer m_Timer;
        bool m_Running = false;

        inline static Application* s_Instance = nullptr;
        inline static std::vector<std::string> s_Args;
    };

    /// Implemented by the client: returns the application to run.
    Application* CreateApplication();
}

/// Generates CreateApplication() for a concrete Application subclass.
#define IMPLEMENT_APPLICATION_CLASS(appClass) \
    Matrix::core::Application* Matrix::core::CreateApplication() \
    { \
        return new appClass(); \
    }
