#pragma once
#include "Common.h"
///@file World.h  Scene container: owns the entity registry, hierarchy, physics and runtime state.

#include "core/UUID.h"
#include "Entity.h"
#include "Components.h"
#include "entt.hpp"

namespace Matrix::physics { class Physics2D; }

namespace Matrix::graphics
{
    /// Describes a contact between two entities reported by the physics system.
    struct MATRIX_DLL Collision2D
    {
        Entity Other;
        glm::vec2 Normal = glm::vec2(0.0f); ///< Points away from Other towards the receiving entity.
        bool IsTrigger = false;
    };

    class MATRIX_DLL World
    {
    public:
        explicit World(const std::string& name = "Untitled");
        ~World();

        World(const World&) = delete;
        World& operator=(const World&) = delete;

        /// Deep copies a world (used to snapshot the editor scene before entering play mode).
        static SharedObj<World> Copy(const SharedObj<World>& other);

        // --- Entities --------------------------------------------------------------------
        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(core::UUID uuid, const std::string& name = std::string());
        /// Legacy name for CreateEntity.
        Entity SpawnEntity(std::string_view name) { return CreateEntity(std::string(name)); }

        /// Destroys an entity and all of its descendants.
        void DestroyEntity(Entity entity);

        /// Duplicates an entity (and its descendants). The copy is placed next to the original.
        Entity DuplicateEntity(Entity entity);

        Entity FindEntityByName(std::string_view name);
        Entity GetEntityByUUID(core::UUID uuid);
        Entity GetEntityByHandle(entt::entity handle);
        bool IsValid(Entity entity) const;
        size_t GetEntityCount() const;

        /// Root entities (no parent) in creation order.
        std::vector<Entity> GetRootEntities();
        /// Every entity in creation order.
        std::vector<Entity> GetAllEntities();

        // --- Hierarchy -------------------------------------------------------------------
        /// Re-parents child under parent (keeps the child's world transform). Pass an invalid parent to unparent.
        void SetParent(Entity child, Entity parent, bool keepWorldTransform = true);
        void RemoveParent(Entity child, bool keepWorldTransform = true);
        Entity GetParent(Entity entity) const;
        std::vector<Entity> GetChildren(Entity entity) const;
        bool IsDescendantOf(Entity entity, Entity ancestor) const;

        glm::mat4 GetWorldTransform(Entity entity) const;
        /// Converts a world-space matrix into the entity's local TransformComponent values.
        void SetWorldTransform(Entity entity, const glm::mat4& worldTransform);

        // --- Runtime -------------------------------------------------------------------
        void OnRuntimeStart();
        void OnRuntimeStop();
        bool IsRunning() const { return m_IsRunning; }
        bool IsPaused() const { return m_IsPaused; }
        void SetPaused(bool paused) { m_IsPaused = paused; }
        /// Advances exactly one simulation step on the next update while paused.
        void Step(int frames = 1) { m_StepFrames = frames; }

        /// Updates scripts, physics and audio. Does nothing while paused (unless stepping).
        void OnUpdateRuntime(float deltaTime);
        /// Editor-mode update (no simulation). Keeps runtime-independent state fresh.
        void OnUpdateEditor(float deltaTime);

        /// Renders using the primary camera. Returns false if the scene has no camera.
        bool OnRenderRuntime();
        /// Renders using an external view-projection (editor camera).
        void OnRenderEditor(const glm::mat4& viewProjection);
        /// Submits every renderable entity to Renderer2D. BeginScene/EndScene are called by this function.
        void RenderScene(const glm::mat4& viewProjection);

        void OnViewportResize(uint32_t width, uint32_t height);
        glm::vec2 GetViewportSize() const { return m_ViewportSize; }

        Entity GetPrimaryCameraEntity();

        /// Called by the physics system; forwards to the entity script if any.
        void OnCollision(Entity entity, const Collision2D& collision);

        // --- Settings --------------------------------------------------------------------
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetFilePath() const { return m_FilePath; }
        void SetFilePath(const std::string& path) { m_FilePath = path; }

        glm::vec2 Gravity = glm::vec2(0.0f, -9.81f);
        glm::vec4 BackgroundColor = glm::vec4(0.11f, 0.11f, 0.13f, 1.0f);

        physics::Physics2D& GetPhysics() { return *m_Physics; }

        entt::registry& GetRegistry() { return m_Registry; }
        const entt::registry& GetRegistry() const { return m_Registry; }

        template <typename... Components>
        auto View() { return m_Registry.view<Components...>(); }

        /// Resolves SpriteComponent::Texture from TexturePath for every sprite (loads on demand).
        void ResolveAssets();

    private:
        Entity CreateEntityInternal(core::UUID uuid, const std::string& name);
        Entity DuplicateRecursive(Entity source, Entity newParent);
        void DestroyRecursive(Entity entity);
        void InstantiateScripts();
        void DestroyScripts();
        void StartAudio();
        void StopAudio();
        void UpdateCameraAspectRatios();

        template <typename T>
        void OnComponentAdded(Entity entity, T& component);

        friend class Entity;
        friend class WorldSerializer;

        std::string m_Name;
        std::string m_FilePath;
        entt::registry m_Registry;
        std::unordered_map<core::UUID, entt::entity> m_EntityMap;
        std::vector<entt::entity> m_EntityOrder; ///< creation order, for stable editor listings
        glm::vec2 m_ViewportSize{ 1280.0f, 720.0f };
        bool m_IsRunning = false;
        bool m_IsPaused = false;
        int m_StepFrames = 0;
        ExclObj<physics::Physics2D> m_Physics;
    };

    // --- Entity template implementations -----------------------------------------------------

    template <typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args)
    {
        MX_ASSERT(m_World && !HasComponent<T>(), "Entity already has component");
        T& component = m_World->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
        m_World->OnComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T, typename... Args>
    T& Entity::AddOrReplaceComponent(Args&&... args)
    {
        T& component = m_World->m_Registry.emplace_or_replace<T>(m_Handle, std::forward<Args>(args)...);
        m_World->OnComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T>
    T& Entity::GetComponent() const
    {
        MX_ASSERT(HasComponent<T>(), "Entity does not have component");
        return m_World->m_Registry.get<T>(m_Handle);
    }

    template <typename T>
    T* Entity::TryGetComponent() const
    {
        if (!m_World || !m_World->m_Registry.valid(m_Handle))
            return nullptr;
        return m_World->m_Registry.try_get<T>(m_Handle);
    }

    template <typename T>
    bool Entity::HasComponent() const
    {
        return m_World && m_World->m_Registry.valid(m_Handle) && m_World->m_Registry.all_of<T>(m_Handle);
    }

    template <typename... T>
    bool Entity::HasAnyComponent() const
    {
        return m_World && m_World->m_Registry.valid(m_Handle) && m_World->m_Registry.any_of<T...>(m_Handle);
    }

    template <typename T>
    void Entity::RemoveComponent()
    {
        if (HasComponent<T>())
            m_World->m_Registry.remove<T>(m_Handle);
    }

    inline bool Entity::IsValid() const
    {
        return m_World != nullptr && m_Handle != entt::null && m_World->m_Registry.valid(m_Handle);
    }

    template <typename T>
    void World::OnComponentAdded(Entity entity, T& component)
    {
        (void)entity;
        if constexpr (std::is_same_v<T, CameraComponent>)
        {
            if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
                component.Camera.SetViewportSize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
        }
        else
        {
            (void)component;
        }
    }
}
