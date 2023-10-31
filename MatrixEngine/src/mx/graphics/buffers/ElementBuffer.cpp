#include "pch.h"
#include "ElementBuffer.h"

#include "GL/glew.h"

namespace Matrix
{
	namespace graphics
	{
		ElementBuffer::ElementBuffer() 
			: m_ID(0)
		{
			glGenBuffers(1, &m_ID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		}

		ElementBuffer::~ElementBuffer()
		{
			glDeleteBuffers(1, &m_ID);
		}

		void ElementBuffer::Bind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		}

		void ElementBuffer::UnBind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		void ElementBuffer::BufferData(uint32_t size, const void* data)
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		}

		uint32_t ElementBuffer::GetId()
		{
			return m_ID;
		}
	}
}
