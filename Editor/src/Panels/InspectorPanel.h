#pragma once
///@file InspectorPanel.h  Edits the components of the selected entity (or the scene settings).

#include "EditorState.h"

namespace Matrix::editor
{
    class InspectorPanel
    {
    public:
        explicit InspectorPanel(EditorState& state);

        void OnImGuiRender();

    private:
        void DrawEntity(graphics::Entity entity);
        void DrawSceneSettings();
        void DrawAddComponentMenu(graphics::Entity entity);

        template <typename T, typename DrawFn>
        void DrawComponent(const char* name, graphics::Entity entity, DrawFn draw, bool removable = true);

        EditorState& m_State;
        char m_NewParameterName[64] = { 0 };
    };
}
