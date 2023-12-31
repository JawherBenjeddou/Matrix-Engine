#include "pch.h"
#include "World.h"

#include "graphics/shaders/ShaderFactory.h"
#include "GL/glew.h"
#include "Components.h"
#include "core/Logging.h"
#include "core/Event.h"
namespace Matrix
{
	namespace graphics
	{
		World::World()

		{
			m_SpriteRenderer = std::make_shared<SpriteRenderer>(m_Entities);
		}

		void World::InitWorld()
		{
			m_SpriteRenderer->Init();
			MX_CORE_WARN("Game World Initialized successfully");
		} 
	
		//TODO : Remove this shit
		void World::OnUpdate()
		{
			
			DrawScreenElements();
		}

		SharedObj<Entity> World::SpawnEntity(std::string_view name)
		{
			 SharedObj<Entity> entity = std::make_shared<Entity>(name.data(), m_Registry.create(), &m_Registry);
			 entity->AddComponent<PSRComponent>();
			 m_Entities.push_back(entity);
			 m_SpriteRenderer->UpdateEntities(m_Entities);
			 return entity;
		}

		SharedObj<Entity> World::FindEntityByName(std::string_view name)
		{
			for (auto entity : m_Entities)
			{
				if (entity->GetName() == name)
				{
					return entity;
				}
			}
		}

		void World::DrawScreenElements()
		{
			m_SpriteRenderer->DrawSprites();
		}

		World::~World()
		{

		}
	
	}
}