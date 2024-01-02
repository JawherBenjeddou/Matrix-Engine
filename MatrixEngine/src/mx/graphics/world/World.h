#pragma once
#include "Common.h"
#include "graphics/world/Entity.h"
#include "graphics/renderer/SpriteRenderer.h"
#include "entt.hpp"


namespace Matrix
{
	namespace core { class EngineCore; }
	namespace graphics
	{						
		//base class for all objects that can be placed in a level.
		class MATRIX_DLL World
		{
		public:
			World();

			~World();

			SharedObj<Entity> SpawnEntity(std::string_view name);
			
			SharedObj<Entity> FindEntityByName(std::string_view name);


		private:
			void DrawScreenElements();

			void OnUpdate();

			void InitWorld();

		private:
			friend class Matrix::core::EngineCore;
			//holds data for entites
			entt::registry m_Registry;
			std::vector<SharedObj<Entity>> m_Entities;
			SharedObj<SpriteRenderer> m_SpriteRenderer;
		};
	}
}

