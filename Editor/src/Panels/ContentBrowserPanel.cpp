#include "ContentBrowserPanel.h"
#include "EditorUI.h"

#include "imgui.h"

namespace Matrix::editor
{
    namespace fs = std::filesystem;

    ContentBrowserPanel::ContentBrowserPanel(EditorState& state)
        : m_State(state)
    {
        m_BaseDirectory = util::FileSystem::GetAssetsDirectory();
        std::error_code ec;
        if (!fs::exists(m_BaseDirectory, ec))
            fs::create_directories(m_BaseDirectory, ec);
        m_CurrentDirectory = m_BaseDirectory;
    }

    void ContentBrowserPanel::Refresh()
    {
        m_Items.clear();
        std::error_code ec;
        if (!fs::is_directory(m_CurrentDirectory, ec))
            m_CurrentDirectory = m_BaseDirectory;

        for (const auto& entry : fs::directory_iterator(m_CurrentDirectory, ec))
        {
            Item item;
            item.Path = entry.path();
            item.Name = entry.path().filename().string();
            item.IsDirectory = entry.is_directory(ec);
            item.Extension = entry.path().extension().string();
            std::transform(item.Extension.begin(), item.Extension.end(), item.Extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            m_Items.push_back(item);
        }
        std::sort(m_Items.begin(), m_Items.end(), [](const Item& a, const Item& b)
        {
            if (a.IsDirectory != b.IsDirectory)
                return a.IsDirectory;
            return a.Name < b.Name;
        });
        m_Dirty = false;
    }

    void ContentBrowserPanel::DrawBreadcrumbs()
    {
        fs::path relative = m_CurrentDirectory.lexically_relative(m_BaseDirectory);
        if (ImGui::SmallButton("assets"))
            { m_CurrentDirectory = m_BaseDirectory; m_Dirty = true; }
        fs::path accumulated = m_BaseDirectory;
        for (const auto& part : relative)
        {
            if (part == ".")
                continue;
            accumulated /= part;
            ImGui::SameLine();
            ImGui::TextDisabled("/");
            ImGui::SameLine();
            std::string label = part.string() + "##" + accumulated.string();
            if (ImGui::SmallButton(label.c_str()))
                { m_CurrentDirectory = accumulated; m_Dirty = true; }
        }
    }

    void ContentBrowserPanel::OnImGuiRender()
    {
        if (!m_State.ShowContentBrowser)
            return;

        ImGui::Begin("Content Browser", &m_State.ShowContentBrowser);

        // Periodically re-scan so files added outside the editor show up.
        m_RefreshTimer += ImGui::GetIO().DeltaTime;
        if (m_RefreshTimer > 2.0f) { m_Dirty = true; m_RefreshTimer = 0.0f; }
        if (m_Dirty)
            Refresh();

        // Toolbar
        bool atRoot = m_CurrentDirectory == m_BaseDirectory;
        ImGui::BeginDisabled(atRoot);
        if (ImGui::Button("<"))
            { m_CurrentDirectory = m_CurrentDirectory.parent_path(); m_Dirty = true; }
        ImGui::EndDisabled();
        ImGui::SameLine();
        DrawBreadcrumbs();
        const float explorerWidth = ImGui::CalcTextSize("Explorer").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        const float sliderWidth = 70.0f;
        const float filterWidth = 110.0f;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        float toolsWidth = filterWidth + sliderWidth + explorerWidth + spacing * 2.0f;
        float toolsStart = ImGui::GetWindowContentRegionMax().x - toolsWidth;
        if (toolsStart > ImGui::GetCursorPosX() + spacing)
        {
            ImGui::SameLine(toolsStart);
            ImGui::SetNextItemWidth(filterWidth);
            ImGui::InputTextWithHint("##filter", "Filter", m_Filter, sizeof(m_Filter));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(sliderWidth);
            ImGui::SliderFloat("##size", &m_ThumbnailSize, 48.0f, 160.0f, "%.0f px");
            ImGui::SameLine();
            if (ImGui::Button("Explorer"))
                util::Platform::OpenInShell(m_CurrentDirectory);
        }
        ImGui::Separator();

        // Grid
        float padding = 10.0f;
        float cellSize = m_ThumbnailSize + padding;
        int columns = std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / cellSize));
        ImGui::BeginChild("##grid");
        ImGui::Columns(columns, nullptr, false);

        std::string filter = m_Filter;
        std::transform(filter.begin(), filter.end(), filter.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        for (const auto& item : m_Items)
        {
            if (!filter.empty())
            {
                std::string lowerName = item.Name;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                if (lowerName.find(filter) == std::string::npos)
                    continue;
            }

            ImGui::PushID(item.Path.string().c_str());
            std::string relativePath = util::FileSystem::MakeRelative(item.Path);

            bool isImage = item.Extension == ".png" || item.Extension == ".jpg" || item.Extension == ".jpeg" || item.Extension == ".bmp" || item.Extension == ".tga";
            bool isScene = item.Extension == graphics::WorldSerializer::Extension;
            bool isAudio = item.Extension == ".wav" || item.Extension == ".mp3" || item.Extension == ".flac";

            ImVec4 tint(1, 1, 1, 1);
            ImVec4 bg = item.IsDirectory ? ImVec4(0.85f, 0.70f, 0.30f, 1.0f)
                      : isScene ? ImVec4(0.30f, 0.60f, 0.95f, 1.0f)
                      : isAudio ? ImVec4(0.45f, 0.80f, 0.50f, 1.0f)
                      : ImVec4(0.30f, 0.30f, 0.34f, 1.0f);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            if (isImage)
            {
                auto texture = graphics::TextureLibrary::Get().Load(relativePath);
                ImGui::ImageButton("##thumb", reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture->GetID())), ImVec2(m_ThumbnailSize, m_ThumbnailSize), ImVec2(0, 1), ImVec2(1, 0), ImVec4(0.12f, 0.12f, 0.14f, 1.0f), tint);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, bg);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(bg.x * 1.15f, bg.y * 1.15f, bg.z * 1.15f, 1.0f));
                const char* glyph = item.IsDirectory ? "DIR" : isScene ? "SCENE" : isAudio ? "AUDIO" : (item.Extension.empty() ? "FILE" : item.Extension.c_str() + 1);
                ImGui::Button(glyph, ImVec2(m_ThumbnailSize, m_ThumbnailSize));
                ImGui::PopStyleColor(2);
            }
            ImGui::PopStyleColor();

            if (!item.IsDirectory && ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload(ui::ContentBrowserPayload, relativePath.c_str(), relativePath.size() + 1);
                ImGui::TextUnformatted(item.Name.c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("%s", relativePath.c_str());
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (item.IsDirectory)
                        { m_CurrentDirectory = item.Path; m_Dirty = true; }
                    else if (isScene && OnOpenScene)
                        OnOpenScene(relativePath);
                    else if (isAudio)
                        audio::AudioEngine::Play(relativePath);
                }
            }
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Show in Explorer"))
                    util::Platform::OpenInShell(item.IsDirectory ? item.Path : item.Path.parent_path());
                if (isScene && ImGui::MenuItem("Open scene") && OnOpenScene)
                    OnOpenScene(relativePath);
                if (isAudio && ImGui::MenuItem("Play"))
                    audio::AudioEngine::Play(relativePath);
                ImGui::EndPopup();
            }

            ImGui::TextWrapped("%s", item.Name.c_str());
            ImGui::NextColumn();
            ImGui::PopID();
        }

        ImGui::Columns(1);
        ImGui::EndChild();
        ImGui::End();
    }
}
