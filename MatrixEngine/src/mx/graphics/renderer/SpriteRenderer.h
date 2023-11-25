#pragma once
#include "Common.h"

#include "graphics/world/Entity.h"

namespace Matrix
{
	namespace graphics
	{
		class MATRIX_DLL SpriteRenderer
		{

		public:
			SpriteRenderer(std::vector<SharedObj<Entity>>& entities) : m_Entities(entities) {}

			void Init();

			void DrawSprites();

			void UpdateEntities(std::vector<SharedObj<Entity>>& entities);
		private:
			std::vector<SharedObj<Entity>>& m_Entities;
		};

	}
}
