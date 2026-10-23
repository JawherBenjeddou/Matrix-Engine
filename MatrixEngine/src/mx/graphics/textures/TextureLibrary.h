#pragma once
#include "Common.h"
///@file TextureLibrary.h  Caches textures by root-relative path so assets are loaded once.

#include "Texture2D.h"

namespace Matrix::graphics
{
    class MATRIX_DLL TextureLibrary
    {
    public:
        static TextureLibrary& Get();

        /// Returns the cached texture for a path, loading it on first use.
        /// If loading fails a magenta "missing texture" placeholder is returned (never nullptr).
        SharedObj<Texture2D> Load(const std::filesystem::path& path, const TextureSpec& spec = TextureSpec());

        /// Returns the cached texture for a path or nullptr if it was never loaded.
        SharedObj<Texture2D> Find(const std::filesystem::path& path) const;

        /// Returns the 1x1 white texture used for untextured quads.
        SharedObj<Texture2D> GetWhiteTexture();

        /// Returns the magenta/black checker placeholder used when a texture cannot be loaded.
        SharedObj<Texture2D> GetMissingTexture();

        /// True if the given texture is the placeholder.
        bool IsMissingTexture(const SharedObj<Texture2D>& texture) const { return texture && texture == m_MissingTexture; }

        /// Reloads every cached texture from disk.
        void ReloadAll();

        /// Releases all cached textures. Must be called before the OpenGL context is destroyed.
        void Clear();

        size_t Count() const { return m_Textures.size(); }

    private:
        TextureLibrary() = default;
        static std::string Key(const std::filesystem::path& path);

        std::unordered_map<std::string, SharedObj<Texture2D>> m_Textures;
        SharedObj<Texture2D> m_WhiteTexture;
        SharedObj<Texture2D> m_MissingTexture;
    };
}
