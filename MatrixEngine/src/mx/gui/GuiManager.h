#pragma once
#include "Common.h"
///@file GuiManager.h  Dear ImGui integration (context, fonts, theme, frame begin/end).

#include "ui/Window.h"
#include "imgui.h"

namespace Matrix::gui
{
    struct MATRIX_DLL GuiSpec
    {
        bool EnableDocking = true;
        /// Multi-viewport (windows dragged outside the main window). Off by default for stability.
        bool EnableViewports = false;
        float FontSize = 17.0f;
        /// Where ImGui stores its layout. Empty disables persistence.
        std::string IniPath = "build/editor_layout.ini";
    };

    /// Owns the ImGui context. Create one per application.
    class MATRIX_DLL GuiManager
    {
    public:
        GuiManager(ui::Window& window, const GuiSpec& spec = GuiSpec());
        ~GuiManager();

        GuiManager(const GuiManager&) = delete;
        GuiManager& operator=(const GuiManager&) = delete;

        bool InitSystem();
        void Shutdown();

        /// Starts a new ImGui frame.
        void BeginFrame();
        /// Renders the frame (and platform windows when viewports are enabled).
        void EndFrame();

        /// Applies the Matrix dark theme to the current ImGui style.
        static void SetDarkTheme();

        bool WantCaptureMouse() const;
        bool WantCaptureKeyboard() const;

        ImFont* GetRegularFont() const { return m_RegularFont; }
        ImFont* GetBoldFont() const { return m_BoldFont; }
        ImFont* GetLargeFont() const { return m_LargeFont; }

        bool IsInitialized() const { return m_Initialized; }
        const GuiSpec& GetSpec() const { return m_Spec; }

        /// True if the layout ini file existed when the GUI was initialized (used to decide whether to build a default layout).
        bool HadSavedLayout() const { return m_HadSavedLayout; }

    private:
        ui::Window& m_Window;
        GuiSpec m_Spec;
        std::string m_IniPathStorage;
        ImFont* m_RegularFont = nullptr;
        ImFont* m_BoldFont = nullptr;
        ImFont* m_LargeFont = nullptr;
        bool m_Initialized = false;
        bool m_HadSavedLayout = false;
    };
}
