#pragma once
///@file EditorState.h  State shared between the editor layer and its panels.

#include "Matrix.h"

namespace Matrix::editor
{
    enum class GizmoMode { Translate = 0, Rotate, Scale };

    struct EditorState
    {
        /// Scene being edited (always valid).
        SharedObj<graphics::World> EditorWorld;
        /// Copy of the editor scene that is simulated during play mode (null when not playing).
        SharedObj<graphics::World> RuntimeWorld;

        /// The world currently shown in the viewport and panels.
        const SharedObj<graphics::World>& ActiveWorld() const { return RuntimeWorld ? RuntimeWorld : EditorWorld; }
        bool IsPlaying() const { return RuntimeWorld != nullptr; }

        graphics::Entity Selected;
        graphics::Entity Hovered;

        graphics::EditorCamera Camera;
        GizmoMode Gizmo = GizmoMode::Translate;

        bool ShowGrid = true;
        bool ShowColliders = true;
        bool ShowSelectionOutline = true;
        bool ShowHierarchy = true;
        bool ShowInspector = true;
        bool ShowContentBrowser = true;
        bool ShowConsole = true;
        bool ShowStats = true;
        bool ShowDemoWindow = false;
        bool ShowAbout = false;
        bool ShowControls = false;

        /// Viewport panel geometry from the last ImGui frame.
        glm::vec2 ViewportSize{ 1280.0f, 720.0f };
        glm::vec2 ViewportBoundsMin{ 0.0f };
        glm::vec2 ViewportBoundsMax{ 0.0f };
        bool ViewportHovered = false;
        bool ViewportFocused = false;

        void SelectEntity(graphics::Entity entity) { Selected = entity; }
        void ClearSelection() { Selected = graphics::Entity(); }
    };
}
