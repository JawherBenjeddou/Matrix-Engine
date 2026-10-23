#pragma once
#include "Common.h"
///@file RenderCommand.h  Thin static wrapper over the OpenGL state used by the renderer.

#include "graphics/buffers/VertexArray.h"

namespace Matrix::graphics
{
    class MATRIX_DLL RenderCommand
    {
    public:
        static void Init();
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        static void SetClearColor(const glm::vec4& color);
        static void Clear();
        static void SetDepthTest(bool enabled);
        static void SetLineWidth(float width);

        /// Draws indexCount indices (or every index in the bound element buffer when 0).
        static void DrawIndexed(const VertexArray& vertexArray, uint32_t indexCount = 0);
        static void DrawLines(const VertexArray& vertexArray, uint32_t vertexCount);
    };
}
