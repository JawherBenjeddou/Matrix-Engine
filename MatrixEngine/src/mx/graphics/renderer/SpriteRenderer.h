#pragma once
#include "Common.h"

#include "glm/glm.hpp"
#include "graphics/shaders/ShaderFactory.h"
#include "graphics/buffers/VertexBuffer.h"
#include "graphics/buffers/VertexArray.h"
#include "graphics/buffers/ElementBuffer.h"

namespace Matrix
{
	namespace graphics
	{
		class MATRIX_DLL SpriteRenderer
		{

		public:
			SpriteRenderer();

			void Init();

		private:
			VertexArray m_QuadVao;
			VertexBuffer m_QuadVbo;
			ElementBuffer m_QuadEbo;
			//std::vector<Entity> m_Entities;
		};

	}
}
