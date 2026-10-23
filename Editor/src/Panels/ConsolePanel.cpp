#include "ConsolePanel.h"

#include "imgui.h"

namespace Matrix::editor
{
    namespace
    {
        const char* LevelNames[] = { "Trace", "Debug", "Info", "Warn", "Error", "Critical" };
        const ImVec4 LevelColors[] = {
            ImVec4(0.55f, 0.55f, 0.58f, 1.0f),
            ImVec4(0.60f, 0.75f, 0.95f, 1.0f),
            ImVec4(0.85f, 0.85f, 0.88f, 1.0f),
            ImVec4(0.95f, 0.80f, 0.30f, 1.0f),
            ImVec4(0.95f, 0.40f, 0.35f, 1.0f),
            ImVec4(1.00f, 0.25f, 0.25f, 1.0f),
        };
    }

    ConsolePanel::ConsolePanel(EditorState& state)
        : m_State(state)
    {
    }

    void ConsolePanel::OnImGuiRender()
    {
        if (!m_State.ShowConsole)
            return;

        ImGui::Begin("Console", &m_State.ShowConsole);

        if (ImGui::Button("Clear"))
        {
            core::Logging::ClearHistory();
            m_Cache.clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(160.0f);
        ImGui::InputTextWithHint("##filter", "Filter", m_Filter, sizeof(m_Filter));
        for (int level = 0; level < 6; ++level)
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, LevelColors[level]);
            ImGui::Checkbox(LevelNames[level], &m_ShowLevels[level]);
            ImGui::PopStyleColor();
        }
        ImGui::Separator();

        uint64_t version = core::Logging::GetHistoryVersion();
        bool newMessages = version != m_LastVersion;
        if (newMessages)
        {
            m_Cache = core::Logging::GetHistory();
            m_LastVersion = version;
        }

        ImGui::BeginChild("##log", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 2.0f));
        std::string filter = m_Filter;
        for (const auto& message : m_Cache)
        {
            int level = static_cast<int>(message.Level);
            if (level < 0 || level > 5 || !m_ShowLevels[level])
                continue;
            if (!filter.empty() && message.Text.find(filter) == std::string::npos && message.Logger.find(filter) == std::string::npos)
                continue;
            ImGui::PushStyleColor(ImGuiCol_Text, LevelColors[level]);
            ImGui::TextUnformatted(("[" + message.Time + "] [" + message.Logger + "] " + message.Text).c_str());
            ImGui::PopStyleColor();
        }
        ImGui::PopStyleVar();
        if (m_AutoScroll && newMessages)
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        ImGui::End();
    }
}
