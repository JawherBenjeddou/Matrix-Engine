#pragma once
#include "Common.h"
///@file ScriptableEntity.h  Base class for C++ behaviours attached through NativeScriptComponent.

#include "World.h"

namespace Matrix::graphics
{
    /// Derive from this class, override the lifecycle hooks and register the type with the ScriptRegistry.
    class MATRIX_DLL ScriptableEntity
    {
    public:
        virtual ~ScriptableEntity() = default;

        template <typename T>
        T& GetComponent() const { return m_Entity.GetComponent<T>(); }

        template <typename T>
        bool HasComponent() const { return m_Entity.HasComponent<T>(); }

        template <typename T>
        T* TryGetComponent() const { return m_Entity.TryGetComponent<T>(); }

        Entity GetEntity() const { return m_Entity; }
        World& GetWorld() const { return *m_Entity.GetWorld(); }

        /// Reads a float parameter from the NativeScriptComponent (or the default when missing).
        float GetParameter(const std::string& name, float defaultValue) const
        {
            if (auto* script = m_Entity.TryGetComponent<NativeScriptComponent>())
                return script->GetParameter(name, defaultValue);
            return defaultValue;
        }

        Entity FindEntity(std::string_view name) const { return GetWorld().FindEntityByName(name); }

    protected:
        /// Called once when the scene starts running (or when the script is added at runtime).
        virtual void OnCreate() {}
        /// Called when the scene stops or the entity is destroyed.
        virtual void OnDestroy() {}
        /// Called every frame while the scene runs.
        virtual void OnUpdate(float deltaTime) { (void)deltaTime; }
        /// Called when a collider on this entity touches another collider.
        virtual void OnCollision(const Collision2D& collision) { (void)collision; }

    private:
        friend class World;
        Entity m_Entity;
    };
}
