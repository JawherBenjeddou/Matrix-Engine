#pragma once
#include "Common.h"
///@file ShaderFactory.h  Named shader registry.

#include "Shader.h"

namespace Matrix::graphics
{
    class MATRIX_DLL ShaderFactory
    {
    public:
        static ShaderFactory& GetInstance();

        /// Compiles a shader from root-relative (or absolute) file paths and registers it under a name.
        /// Returns nullptr if compilation failed.
        SharedObj<Shader> CreateShader(const std::string& shaderName, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

        /// Compiles a shader from source strings and registers it under a name.
        SharedObj<Shader> CreateShaderFromSource(const std::string& shaderName, const std::string& vertexSource, const std::string& fragmentSource);

        /// Returns the shader registered under the name, or nullptr.
        SharedObj<Shader> GetShader(std::string_view shaderName) const;

        bool Exists(std::string_view shaderName) const;

        /// Recompiles every file-based shader from disk. Returns the number of shaders that failed.
        uint32_t ReloadAll();

        /// Releases every shader. Must be called while the OpenGL context is still alive.
        void Cleanup();

    private:
        ShaderFactory() = default;
        ShaderFactory(const ShaderFactory&) = delete;
        ShaderFactory& operator=(const ShaderFactory&) = delete;

        struct Entry
        {
            SharedObj<Shader> Program;
            std::filesystem::path VertexPath;
            std::filesystem::path FragmentPath;
        };
        std::unordered_map<std::string, Entry> m_Shaders;
    };
}
