#include "pch.h"
#include "VertexArray.h"

#include "GL/glew.h"

namespace Matrix
{
	namespace graphics
	{
		VertexArray::VertexArray()
			: m_ID(0)
		{
		
		}

		void VertexArray::Init()
		{

			glGenVertexArrays(1, &m_ID);
			glBindVertexArray(m_ID);
		}

		VertexArray::~VertexArray()
		{
			glDeleteVertexArrays(1, &m_ID);
		}

		void VertexArray::Bind() const
		{
			glBindVertexArray(m_ID);
		}

		void VertexArray::UnBind() const
		{
			glBindVertexArray(0);
		}

		uint32_t VertexArray::GetId()
		{
			return m_ID;
		}
	}
}
