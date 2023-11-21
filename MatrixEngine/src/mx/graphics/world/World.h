#pragma once
#include "Common.h"



#include "graphics/cameras/OrthoCamera.h"
#include "graphics/world/Entity.h"

#include "entt.hpp"


namespace Matrix
{
	namespace core
	{
		class EngineCore;
	}
}

namespace Matrix
{
	namespace graphics
	{						
		class MATRIX_DLL World
		{
		public:

			World(const OrthoCamera& camera);


			Entity SpawnEntity(std::string name);
			

			~World();

			World* GetWorld();

		private:
			void OnUpdate();

			void InitWorld();

		private:
			friend class Matrix::core::EngineCore;
			//holds data for entites
			entt::registry m_Registry;
			//world camera
			OrthoCamera m_Camera;

			std::vector<Entity> m_Entities;
		};
	}
}

