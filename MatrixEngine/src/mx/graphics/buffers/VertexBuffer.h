#pragma once
#include "Common.h"
///@file VertexBuffer.h  OpenGL array buffer wrapper.

#include "BufferLayout.h"

namespace Matrix::graphics
{
    class MATRIX_DLL VertexBuffer
    {
    public:
        VertexBuffer();
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        /// Creates the GPU buffer. Must be called with a valid OpenGL context.
        void Init();

        /// Creates a dynamic buffer of the given byte size (contents undefined until BufferSubData).
        void InitDynamic(uint32_t size);

        void Bind() const;
        void UnBind() const;

        /// Uploads static data.
        void BufferData(uint32_t size, const void* data);

        /// Updates a region of a dynamic buffer.
        void BufferSubData(uint32_t offset, uint32_t size, const void* data);

        const BufferLayout& GetLayout() const { return m_Layout; }
        void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

        uint32_t GetId() const { return m_ID; }

    private:
        uint32_t m_ID;
        BufferLayout m_Layout;
    };
}
