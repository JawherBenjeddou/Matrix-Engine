#include "EditorUI.h"

#include "imgui_internal.h"

namespace Matrix::editor::ui
{
    namespace
    {
        bool BeginProperty(const std::string& label, float columnWidth)
        {
            ImGui::PushID(label.c_str());
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(label.c_str());
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1);
            return true;
        }

        void EndProperty()
        {
            ImGui::PopItemWidth();
            ImGui::Columns(1);
            ImGui::PopID();
        }

        bool AxisButton(const char* text, const ImVec4& color, const ImVec2& size, float& value, float resetValue)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(color.x * 1.2f, color.y * 1.2f, color.z * 1.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
            PushBoldFont();
            bool pressed = ImGui::Button(text, size);
            PopBoldFont();
            ImGui::PopStyleColor(3);
            if (pressed)
                value = resetValue;
            return pressed;
        }
    }

    void PushBoldFont()
    {
        auto* gui = core::Application::Get().GetEngine().GetGui();
        ImFont* font = gui ? gui->GetBoldFont() : nullptr;
        ImGui::PushFont(font ? font : ImGui::GetFont());
    }

    void PopBoldFont()
    {
        ImGui::PopFont();
    }

    void HelpMarker(const char* text)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.0f);
            ImGui::TextUnformatted(text);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float speed, float columnWidth)
    {
        bool changed = false;
        BeginProperty(label, columnWidth);
        ImGui::PopItemWidth();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        changed |= AxisButton("X", ImVec4(0.80f, 0.20f, 0.20f, 1.0f), buttonSize, values.x, resetValue);
        ImGui::SameLine();
        changed |= ImGui::DragFloat("##X", &values.x, speed, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        changed |= AxisButton("Y", ImVec4(0.25f, 0.65f, 0.20f, 1.0f), buttonSize, values.y, resetValue);
        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Y", &values.y, speed, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        changed |= AxisButton("Z", ImVec4(0.15f, 0.35f, 0.85f, 1.0f), buttonSize, values.z, resetValue);
        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Z", &values.z, speed, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::PushItemWidth(-1);
        EndProperty();
        return changed;
    }

    bool DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue, float speed, float columnWidth)
    {
        bool changed = false;
        BeginProperty(label, columnWidth);
        ImGui::PopItemWidth();

        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        changed |= AxisButton("X", ImVec4(0.80f, 0.20f, 0.20f, 1.0f), buttonSize, values.x, resetValue);
        ImGui::SameLine();
        changed |= ImGui::DragFloat("##X", &values.x, speed, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        changed |= AxisButton("Y", ImVec4(0.25f, 0.65f, 0.20f, 1.0f), buttonSize, values.y, resetValue);
        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Y", &values.y, speed, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::PushItemWidth(-1);
        EndProperty();
        return changed;
    }

    bool DrawFloatControl(const std::string& label, float& value, float speed, float min, float max, float columnWidth, const char* format)
    {
        BeginProperty(label, columnWidth);
        bool changed = ImGui::DragFloat("##value", &value, speed, min, max, format);
        EndProperty();
        return changed;
    }

    bool DrawBoolControl(const std::string& label, bool& value, float columnWidth)
    {
        BeginProperty(label, columnWidth);
        bool changed = ImGui::Checkbox("##value", &value);
        EndProperty();
        return changed;
    }

    bool DrawColorControl(const std::string& label, glm::vec4& color, float columnWidth)
    {
        BeginProperty(label, columnWidth);
        bool changed = ImGui::ColorEdit4("##value", glm::value_ptr(color), ImGuiColorEditFlags_AlphaBar);
        EndProperty();
        return changed;
    }

    bool DrawTextControl(const std::string& label, std::string& value, float columnWidth)
    {
        BeginProperty(label, columnWidth);
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "%s", value.c_str());
        bool changed = ImGui::InputText("##value", buffer, sizeof(buffer));
        if (changed)
            value = buffer;
        EndProperty();
        return changed;
    }

    bool DrawComboControl(const std::string& label, int& current, const char* const* items, int count, float columnWidth)
    {
        BeginProperty(label, columnWidth);
        bool changed = ImGui::Combo("##value", &current, items, count);
        EndProperty();
        return changed;
    }

    std::optional<std::string> AcceptAssetDrop(const char* extensions)
    {
        std::optional<std::string> result;
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ContentBrowserPayload))
            {
                std::string path(static_cast<const char*>(payload->Data));
                std::string ext = std::filesystem::path(path).extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                std::string list = std::string(",") + extensions + ",";
                if (ext.empty() || list.find("," + ext + ",") != std::string::npos || list == ",*,")
                    result = path;
            }
            ImGui::EndDragDropTarget();
        }
        return result;
    }

    bool DrawAssetPathControl(const std::string& label, std::string& path, const char* dialogFilter, const char* dropExtensions, float columnWidth)
    {
        bool changed = false;
        BeginProperty(label, columnWidth);
        ImGui::PopItemWidth();

        float buttonWidth = ImGui::CalcTextSize("...").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        float clearWidth = ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        float available = ImGui::GetContentRegionAvail().x;
        ImGui::PushItemWidth(std::max(available - buttonWidth - clearWidth - ImGui::GetStyle().ItemSpacing.x * 2.0f, 40.0f));

        char buffer[512];
        std::snprintf(buffer, sizeof(buffer), "%s", path.c_str());
        if (ImGui::InputText("##path", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            path = buffer;
            changed = true;
        }
        if (auto dropped = AcceptAssetDrop(dropExtensions))
        {
            path = *dropped;
            changed = true;
        }
        if (ImGui::IsItemHovered() && !path.empty())
            ImGui::SetTooltip("%s", path.c_str());
        ImGui::PopItemWidth();

        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            std::string chosen = util::FileDialogs::OpenFile(dialogFilter, util::FileSystem::GetAssetsDirectory());
            if (!chosen.empty())
            {
                path = util::FileSystem::MakeRelative(chosen);
                changed = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            path.clear();
            changed = true;
        }

        ImGui::PushItemWidth(-1);
        EndProperty();
        return changed;
    }
}
