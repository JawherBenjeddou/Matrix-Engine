#include "pch.h"
#include "GuiManager.h"

#include "core/Logging.h"
#include "util/FileSystem.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

namespace Matrix::gui
{
    GuiManager::GuiManager(ui::Window& window, const GuiSpec& spec)
        : m_Window(window), m_Spec(spec)
    {
    }

    GuiManager::~GuiManager()
    {
        Shutdown();
    }

    bool GuiManager::InitSystem()
    {
        if (m_Initialized)
            return true;
        if (!m_Window.GetNativeWindow())
        {
            MX_CORE_ERROR("GuiManager requires an initialized window");
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        if (m_Spec.EnableDocking)
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        if (m_Spec.EnableViewports)
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        if (m_Spec.IniPath.empty())
        {
            io.IniFilename = nullptr;
        }
        else
        {
            std::filesystem::path ini = util::FileSystem::Resolve(m_Spec.IniPath);
            std::error_code ec;
            if (ini.has_parent_path())
                std::filesystem::create_directories(ini.parent_path(), ec);
            m_HadSavedLayout = std::filesystem::exists(ini, ec);
            m_IniPathStorage = ini.string();
            io.IniFilename = m_IniPathStorage.c_str();
        }

        // Fonts
        std::string regular = util::FileSystem::Resolve("resources/fonts/opensans/OpenSans-Regular.ttf").string();
        std::string bold = util::FileSystem::Resolve("resources/fonts/opensans/OpenSans-Bold.ttf").string();
        if (std::filesystem::exists(regular))
        {
            m_RegularFont = io.Fonts->AddFontFromFileTTF(regular.c_str(), m_Spec.FontSize);
            m_BoldFont = std::filesystem::exists(bold) ? io.Fonts->AddFontFromFileTTF(bold.c_str(), m_Spec.FontSize) : m_RegularFont;
            m_LargeFont = io.Fonts->AddFontFromFileTTF(std::filesystem::exists(bold) ? bold.c_str() : regular.c_str(), m_Spec.FontSize * 1.6f);
            io.FontDefault = m_RegularFont;
        }
        else
        {
            MX_CORE_WARN("Editor fonts not found at '{}'; falling back to the ImGui default font", regular);
            m_RegularFont = m_BoldFont = m_LargeFont = io.Fonts->AddFontDefault();
        }

        SetDarkTheme();

        ImGui_ImplGlfw_InitForOpenGL(m_Window.GetNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 450");

        m_Initialized = true;
        MX_CORE_INFO("GUI system initialized (ImGui {})", IMGUI_VERSION);
        return true;
    }

    void GuiManager::Shutdown()
    {
        if (!m_Initialized)
            return;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        m_Initialized = false;
        MX_CORE_INFO("GUI system shut down");
    }

    void GuiManager::BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GuiManager::EndFrame()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(m_Window.GetWidth()), static_cast<float>(m_Window.GetHeight()));

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }
    }

    bool GuiManager::WantCaptureMouse() const
    {
        return m_Initialized && ImGui::GetIO().WantCaptureMouse;
    }

    bool GuiManager::WantCaptureKeyboard() const
    {
        return m_Initialized && ImGui::GetIO().WantCaptureKeyboard;
    }

    void GuiManager::SetDarkTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        const ImVec4 bg        = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
        const ImVec4 bgDark    = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        const ImVec4 panel     = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
        const ImVec4 panelHov  = ImVec4(0.21f, 0.21f, 0.24f, 1.00f);
        const ImVec4 panelAct  = ImVec4(0.26f, 0.26f, 0.30f, 1.00f);
        const ImVec4 accent    = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        const ImVec4 accentHov = ImVec4(0.36f, 0.66f, 1.00f, 1.00f);
        const ImVec4 accentAct = ImVec4(0.20f, 0.50f, 0.90f, 1.00f);
        const ImVec4 text      = ImVec4(0.92f, 0.92f, 0.94f, 1.00f);
        const ImVec4 textDim   = ImVec4(0.55f, 0.55f, 0.58f, 1.00f);
        const ImVec4 border    = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);

        colors[ImGuiCol_Text] = text;
        colors[ImGuiCol_TextDisabled] = textDim;
        colors[ImGuiCol_WindowBg] = bg;
        colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.15f, 0.98f);
        colors[ImGuiCol_Border] = border;
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_FrameBg] = panel;
        colors[ImGuiCol_FrameBgHovered] = panelHov;
        colors[ImGuiCol_FrameBgActive] = panelAct;
        colors[ImGuiCol_TitleBg] = bgDark;
        colors[ImGuiCol_TitleBgActive] = bgDark;
        colors[ImGuiCol_TitleBgCollapsed] = bgDark;
        colors[ImGuiCol_MenuBarBg] = bgDark;
        colors[ImGuiCol_ScrollbarBg] = bgDark;
        colors[ImGuiCol_ScrollbarGrab] = panelHov;
        colors[ImGuiCol_ScrollbarGrabHovered] = panelAct;
        colors[ImGuiCol_ScrollbarGrabActive] = accent;
        colors[ImGuiCol_CheckMark] = accent;
        colors[ImGuiCol_SliderGrab] = accent;
        colors[ImGuiCol_SliderGrabActive] = accentHov;
        colors[ImGuiCol_Button] = panel;
        colors[ImGuiCol_ButtonHovered] = panelHov;
        colors[ImGuiCol_ButtonActive] = panelAct;
        colors[ImGuiCol_Header] = panel;
        colors[ImGuiCol_HeaderHovered] = panelHov;
        colors[ImGuiCol_HeaderActive] = panelAct;
        colors[ImGuiCol_Separator] = border;
        colors[ImGuiCol_SeparatorHovered] = accent;
        colors[ImGuiCol_SeparatorActive] = accentHov;
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_ResizeGripHovered] = accent;
        colors[ImGuiCol_ResizeGripActive] = accentHov;
        colors[ImGuiCol_Tab] = bgDark;
        colors[ImGuiCol_TabHovered] = panelHov;
        colors[ImGuiCol_TabActive] = bg;
        colors[ImGuiCol_TabUnfocused] = bgDark;
        colors[ImGuiCol_TabUnfocusedActive] = bg;
        colors[ImGuiCol_DockingPreview] = ImVec4(accent.x, accent.y, accent.z, 0.6f);
        colors[ImGuiCol_DockingEmptyBg] = bgDark;
        colors[ImGuiCol_PlotLines] = accent;
        colors[ImGuiCol_PlotLinesHovered] = accentHov;
        colors[ImGuiCol_PlotHistogram] = accent;
        colors[ImGuiCol_PlotHistogramHovered] = accentHov;
        colors[ImGuiCol_TableHeaderBg] = panel;
        colors[ImGuiCol_TableBorderStrong] = border;
        colors[ImGuiCol_TableBorderLight] = ImVec4(border.x, border.y, border.z, 0.5f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
        colors[ImGuiCol_DragDropTarget] = accentHov;
        colors[ImGuiCol_NavHighlight] = accent;
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.4f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.CellPadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
        style.IndentSpacing = 18.0f;
        style.ScrollbarSize = 13.0f;
        style.GrabMinSize = 10.0f;
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;
        style.WindowRounding = 4.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 3.0f;
        style.PopupRounding = 4.0f;
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 4.0f;
        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.DockingSeparatorSize = 2.0f;

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            colors[ImGuiCol_WindowBg].w = 1.0f;
        }
    }
}
