#pragma once
///@file SceneHierarchyPanel.h  Tree view of every entity in the active world.

#include "EditorState.h"

namespace Matrix::editor
{
    enum class EntityKind { Empty = 0, Sprite, Circle, Camera, PhysicsBox, PhysicsCircle, StaticPlatform };

    class SceneHierarchyPanel
    {
    public:
        explicit SceneHierarchyPanel(EditorState& state);

        void OnImGuiRender();

        /// Creates a preset entity in the world (used by this panel and the editor menus).
        static graphics::Entity CreateEntity(graphics::World& world, EntityKind kind, graphics::Entity parent = graphics::Entity(), const glm::vec2& position = glm::vec2(0.0f));

        /// Invoked when the user double-clicks an entity.
        std::function<void(graphics::Entity)> OnFocusEntity;

    private:
        void DrawEntityNode(graphics::Entity entity);
        void DrawCreateMenu(graphics::Entity parent);
        bool MatchesFilter(graphics::Entity entity) const;

        EditorState& m_State;
        char m_Filter[128] = { 0 };
        graphics::Entity m_PendingDelete;
    };
}
