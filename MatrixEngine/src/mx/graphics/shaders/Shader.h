#pragma once
#include "Common.h"
///@file Shader.h  OpenGL shader program wrapper with uniform caching.

namespace Matrix::graphics
{
    class MATRIX_DLL Shader
    {
    public:
        Shader() = default;
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        /// Compiles and links from vertex/fragment source files. Returns false (and logs) on failure.
        bool CompileFromFiles(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

        /// Compiles and links from in-memory source strings. Returns false (and logs) on failure.
        bool CompileFromSource(const std::string& vertexSource, const std::string& fragmentSource, const std::string& debugName = "shader");

        void EnableShaderProgram() const;
        void DisableShaderProgram() const;
        void Bind() const { EnableShaderProgram(); }
        void Unbind() const { DisableShaderProgram(); }

        bool IsValid() const { return m_ID != 0; }
        uint32_t GetId() const { return m_ID; }
        const std::string& GetName() const { return m_Name; }

        void SetInt(std::string_view name, int value) const;
        void SetIntArray(std::string_view name, const int* values, uint32_t count) const;
        void SetFloat(std::string_view name, float value) const;
        void SetFloat2(std::string_view name, const glm::vec2& value) const;
        void SetFloat3(std::string_view name, const glm::vec3& value) const;
        void SetFloat4(std::string_view name, const glm::vec4& value) const;
        void SetMat4(std::string_view name, const glm::mat4& value) const;

        /// Backwards compatible typed setter.
        template <typename T>
        void SetUniformValue(std::string_view name, const T& value) const
        {
            if constexpr (std::is_same_v<T, int>)            SetInt(name, value);
            else if constexpr (std::is_same_v<T, float>)     SetFloat(name, value);
            else if constexpr (std::is_same_v<T, glm::vec2>) SetFloat2(name, value);
            else if constexpr (std::is_same_v<T, glm::vec3>) SetFloat3(name, value);
            else if constexpr (std::is_same_v<T, glm::vec4>) SetFloat4(name, value);
            else if constexpr (std::is_same_v<T, glm::mat4>) SetMat4(name, value);
            else static_assert(sizeof(T) == 0, "Unsupported uniform type");
        }

    private:
        int32_t GetUniformLocation(std::string_view name) const;
        static uint32_t CompileStage(uint32_t type, const std::string& source, const std::string& debugName);

        uint32_t m_ID = 0;
        std::string m_Name;
        mutable std::unordered_map<std::string, int32_t> m_UniformCache;
    };
}
