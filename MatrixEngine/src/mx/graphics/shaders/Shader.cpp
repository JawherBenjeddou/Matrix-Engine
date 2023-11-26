#include "pch.h"

#include "Shader.h"
#include "core/Logging.h"

namespace Matrix
{
	namespace graphics
	{
		
		void Shader::CompileShaderProgram(std::string_view fragpath, std::string_view vertpath)
		{
			//Handling Files
			std::string vShaderCode = ReadFile(vertpath);
			std::string fShaderCode = ReadFile(fragpath);
			const char* vCode = vShaderCode.c_str();
			const char* fCode = fShaderCode.c_str();

			uint32_t vertex, fragment;
			int IsSuccess;
			char VertexErrorInfo[512];

			//Create and Compile the vertex shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vCode, NULL);
			glCompileShader(vertex);

			// print compile errors if any
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &IsSuccess);
			if (!IsSuccess)
			{
				glGetShaderInfoLog(vertex, 512, NULL, VertexErrorInfo);
				MX_CORE_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED");
				MX_ASSERT(false, VertexErrorInfo);
			};

			char FragmentErrorInfo[512];

			//Create and Compile the fragment shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fCode, NULL);
			glCompileShader(fragment);

			//check for compile errors for fragment Shader
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &IsSuccess);
			if (!IsSuccess)
			{
				glGetShaderInfoLog(fragment, 512, NULL, FragmentErrorInfo);
				MX_CORE_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
				MX_ASSERT(false, FragmentErrorInfo);
			}


			//LINKING SHADERS TO THE PROGRAM
			char LinkingErrorInfo[512];
			m_ID = glCreateProgram();
			glAttachShader(m_ID, vertex);
			glAttachShader(m_ID, fragment);
			glLinkProgram(m_ID);

			// print linking errors if any
			glGetProgramiv(m_ID, GL_LINK_STATUS, &IsSuccess);
			if (!IsSuccess)
			{
				glGetProgramInfoLog(m_ID, 512, NULL, LinkingErrorInfo);
				MX_CORE_ERROR("ERROR::SHADER::LINKING::LINKING_FAILED");
				MX_ASSERT(false, LinkingErrorInfo);
			}

			//Delete shaders 
			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}

		void Shader::EnableShaderProgram()
		{
			glUseProgram(m_ID);
		}

		void Shader::DisableShaderProgram()
		{
			glUseProgram(0);
		}

		std::string Shader::ReadFile(std::string_view filepath)
		{
			std::string shader_code;
			std::ifstream shaderFile;

			shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
				shaderFile.open(filepath.data());
				std::stringstream shaderStream;
				shaderStream << shaderFile.rdbuf();
				shaderFile.close();
				shader_code = shaderStream.str();
			}
			catch (std::ifstream::failure e) {
				MX_ASSERT(false,"ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ");
			}

			return shader_code;
		}
	}
}
