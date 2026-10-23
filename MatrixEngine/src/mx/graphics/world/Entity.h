#pragma once
#include "Common.h"
///@file Entity.h  Lightweight handle to an entity living inside a World.

#include "core/UUID.h"
#include "entt.hpp"

namespace Matrix::graphics
{
    class World;

    /// Copyable value type: an entt handle plus the owning World. Template helpers are defined in World.h.
    class MATRIX_DLL Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, World* world) : m_Handle(handle), m_World(world) {}
        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;

        template <typename T, typename... Args>
        T& AddComponent(Args&&... args);

        template <typename T, typename... Args>
        T& AddOrReplaceComponent(Args&&... args);

        template <typename T>
        T& GetComponent() const;

        template <typename T>
        T* TryGetComponent() const;

        template <typename T>
        bool HasComponent() const;

        template <typename... T>
        bool HasAnyComponent() const;

        template <typename T>
        void RemoveComponent();

        /// True if the handle refers to a live entity.
        bool IsValid() const;
        operator bool() const { return IsValid(); }
        operator entt::entity() const { return m_Handle; }
        operator uint32_t() const { return static_cast<uint32_t>(m_Handle); }

        core::UUID GetUUID() const;
        const std::string& GetName() const;
        void SetName(const std::string& name);

        Entity GetParent() const;
        std::vector<Entity> GetChildren() const;
        bool HasParent() const;

        /// World-space transform (parents applied).
        glm::mat4 GetWorldTransform() const;
        glm::vec2 GetWorldPosition() const;

        entt::entity GetHandle() const { return m_Handle; }
        World* GetWorld() const { return m_World; }

        bool operator==(const Entity& other) const { return m_Handle == other.m_Handle && m_World == other.m_World; }
        bool operator!=(const Entity& other) const { return !(*this == other); }

    private:
        entt::entity m_Handle{ entt::null };
        World* m_World = nullptr;
    };
}
