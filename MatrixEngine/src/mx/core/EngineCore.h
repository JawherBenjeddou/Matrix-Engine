#pragma once
#include "Common.h"
///@file EngineCore.h  Owns and sequences every engine subsystem.

#include "ui/Window.h"
#include "gui/GuiManager.h"

namespace Matrix::core
{
    struct MATRIX_DLL EngineSpec
    {
        ui::WindowSpec Window;
        gui::GuiSpec Gui;
        bool EnableGui = true;
        bool EnableAudio = true;
        /// Root-relative log file path (empty disables the file sink).
        std::string LogFile = "logs/MatrixEngine.log";
        glm::vec4 ClearColor = glm::vec4(0.09f, 0.09f, 0.10f, 1.0f);
    };

    class MATRIX_DLL EngineCore
    {
    public:
        explicit EngineCore(const EngineSpec& spec = EngineSpec());
        ~EngineCore();

        EngineCore(const EngineCore&) = delete;
        EngineCore& operator=(const EngineCore&) = delete;

        /// Initializes every subsystem in order. Returns false if a required subsystem failed.
        bool Initialize();

        /// Shuts every subsystem down in reverse order. Safe to call more than once.
        void Shutdown();

        /// Polls input/OS events and clears the back buffer.
        void BeginFrame();

        /// Updates audio bookkeeping and presents the frame.
        void EndFrame();

        bool IsWindowClosed() const;
        bool IsInitialized() const { return m_Initialized; }

        ui::Window& GetWindow() { return *m_Window; }
        /// nullptr when the GUI is disabled.
        gui::GuiManager* GetGui() { return m_Gui.get(); }
        const EngineSpec& GetSpec() const { return m_Spec; }

        void LogSystemInfo() const;

    private:
        EngineSpec m_Spec;
        ExclObj<ui::Window> m_Window;
        ExclObj<gui::GuiManager> m_Gui;
        bool m_Initialized = false;
        bool m_AudioEnabled = false;
    };
}
