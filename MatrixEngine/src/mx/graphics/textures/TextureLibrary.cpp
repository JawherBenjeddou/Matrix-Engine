#include "pch.h"
#include "TextureLibrary.h"

#include "core/Logging.h"
#include "util/FileSystem.h"

namespace Matrix::graphics
{
    TextureLibrary& TextureLibrary::Get()
    {
        static TextureLibrary instance;
        return instance;
    }

    std::string TextureLibrary::Key(const std::filesystem::path& path)
    {
        return util::FileSystem::MakeRelative(util::FileSystem::Resolve(path));
    }

    SharedObj<Texture2D> TextureLibrary::Load(const std::filesystem::path& path, const TextureSpec& spec)
    {
        if (path.empty())
            return GetWhiteTexture();

        std::string key = Key(path);
        auto it = m_Textures.find(key);
        if (it != m_Textures.end())
            return it->second;

        auto texture = CreateShared<Texture2D>();
        if (!texture->LoadTexture2D(path, spec))
            return GetMissingTexture();

        m_Textures[key] = texture;
        return texture;
    }

    SharedObj<Texture2D> TextureLibrary::Find(const std::filesystem::path& path) const
    {
        auto it = m_Textures.find(Key(path));
        return it == m_Textures.end() ? nullptr : it->second;
    }

    SharedObj<Texture2D> TextureLibrary::GetWhiteTexture()
    {
        if (!m_WhiteTexture)
        {
            m_WhiteTexture = CreateShared<Texture2D>();
            TextureSpec spec;
            spec.Filter = TextureFilter::NEAREST;
            spec.GenerateMipmaps = false;
            m_WhiteTexture->Create(1, 1, spec);
            uint32_t white = 0xFFFFFFFFu;
            m_WhiteTexture->SetData(&white, sizeof(white));
        }
        return m_WhiteTexture;
    }

    SharedObj<Texture2D> TextureLibrary::GetMissingTexture()
    {
        if (!m_MissingTexture)
        {
            constexpr uint32_t size = 8;
            std::array<uint32_t, size * size> pixels{};
            for (uint32_t y = 0; y < size; ++y)
                for (uint32_t x = 0; x < size; ++x)
                    pixels[y * size + x] = ((x / 4 + y / 4) % 2 == 0) ? 0xFFFF00FFu : 0xFF000000u; // ABGR: magenta / black

            m_MissingTexture = CreateShared<Texture2D>();
            TextureSpec spec;
            spec.Filter = TextureFilter::NEAREST;
            spec.GenerateMipmaps = false;
            m_MissingTexture->Create(size, size, spec);
            m_MissingTexture->SetData(pixels.data(), static_cast<uint32_t>(pixels.size() * sizeof(uint32_t)));
        }
        return m_MissingTexture;
    }

    void TextureLibrary::ReloadAll()
    {
        for (auto& [key, texture] : m_Textures)
        {
            if (!texture->LoadTexture2D(std::filesystem::path(key), TextureSpec()))
                MX_CORE_WARN("Could not reload texture '{}'", key);
        }
    }

    void TextureLibrary::Clear()
    {
        m_Textures.clear();
        m_WhiteTexture.reset();
        m_MissingTexture.reset();
    }
}
