#include "pch.h"

#include "Texture2D.h"
#include "core/Logging.h"

#include "stb_image.h"


namespace Matrix
{
	namespace graphics
	{
		Texture2D::Texture2D()
			:m_TextureID(0),
			m_TexturePath(""),
			m_Width(0),
			m_Height(0),
			m_NrChannels(0),
			m_NrComponents(0)
		{

		}
		Texture2D::~Texture2D()
		{
			glDeleteTextures(1, &m_TextureID);
		}
		void Texture2D::LoadTexture2D(std::string_view filepath, TextureWrap wrap, TextureFilter filter)
		{
			m_TexturePath = filepath.data();

			glGenTextures(1, &m_TextureID);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			unsigned char* data = stbi_load(filepath.data(), &m_Width, &m_Height, &m_NrChannels, 0);

			if (data)
			{
				//TODO : figure out why i'm getting format and tex type uncompatible?
				int32_t format = 0;
				if (m_NrComponents == 1)
					format = GL_RED;
				else if (m_NrComponents == 3)
					format = GL_RGB;
				else if (m_NrComponents == 4)
					format = GL_RGBA;

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				MX_CORE_WARN("Texture successfully  Loaded");
				stbi_image_free(data);
			}
			else
			{
				MX_CORE_ERROR("Texture loading error for file: {}", filepath);
				MX_ASSERT(false, "ERROR LOADING THE TEXTURE");
				stbi_image_free(data);

			}
		}

		void Texture2D::Bind() const
		{
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
		}

		void Texture2D::Unbind() const
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		uint32_t Texture2D::GetID() const
		{
			return m_TextureID;
		}
		uint32_t Texture2D::GetWidht() const
		{
			return m_Width;
		}
		uint32_t Texture2D::GetHeight() const
		{
			return m_Height;
		}
	}
}