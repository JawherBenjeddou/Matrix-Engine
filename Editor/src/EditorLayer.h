#pragma once
///@file EditorLayer.h  The editor itself: dockspace, viewport, play mode, scene file handling and panels.

#include "EditorState.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/StatsPanel.h"

namespace Matrix::editor
{
    class EditorLayer
    {
    public:
        EditorLayer();
        ~EditorLayer();

        bool OnAttach();
        void OnDetach();
        void OnUpdate(float deltaTime);
        void OnImGuiRender();
        void OnPostRender();

        // --- Scene file handling -----------------------------------------------------------
        void NewScene();
        bool OpenScene(const std::filesystem::path& path);
        void OpenSceneDialog();
        bool SaveScene();
        bool SaveSceneAs();

        // --- Play mode ---------------------------------------------------------------------
        void OnScenePlay();
        void OnScenePause();
        void OnSceneStop();
        void OnSceneStep();

        /// Captures the editor window to docs/screenshots (or the given path).
        bool CaptureScreenshot(const std::filesystem::path& path = {});

        void FocusEntity(graphics::Entity entity);

    private:
        void RenderViewport();
        void DrawOverlays(const glm::mat4& viewProjection);
        void DrawGrid();
        void HandleViewportInput();
        void HandleShortcuts();
        void UpdateMousePicking();

        void DrawDockspace();
        void DrawMenuBar();
        void DrawToolbar();
        void DrawViewportPanel();
        void DrawAboutWindow();
        void DrawControlsWindow();
        void BuildDefaultLayout(unsigned int dockspaceId);
        void UpdateWindowTitle();
        glm::mat4 GetViewProjection() const;
        glm::vec2 GetMouseInViewport() const;

        EditorState m_State;
        ExclObj<graphics::Framebuffer> m_Framebuffer;

        SceneHierarchyPanel m_HierarchyPanel;
        InspectorPanel m_InspectorPanel;
        ContentBrowserPanel m_ContentBrowserPanel;
        ConsolePanel m_ConsolePanel;
        StatsPanel m_StatsPanel;

        bool m_LayoutBuilt = false;
        bool m_ResetLayoutRequested = false;

        // Gizmo dragging
        bool m_Dragging = false;
        glm::vec2 m_DragLastWorld{ 0.0f };
        glm::vec2 m_DragLastMouse{ 0.0f };

        // Screenshots are captured in OnPostRender() once the whole frame (including the UI) has been drawn.
        std::string m_PendingScreenshotPath;
        bool m_CloseAfterScreenshot = false;

        // Automated screenshot support (--screenshot <path> [--frames N] [--play])
        std::string m_AutoScreenshotPath;
        int m_AutoScreenshotFrame = 0;
        int m_FrameCounter = 0;
    };
}
