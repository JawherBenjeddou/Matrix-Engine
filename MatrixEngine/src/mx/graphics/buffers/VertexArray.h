#pragma once
#include "Common.h"
///@file VertexArray.h  OpenGL vertex array object wrapper.

#include "VertexBuffer.h"
#include "ElementBuffer.h"

namespace Matrix::graphics
{
    class MATRIX_DLL VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        /// Creates the VAO. Must be called with a valid OpenGL context.
        void Init();

        void Bind() const;
        void UnBind() const;

        /// Attaches a vertex buffer and configures its attributes from the buffer layout.
        void AddVertexBuffer(const SharedObj<VertexBuffer>& vertexBuffer);

        /// Attaches the index buffer.
        void SetElementBuffer(const SharedObj<ElementBuffer>& elementBuffer);

        const std::vector<SharedObj<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
        const SharedObj<ElementBuffer>& GetElementBuffer() const { return m_ElementBuffer; }

        uint32_t GetId() const { return m_ID; }

    private:
        uint32_t m_ID;
        uint32_t m_AttributeIndex = 0;
        std::vector<SharedObj<VertexBuffer>> m_VertexBuffers;
        SharedObj<ElementBuffer> m_ElementBuffer;
    };
}
