#pragma once

#include "Common.h"
#include "core/Logging.h"

#include "GL/glew.h"

namespace Matrix
{
    namespace graphics
    {
        void GLAPIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
            if (severity == GL_DEBUG_SEVERITY_HIGH) {
                MX_CORE_CRITICAL("OpenGL Error (High): {} ", message);
                MX_ASSERT(false, message);
            }
            else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
                MX_CORE_ERROR("OpenGL Error (Medium): {} ", message);
                MX_ASSERT(false, message);
             }
            else if (severity == GL_DEBUG_SEVERITY_LOW) {
                MX_CORE_ERROR("OpenGL Error (LOW): {} ", message);
                MX_ASSERT(false, message);
            }
        }

        MATRIX_DLL inline void EnableDebugContext()
        {
#ifdef MX_DEBUG_MODE
            // Enable the OpenGL debug output
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(OpenGLDebugCallback, nullptr);
#endif // MX_DEBUG_MODE
        }

        MATRIX_DLL inline void DisableDebugContext()
        {
            glDebugMessageCallback(nullptr, nullptr);
            glDisable(GL_DEBUG_OUTPUT);
        }
    }
}
