#pragma once
#include "Common.h"

#include "glm/glm.hpp"
#include "graphics/shaders/ShaderFactory.h"

#include "graphics/world/Entity.h"

namespace Matrix
{
	namespace graphics
	{
		class MATRIX_DLL SpriteRenderer
		{

		public:
			SpriteRenderer(std::vector<Entity>& entities) : m_Entities(entities) {}

			void Init();

			void DrawSprites();

			void UpdateEntities(std::vector<Entity>& entities);
		private:
			std::vector<Entity>& m_Entities;
		};

	}
}
