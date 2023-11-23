#include "pch.h"
#include "World.h"

#include "graphics/shaders/ShaderFactory.h"
#include "GL/glew.h"
#include "Components.h"

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
		}
	
		//TODO : Remove this shit
		void World::OnUpdate()
		{
			ShaderFactory::GetInstance().GetShader("defaultshader")->UseShaderProgram();

			DrawScreenElements();
		}

		Entity World::SpawnEntity(std::string name)
		{
			 Entity entity(name, m_Registry.create(), &m_Registry);
			 entity.AddComponent<PSRComponent>();
			 m_Entities.push_back(entity);
			 m_SpriteRenderer->UpdateEntities(m_Entities);
			 return entity;
		}

		void World::DrawScreenElements()
		{
			//TODO : why the changing the position changes the scale lmao?? (maybe because camera broke)
			move += 0.0001f;
			m_Entities[0].GetComponent<PSRComponent>().Position.x += move;
			m_Entities[0].GetComponent<PSRComponent>().updateTransform();

			m_SpriteRenderer->DrawSprites();
		}

		World::~World()
		{

		}
	
	}
}