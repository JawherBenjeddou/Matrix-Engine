#include "pch.h"
#include "VertexArray.h"
#include "core/Logging.h"

#include "GL/glew.h"

namespace Matrix::graphics
{
    namespace
    {
        GLenum ToOpenGLBaseType(ShaderDataType type)
        {
            switch (type)
            {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
                return GL_FLOAT;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
                return GL_INT;
            case ShaderDataType::Bool:
                return GL_BOOL;
            default:
                return 0;
            }
        }
    }

    VertexArray::VertexArray()
        : m_ID(0)
    {
    }

    VertexArray::~VertexArray()
    {
        if (m_ID != 0)
            glDeleteVertexArrays(1, &m_ID);
    }

    void VertexArray::Init()
    {
        if (m_ID == 0)
            glGenVertexArrays(1, &m_ID);
        glBindVertexArray(m_ID);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_ID);
    }

    void VertexArray::UnBind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddVertexBuffer(const SharedObj<VertexBuffer>& vertexBuffer)
    {
        MX_ASSERT(!vertexBuffer->GetLayout().GetElements().empty(), "Vertex buffer has no layout");

        glBindVertexArray(m_ID);
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout)
        {
            switch (element.Type)
            {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
                glEnableVertexAttribArray(m_AttributeIndex);
                glVertexAttribPointer(m_AttributeIndex,
                    element.GetComponentCount(),
                    ToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    reinterpret_cast<const void*>(element.Offset));
                ++m_AttributeIndex;
                break;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
                glEnableVertexAttribArray(m_AttributeIndex);
                glVertexAttribIPointer(m_AttributeIndex,
                    element.GetComponentCount(),
                    ToOpenGLBaseType(element.Type),
                    layout.GetStride(),
                    reinterpret_cast<const void*>(element.Offset));
                ++m_AttributeIndex;
                break;
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            {
                uint32_t count = element.GetComponentCount();
                for (uint32_t i = 0; i < count; ++i)
                {
                    glEnableVertexAttribArray(m_AttributeIndex);
                    glVertexAttribPointer(m_AttributeIndex,
                        count,
                        ToOpenGLBaseType(element.Type),
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        reinterpret_cast<const void*>(element.Offset + sizeof(float) * count * i));
                    glVertexAttribDivisor(m_AttributeIndex, 1);
                    ++m_AttributeIndex;
                }
                break;
            }
            default:
                MX_ASSERT(false, "Unknown ShaderDataType");
                break;
            }
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::SetElementBuffer(const SharedObj<ElementBuffer>& elementBuffer)
    {
        glBindVertexArray(m_ID);
        elementBuffer->Bind();
        m_ElementBuffer = elementBuffer;
    }
}
