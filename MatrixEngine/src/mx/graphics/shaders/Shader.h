#pragma once
#include "GL/glew.h"
#include "Common.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Matrix
{
	namespace graphics
	{
		class MATRIX_DLL Shader
		{
		public:
			template <typename Type>
			void SetUniformValue(std::string_view name, Type value) const = delete;

			template <>
			void SetUniformValue<int>(std::string_view name, int value) const {
				glUniform1i(glGetUniformLocation(m_ID, name.data()), value);
			}

			template <>
			void SetUniformValue<float>(std::string_view name, float value) const {
				glUniform1f(glGetUniformLocation(m_ID, name.data()), value);
			}
			template <>
			void SetUniformValue<glm::vec3>(std::string_view name,glm::vec3 RGB) const {
				glUniform3f(glGetUniformLocation(m_ID, name.data()), RGB.x, RGB.y, RGB.z);
			}
			template<>
			void SetUniformValue<glm::mat4>(std::string_view name,glm::mat4 Matrix) const {
				uint32_t Location = glGetUniformLocation(m_ID, name.data());
				glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(Matrix));
			}

			uint32_t GetId() const {
				return m_ID;
			}

			void UseShaderProgram();
		
		private:
			void CompileShaderProgram(std::string_view fragpath, std::string_view vertpath);

			std::string ReadFile(std::string_view filepath);

		private:
			friend class ShaderFactory;
			//PROGRAM ID 
			uint32_t m_ID;
		};
	}
}
