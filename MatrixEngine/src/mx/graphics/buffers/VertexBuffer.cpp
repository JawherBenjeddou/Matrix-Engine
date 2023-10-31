#include "pch.h"
#include "VertexBuffer.h"

#include "GL/glew.h"

namespace Matrix
{
	namespace graphics
	{
		VertexBuffer::VertexBuffer() 
			: m_ID(0)
		{
			glGenBuffers(1, &m_ID);
			glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		}

		VertexBuffer::~VertexBuffer()
		{
			glDeleteBuffers(1, &m_ID);
		}

		void VertexBuffer::Bind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		}

		void VertexBuffer::UnBind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void VertexBuffer::BufferData(uint32_t size, const void* data)
		{
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		}

		uint32_t VertexBuffer::GetId()
		{
			return m_ID;
		}

		void VertexBuffer::BufferSubData(uint32_t& offset, uint32_t size, const void* data)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		}
	}
}
