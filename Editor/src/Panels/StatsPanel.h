#pragma once
///@file StatsPanel.h  Frame timing, renderer and scene statistics.

#include "EditorState.h"

namespace Matrix::editor
{
    class StatsPanel
    {
    public:
        explicit StatsPanel(EditorState& state);

        void OnImGuiRender();

    private:
        EditorState& m_State;
        std::array<float, 120> m_FrameTimes{};
        size_t m_FrameIndex = 0;
    };
}
