#include "pch.h"
#include "ShaderFactory.h"


namespace Matrix
{
	namespace graphics
	{
		void ShaderFactory::CreateShader(const std::string& shadername,std::string_view fragpath, std::string_view vertpath)
		{
			Shader* shader = new Shader();
			shader->CompileShaderProgram(fragpath, vertpath);
			m_Shaders.insert({ shadername, shader });

		}
		Shader* ShaderFactory::GetShader(std::string_view shadername)
		{
			auto it = m_Shaders.find(shadername.data());

			if (it != m_Shaders.end()) {
				// Shader found, return it
				return it->second;
			}
			else {
				MX_ASSERT_NULL(nullptr, "ERROR NULLPTR::SHADER DOES NOT EXIST !");
			}
		}

		void ShaderFactory::Cleanup()
		{
			for (auto it = m_Shaders.begin(); it != m_Shaders.end(); ++it)
			{
				delete it->second; // Delete the Shader object
			}
			m_Shaders.clear();
		}
	}
}
