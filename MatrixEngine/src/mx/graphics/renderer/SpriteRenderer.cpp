#include "pch.h"
#include "SpriteRenderer.h"

#include "graphics/buffers/VertexBuffer.h"
#include "graphics/buffers/VertexArray.h"
#include "graphics/buffers/ElementBuffer.h"

namespace Matrix
{
	namespace graphics
	{
		struct RendererStorage
		{
			ExclObj<VertexArray> QuadVao = std::make_unique<VertexArray>();
			ExclObj<VertexBuffer> QuadVbo = std::make_unique<VertexBuffer>();
			ExclObj<ElementBuffer > QuadEbo = std::make_unique<ElementBuffer>();
		};
		static RendererStorage s_storage;

		SpriteRenderer::SpriteRenderer()
		{
		}
		void SpriteRenderer::Init()
		{
			float vertices[] = {
			 -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
			  0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
			  0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
			 -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // top left
			};
			unsigned int indices[] = {
			 0, 1, 2, // first triangle
			 2, 3, 0  // second triangle
			};
			s_storage.QuadVbo->Init();
			s_storage.QuadVbo->Bind();
			s_storage.QuadVbo->BufferData(sizeof(vertices), vertices);
			s_storage.QuadEbo->Init();
			s_storage.QuadEbo->Bind();
			s_storage.QuadEbo->BufferData(sizeof(indices), indices);
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// texture coord attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

		}
		
	}
}
