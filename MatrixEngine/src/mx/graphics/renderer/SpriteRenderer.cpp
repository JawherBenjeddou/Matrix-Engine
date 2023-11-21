#include "pch.h"
#include "SpriteRenderer.h"


namespace Matrix
{
	namespace graphics
	{
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
			using namespace Matrix::graphics;
			m_QuadVao.Init();
			m_QuadVao.Bind();
			m_QuadVbo.Init();
			m_QuadVbo.Bind();
			m_QuadVbo.BufferData(sizeof(vertices), vertices);
			m_QuadEbo.Init();
			m_QuadEbo.Bind();
			m_QuadEbo.BufferData(sizeof(indices), indices);
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// texture coord attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			//float vertices[] = {
			//  -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
			//   0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
			//   0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
			//  -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // top left
			//};

			//unsigned int indices[] = {
			//	0, 1, 2, // first triangle
			//	2, 3, 0  // second triangle
			//};

			//uint32_t VBO;
			//glGenBuffers(1, &VBO);
			//glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			//unsigned int EBO;
			//glGenBuffers(1, &EBO);
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			//// position attribute
			//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			//glEnableVertexAttribArray(0);

			//// texture coord attribute
			//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			//glEnableVertexAttribArray(1);

		}
		
	}
}
