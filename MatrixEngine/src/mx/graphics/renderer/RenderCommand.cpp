#include "pch.h"
#include "RenderCommand.h"

#include "GL/glew.h"

namespace Matrix::graphics
{
    void RenderCommand::Init()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_LINE_SMOOTH);
    }

    void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void RenderCommand::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderCommand::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void RenderCommand::SetDepthTest(bool enabled)
    {
        if (enabled)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }

    void RenderCommand::SetLineWidth(float width)
    {
        glLineWidth(width);
    }

    void RenderCommand::DrawIndexed(const VertexArray& vertexArray, uint32_t indexCount)
    {
        vertexArray.Bind();
        uint32_t count = indexCount ? indexCount : vertexArray.GetElementBuffer()->GetCount();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, nullptr);
    }

    void RenderCommand::DrawLines(const VertexArray& vertexArray, uint32_t vertexCount)
    {
        vertexArray.Bind();
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertexCount));
    }
}
