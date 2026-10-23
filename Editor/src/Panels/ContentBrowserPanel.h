#pragma once
///@file ContentBrowserPanel.h  Browses the project's assets folder; items can be dragged into other panels.

#include "EditorState.h"

namespace Matrix::editor
{
    class ContentBrowserPanel
    {
    public:
        explicit ContentBrowserPanel(EditorState& state);

        void OnImGuiRender();

        /// Invoked when the user double-clicks a scene file (root-relative path).
        std::function<void(const std::string&)> OnOpenScene;

    private:
        void Refresh();
        void DrawBreadcrumbs();

        struct Item
        {
            std::filesystem::path Path;
            std::string Name;
            std::string Extension;
            bool IsDirectory = false;
        };

        EditorState& m_State;
        std::filesystem::path m_BaseDirectory;
        std::filesystem::path m_CurrentDirectory;
        std::vector<Item> m_Items;
        float m_ThumbnailSize = 88.0f;
        char m_Filter[128] = { 0 };
        bool m_Dirty = true;
        float m_RefreshTimer = 0.0f;
    };
}
