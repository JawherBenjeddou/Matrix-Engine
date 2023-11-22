#include "pch.h"
#include "World.h"

#include "graphics/shaders/ShaderFactory.h"
#include "GL/glew.h"
#include "Components.h"

namespace Matrix
{
	namespace graphics
	{
		World::World(SharedObj<OrthoCamera> camera)
			:m_Camera(camera)
		{

		}

		void World::InitWorld()
		{

		}
		//TODO : Remove this shit
		void World::OnUpdate()
		{
			ShaderFactory::GetInstance().GetShader("defaultshader")->UseShaderProgram();
			ShaderFactory::GetInstance().GetShader("defaultshader")->SetUniformValue<glm::mat4>("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		Entity World::SpawnEntity(std::string name)
		{
			 Entity entity(name, m_Registry.create(), &m_Registry);
			 entity.AddComponent<PSRComponent>();
			 m_Entities.push_back(entity);
			 return entity;
		}

		World::~World()
		{

		}
	
	}
}