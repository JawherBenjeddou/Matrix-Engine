#include "EditorLayer.h"
#include "EditorUI.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace Matrix::editor
{
    using namespace graphics;

    namespace
    {
        constexpr const char* SceneFilter = "Matrix Scene (*.mxscene)\0*.mxscene\0All files\0*.*\0";
        constexpr const char* DefaultScenePath = "assets/scenes/Demo.mxscene";

        std::string Timestamp()
        {
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm local{};
            localtime_s(&local, &now);
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &local);
            return buffer;
        }
    }

    EditorLayer::EditorLayer()
        : m_HierarchyPanel(m_State),
          m_InspectorPanel(m_State),
          m_ContentBrowserPanel(m_State),
          m_ConsolePanel(m_State),
          m_StatsPanel(m_State)
    {
    }

    EditorLayer::~EditorLayer() = default;

    bool EditorLayer::OnAttach()
    {
        FramebufferSpec spec;
        spec.Width = 1280;
        spec.Height = 720;
        spec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::DEPTH24STENCIL8 };
        m_Framebuffer = CreateExcl<Framebuffer>(spec);

        m_HierarchyPanel.OnFocusEntity = [this](Entity entity) { FocusEntity(entity); };
        m_ContentBrowserPanel.OnOpenScene = [this](const std::string& path) { OpenScene(path); };

        // Command line: --scene <path>, --screenshot <path>, --frames <n>, --play
        std::string scenePath = core::Application::GetArgValue("--scene", DefaultScenePath);
        if (!util::FileSystem::Exists(scenePath) || !OpenScene(scenePath))
            NewScene();

        m_AutoScreenshotPath = core::Application::GetArgValue("--screenshot");
        if (!m_AutoScreenshotPath.empty())
        {
            m_AutoScreenshotFrame = std::max(5, std::atoi(core::Application::GetArgValue("--frames", "60").c_str()));
            MX_INFO("Automatic screenshot scheduled to '{}' after {} frames", m_AutoScreenshotPath, m_AutoScreenshotFrame);
        }
        if (core::Application::HasArg("--play"))
            OnScenePlay();

        // --camera x,y,zoom and --select <entity name> make automated screenshots reproducible.
        std::string cameraArg = core::Application::GetArgValue("--camera");
        if (!cameraArg.empty())
        {
            float x = 0.0f, y = 0.0f, zoom = 12.0f;
            if (std::sscanf(cameraArg.c_str(), "%f,%f,%f", &x, &y, &zoom) >= 2)
            {
                m_State.Camera.SetPosition({ x, y });
                m_State.Camera.SetZoom(zoom);
            }
        }
        std::string selectArg = core::Application::GetArgValue("--select");
        if (!selectArg.empty())
            m_State.Selected = m_State.ActiveWorld()->FindEntityByName(selectArg);

        m_State.Camera.SetViewportSize(static_cast<uint32_t>(m_State.ViewportSize.x), static_cast<uint32_t>(m_State.ViewportSize.y));
        UpdateWindowTitle();
        return true;
    }

    void EditorLayer::OnDetach()
    {
        if (m_State.IsPlaying())
            OnSceneStop();
        m_State.EditorWorld.reset();
        m_Framebuffer.reset();
    }

    // --- Scene handling ----------------------------------------------------------------------

    void EditorLayer::NewScene()
    {
        if (m_State.IsPlaying())
            OnSceneStop();
        m_State.EditorWorld = CreateShared<World>("Untitled");
        m_State.EditorWorld->OnViewportResize(static_cast<uint32_t>(m_State.ViewportSize.x), static_cast<uint32_t>(m_State.ViewportSize.y));
        m_State.ClearSelection();
        m_State.Camera.Reset();
        UpdateWindowTitle();
        MX_INFO("New scene created");
    }

    bool EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        if (path.extension() != WorldSerializer::Extension)
        {
            MX_WARN("'{}' is not a Matrix scene file", path.string());
            return false;
        }
        if (m_State.IsPlaying())
            OnSceneStop();

        auto world = CreateShared<World>();
        WorldSerializer serializer(world);
        if (!serializer.Deserialize(path))
            return false;

        m_State.EditorWorld = world;
        m_State.EditorWorld->OnViewportResize(static_cast<uint32_t>(m_State.ViewportSize.x), static_cast<uint32_t>(m_State.ViewportSize.y));
        m_State.ClearSelection();
        UpdateWindowTitle();
        return true;
    }

    void EditorLayer::OpenSceneDialog()
    {
        std::string chosen = util::FileDialogs::OpenFile(SceneFilter, util::FileSystem::GetAssetsDirectory() / "scenes");
        if (!chosen.empty())
            OpenScene(chosen);
    }

    bool EditorLayer::SaveScene()
    {
        if (m_State.EditorWorld->GetFilePath().empty())
            return SaveSceneAs();
        WorldSerializer serializer(m_State.EditorWorld);
        bool ok = serializer.Serialize(m_State.EditorWorld->GetFilePath());
        UpdateWindowTitle();
        return ok;
    }

    bool EditorLayer::SaveSceneAs()
    {
        std::string chosen = util::FileDialogs::SaveFile(SceneFilter, util::FileSystem::GetAssetsDirectory() / "scenes", "mxscene");
        if (chosen.empty())
            return false;
        std::filesystem::path path(chosen);
        if (path.extension() != WorldSerializer::Extension)
            path += WorldSerializer::Extension;
        WorldSerializer serializer(m_State.EditorWorld);
        bool ok = serializer.Serialize(path);
        UpdateWindowTitle();
        return ok;
    }

    void EditorLayer::UpdateWindowTitle()
    {
        std::string file = m_State.EditorWorld->GetFilePath();
        std::string title = "Matrix Editor - " + m_State.EditorWorld->GetName();
        if (!file.empty())
            title += "  [" + file + "]";
        if (m_State.IsPlaying())
            title += "  (playing)";
        core::Application::Get().GetWindow().SetTitle(title);
    }

    // --- Play mode ---------------------------------------------------------------------------

    void EditorLayer::OnScenePlay()
    {
        if (m_State.IsPlaying())
        {
            m_State.RuntimeWorld->SetPaused(false);
            return;
        }
        core::UUID selectedId = m_State.Selected ? m_State.Selected.GetUUID() : core::UUID::Invalid();
        m_State.RuntimeWorld = World::Copy(m_State.EditorWorld);
        m_State.RuntimeWorld->OnViewportResize(static_cast<uint32_t>(m_State.ViewportSize.x), static_cast<uint32_t>(m_State.ViewportSize.y));
        m_State.RuntimeWorld->OnRuntimeStart();
        m_State.Selected = selectedId.IsValid() ? m_State.RuntimeWorld->GetEntityByUUID(selectedId) : Entity();
        UpdateWindowTitle();
    }

    void EditorLayer::OnScenePause()
    {
        if (m_State.IsPlaying())
            m_State.RuntimeWorld->SetPaused(!m_State.RuntimeWorld->IsPaused());
    }

    void EditorLayer::OnSceneStep()
    {
        if (!m_State.IsPlaying())
            OnScenePlay();
        m_State.RuntimeWorld->SetPaused(true);
        m_State.RuntimeWorld->Step(1);
    }

    void EditorLayer::OnSceneStop()
    {
        if (!m_State.IsPlaying())
            return;
        core::UUID selectedId = m_State.Selected ? m_State.Selected.GetUUID() : core::UUID::Invalid();
        m_State.RuntimeWorld->OnRuntimeStop();
        m_State.RuntimeWorld.reset();
        m_State.Selected = selectedId.IsValid() ? m_State.EditorWorld->GetEntityByUUID(selectedId) : Entity();
        m_State.Hovered = Entity();
        UpdateWindowTitle();
    }

    bool EditorLayer::CaptureScreenshot(const std::filesystem::path& path)
    {
        std::filesystem::path target = path;
        if (target.empty())
            target = util::FileSystem::GetRootDirectory() / "screenshots" / ("MatrixEditor_" + Timestamp() + ".png");
        // Deferred: the back buffer only contains the complete frame after the GUI has been rendered.
        m_PendingScreenshotPath = util::FileSystem::Resolve(target).string();
        return true;
    }

    void EditorLayer::OnPostRender()
    {
        if (m_PendingScreenshotPath.empty())
            return;
        core::Application::Get().GetWindow().CaptureScreenshot(m_PendingScreenshotPath);
        m_PendingScreenshotPath.clear();
        if (m_CloseAfterScreenshot)
            core::Application::Get().Close();
    }

    void EditorLayer::FocusEntity(Entity entity)
    {
        if (entity && m_State.ActiveWorld()->IsValid(entity))
            m_State.Camera.Focus(entity.GetWorldPosition());
    }

    // --- Frame update ------------------------------------------------------------------------

    glm::vec2 EditorLayer::GetMouseInViewport() const
    {
        glm::vec2 mouse = Matrix::ui::Input::GetMousePosition();
        return mouse - m_State.ViewportBoundsMin;
    }

    glm::mat4 EditorLayer::GetViewProjection() const
    {
        if (m_State.IsPlaying())
        {
            Entity camera = m_State.RuntimeWorld->GetPrimaryCameraEntity();
            if (camera)
            {
                const auto& component = camera.GetComponent<CameraComponent>();
                return component.Camera.GetProjection() * glm::inverse(m_State.RuntimeWorld->GetWorldTransform(camera));
            }
        }
        return m_State.Camera.GetViewProjection();
    }

    void EditorLayer::OnUpdate(float deltaTime)
    {
        Renderer2D::ResetStats();

        // Keep the framebuffer in sync with the viewport panel.
        uint32_t width = static_cast<uint32_t>(std::max(m_State.ViewportSize.x, 1.0f));
        uint32_t height = static_cast<uint32_t>(std::max(m_State.ViewportSize.y, 1.0f));
        if (m_Framebuffer->GetWidth() != width || m_Framebuffer->GetHeight() != height)
        {
            m_Framebuffer->Resize(width, height);
            m_State.Camera.SetViewportSize(width, height);
            m_State.EditorWorld->OnViewportResize(width, height);
            if (m_State.RuntimeWorld)
                m_State.RuntimeWorld->OnViewportResize(width, height);
        }

        bool gameCameraActive = m_State.IsPlaying() && m_State.RuntimeWorld->GetPrimaryCameraEntity();
        bool textInput = ImGui::GetCurrentContext() && ImGui::GetIO().WantTextInput;
        if (!gameCameraActive && !textInput)
            m_State.Camera.OnUpdate(deltaTime, m_State.ViewportHovered, m_State.ViewportFocused, GetMouseInViewport());

        if (m_State.IsPlaying())
            m_State.RuntimeWorld->OnUpdateRuntime(deltaTime);
        else
            m_State.EditorWorld->OnUpdateEditor(deltaTime);

        RenderViewport();
        HandleViewportInput();
        HandleShortcuts();

        // Automated screenshot (used for documentation and smoke tests).
        if (!m_AutoScreenshotPath.empty())
        {
            ++m_FrameCounter;
            if (m_FrameCounter == m_AutoScreenshotFrame)
            {
                m_CloseAfterScreenshot = !core::Application::HasArg("--keep-open");
                CaptureScreenshot(m_AutoScreenshotPath);
                m_AutoScreenshotPath.clear();
            }
        }
    }

    void EditorLayer::RenderViewport()
    {
        World& world = *m_State.ActiveWorld();
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor(world.BackgroundColor);
        RenderCommand::Clear();
        m_Framebuffer->ClearAttachment(1, -1);

        glm::mat4 viewProjection = GetViewProjection();
        world.RenderScene(viewProjection);
        DrawOverlays(viewProjection);
        UpdateMousePicking();

        m_Framebuffer->Unbind();
        // Restore the window viewport for ImGui.
        auto& window = core::Application::Get().GetWindow();
        RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());
    }

    void EditorLayer::DrawGrid()
    {
        const auto& camera = m_State.Camera.GetCamera();
        CameraBounds bounds = camera.GetBounds();
        glm::vec2 position = m_State.Camera.GetPosition();
        float zoom = camera.GetZoom();
        float spacing = zoom < 30.0f ? 1.0f : zoom < 300.0f ? 10.0f : 100.0f;

        float left = std::floor((bounds.Left + position.x) / spacing) * spacing;
        float right = std::ceil((bounds.Right + position.x) / spacing) * spacing;
        float bottom = std::floor((bounds.Bottom + position.y) / spacing) * spacing;
        float top = std::ceil((bounds.Top + position.y) / spacing) * spacing;

        const glm::vec4 minor(1.0f, 1.0f, 1.0f, 0.06f);
        const glm::vec4 major(1.0f, 1.0f, 1.0f, 0.16f);
        const glm::vec4 axisX(0.9f, 0.3f, 0.3f, 0.6f);
        const glm::vec4 axisY(0.3f, 0.9f, 0.3f, 0.6f);
        const float z = -50.0f;

        for (float x = left; x <= right; x += spacing)
        {
            glm::vec4 color = std::abs(x) < 0.001f ? axisY : (std::fmod(std::abs(x), spacing * 10.0f) < 0.001f ? major : minor);
            Renderer2D::DrawLine({ x, bottom, z }, { x, top, z }, color);
        }
        for (float y = bottom; y <= top; y += spacing)
        {
            glm::vec4 color = std::abs(y) < 0.001f ? axisX : (std::fmod(std::abs(y), spacing * 10.0f) < 0.001f ? major : minor);
            Renderer2D::DrawLine({ left, y, z }, { right, y, z }, color);
        }
    }

    void EditorLayer::DrawOverlays(const glm::mat4& viewProjection)
    {
        World& world = *m_State.ActiveWorld();
        bool gameCameraActive = m_State.IsPlaying() && world.GetPrimaryCameraEntity();

        RenderCommand::SetDepthTest(false);
        Renderer2D::BeginScene(viewProjection);

        if (m_State.ShowGrid && !gameCameraActive)
            DrawGrid();

        if (m_State.ShowColliders)
        {
            const glm::vec4 colliderColor(0.2f, 1.0f, 0.3f, 0.9f);
            for (auto handle : world.View<TransformComponent, BoxCollider2DComponent>())
            {
                Entity entity(handle, &world);
                auto aabb = physics::Physics2D::ComputeBoxAABB(world.GetWorldTransform(entity), entity.GetComponent<BoxCollider2DComponent>());
                Renderer2D::DrawRect(glm::vec3(aabb.Center(), 0.0f), aabb.Max - aabb.Min, colliderColor);
            }
            for (auto handle : world.View<TransformComponent, CircleCollider2DComponent>())
            {
                Entity entity(handle, &world);
                glm::vec2 center; float radius;
                physics::Physics2D::ComputeCircle(world.GetWorldTransform(entity), entity.GetComponent<CircleCollider2DComponent>(), center, radius);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(center, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(radius * 2.0f));
                Renderer2D::DrawCircle(transform, colliderColor, 0.06f, 0.01f);
            }
        }

        // Camera frustums.
        if (!gameCameraActive)
        {
            for (auto handle : world.View<TransformComponent, CameraComponent>())
            {
                Entity entity(handle, &world);
                const auto& camera = entity.GetComponent<CameraComponent>();
                float h = camera.Camera.GetOrthographicSize();
                float w = h * camera.Camera.GetAspectRatio();
                glm::mat4 transform = world.GetWorldTransform(entity) * glm::scale(glm::mat4(1.0f), glm::vec3(w, h, 1.0f));
                Renderer2D::DrawRect(transform, camera.Primary ? glm::vec4(1.0f, 1.0f, 1.0f, 0.7f) : glm::vec4(1.0f, 1.0f, 1.0f, 0.3f));
            }
        }

        if (m_State.ShowSelectionOutline && m_State.Selected && world.IsValid(m_State.Selected))
        {
            glm::mat4 transform = world.GetWorldTransform(m_State.Selected);
            Renderer2D::DrawRect(transform, glm::vec4(1.0f, 0.6f, 0.1f, 1.0f));
            // Pivot marker
            glm::vec3 pivot(transform[3]);
            float size = m_State.Camera.GetZoom() * 0.012f;
            Renderer2D::DrawLine(pivot - glm::vec3(size, 0, 0), pivot + glm::vec3(size, 0, 0), glm::vec4(1.0f, 0.6f, 0.1f, 1.0f));
            Renderer2D::DrawLine(pivot - glm::vec3(0, size, 0), pivot + glm::vec3(0, size, 0), glm::vec4(1.0f, 0.6f, 0.1f, 1.0f));
        }
        else if (m_State.Hovered && world.IsValid(m_State.Hovered) && m_State.Hovered != m_State.Selected)
        {
            Renderer2D::DrawRect(world.GetWorldTransform(m_State.Hovered), glm::vec4(1.0f, 1.0f, 1.0f, 0.35f));
        }

        Renderer2D::EndScene();
        RenderCommand::SetDepthTest(true);
    }

    void EditorLayer::UpdateMousePicking()
    {
        m_State.Hovered = Entity();
        if (!m_State.ViewportHovered)
            return;
        glm::vec2 mouse = GetMouseInViewport();
        int x = static_cast<int>(mouse.x);
        int y = static_cast<int>(m_State.ViewportSize.y - mouse.y); // framebuffer origin is bottom-left
        if (x < 0 || y < 0 || x >= static_cast<int>(m_Framebuffer->GetWidth()) || y >= static_cast<int>(m_Framebuffer->GetHeight()))
            return;
        int id = m_Framebuffer->ReadPixel(1, x, y);
        if (id >= 0)
            m_State.Hovered = m_State.ActiveWorld()->GetEntityByHandle(static_cast<entt::entity>(id));
    }

    void EditorLayer::HandleViewportInput()
    {
        using Matrix::ui::Input;
        World& world = *m_State.ActiveWorld();
        bool panning = Input::IsAltDown() || Input::IsMouseButtonDown(Mouse::BUTTON_MIDDLE) || Input::IsMouseButtonDown(Mouse::BUTTON_RIGHT);

        if (m_State.ViewportHovered && Input::IsMouseButtonPressed(Mouse::BUTTON_LEFT) && !panning)
        {
            if (m_State.Hovered && m_State.Hovered == m_State.Selected)
            {
                m_Dragging = true;
            }
            else
            {
                m_State.Selected = m_State.Hovered;
                m_Dragging = m_State.Selected.IsValid();
            }
            m_DragLastWorld = m_State.Camera.ScreenToWorld(GetMouseInViewport());
            m_DragLastMouse = GetMouseInViewport();
        }

        if (!Input::IsMouseButtonDown(Mouse::BUTTON_LEFT))
            m_Dragging = false;

        if (m_Dragging && m_State.Selected && world.IsValid(m_State.Selected))
        {
            glm::vec2 mouseNow = GetMouseInViewport();
            glm::vec2 worldNow = m_State.Camera.ScreenToWorld(mouseNow);
            glm::vec2 worldDelta = worldNow - m_DragLastWorld;
            glm::vec2 mouseDelta = mouseNow - m_DragLastMouse;
            if (worldDelta.x != 0.0f || worldDelta.y != 0.0f || mouseDelta.x != 0.0f)
            {
                auto& transform = m_State.Selected.GetComponent<TransformComponent>();
                switch (m_State.Gizmo)
                {
                case GizmoMode::Translate:
                {
                    glm::mat4 worldTransform = world.GetWorldTransform(m_State.Selected);
                    worldTransform[3].x += worldDelta.x;
                    worldTransform[3].y += worldDelta.y;
                    world.SetWorldTransform(m_State.Selected, worldTransform);
                    if (Input::IsControlDown())
                    {
                        transform.Position.x = std::round(transform.Position.x * 2.0f) * 0.5f;
                        transform.Position.y = std::round(transform.Position.y * 2.0f) * 0.5f;
                    }
                    break;
                }
                case GizmoMode::Rotate:
                    transform.Rotation += mouseDelta.x * 0.5f;
                    if (Input::IsControlDown())
                        transform.Rotation = std::round(transform.Rotation / 15.0f) * 15.0f;
                    break;
                case GizmoMode::Scale:
                {
                    float factor = 1.0f + (mouseDelta.x - mouseDelta.y) * 0.01f;
                    transform.Scale.x *= factor;
                    transform.Scale.y *= factor;
                    break;
                }
                }
            }
            m_DragLastWorld = worldNow;
            m_DragLastMouse = mouseNow;
        }
    }

    void EditorLayer::HandleShortcuts()
    {
        using Matrix::ui::Input;
        if (ImGui::GetCurrentContext() && ImGui::GetIO().WantTextInput)
            return;

        bool ctrl = Input::IsControlDown();
        bool shift = Input::IsShiftDown();
        World& world = *m_State.ActiveWorld();

        if (ctrl && Input::IsKeyPressed(Key::N)) NewScene();
        if (ctrl && Input::IsKeyPressed(Key::O)) OpenSceneDialog();
        if (ctrl && Input::IsKeyPressed(Key::S)) { if (shift) SaveSceneAs(); else SaveScene(); }
        if (ctrl && Input::IsKeyPressed(Key::P)) { if (m_State.IsPlaying()) OnSceneStop(); else OnScenePlay(); }
        if (ctrl && Input::IsKeyPressed(Key::D) && m_State.Selected && world.IsValid(m_State.Selected))
            m_State.Selected = world.DuplicateEntity(m_State.Selected);
        if (Input::IsKeyPressed(Key::DEL) && m_State.Selected && world.IsValid(m_State.Selected))
        {
            world.DestroyEntity(m_State.Selected);
            m_State.ClearSelection();
        }
        if (Input::IsKeyPressed(Key::F12)) CaptureScreenshot();
        if (Input::IsKeyPressed(Key::ESCAPE) && m_State.ViewportHovered) m_State.ClearSelection();

        if (m_State.ViewportHovered || m_State.ViewportFocused)
        {
            if (Input::IsKeyPressed(Key::F)) FocusEntity(m_State.Selected);
            if (!ctrl && Input::IsKeyPressed(Key::Q)) m_State.Gizmo = GizmoMode::Translate;
            if (!ctrl && Input::IsKeyPressed(Key::E)) m_State.Gizmo = GizmoMode::Rotate;
            if (!ctrl && Input::IsKeyPressed(Key::R)) m_State.Gizmo = GizmoMode::Scale;
            if (Input::IsKeyPressed(Key::HOME)) m_State.Camera.Reset();
        }
    }

    // --- ImGui -------------------------------------------------------------------------------

    void EditorLayer::OnImGuiRender()
    {
        DrawDockspace();
        DrawToolbar();
        DrawViewportPanel();
        m_HierarchyPanel.OnImGuiRender();
        m_InspectorPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();
        m_ConsolePanel.OnImGuiRender();
        m_StatsPanel.OnImGuiRender();
        if (m_State.ShowDemoWindow)
            ImGui::ShowDemoWindow(&m_State.ShowDemoWindow);
        DrawAboutWindow();
        DrawControlsWindow();
    }

    void EditorLayer::DrawDockspace()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                               | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
                               | ImGuiWindowFlags_NoBackground;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("MatrixDockspace", nullptr, flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspaceId = ImGui::GetID("MatrixDockspaceID");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        // Build the default layout once the dockspace node exists (first run or explicit reset).
        bool hadLayout = core::Application::Get().GetEngine().GetGui()->HadSavedLayout();
        if (!m_LayoutBuilt && (!hadLayout || m_ResetLayoutRequested))
        {
            BuildDefaultLayout(dockspaceId);
            m_ResetLayoutRequested = false;
        }
        m_LayoutBuilt = true;

        DrawMenuBar();
        ImGui::End();
    }

    void EditorLayer::BuildDefaultLayout(unsigned int dockspaceId)
    {
        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

        ImGuiID center = dockspaceId;
        ImGuiID left = ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.20f, nullptr, &center);
        ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.26f, nullptr, &center);
        ImGuiID bottom = ImGui::DockBuilderSplitNode(center, ImGuiDir_Down, 0.30f, nullptr, &center);
        ImGuiID top = ImGui::DockBuilderSplitNode(center, ImGuiDir_Up, 0.07f, nullptr, &center);
        ImGuiID rightBottom = ImGui::DockBuilderSplitNode(right, ImGuiDir_Down, 0.35f, nullptr, &right);
        ImGuiID bottomRight = ImGui::DockBuilderSplitNode(bottom, ImGuiDir_Right, 0.5f, nullptr, &bottom);

        ImGui::DockBuilderDockWindow("Scene Hierarchy", left);
        ImGui::DockBuilderDockWindow("Inspector", right);
        ImGui::DockBuilderDockWindow("Statistics", rightBottom);
        ImGui::DockBuilderDockWindow("Content Browser", bottom);
        ImGui::DockBuilderDockWindow("Console", bottomRight);
        ImGui::DockBuilderDockWindow("Toolbar", top);
        ImGui::DockBuilderDockWindow("Viewport", center);
        ImGui::DockBuilderFinish(dockspaceId);
    }

    void EditorLayer::DrawMenuBar()
    {
        if (!ImGui::BeginMenuBar())
            return;

        World& world = *m_State.ActiveWorld();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) NewScene();
            if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) OpenSceneDialog();
            if (util::FileSystem::Exists(DefaultScenePath) && ImGui::MenuItem("Open Demo Scene")) OpenScene(DefaultScenePath);
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S")) SaveScene();
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SaveSceneAs();
            ImGui::Separator();
            if (ImGui::MenuItem("Screenshot", "F12")) CaptureScreenshot();
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) core::Application::Get().Close();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            bool hasSelection = m_State.Selected && world.IsValid(m_State.Selected);
            if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, hasSelection)) m_State.Selected = world.DuplicateEntity(m_State.Selected);
            if (ImGui::MenuItem("Delete", "Del", false, hasSelection)) { world.DestroyEntity(m_State.Selected); m_State.ClearSelection(); }
            if (ImGui::MenuItem("Deselect", "Esc", false, hasSelection)) m_State.ClearSelection();
            ImGui::Separator();
            if (ImGui::MenuItem("Focus Selected", "F", false, hasSelection)) FocusEntity(m_State.Selected);
            if (ImGui::MenuItem("Reset Camera", "Home")) m_State.Camera.Reset();
            ImGui::Separator();
            if (ImGui::MenuItem("Translate", "Q", m_State.Gizmo == GizmoMode::Translate)) m_State.Gizmo = GizmoMode::Translate;
            if (ImGui::MenuItem("Rotate", "E", m_State.Gizmo == GizmoMode::Rotate)) m_State.Gizmo = GizmoMode::Rotate;
            if (ImGui::MenuItem("Scale", "R", m_State.Gizmo == GizmoMode::Scale)) m_State.Gizmo = GizmoMode::Scale;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Entity"))
        {
            glm::vec2 at = m_State.Camera.GetPosition();
            Entity created;
            if (ImGui::MenuItem("Empty Entity")) created = SceneHierarchyPanel::CreateEntity(world, EntityKind::Empty, {}, at);
            if (ImGui::MenuItem("Sprite")) created = SceneHierarchyPanel::CreateEntity(world, EntityKind::Sprite, {}, at);
            if (ImGui::MenuItem("Circle")) created = SceneHierarchyPanel::CreateEntity(world, EntityKind::Circle, {}, at);
            if (ImGui::MenuItem("Camera")) created = SceneHierarchyPanel::CreateEntity(world, EntityKind::Camera, {}, at);
            ImGui::Separator();
            if (ImGui::MenuItem("Physics Box")) created = SceneHierarchyPanel::CreateEntity(world, EntityKind::PhysicsBox, {}, at);
            if (ImGui::MenuItem("Physics Circle")) created = SceneHierarchyPanel::CreateEntity(world, EntityKind::PhysicsCircle, {}, at);
            if (ImGui::MenuItem("Static Platform")) created = SceneHierarchyPanel::CreateEntity(world, EntityKind::StaticPlatform, {}, at);
            if (created) m_State.SelectEntity(created);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Scene"))
        {
            bool playing = m_State.IsPlaying();
            if (ImGui::MenuItem("Play", "Ctrl+P", false, !playing || world.IsPaused())) OnScenePlay();
            if (ImGui::MenuItem("Pause", nullptr, playing && world.IsPaused(), playing)) OnScenePause();
            if (ImGui::MenuItem("Step", nullptr, false)) OnSceneStep();
            if (ImGui::MenuItem("Stop", "Ctrl+P", false, playing)) OnSceneStop();
            ImGui::Separator();
            if (ImGui::MenuItem("Reload Shaders")) ShaderFactory::GetInstance().ReloadAll();
            if (ImGui::MenuItem("Reload Textures")) TextureLibrary::Get().ReloadAll();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Scene Hierarchy", nullptr, &m_State.ShowHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &m_State.ShowInspector);
            ImGui::MenuItem("Content Browser", nullptr, &m_State.ShowContentBrowser);
            ImGui::MenuItem("Console", nullptr, &m_State.ShowConsole);
            ImGui::MenuItem("Statistics", nullptr, &m_State.ShowStats);
            ImGui::Separator();
            ImGui::MenuItem("Show Grid", nullptr, &m_State.ShowGrid);
            ImGui::MenuItem("Show Colliders", nullptr, &m_State.ShowColliders);
            ImGui::MenuItem("Show Selection Outline", nullptr, &m_State.ShowSelectionOutline);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Demo Window", nullptr, &m_State.ShowDemoWindow);
            if (ImGui::MenuItem("Reset Layout")) { m_ResetLayoutRequested = true; m_LayoutBuilt = false; }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("Controls", nullptr, &m_State.ShowControls);
            ImGui::MenuItem("About Matrix Engine", nullptr, &m_State.ShowAbout);
            ImGui::EndMenu();
        }

        // Right-aligned status.
        std::string status = m_State.IsPlaying() ? (world.IsPaused() ? "PAUSED" : "PLAYING") : "EDIT";
        float width = ImGui::CalcTextSize(status.c_str()).x + 20.0f;
        ImGui::SameLine(ImGui::GetWindowWidth() - width);
        ImGui::TextColored(m_State.IsPlaying() ? ImVec4(0.4f, 0.9f, 0.4f, 1.0f) : ImVec4(0.6f, 0.6f, 0.65f, 1.0f), "%s", status.c_str());
        ImGui::EndMenuBar();
    }

    void EditorLayer::DrawToolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 4.0f));
        ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        bool playing = m_State.IsPlaying();
        bool paused = playing && m_State.RuntimeWorld->IsPaused();
        float buttonWidth = 64.0f;
        float totalWidth = buttonWidth * 4 + ImGui::GetStyle().ItemSpacing.x * 3;

        // Gizmo mode on the left.
        auto modeButton = [&](const char* label, GizmoMode mode)
        {
            bool active = m_State.Gizmo == mode;
            if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.8f));
            if (ImGui::Button(label, ImVec2(buttonWidth, 0))) m_State.Gizmo = mode;
            if (active) ImGui::PopStyleColor();
            ImGui::SameLine();
        };
        modeButton("Move (Q)", GizmoMode::Translate);
        modeButton("Rotate (E)", GizmoMode::Rotate);
        modeButton("Scale (R)", GizmoMode::Scale);

        // Transport controls centred.
        ImGui::SameLine(std::max((ImGui::GetWindowContentRegionMax().x - totalWidth) * 0.5f, ImGui::GetCursorPosX() + 20.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, playing && !paused ? ImVec4(0.2f, 0.6f, 0.3f, 1.0f) : ImVec4(0.16f, 0.16f, 0.18f, 1.0f));
        if (ImGui::Button(playing && !paused ? "Playing" : "Play", ImVec2(buttonWidth, 0))) OnScenePlay();
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::BeginDisabled(!playing);
        ImGui::PushStyleColor(ImGuiCol_Button, paused ? ImVec4(0.75f, 0.6f, 0.2f, 1.0f) : ImVec4(0.16f, 0.16f, 0.18f, 1.0f));
        if (ImGui::Button("Pause", ImVec2(buttonWidth, 0))) OnScenePause();
        ImGui::PopStyleColor();
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Step", ImVec2(buttonWidth, 0))) OnSceneStep();
        ImGui::SameLine();
        ImGui::BeginDisabled(!playing);
        ImGui::PushStyleColor(ImGuiCol_Button, playing ? ImVec4(0.7f, 0.25f, 0.25f, 1.0f) : ImVec4(0.16f, 0.16f, 0.18f, 1.0f));
        if (ImGui::Button("Stop", ImVec2(buttonWidth, 0))) OnSceneStop();
        ImGui::PopStyleColor();
        ImGui::EndDisabled();

        // Options on the right.
        float optionsWidth = 260.0f;
        ImGui::SameLine(std::max(ImGui::GetWindowContentRegionMax().x - optionsWidth, ImGui::GetCursorPosX() + 20.0f));
        ImGui::Checkbox("Grid", &m_State.ShowGrid);
        ImGui::SameLine();
        ImGui::Checkbox("Colliders", &m_State.ShowColliders);
        ImGui::SameLine();
        if (ImGui::Button("Screenshot")) CaptureScreenshot();

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::DrawViewportPanel()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
        ImVec2 regionMax = ImGui::GetWindowContentRegionMax();
        ImVec2 offset = ImGui::GetWindowPos();
        m_State.ViewportBoundsMin = { regionMin.x + offset.x, regionMin.y + offset.y };
        m_State.ViewportBoundsMax = { regionMax.x + offset.x, regionMax.y + offset.y };

        m_State.ViewportFocused = ImGui::IsWindowFocused();
        m_State.ViewportHovered = ImGui::IsWindowHovered();

        ImVec2 size = ImGui::GetContentRegionAvail();
        if (size.x >= 1.0f && size.y >= 1.0f)
            m_State.ViewportSize = { size.x, size.y };

        uint32_t texture = m_Framebuffer->GetColorAttachmentRendererID(0);
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture)), ImVec2(m_State.ViewportSize.x, m_State.ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

        // Drop assets into the viewport: scenes open, images become sprites.
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ui::ContentBrowserPayload))
            {
                std::filesystem::path path(static_cast<const char*>(payload->Data));
                std::string ext = path.extension().string();
                if (ext == WorldSerializer::Extension)
                {
                    OpenScene(path);
                }
                else if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga")
                {
                    glm::vec2 worldPos = m_State.Camera.ScreenToWorld(GetMouseInViewport());
                    Entity sprite = SceneHierarchyPanel::CreateEntity(*m_State.ActiveWorld(), EntityKind::Sprite, {}, worldPos);
                    sprite.SetName(path.stem().string());
                    auto& component = sprite.GetComponent<SpriteComponent>();
                    component.TexturePath = util::FileSystem::Normalize(path);
                    component.Color = glm::vec4(1.0f);
                    auto tex = TextureLibrary::Get().Load(component.TexturePath);
                    if (tex && tex->GetHeight() > 0)
                        sprite.GetComponent<TransformComponent>().Scale = glm::vec3(tex->GetAspectRatio(), 1.0f, 1.0f);
                    m_State.SelectEntity(sprite);
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Overlay text in the viewport corner.
        ImGui::SetCursorPos(ImVec2(regionMin.x + 8.0f, regionMin.y + 8.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.55f));
        const char* gizmo = m_State.Gizmo == GizmoMode::Translate ? "Move" : m_State.Gizmo == GizmoMode::Rotate ? "Rotate" : "Scale";
        if (m_State.IsPlaying())
            ImGui::Text("%s  |  %s", m_State.RuntimeWorld->IsPaused() ? "Paused" : "Playing", m_State.RuntimeWorld->GetPrimaryCameraEntity() ? "Scene camera" : "Editor camera (no primary camera)");
        else
            ImGui::Text("%s  |  zoom %.1f  |  %s", gizmo, m_State.Camera.GetZoom(), m_State.Hovered ? m_State.Hovered.GetName().c_str() : "");
        ImGui::PopStyleColor();

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::DrawAboutWindow()
    {
        if (!m_State.ShowAbout)
            return;
        ImGui::SetNextWindowSize(ImVec2(420, 0), ImGuiCond_Appearing);
        if (ImGui::Begin("About Matrix Engine", &m_State.ShowAbout, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse))
        {
            ui::PushBoldFont();
            ImGui::Text("%s %s", MX_ENGINE_NAME, MX_VERSION_STRING);
            ui::PopBoldFont();
            ImGui::TextWrapped("An open-source 2D game engine with an entity-component scene model, a batched OpenGL renderer, built-in 2D physics, audio, C++ scripting and this editor.");
            ImGui::Separator();
            auto& window = core::Application::Get().GetWindow();
            ImGui::TextWrapped("Renderer: %s", window.GetGLRenderer().c_str());
            ImGui::TextWrapped("OpenGL: %s", window.GetGLVersion().c_str());
            ImGui::Text("Dear ImGui %s", IMGUI_VERSION);
            ImGui::Separator();
            ImGui::TextDisabled("https://github.com/JawherBenjeddou/Matrix-Engine");
        }
        ImGui::End();
    }

    void EditorLayer::DrawControlsWindow()
    {
        if (!m_State.ShowControls)
            return;
        ImGui::SetNextWindowSize(ImVec2(460, 0), ImGuiCond_Appearing);
        if (ImGui::Begin("Editor Controls", &m_State.ShowControls, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse))
        {
            if (ImGui::BeginTable("controls", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH))
            {
                auto row = [](const char* key, const char* action) { ImGui::TableNextRow(); ImGui::TableNextColumn(); ImGui::TextUnformatted(key); ImGui::TableNextColumn(); ImGui::TextUnformatted(action); };
                row("Left click", "Select entity (drag to move/rotate/scale with the active tool)");
                row("Middle / right drag, Alt+drag", "Pan the editor camera");
                row("Mouse wheel", "Zoom towards the cursor");
                row("W A S D / arrows", "Pan the camera (Shift = faster)");
                row("Q / E / R", "Move / Rotate / Scale tool");
                row("Ctrl while dragging", "Snap position (0.5) or rotation (15 deg)");
                row("F", "Focus the selected entity");
                row("Home", "Reset the editor camera");
                row("Ctrl+N / Ctrl+O / Ctrl+S", "New / open / save scene");
                row("Ctrl+Shift+S", "Save scene as...");
                row("Ctrl+D / Delete", "Duplicate / delete selection");
                row("Ctrl+P", "Play / stop");
                row("F12", "Save a screenshot");
                row("In play mode: A/D, Space", "Move / jump (PlatformerController2D)");
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}
