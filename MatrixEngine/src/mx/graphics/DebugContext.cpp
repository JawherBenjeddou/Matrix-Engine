#include "pch.h"
#include "DebugContext.h"
#include "core/Logging.h"

#include "GL/glew.h"

namespace Matrix::graphics
{
    namespace
    {
        void GLAPIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, const void*)
        {
            // Ignore redundant notifications (buffer usage hints etc.).
            if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
                return;
            (void)source;
            // Driver performance hints (e.g. shader recompiles on state change) are informational only.
            if (type == GL_DEBUG_TYPE_PERFORMANCE)
            {
                MX_CORE_TRACE("[OpenGL] (performance, id {}) {}", id, message);
                return;
            }
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH:   MX_CORE_ERROR("[OpenGL] (high, id {}) {}", id, message); break;
            case GL_DEBUG_SEVERITY_MEDIUM: MX_CORE_WARN("[OpenGL] (medium, id {}) {}", id, message); break;
            case GL_DEBUG_SEVERITY_LOW:    MX_CORE_WARN("[OpenGL] (low, id {}) {}", id, message); break;
            default: break;
            }
        }
    }

    void EnableDebugContext()
    {
#ifdef MX_DEBUG_MODE
        if (!GLEW_KHR_debug)
            return;
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLDebugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
    }

    void DisableDebugContext()
    {
        if (!GLEW_KHR_debug)
            return;
        glDebugMessageCallback(nullptr, nullptr);
        glDisable(GL_DEBUG_OUTPUT);
    }
}
