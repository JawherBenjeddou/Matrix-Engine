#pragma once
///@file ConsolePanel.h  Shows the engine/application log inside the editor.

#include "EditorState.h"

namespace Matrix::editor
{
    class ConsolePanel
    {
    public:
        explicit ConsolePanel(EditorState& state);

        void OnImGuiRender();

    private:
        EditorState& m_State;
        bool m_ShowLevels[6] = { false, true, true, true, true, true }; // trace hidden by default
        bool m_AutoScroll = true;
        char m_Filter[128] = { 0 };
        uint64_t m_LastVersion = 0;
        std::vector<core::LogMessage> m_Cache;
    };
}
