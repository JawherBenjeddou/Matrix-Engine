#pragma once
#include "Common.h"
///@file Texture2D.h

#include "GL/glew.h"


namespace Matrix
{
	namespace graphics
	{
		enum class MATRIX_DLL TextureWrap
		{
			DEFAULT = GL_REPEAT,
			MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
			CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
		};

		enum class MATRIX_DLL TextureFilter
		{
			NEAREST = GL_NEAREST,
			LINEAR = GL_LINEAR
		};	

		class MATRIX_DLL Texture2D 
		{

		public:

			/// Default constructor.
			Texture2D();

			/// Copy constructor is deleted.
			Texture2D(const Texture2D& other) = delete; 

			/// Move constructor.
			Texture2D(Texture2D&& other) noexcept
				:m_TextureID(std::move(other.m_TextureID)),
				 m_Width(std::move(other.m_Width)),
				 m_Height(std::move(other.m_Height)),
				 m_NrChannels(std::move(other.m_NrChannels)),
				 m_TexType(std::move(other.m_TexType))
			{
				other.m_TextureID = 0;
				other.m_Width = 0;
				other.m_Height = 0;
				other.m_NrChannels = 0;
				other.m_TexType.clear();
			}

			~Texture2D();
		
		public:

			/// Load a 2D texture from a file.
			/// \param filepath :  The path to the image file.
			/// \param wrap  : The texture wrapping mode (default is TextureWrap::DEFAULT).
			/// \param filter : The texture filtering mode (default is TextureFilter::LINEAR).
			void LoadTexture2D(std::string_view filepath, TextureWrap wrap = TextureWrap::DEFAULT, TextureFilter filter = TextureFilter::LINEAR);
			
			/// Bind the texture to a specific texture unit.
			void Bind() const;

			/// Unbind the currently bound texture.
			void Unbind() const;

			/// Get the ID of the texture.
			uint32_t GetID() const;

			/// Get the width of the texture.
			uint32_t GetWidht() const;

			/// Get the height of the texture.
			uint32_t GetHeight() const;

		private:
			uint32_t m_TextureID;
			std::string m_TexType;
			std::string m_TexturePath;

			int32_t m_Width;
			int32_t m_Height;
			int32_t m_NrChannels;
			int32_t m_NrComponents;
		};

	}
}

