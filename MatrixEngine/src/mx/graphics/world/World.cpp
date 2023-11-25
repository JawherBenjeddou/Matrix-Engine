#include "pch.h"
#include "World.h"

#include "graphics/shaders/ShaderFactory.h"
#include "GL/glew.h"
#include "Components.h"
#include "core/Logging.h"
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
			SpawnEntity("jawher");
			m_Entities[0]->GetComponent<PSRComponent>().Position.x += 3.0f;
			m_Entities[0]->GetComponent<PSRComponent>().updateTransform();
			SpawnEntity("test");
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

		/*SharedObj<Entity> World::FindEntityByName(std::string_view name)
		{
			return ;
		}*/

		void World::DrawScreenElements()
		{
			m_SpriteRenderer->DrawSprites();
		}

		World::~World()
		{

		}
	
	}
}