#pragma once
#include "Common.h"

#include "entt.hpp"

namespace Matrix
{
    namespace graphics
    {
        class MATRIX_DLL Entity
        {
        public:
            Entity(const std::string& entityName, entt::entity handle, entt::registry* registry)
                : m_Entity(handle), m_Registry(registry), m_Name(entityName) {}

        /* template<typename T>
            bool HasComponent() const
            {
                return m_Registry->has<T>(m_Entity);
            }*/

            template<typename T>
            T& AddComponent()
            {
                return m_Registry->emplace<T>(m_Entity);
            }

            template<typename T>
            void RemoveComponent()
            {
                m_Registry->remove<T>(m_Entity);
            }

            template<typename T>
            T& GetComponent()
            {
                return m_Registry->get<T>(m_Entity);
            }

            const std::string GetName() const { return m_Name; }

        private:
            std::string m_Name;
            entt::entity m_Entity;
            entt::registry* m_Registry;
        };
    }
}
