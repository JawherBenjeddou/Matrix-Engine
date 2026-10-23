#include "SceneHierarchyPanel.h"
#include "EditorUI.h"

#include "imgui.h"

namespace Matrix::editor
{
    using namespace graphics;

    SceneHierarchyPanel::SceneHierarchyPanel(EditorState& state)
        : m_State(state)
    {
    }

    Entity SceneHierarchyPanel::CreateEntity(World& world, EntityKind kind, Entity parent, const glm::vec2& position)
    {
        Entity entity;
        switch (kind)
        {
        case EntityKind::Sprite:
            entity = world.CreateEntity("Sprite");
            entity.AddComponent<SpriteComponent>(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
            break;
        case EntityKind::Circle:
            entity = world.CreateEntity("Circle");
            entity.AddComponent<CircleRendererComponent>();
            break;
        case EntityKind::Camera:
        {
            entity = world.CreateEntity("Camera");
            auto& camera = entity.AddComponent<CameraComponent>();
            camera.Primary = !world.GetPrimaryCameraEntity();
            break;
        }
        case EntityKind::PhysicsBox:
        {
            entity = world.CreateEntity("Physics Box");
            entity.AddComponent<SpriteComponent>(glm::vec4(0.95f, 0.65f, 0.25f, 1.0f));
            auto& body = entity.AddComponent<RigidBody2DComponent>();
            body.Type = BodyType2D::Dynamic;
            entity.AddComponent<BoxCollider2DComponent>();
            break;
        }
        case EntityKind::PhysicsCircle:
        {
            entity = world.CreateEntity("Physics Circle");
            auto& circle = entity.AddComponent<CircleRendererComponent>();
            circle.Color = glm::vec4(0.35f, 0.75f, 0.95f, 1.0f);
            auto& body = entity.AddComponent<RigidBody2DComponent>();
            body.Type = BodyType2D::Dynamic;
            body.Restitution = 0.5f;
            entity.AddComponent<CircleCollider2DComponent>();
            break;
        }
        case EntityKind::StaticPlatform:
        {
            entity = world.CreateEntity("Platform");
            entity.AddComponent<SpriteComponent>(glm::vec4(0.45f, 0.5f, 0.55f, 1.0f));
            entity.GetComponent<TransformComponent>().Scale = glm::vec3(6.0f, 0.5f, 1.0f);
            auto& body = entity.AddComponent<RigidBody2DComponent>();
            body.Type = BodyType2D::Static;
            entity.AddComponent<BoxCollider2DComponent>();
            break;
        }
        case EntityKind::Empty:
        default:
            entity = world.CreateEntity("Entity");
            break;
        }

        auto& transform = entity.GetComponent<TransformComponent>();
        transform.Position.x = position.x;
        transform.Position.y = position.y;
        if (parent)
            world.SetParent(entity, parent, false);
        return entity;
    }

    bool SceneHierarchyPanel::MatchesFilter(Entity entity) const
    {
        if (m_Filter[0] == '\0')
            return true;
        std::string name = entity.GetName();
        std::string filter = m_Filter;
        auto lower = [](std::string s) { std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); }); return s; };
        if (lower(name).find(lower(filter)) != std::string::npos)
            return true;
        for (Entity child : entity.GetChildren())
            if (MatchesFilter(child))
                return true;
        return false;
    }

    void SceneHierarchyPanel::DrawCreateMenu(Entity parent)
    {
        World& world = *m_State.ActiveWorld();
        glm::vec2 position = parent ? glm::vec2(0.0f) : m_State.Camera.GetPosition();
        Entity created;
        if (ImGui::MenuItem("Empty Entity"))      created = CreateEntity(world, EntityKind::Empty, parent, position);
        if (ImGui::MenuItem("Sprite"))            created = CreateEntity(world, EntityKind::Sprite, parent, position);
        if (ImGui::MenuItem("Circle"))            created = CreateEntity(world, EntityKind::Circle, parent, position);
        if (ImGui::MenuItem("Camera"))            created = CreateEntity(world, EntityKind::Camera, parent, position);
        ImGui::Separator();
        if (ImGui::MenuItem("Physics Box"))       created = CreateEntity(world, EntityKind::PhysicsBox, parent, position);
        if (ImGui::MenuItem("Physics Circle"))    created = CreateEntity(world, EntityKind::PhysicsCircle, parent, position);
        if (ImGui::MenuItem("Static Platform"))   created = CreateEntity(world, EntityKind::StaticPlatform, parent, position);
        if (created)
            m_State.SelectEntity(created);
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        if (!MatchesFilter(entity))
            return;

        std::vector<Entity> children = entity.GetChildren();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (m_State.Selected == entity)
            flags |= ImGuiTreeNodeFlags_Selected;
        if (children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf;
        if (m_Filter[0] != '\0' || !entity.HasParent())
            flags |= ImGuiTreeNodeFlags_DefaultOpen;

        std::string label = entity.GetName();
        if (entity.HasComponent<CameraComponent>()) label += "  [camera]";
        else if (entity.HasComponent<NativeScriptComponent>()) label += "  [script]";

        bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uintptr_t>(static_cast<uint32_t>(entity))), flags, "%s", label.c_str());

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen())
            m_State.SelectEntity(entity);
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && OnFocusEntity)
            OnFocusEntity(entity);

        // Drag & drop re-parenting.
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            uint64_t uuid = entity.GetUUID();
            ImGui::SetDragDropPayload(ui::EntityPayload, &uuid, sizeof(uuid));
            ImGui::TextUnformatted(entity.GetName().c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ui::EntityPayload))
            {
                uint64_t uuid = *static_cast<const uint64_t*>(payload->Data);
                Entity dragged = m_State.ActiveWorld()->GetEntityByUUID(core::UUID(uuid));
                if (dragged && dragged != entity)
                    m_State.ActiveWorld()->SetParent(dragged, entity, true);
            }
            ImGui::EndDragDropTarget();
        }

        // Context menu.
        if (ImGui::BeginPopupContextItem())
        {
            m_State.SelectEntity(entity);
            if (ImGui::BeginMenu("Create Child"))
            {
                DrawCreateMenu(entity);
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Duplicate", "Ctrl+D"))
                m_State.SelectEntity(m_State.ActiveWorld()->DuplicateEntity(entity));
            if (entity.HasParent() && ImGui::MenuItem("Unparent"))
                m_State.ActiveWorld()->RemoveParent(entity, true);
            if (ImGui::MenuItem("Focus", "F") && OnFocusEntity)
                OnFocusEntity(entity);
            ImGui::Separator();
            if (ImGui::MenuItem("Delete", "Del"))
                m_PendingDelete = entity;
            ImGui::EndPopup();
        }

        if (opened)
        {
            for (Entity child : children)
                DrawEntityNode(child);
            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        if (!m_State.ShowHierarchy)
            return;

        ImGui::Begin("Scene Hierarchy", &m_State.ShowHierarchy);
        World& world = *m_State.ActiveWorld();

        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##filter", "Search entities...", m_Filter, sizeof(m_Filter));

        ImGui::BeginChild("##tree", ImVec2(0, 0), false);
        for (Entity root : world.GetRootEntities())
            DrawEntityNode(root);

        // Click on empty space clears the selection; right-click opens the create menu.
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
            m_State.ClearSelection();
        if (ImGui::BeginPopupContextWindow("##hierarchy_context", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::BeginMenu("Create"))
            {
                DrawCreateMenu(Entity());
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }

        // Dropping an entity on the empty area un-parents it.
        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, std::max(ImGui::GetContentRegionAvail().y, 20.0f)));
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ui::EntityPayload))
            {
                uint64_t uuid = *static_cast<const uint64_t*>(payload->Data);
                Entity dragged = world.GetEntityByUUID(core::UUID(uuid));
                if (dragged)
                    world.RemoveParent(dragged, true);
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::TextDisabled("%zu entities", world.GetEntityCount());

        if (m_PendingDelete)
        {
            if (m_State.Selected == m_PendingDelete || world.IsDescendantOf(m_State.Selected, m_PendingDelete))
                m_State.ClearSelection();
            world.DestroyEntity(m_PendingDelete);
            m_PendingDelete = Entity();
        }

        ImGui::End();
    }
}
