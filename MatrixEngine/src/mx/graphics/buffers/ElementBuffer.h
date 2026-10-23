#pragma once
#include "Common.h"
///@file ElementBuffer.h  OpenGL index buffer wrapper.

namespace Matrix::graphics
{
    class MATRIX_DLL ElementBuffer
    {
    public:
        ElementBuffer();
        ~ElementBuffer();

        ElementBuffer(const ElementBuffer&) = delete;
        ElementBuffer& operator=(const ElementBuffer&) = delete;

        /// Creates the GPU buffer. Must be called with a valid OpenGL context.
        void Init();

        void Bind() const;
        void UnBind() const;

        /// Uploads 32-bit indices. size is in bytes.
        void BufferData(uint32_t size, const void* data);

        uint32_t GetCount() const { return m_Count; }
        uint32_t GetId() const { return m_ID; }

    private:
        uint32_t m_ID;
        uint32_t m_Count = 0;
    };
}
