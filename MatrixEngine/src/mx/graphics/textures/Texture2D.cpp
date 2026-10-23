#include "pch.h"
#include "Texture2D.h"

#include "core/Logging.h"
#include "util/FileSystem.h"

#include "GL/glew.h"
#include "stb_image.h"

namespace Matrix::graphics
{
    namespace
    {
        GLint ToGLWrap(TextureWrap wrap)
        {
            switch (wrap)
            {
            case TextureWrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
            case TextureWrap::CLAMP_TO_EDGE:   return GL_CLAMP_TO_EDGE;
            case TextureWrap::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
            default:                           return GL_REPEAT;
            }
        }
    }

    Texture2D::Texture2D()
        : m_TextureID(0), m_Width(0), m_Height(0), m_NrChannels(0)
    {
    }

    Texture2D::~Texture2D()
    {
        Release();
    }

    void Texture2D::Release()
    {
        if (m_TextureID != 0)
        {
            glDeleteTextures(1, &m_TextureID);
            m_TextureID = 0;
        }
    }

    void Texture2D::ApplyParameters(const TextureSpec& spec) const
    {
        GLint wrap = ToGLWrap(spec.Wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        if (spec.Filter == TextureFilter::NEAREST)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, spec.GenerateMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, spec.GenerateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }

    bool Texture2D::LoadTexture2D(std::string_view filepath, TextureWrap wrap, TextureFilter filter)
    {
        TextureSpec spec;
        spec.Wrap = wrap;
        spec.Filter = filter;
        return LoadTexture2D(std::filesystem::path(filepath), spec);
    }

    bool Texture2D::LoadTexture2D(const std::filesystem::path& filepath, const TextureSpec& spec)
    {
        std::filesystem::path resolved = util::FileSystem::Resolve(filepath);
        std::string pathString = resolved.string();

        stbi_set_flip_vertically_on_load(spec.FlipVertically ? 1 : 0);
        int width = 0, height = 0, channels = 0;
        // Always request RGBA so every texture shares the same upload path.
        unsigned char* data = stbi_load(pathString.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!data)
        {
            MX_CORE_ERROR("Failed to load texture '{}': {}", pathString, stbi_failure_reason() ? stbi_failure_reason() : "unknown error");
            return false;
        }

        Release();
        m_TexturePath = util::FileSystem::MakeRelative(resolved);
        m_Width = width;
        m_Height = height;
        m_NrChannels = channels;

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        ApplyParameters(spec);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        if (spec.GenerateMipmaps)
            glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
        MX_CORE_TRACE("Texture loaded '{}' ({}x{}, {} channels)", m_TexturePath, width, height, channels);
        return true;
    }

    bool Texture2D::Create(uint32_t width, uint32_t height, const TextureSpec& spec)
    {
        if (width == 0 || height == 0)
            return false;
        Release();
        m_TexturePath.clear();
        m_Width = static_cast<int32_t>(width);
        m_Height = static_cast<int32_t>(height);
        m_NrChannels = 4;

        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        TextureSpec local = spec;
        local.GenerateMipmaps = false;
        ApplyParameters(local);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }

    void Texture2D::SetData(const void* data, uint32_t size)
    {
        MX_ASSERT(size == static_cast<uint32_t>(m_Width * m_Height * 4), "Texture data must cover the entire texture (RGBA8)");
        (void)size;
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture2D::Bind(uint32_t slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }

    void Texture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
