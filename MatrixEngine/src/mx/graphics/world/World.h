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
		//base class for all objects that can be placed in a level.
		class MATRIX_DLL World
		{
		public:

			World();


			Entity SpawnEntity(std::string name);
			

			~World();


		private:
			void DrawScreenElements();

			void OnUpdate();

			void InitWorld();

		private:
			friend class Matrix::core::EngineCore;


			//very temporary
			float move = 0.0f;
			//holds data for entites
			entt::registry m_Registry;

			std::vector<Entity> m_Entities;

			SharedObj<SpriteRenderer> m_SpriteRenderer;
		};
	}
}

