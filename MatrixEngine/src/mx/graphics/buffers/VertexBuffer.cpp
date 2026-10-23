#include "pch.h"
#include "VertexBuffer.h"

#include "GL/glew.h"

namespace Matrix::graphics
{
    VertexBuffer::VertexBuffer()
        : m_ID(0)
    {
    }

    VertexBuffer::~VertexBuffer()
    {
        if (m_ID != 0)
            glDeleteBuffers(1, &m_ID);
    }

    void VertexBuffer::Init()
    {
        if (m_ID == 0)
            glGenBuffers(1, &m_ID);
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    }

    void VertexBuffer::InitDynamic(uint32_t size)
    {
        Init();
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
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
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void VertexBuffer::BufferSubData(uint32_t offset, uint32_t size, const void* data)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_ID);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }
}
