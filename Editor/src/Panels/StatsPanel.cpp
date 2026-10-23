#include "StatsPanel.h"

#include "imgui.h"

namespace Matrix::editor
{
    StatsPanel::StatsPanel(EditorState& state)
        : m_State(state)
    {
    }

    void StatsPanel::OnImGuiRender()
    {
        if (!m_State.ShowStats)
            return;

        ImGui::Begin("Statistics", &m_State.ShowStats);

        auto& app = core::Application::Get();
        const auto& timer = app.GetTimer();
        float frameMs = timer.RawDeltaTime() * 1000.0f;
        m_FrameTimes[m_FrameIndex] = frameMs;
        m_FrameIndex = (m_FrameIndex + 1) % m_FrameTimes.size();

        ImGui::Text("FPS: %.1f", timer.FPS());
        ImGui::Text("Frame time: %.2f ms", frameMs);
        ImGui::PlotLines("##frametimes", m_FrameTimes.data(), static_cast<int>(m_FrameTimes.size()), static_cast<int>(m_FrameIndex), nullptr, 0.0f, 33.0f, ImVec2(-1, 50));

        bool vsync = app.GetWindow().IsVSync();
        if (ImGui::Checkbox("VSync", &vsync))
            app.GetWindow().SetVSync(vsync);

        ImGui::Separator();
        auto stats = graphics::Renderer2D::GetStats();
        ImGui::Text("Draw calls: %u", stats.DrawCalls);
        ImGui::Text("Quads: %u   Circles: %u   Lines: %u", stats.QuadCount, stats.CircleCount, stats.LineCount);
        ImGui::Text("Vertices: %u   Indices: %u", stats.GetTotalVertexCount(), stats.GetTotalIndexCount());
        ImGui::Text("Textures loaded: %zu", graphics::TextureLibrary::Get().Count());

        ImGui::Separator();
        auto& world = *m_State.ActiveWorld();
        ImGui::Text("Entities: %zu", world.GetEntityCount());
        ImGui::Text("Scene: %s%s", world.GetName().c_str(), m_State.IsPlaying() ? (world.IsPaused() ? " (paused)" : " (playing)") : "");
        ImGui::Text("Physics contacts: %u", world.GetPhysics().GetLastContactCount());
        ImGui::Text("Active sounds: %zu", audio::AudioEngine::GetActiveSoundCount());
        ImGui::Text("Viewport: %.0f x %.0f", m_State.ViewportSize.x, m_State.ViewportSize.y);
        ImGui::Text("Editor camera: (%.2f, %.2f) zoom %.2f", m_State.Camera.GetPosition().x, m_State.Camera.GetPosition().y, m_State.Camera.GetZoom());

        ImGui::Separator();
        ImGui::TextWrapped("GPU: %s", app.GetWindow().GetGLRenderer().c_str());
        ImGui::TextWrapped("OpenGL: %s", app.GetWindow().GetGLVersion().c_str());

        ImGui::End();
    }
}
