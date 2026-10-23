#include "pch.h"
#include "World.h"

#include "ScriptableEntity.h"
#include "ScriptRegistry.h"
#include "core/Logging.h"
#include "audio/AudioEngine.h"
#include "physics/Physics2D.h"
#include "graphics/renderer/Renderer2D.h"
#include "graphics/textures/TextureLibrary.h"

namespace Matrix::graphics
{
    namespace
    {
        template <typename... Component>
        void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<core::UUID, entt::entity>& enttMap)
        {
            ([&]()
            {
                auto view = src.view<Component>();
                for (auto srcEntity : view)
                {
                    core::UUID uuid = src.get<IDComponent>(srcEntity).ID;
                    auto it = enttMap.find(uuid);
                    if (it == enttMap.end())
                        continue;
                    dst.emplace_or_replace<Component>(it->second, src.get<Component>(srcEntity));
                }
            }(), ...);
        }

        template <typename... Component>
        void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<core::UUID, entt::entity>& enttMap)
        {
            CopyComponent<Component...>(dst, src, enttMap);
        }

        template <typename... Component>
        void CopyComponentIfExists(Entity dst, Entity src)
        {
            ([&]()
            {
                if (src.HasComponent<Component>())
                    dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
            }(), ...);
        }

        template <typename... Component>
        void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
        {
            CopyComponentIfExists<Component...>(dst, src);
        }

        void DecomposeLocal(const glm::mat4& local, TransformComponent& transform)
        {
            transform.Position = glm::vec3(local[3]);
            glm::vec2 xAxis(local[0]);
            glm::vec2 yAxis(local[1]);
            float scaleX = glm::length(xAxis);
            float scaleY = glm::length(yAxis);
            // Detect mirrored axes.
            if (glm::determinant(glm::mat3(local)) < 0.0f)
                scaleY = -scaleY;
            transform.Scale = glm::vec3(scaleX, scaleY, transform.Scale.z);
            transform.Rotation = scaleX > 1e-6f ? glm::degrees(std::atan2(xAxis.y, xAxis.x)) : 0.0f;
        }
    }

    // --- Entity non-template members ---------------------------------------------------------

    core::UUID Entity::GetUUID() const
    {
        return GetComponent<IDComponent>().ID;
    }

    const std::string& Entity::GetName() const
    {
        return GetComponent<TagComponent>().Tag;
    }

    void Entity::SetName(const std::string& name)
    {
        GetComponent<TagComponent>().Tag = name;
    }

    Entity Entity::GetParent() const
    {
        return m_World ? m_World->GetParent(*this) : Entity();
    }

    std::vector<Entity> Entity::GetChildren() const
    {
        return m_World ? m_World->GetChildren(*this) : std::vector<Entity>();
    }

    bool Entity::HasParent() const
    {
        return GetParent().IsValid();
    }

    glm::mat4 Entity::GetWorldTransform() const
    {
        return m_World ? m_World->GetWorldTransform(*this) : glm::mat4(1.0f);
    }

    glm::vec2 Entity::GetWorldPosition() const
    {
        return glm::vec2(GetWorldTransform()[3]);
    }

    // --- World -------------------------------------------------------------------------------

    World::World(const std::string& name)
        : m_Name(name), m_Physics(CreateExcl<physics::Physics2D>(*this))
    {
    }

    World::~World()
    {
        if (m_IsRunning)
            OnRuntimeStop();
    }

    SharedObj<World> World::Copy(const SharedObj<World>& other)
    {
        auto newWorld = CreateShared<World>(other->m_Name);
        newWorld->m_FilePath = other->m_FilePath;
        newWorld->m_ViewportSize = other->m_ViewportSize;
        newWorld->Gravity = other->Gravity;
        newWorld->BackgroundColor = other->BackgroundColor;

        std::unordered_map<core::UUID, entt::entity> enttMap;
        for (entt::entity srcHandle : other->m_EntityOrder)
        {
            core::UUID uuid = other->m_Registry.get<IDComponent>(srcHandle).ID;
            const std::string& name = other->m_Registry.get<TagComponent>(srcHandle).Tag;
            Entity created = newWorld->CreateEntityInternal(uuid, name);
            enttMap[uuid] = created.GetHandle();
        }

        CopyComponent(AllComponents{}, newWorld->m_Registry, other->m_Registry, enttMap);
        newWorld->UpdateCameraAspectRatios();
        return newWorld;
    }

    Entity World::CreateEntityInternal(core::UUID uuid, const std::string& name)
    {
        Entity entity(m_Registry.create(), this);
        m_Registry.emplace<IDComponent>(entity.GetHandle(), uuid);
        m_Registry.emplace<TagComponent>(entity.GetHandle(), name.empty() ? std::string("Entity") : name);
        m_Registry.emplace<TransformComponent>(entity.GetHandle());
        m_EntityMap[uuid] = entity.GetHandle();
        m_EntityOrder.push_back(entity.GetHandle());
        return entity;
    }

    Entity World::CreateEntity(const std::string& name)
    {
        return CreateEntityInternal(core::UUID(), name);
    }

    Entity World::CreateEntityWithUUID(core::UUID uuid, const std::string& name)
    {
        if (!uuid.IsValid() || m_EntityMap.find(uuid) != m_EntityMap.end())
        {
            MX_CORE_WARN("Entity UUID {} is invalid or already used; generating a new one", static_cast<uint64_t>(uuid));
            uuid = core::UUID();
        }
        return CreateEntityInternal(uuid, name);
    }

    void World::DestroyRecursive(Entity entity)
    {
        for (Entity child : GetChildren(entity))
            DestroyRecursive(child);

        if (auto* script = entity.TryGetComponent<NativeScriptComponent>(); script && script->Instance)
        {
            script->Instance->OnDestroy();
            delete script->Instance;
            script->Instance = nullptr;
        }
        if (auto* audio = entity.TryGetComponent<AudioSourceComponent>(); audio && audio->RuntimeHandle)
        {
            audio::AudioEngine::Stop(audio->RuntimeHandle);
            audio->RuntimeHandle = 0;
        }

        m_EntityMap.erase(entity.GetUUID());
        m_EntityOrder.erase(std::remove(m_EntityOrder.begin(), m_EntityOrder.end(), entity.GetHandle()), m_EntityOrder.end());
        m_Registry.destroy(entity.GetHandle());
    }

    void World::DestroyEntity(Entity entity)
    {
        if (!IsValid(entity))
            return;
        RemoveParent(entity, false);
        DestroyRecursive(entity);
    }

    Entity World::DuplicateRecursive(Entity source, Entity newParent)
    {
        Entity copy = CreateEntity(source.GetName());
        CopyComponentIfExists(AllComponents{}, copy, source);
        if (copy.HasComponent<RelationshipComponent>())
            copy.RemoveComponent<RelationshipComponent>();
        if (newParent)
            SetParent(copy, newParent, false);
        for (Entity child : GetChildren(source))
            DuplicateRecursive(child, copy);
        return copy;
    }

    Entity World::DuplicateEntity(Entity entity)
    {
        if (!IsValid(entity))
            return Entity();
        Entity copy = DuplicateRecursive(entity, GetParent(entity));
        copy.SetName(entity.GetName() + " (copy)");
        return copy;
    }

    Entity World::FindEntityByName(std::string_view name)
    {
        for (entt::entity handle : m_EntityOrder)
        {
            if (m_Registry.get<TagComponent>(handle).Tag == name)
                return Entity(handle, this);
        }
        return Entity();
    }

    Entity World::GetEntityByUUID(core::UUID uuid)
    {
        auto it = m_EntityMap.find(uuid);
        return it == m_EntityMap.end() ? Entity() : Entity(it->second, this);
    }

    Entity World::GetEntityByHandle(entt::entity handle)
    {
        return m_Registry.valid(handle) ? Entity(handle, this) : Entity();
    }

    bool World::IsValid(Entity entity) const
    {
        return entity.GetWorld() == this && entity.GetHandle() != entt::null && m_Registry.valid(entity.GetHandle());
    }

    size_t World::GetEntityCount() const
    {
        return m_EntityOrder.size();
    }

    std::vector<Entity> World::GetRootEntities()
    {
        std::vector<Entity> roots;
        for (entt::entity handle : m_EntityOrder)
        {
            Entity entity(handle, this);
            if (!GetParent(entity))
                roots.push_back(entity);
        }
        return roots;
    }

    std::vector<Entity> World::GetAllEntities()
    {
        std::vector<Entity> entities;
        entities.reserve(m_EntityOrder.size());
        for (entt::entity handle : m_EntityOrder)
            entities.emplace_back(handle, this);
        return entities;
    }

    // --- Hierarchy ---------------------------------------------------------------------------

    Entity World::GetParent(Entity entity) const
    {
        if (!IsValid(entity))
            return Entity();
        const auto* relationship = m_Registry.try_get<RelationshipComponent>(entity.GetHandle());
        if (!relationship || !relationship->Parent.IsValid())
            return Entity();
        auto it = m_EntityMap.find(relationship->Parent);
        return it == m_EntityMap.end() ? Entity() : Entity(it->second, const_cast<World*>(this));
    }

    std::vector<Entity> World::GetChildren(Entity entity) const
    {
        std::vector<Entity> children;
        if (!IsValid(entity))
            return children;
        const auto* relationship = m_Registry.try_get<RelationshipComponent>(entity.GetHandle());
        if (!relationship)
            return children;
        for (core::UUID childId : relationship->Children)
        {
            auto it = m_EntityMap.find(childId);
            if (it != m_EntityMap.end())
                children.emplace_back(it->second, const_cast<World*>(this));
        }
        return children;
    }

    bool World::IsDescendantOf(Entity entity, Entity ancestor) const
    {
        Entity current = GetParent(entity);
        for (int depth = 0; current && depth < 256; ++depth)
        {
            if (current == ancestor)
                return true;
            current = GetParent(current);
        }
        return false;
    }

    void World::RemoveParent(Entity child, bool keepWorldTransform)
    {
        if (!IsValid(child))
            return;
        auto* relationship = m_Registry.try_get<RelationshipComponent>(child.GetHandle());
        if (!relationship || !relationship->Parent.IsValid())
            return;

        glm::mat4 world = GetWorldTransform(child);
        Entity parent = GetParent(child);
        if (parent)
        {
            auto& parentRel = parent.GetComponent<RelationshipComponent>();
            auto& siblings = parentRel.Children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), child.GetUUID()), siblings.end());
        }
        relationship->Parent = core::UUID::Invalid();
        if (keepWorldTransform)
            SetWorldTransform(child, world);
    }

    void World::SetParent(Entity child, Entity parent, bool keepWorldTransform)
    {
        if (!IsValid(child))
            return;
        if (!parent || !IsValid(parent))
        {
            RemoveParent(child, keepWorldTransform);
            return;
        }
        if (child == parent || IsDescendantOf(parent, child))
        {
            MX_CORE_WARN("Cannot parent '{}' under '{}' (would create a cycle)", child.GetName(), parent.GetName());
            return;
        }
        if (GetParent(child) == parent)
            return;

        glm::mat4 world = GetWorldTransform(child);
        RemoveParent(child, false);

        auto& childRel = child.HasComponent<RelationshipComponent>() ? child.GetComponent<RelationshipComponent>() : child.AddComponent<RelationshipComponent>();
        auto& parentRel = parent.HasComponent<RelationshipComponent>() ? parent.GetComponent<RelationshipComponent>() : parent.AddComponent<RelationshipComponent>();
        childRel.Parent = parent.GetUUID();
        parentRel.Children.push_back(child.GetUUID());

        if (keepWorldTransform)
            SetWorldTransform(child, world);
    }

    glm::mat4 World::GetWorldTransform(Entity entity) const
    {
        if (!IsValid(entity))
            return glm::mat4(1.0f);
        glm::mat4 local = m_Registry.get<TransformComponent>(entity.GetHandle()).GetTransform();
        Entity parent = GetParent(entity);
        if (!parent)
            return local;

        // Walk up iteratively with a depth guard against malformed data.
        glm::mat4 result = local;
        for (int depth = 0; parent && depth < 256; ++depth)
        {
            result = m_Registry.get<TransformComponent>(parent.GetHandle()).GetTransform() * result;
            parent = GetParent(parent);
        }
        return result;
    }

    void World::SetWorldTransform(Entity entity, const glm::mat4& worldTransform)
    {
        if (!IsValid(entity))
            return;
        glm::mat4 local = worldTransform;
        Entity parent = GetParent(entity);
        if (parent)
            local = glm::inverse(GetWorldTransform(parent)) * worldTransform;
        DecomposeLocal(local, entity.GetComponent<TransformComponent>());
    }

    // --- Runtime -----------------------------------------------------------------------------

    void World::InstantiateScripts()
    {
        for (entt::entity handle : m_EntityOrder)
        {
            auto* script = m_Registry.try_get<NativeScriptComponent>(handle);
            if (!script || script->Instance || script->ScriptName.empty())
                continue;
            script->Instance = ScriptRegistry::Instantiate(script->ScriptName);
            if (!script->Instance)
                continue;
            script->Instance->m_Entity = Entity(handle, this);
            script->Instance->OnCreate();
        }
    }

    void World::DestroyScripts()
    {
        auto view = m_Registry.view<NativeScriptComponent>();
        for (auto handle : view)
        {
            auto& script = view.get<NativeScriptComponent>(handle);
            if (script.Instance)
            {
                script.Instance->OnDestroy();
                delete script.Instance;
                script.Instance = nullptr;
            }
        }
    }

    void World::StartAudio()
    {
        auto view = m_Registry.view<AudioSourceComponent>();
        for (auto handle : view)
        {
            auto& source = view.get<AudioSourceComponent>(handle);
            if (source.PlayOnStart && !source.ClipPath.empty())
                source.RuntimeHandle = audio::AudioEngine::Play(source.ClipPath, source.Volume, source.Pitch, source.Loop);
        }
    }

    void World::StopAudio()
    {
        auto view = m_Registry.view<AudioSourceComponent>();
        for (auto handle : view)
        {
            auto& source = view.get<AudioSourceComponent>(handle);
            if (source.RuntimeHandle)
            {
                audio::AudioEngine::Stop(source.RuntimeHandle);
                source.RuntimeHandle = 0;
            }
        }
    }

    void World::OnRuntimeStart()
    {
        if (m_IsRunning)
            return;
        m_IsRunning = true;
        m_IsPaused = false;
        m_StepFrames = 0;
        m_Physics->Reset();
        ResolveAssets();
        InstantiateScripts();
        StartAudio();
        MX_CORE_INFO("World '{}' started ({} entities)", m_Name, m_EntityOrder.size());
    }

    void World::OnRuntimeStop()
    {
        if (!m_IsRunning)
            return;
        DestroyScripts();
        StopAudio();
        m_IsRunning = false;
        m_IsPaused = false;
        MX_CORE_INFO("World '{}' stopped", m_Name);
    }

    void World::OnUpdateRuntime(float deltaTime)
    {
        if (!m_IsRunning)
            return;
        if (m_IsPaused)
        {
            if (m_StepFrames <= 0)
                return;
            --m_StepFrames;
        }

        // Scripts (copy handles: scripts may create/destroy entities).
        std::vector<entt::entity> order = m_EntityOrder;
        for (entt::entity handle : order)
        {
            if (!m_Registry.valid(handle))
                continue;
            auto* script = m_Registry.try_get<NativeScriptComponent>(handle);
            if (!script)
                continue;
            if (!script->Instance && !script->ScriptName.empty())
            {
                script->Instance = ScriptRegistry::Instantiate(script->ScriptName);
                if (script->Instance)
                {
                    script->Instance->m_Entity = Entity(handle, this);
                    script->Instance->OnCreate();
                }
            }
            if (script->Instance)
                script->Instance->OnUpdate(deltaTime);
        }

        m_Physics->Step(deltaTime);
    }

    void World::OnUpdateEditor(float deltaTime)
    {
        (void)deltaTime;
    }

    void World::OnCollision(Entity entity, const Collision2D& collision)
    {
        auto* script = entity.TryGetComponent<NativeScriptComponent>();
        if (script && script->Instance)
            script->Instance->OnCollision(collision);
    }

    // --- Rendering ---------------------------------------------------------------------------

    void World::ResolveAssets()
    {
        if (!Renderer2D::IsInitialized())
            return;
        auto view = m_Registry.view<SpriteComponent>();
        for (auto handle : view)
        {
            auto& sprite = view.get<SpriteComponent>(handle);
            if (sprite.TexturePath.empty())
                sprite.Texture = nullptr;
            else if (!sprite.Texture || sprite.Texture->GetPath() != sprite.TexturePath)
                sprite.Texture = TextureLibrary::Get().Load(sprite.TexturePath);
        }
    }

    void World::RenderScene(const glm::mat4& viewProjection)
    {
        if (!Renderer2D::IsInitialized())
            return;

        ResolveAssets();

        struct SpriteItem { entt::entity Handle; glm::mat4 Transform; float Depth; };
        std::vector<SpriteItem> sprites;
        for (auto handle : m_Registry.view<TransformComponent, SpriteComponent>())
        {
            glm::mat4 transform = GetWorldTransform(Entity(handle, this));
            sprites.push_back({ handle, transform, transform[3].z });
        }
        // Back-to-front so alpha blending behaves.
        std::stable_sort(sprites.begin(), sprites.end(), [](const SpriteItem& a, const SpriteItem& b) { return a.Depth < b.Depth; });

        Renderer2D::BeginScene(viewProjection);
        for (const auto& item : sprites)
        {
            const auto& sprite = m_Registry.get<SpriteComponent>(item.Handle);
            if (sprite.Texture)
                Renderer2D::DrawQuad(item.Transform, sprite.Texture, sprite.Tiling, sprite.Color, static_cast<int>(item.Handle));
            else
                Renderer2D::DrawQuad(item.Transform, sprite.Color, static_cast<int>(item.Handle));
        }
        for (auto handle : m_Registry.view<TransformComponent, CircleRendererComponent>())
        {
            const auto& circle = m_Registry.get<CircleRendererComponent>(handle);
            Renderer2D::DrawCircle(GetWorldTransform(Entity(handle, this)), circle.Color, circle.Thickness, circle.Fade, static_cast<int>(handle));
        }
        Renderer2D::EndScene();
    }

    Entity World::GetPrimaryCameraEntity()
    {
        for (entt::entity handle : m_EntityOrder)
        {
            auto* camera = m_Registry.try_get<CameraComponent>(handle);
            if (camera && camera->Primary)
                return Entity(handle, this);
        }
        return Entity();
    }

    bool World::OnRenderRuntime()
    {
        Entity cameraEntity = GetPrimaryCameraEntity();
        if (!cameraEntity)
            return false;
        const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
        glm::mat4 view = glm::inverse(GetWorldTransform(cameraEntity));
        RenderScene(camera.GetProjection() * view);
        return true;
    }

    void World::OnRenderEditor(const glm::mat4& viewProjection)
    {
        RenderScene(viewProjection);
    }

    void World::UpdateCameraAspectRatios()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto handle : view)
        {
            auto& camera = view.get<CameraComponent>(handle);
            if (!camera.FixedAspectRatio)
                camera.Camera.SetViewportSize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
        }
    }

    void World::OnViewportResize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;
        m_ViewportSize = { static_cast<float>(width), static_cast<float>(height) };
        UpdateCameraAspectRatios();
    }

}
