#include "InspectorPanel.h"
#include "EditorUI.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace Matrix::editor
{
    using namespace graphics;

    InspectorPanel::InspectorPanel(EditorState& state)
        : m_State(state)
    {
    }

    template <typename T, typename DrawFn>
    void InspectorPanel::DrawComponent(const char* name, Entity entity, DrawFn draw, bool removable)
    {
        if (!entity.HasComponent<T>())
            return;

        const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
                                       | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;
        auto& component = entity.GetComponent<T>();
        ImVec2 contentRegion = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(T).hash_code()), flags, "%s", name);
        ImGui::PopStyleVar();

        ImGui::SameLine(contentRegion.x - lineHeight * 0.5f);
        if (ImGui::Button("...", ImVec2(lineHeight, lineHeight)))
            ImGui::OpenPopup("ComponentSettings");

        bool remove = false;
        if (ImGui::BeginPopup("ComponentSettings"))
        {
            if (removable && ImGui::MenuItem("Remove component"))
                remove = true;
            if (!removable)
                ImGui::TextDisabled("Required component");
            ImGui::EndPopup();
        }

        if (open)
        {
            draw(component);
            ImGui::TreePop();
        }

        if (remove)
            entity.RemoveComponent<T>();
    }

    void InspectorPanel::DrawAddComponentMenu(Entity entity)
    {
        if (ImGui::BeginPopup("AddComponent"))
        {
            auto item = [&](const char* label, auto adder, bool present)
            {
                if (ImGui::MenuItem(label, nullptr, false, !present))
                {
                    adder();
                    ImGui::CloseCurrentPopup();
                }
            };
            item("Sprite", [&] { entity.AddComponent<SpriteComponent>(); }, entity.HasComponent<SpriteComponent>());
            item("Circle Renderer", [&] { entity.AddComponent<CircleRendererComponent>(); }, entity.HasComponent<CircleRendererComponent>());
            item("Camera", [&] { auto& c = entity.AddComponent<CameraComponent>(); c.Primary = !m_State.ActiveWorld()->GetPrimaryCameraEntity(); }, entity.HasComponent<CameraComponent>());
            ImGui::Separator();
            item("Rigid Body 2D", [&] { entity.AddComponent<RigidBody2DComponent>(); }, entity.HasComponent<RigidBody2DComponent>());
            item("Box Collider 2D", [&] { entity.AddComponent<BoxCollider2DComponent>(); }, entity.HasComponent<BoxCollider2DComponent>());
            item("Circle Collider 2D", [&] { entity.AddComponent<CircleCollider2DComponent>(); }, entity.HasComponent<CircleCollider2DComponent>());
            ImGui::Separator();
            item("Native Script", [&] { entity.AddComponent<NativeScriptComponent>(); }, entity.HasComponent<NativeScriptComponent>());
            item("Audio Source", [&] { entity.AddComponent<AudioSourceComponent>(); }, entity.HasComponent<AudioSourceComponent>());
            ImGui::EndPopup();
        }
    }

    void InspectorPanel::DrawEntity(Entity entity)
    {
        // Name
        {
            auto& tag = entity.GetComponent<TagComponent>();
            char buffer[256];
            std::snprintf(buffer, sizeof(buffer), "%s", tag.Tag.c_str());
            ImGui::SetNextItemWidth(-1);
            ui::PushBoldFont();
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
                tag.Tag = buffer;
            ui::PopBoldFont();
        }

        ImGui::TextDisabled("UUID %llu", static_cast<unsigned long long>(entity.GetUUID()));
        if (Entity parent = entity.GetParent())
        {
            ImGui::SameLine();
            ImGui::TextDisabled("| child of %s", parent.GetName().c_str());
        }

        if (ImGui::Button("Add Component", ImVec2(-1, 0)))
            ImGui::OpenPopup("AddComponent");
        DrawAddComponentMenu(entity);

        DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& t)
        {
            ui::DrawVec3Control("Position", t.Position);
            ui::DrawFloatControl("Rotation", t.Rotation, 0.5f, 0.0f, 0.0f, 90.0f, "%.1f deg");
            ui::DrawVec3Control("Scale", t.Scale, 1.0f);
        }, false);

        DrawComponent<SpriteComponent>("Sprite", entity, [](SpriteComponent& s)
        {
            ui::DrawColorControl("Color", s.Color);
            if (ui::DrawAssetPathControl("Texture", s.TexturePath, "Images (*.png;*.jpg;*.jpeg;*.bmp;*.tga)\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0All files\0*.*\0", ".png,.jpg,.jpeg,.bmp,.tga"))
                s.Texture = nullptr;
            ui::DrawVec2Control("Tiling", s.Tiling, 1.0f, 0.05f);
            if (s.Texture && s.Texture->IsLoaded())
            {
                bool missing = TextureLibrary::Get().IsMissingTexture(s.Texture);
                float aspect = s.Texture->GetAspectRatio();
                float width = std::min(ImGui::GetContentRegionAvail().x, 128.0f);
                ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(s.Texture->GetID())), ImVec2(width, width / std::max(aspect, 0.01f)), ImVec2(0, 1), ImVec2(1, 0));
                if (missing)
                    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Texture could not be loaded");
                else
                    ImGui::TextDisabled("%ux%u", s.Texture->GetWidth(), s.Texture->GetHeight());
            }
        });

        DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](CircleRendererComponent& c)
        {
            ui::DrawColorControl("Color", c.Color);
            ui::DrawFloatControl("Thickness", c.Thickness, 0.01f, 0.0f, 1.0f);
            ui::DrawFloatControl("Fade", c.Fade, 0.001f, 0.0f, 1.0f);
        });

        DrawComponent<CameraComponent>("Camera", entity, [this, entity](CameraComponent& c)
        {
            if (ui::DrawBoolControl("Primary", c.Primary) && c.Primary)
            {
                // Only one primary camera at a time.
                for (auto handle : m_State.ActiveWorld()->View<CameraComponent>())
                    if (handle != entity.GetHandle())
                        m_State.ActiveWorld()->GetRegistry().get<CameraComponent>(handle).Primary = false;
            }
            float size = c.Camera.GetOrthographicSize();
            if (ui::DrawFloatControl("Size", size, 0.1f, 0.1f, 1000.0f))
                c.Camera.SetOrthographicSize(size);
            float nearClip = c.Camera.GetOrthographicNearClip();
            if (ui::DrawFloatControl("Near", nearClip, 0.1f))
                c.Camera.SetOrthographicNearClip(nearClip);
            float farClip = c.Camera.GetOrthographicFarClip();
            if (ui::DrawFloatControl("Far", farClip, 0.1f))
                c.Camera.SetOrthographicFarClip(farClip);
            ui::DrawBoolControl("Fixed Aspect", c.FixedAspectRatio);
        });

        DrawComponent<RigidBody2DComponent>("Rigid Body 2D", entity, [](RigidBody2DComponent& rb)
        {
            static const char* types[] = { "Static", "Dynamic", "Kinematic" };
            int type = static_cast<int>(rb.Type);
            if (ui::DrawComboControl("Body Type", type, types, 3))
                rb.Type = static_cast<BodyType2D>(type);
            ui::DrawFloatControl("Mass", rb.Mass, 0.05f, 0.01f, 1000.0f);
            ui::DrawFloatControl("Gravity Scale", rb.GravityScale, 0.05f, -10.0f, 10.0f);
            ui::DrawFloatControl("Restitution", rb.Restitution, 0.01f, 0.0f, 1.0f);
            ui::DrawFloatControl("Friction", rb.Friction, 0.01f, 0.0f, 1.0f);
            ui::DrawFloatControl("Damping", rb.LinearDamping, 0.01f, 0.0f, 20.0f);
            ui::DrawVec2Control("Velocity", rb.Velocity, 0.0f, 0.1f);
            ImGui::TextDisabled(rb.IsGrounded ? "Grounded" : "In the air");
        });

        DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](BoxCollider2DComponent& c)
        {
            ui::DrawVec2Control("Offset", c.Offset);
            ui::DrawVec2Control("Half Size", c.Size, 0.5f, 0.01f);
            ui::DrawBoolControl("Is Trigger", c.IsTrigger);
        });

        DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](CircleCollider2DComponent& c)
        {
            ui::DrawVec2Control("Offset", c.Offset);
            ui::DrawFloatControl("Radius", c.Radius, 0.01f, 0.01f, 1000.0f);
            ui::DrawBoolControl("Is Trigger", c.IsTrigger);
        });

        DrawComponent<NativeScriptComponent>("Native Script", entity, [this](NativeScriptComponent& script)
        {
            std::vector<std::string> names = ScriptRegistry::GetNames();
            std::string preview = script.ScriptName.empty() ? "(none)" : script.ScriptName;
            if (!script.ScriptName.empty() && !ScriptRegistry::Exists(script.ScriptName))
                preview += " (missing)";

            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, 90.0f);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Script");
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-1);
            if (ImGui::BeginCombo("##script", preview.c_str()))
            {
                if (ImGui::Selectable("(none)", script.ScriptName.empty()))
                    script.ScriptName.clear();
                for (const auto& name : names)
                {
                    bool selected = name == script.ScriptName;
                    if (ImGui::Selectable(name.c_str(), selected))
                    {
                        script.ScriptName = name;
                        for (const auto& param : ScriptRegistry::GetParameters(name))
                            if (script.Parameters.find(param.Name) == script.Parameters.end())
                                script.Parameters[param.Name] = param.DefaultValue;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Columns(1);

            if (script.Instance)
                ImGui::TextDisabled("Running");

            // Parameters: registered defaults first, then any extra values stored in the scene.
            ImGui::Spacing();
            ImGui::TextUnformatted("Parameters");
            for (const auto& param : ScriptRegistry::GetParameters(script.ScriptName))
            {
                float& value = script.Parameters.try_emplace(param.Name, param.DefaultValue).first->second;
                ui::DrawFloatControl(param.Name, value, 0.05f);
            }
            std::vector<std::string> toRemove;
            for (auto& [name, value] : script.Parameters)
            {
                bool registered = false;
                for (const auto& param : ScriptRegistry::GetParameters(script.ScriptName))
                    if (param.Name == name) { registered = true; break; }
                if (registered)
                    continue;
                ImGui::PushID(name.c_str());
                ui::DrawFloatControl(name, value, 0.05f);
                ImGui::SameLine();
                if (ImGui::SmallButton("x"))
                    toRemove.push_back(name);
                ImGui::PopID();
            }
            for (const auto& name : toRemove)
                script.Parameters.erase(name);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 60.0f);
            ImGui::InputTextWithHint("##newparam", "new parameter", m_NewParameterName, sizeof(m_NewParameterName));
            ImGui::SameLine();
            if (ImGui::Button("Add") && m_NewParameterName[0] != '\0')
            {
                script.Parameters.try_emplace(m_NewParameterName, 0.0f);
                m_NewParameterName[0] = '\0';
            }
        });

        DrawComponent<AudioSourceComponent>("Audio Source", entity, [](AudioSourceComponent& a)
        {
            ui::DrawAssetPathControl("Clip", a.ClipPath, "Audio (*.wav;*.mp3;*.flac)\0*.wav;*.mp3;*.flac\0All files\0*.*\0", ".wav,.mp3,.flac");
            ui::DrawFloatControl("Volume", a.Volume, 0.01f, 0.0f, 2.0f);
            ui::DrawFloatControl("Pitch", a.Pitch, 0.01f, 0.1f, 4.0f);
            ui::DrawBoolControl("Loop", a.Loop);
            ui::DrawBoolControl("Play On Start", a.PlayOnStart);
            if (ImGui::Button("Preview") && !a.ClipPath.empty())
                audio::AudioEngine::Play(a.ClipPath, a.Volume, a.Pitch, false);
            ImGui::SameLine();
            if (ImGui::Button("Stop All"))
                audio::AudioEngine::StopAll();
            if (!audio::AudioEngine::IsInitialized())
                ImGui::TextDisabled("Audio device unavailable");
        });
    }

    void InspectorPanel::DrawSceneSettings()
    {
        World& world = *m_State.ActiveWorld();
        ui::PushBoldFont();
        ImGui::TextUnformatted("Scene Settings");
        ui::PopBoldFont();
        ImGui::TextDisabled("%s", world.GetFilePath().empty() ? "(unsaved scene)" : world.GetFilePath().c_str());
        ImGui::Separator();

        std::string name = world.GetName();
        if (ui::DrawTextControl("Name", name))
            world.SetName(name);
        ui::DrawColorControl("Background", world.BackgroundColor);
        ui::DrawVec2Control("Gravity", world.Gravity, 0.0f, 0.1f);

        ImGui::Spacing();
        ImGui::TextDisabled("Select an entity in the hierarchy or viewport to edit its components.");
    }

    void InspectorPanel::OnImGuiRender()
    {
        if (!m_State.ShowInspector)
            return;

        ImGui::Begin("Inspector", &m_State.ShowInspector);
        Entity selected = m_State.Selected;
        if (selected && m_State.ActiveWorld()->IsValid(selected))
            DrawEntity(selected);
        else
        {
            m_State.ClearSelection();
            DrawSceneSettings();
        }
        ImGui::End();
    }
}
