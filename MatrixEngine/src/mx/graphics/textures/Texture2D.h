#pragma once
#include "Common.h"
///@file Texture2D.h  OpenGL 2D texture.

namespace Matrix::graphics
{
    enum class TextureWrap : uint8_t
    {
        DEFAULT = 0,     ///< Repeat
        REPEAT = 0,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    enum class TextureFilter : uint8_t
    {
        LINEAR = 0,
        NEAREST
    };

    struct MATRIX_DLL TextureSpec
    {
        TextureWrap Wrap = TextureWrap::REPEAT;
        TextureFilter Filter = TextureFilter::LINEAR;
        bool GenerateMipmaps = true;
        bool FlipVertically = true;
    };

    class MATRIX_DLL Texture2D
    {
    public:
        Texture2D();
        ~Texture2D();

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;

        /// Loads an image file (PNG, JPG, BMP, TGA...). Returns false and leaves the texture empty on failure.
        bool LoadTexture2D(const std::filesystem::path& filepath, const TextureSpec& spec = TextureSpec());

        /// Legacy overload kept for API compatibility.
        bool LoadTexture2D(std::string_view filepath, TextureWrap wrap, TextureFilter filter = TextureFilter::LINEAR);

        /// Creates an empty RGBA8 texture of the given size.
        bool Create(uint32_t width, uint32_t height, const TextureSpec& spec = TextureSpec());

        /// Uploads raw RGBA8 pixel data. size must equal width * height * 4.
        void SetData(const void* data, uint32_t size);

        /// Binds to a texture unit (slot).
        void Bind(uint32_t slot = 0) const;
        void Unbind() const;

        bool IsLoaded() const { return m_TextureID != 0; }
        uint32_t GetID() const { return m_TextureID; }
        uint32_t GetWidth() const { return static_cast<uint32_t>(m_Width); }
        uint32_t GetHeight() const { return static_cast<uint32_t>(m_Height); }
        uint32_t GetWidht() const { return GetWidth(); } ///< Legacy misspelling kept for compatibility.
        uint32_t GetChannels() const { return static_cast<uint32_t>(m_NrChannels); }
        const std::string& GetPath() const { return m_TexturePath; }
        float GetAspectRatio() const { return m_Height > 0 ? static_cast<float>(m_Width) / static_cast<float>(m_Height) : 1.0f; }

        bool operator==(const Texture2D& other) const { return m_TextureID == other.m_TextureID; }

    private:
        void ApplyParameters(const TextureSpec& spec) const;
        void Release();

        uint32_t m_TextureID;
        std::string m_TexturePath;
        int32_t m_Width;
        int32_t m_Height;
        int32_t m_NrChannels;
    };
}
