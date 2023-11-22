#pragma once
#include "Common.h"



#include "graphics/cameras/OrthoCamera.h"
#include "graphics/world/Entity.h"
#include "graphics/renderer/SpriteRenderer.h"

#include "entt.hpp"


namespace Matrix
{
	namespace core { class EngineCore; }
	namespace graphics
	{						
		class MATRIX_DLL World
		{
		public:

			World(SharedObj<OrthoCamera> camera);


			Entity SpawnEntity(std::string name);
			

			~World();


		private:
			void OnUpdate();

			void InitWorld();

		private:
			friend class Matrix::core::EngineCore;

			//holds data for entites
			entt::registry m_Registry;

			//world camera
			SharedObj<OrthoCamera> m_Camera;

			std::vector<Entity> m_Entities;
		};
	}
}

