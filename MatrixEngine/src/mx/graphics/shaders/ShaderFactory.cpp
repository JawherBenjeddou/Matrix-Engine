#include "pch.h"
#include "ShaderFactory.h"

#include "core/Logging.h"
#include "util/FileSystem.h"

namespace Matrix::graphics
{
    ShaderFactory& ShaderFactory::GetInstance()
    {
        static ShaderFactory instance;
        return instance;
    }

    SharedObj<Shader> ShaderFactory::CreateShader(const std::string& shaderName, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
    {
        auto shader = CreateShared<Shader>();
        std::filesystem::path vertex = util::FileSystem::Resolve(vertexPath);
        std::filesystem::path fragment = util::FileSystem::Resolve(fragmentPath);
        if (!shader->CompileFromFiles(vertex, fragment))
        {
            MX_CORE_ERROR("Shader '{}' could not be created", shaderName);
            return nullptr;
        }
        m_Shaders[shaderName] = Entry{ shader, vertex, fragment };
        MX_CORE_INFO("Shader '{}' compiled", shaderName);
        return shader;
    }

    SharedObj<Shader> ShaderFactory::CreateShaderFromSource(const std::string& shaderName, const std::string& vertexSource, const std::string& fragmentSource)
    {
        auto shader = CreateShared<Shader>();
        if (!shader->CompileFromSource(vertexSource, fragmentSource, shaderName))
            return nullptr;
        m_Shaders[shaderName] = Entry{ shader, {}, {} };
        return shader;
    }

    SharedObj<Shader> ShaderFactory::GetShader(std::string_view shaderName) const
    {
        auto it = m_Shaders.find(std::string(shaderName));
        return it == m_Shaders.end() ? nullptr : it->second.Program;
    }

    bool ShaderFactory::Exists(std::string_view shaderName) const
    {
        return m_Shaders.find(std::string(shaderName)) != m_Shaders.end();
    }

    uint32_t ShaderFactory::ReloadAll()
    {
        uint32_t failures = 0;
        for (auto& [name, entry] : m_Shaders)
        {
            if (entry.VertexPath.empty())
                continue;
            if (!entry.Program->CompileFromFiles(entry.VertexPath, entry.FragmentPath))
                ++failures;
            else
                MX_CORE_INFO("Shader '{}' reloaded", name);
        }
        return failures;
    }

    void ShaderFactory::Cleanup()
    {
        m_Shaders.clear();
    }
}
