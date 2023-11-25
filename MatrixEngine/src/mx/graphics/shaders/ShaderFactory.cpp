#include "pch.h"
#include "ShaderFactory.h"
#include "core/Logging.h"
namespace Matrix
{
	namespace graphics
	{
		void ShaderFactory::CreateShader(const std::string& shadername,std::string_view fragpath, std::string_view vertpath)
		{
			SharedObj<Shader> shader = std::make_shared<Shader>();
			shader->CompileShaderProgram(fragpath, vertpath);
			MX_CORE_WARN("{} compiled successfully", shadername);
			m_Shaders.insert({ shadername, shader});

		}
		SharedObj<Shader> ShaderFactory::GetShader(std::string_view shadername)
		{
			auto it = m_Shaders.find(shadername.data());

			if (it != m_Shaders.end()) {
				// Shader found, return it
				return it->second;
			}
			else
			{
				return nullptr;
			}
		}

		void ShaderFactory::Cleanup()
		{
			for (auto& [shaderName, shaderPtr] : m_Shaders)
			{
				shaderPtr.reset();
			}
			m_Shaders.clear();
			MX_CORE_WARN("ALL SHADERS ARE CLEARED");
		}
	}
}
