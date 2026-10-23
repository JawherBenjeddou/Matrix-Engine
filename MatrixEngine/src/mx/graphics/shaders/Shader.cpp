#include "pch.h"
#include "Shader.h"

#include "core/Logging.h"
#include "util/FileSystem.h"

#include "GL/glew.h"

namespace Matrix::graphics
{
    Shader::~Shader()
    {
        if (m_ID != 0)
            glDeleteProgram(m_ID);
    }

    bool Shader::CompileFromFiles(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
    {
        auto vertexSource = util::FileSystem::ReadText(vertexPath);
        if (!vertexSource)
        {
            MX_CORE_ERROR("Could not read vertex shader '{}'", vertexPath.string());
            return false;
        }
        auto fragmentSource = util::FileSystem::ReadText(fragmentPath);
        if (!fragmentSource)
        {
            MX_CORE_ERROR("Could not read fragment shader '{}'", fragmentPath.string());
            return false;
        }
        return CompileFromSource(*vertexSource, *fragmentSource, vertexPath.stem().string());
    }

    uint32_t Shader::CompileStage(uint32_t type, const std::string& source, const std::string& debugName)
    {
        uint32_t shader = glCreateShader(type);
        const char* code = source.c_str();
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(static_cast<size_t>(std::max(length, 1)), '\0');
            glGetShaderInfoLog(shader, length, nullptr, log.data());
            MX_CORE_ERROR("{} shader '{}' failed to compile:\n{}", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", debugName, log);
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    bool Shader::CompileFromSource(const std::string& vertexSource, const std::string& fragmentSource, const std::string& debugName)
    {
        m_Name = debugName;

        uint32_t vertex = CompileStage(GL_VERTEX_SHADER, vertexSource, debugName);
        if (vertex == 0)
            return false;
        uint32_t fragment = CompileStage(GL_FRAGMENT_SHADER, fragmentSource, debugName);
        if (fragment == 0)
        {
            glDeleteShader(vertex);
            return false;
        }

        uint32_t program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);

        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        glDetachShader(program, vertex);
        glDetachShader(program, fragment);
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        if (!success)
        {
            GLint length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::string log(static_cast<size_t>(std::max(length, 1)), '\0');
            glGetProgramInfoLog(program, length, nullptr, log.data());
            MX_CORE_ERROR("Shader program '{}' failed to link:\n{}", debugName, log);
            glDeleteProgram(program);
            return false;
        }

        if (m_ID != 0)
            glDeleteProgram(m_ID);
        m_ID = program;
        m_UniformCache.clear();
        return true;
    }

    void Shader::EnableShaderProgram() const
    {
        glUseProgram(m_ID);
    }

    void Shader::DisableShaderProgram() const
    {
        glUseProgram(0);
    }

    int32_t Shader::GetUniformLocation(std::string_view name) const
    {
        std::string key(name);
        auto it = m_UniformCache.find(key);
        if (it != m_UniformCache.end())
            return it->second;
        int32_t location = glGetUniformLocation(m_ID, key.c_str());
        if (location == -1)
            MX_CORE_WARN("Uniform '{}' not found in shader '{}'", key, m_Name);
        m_UniformCache[key] = location;
        return location;
    }

    void Shader::SetInt(std::string_view name, int value) const
    {
        glUniform1i(GetUniformLocation(name), value);
    }

    void Shader::SetIntArray(std::string_view name, const int* values, uint32_t count) const
    {
        glUniform1iv(GetUniformLocation(name), static_cast<GLsizei>(count), values);
    }

    void Shader::SetFloat(std::string_view name, float value) const
    {
        glUniform1f(GetUniformLocation(name), value);
    }

    void Shader::SetFloat2(std::string_view name, const glm::vec2& value) const
    {
        glUniform2f(GetUniformLocation(name), value.x, value.y);
    }

    void Shader::SetFloat3(std::string_view name, const glm::vec3& value) const
    {
        glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }

    void Shader::SetFloat4(std::string_view name, const glm::vec4& value) const
    {
        glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void Shader::SetMat4(std::string_view name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }
}
