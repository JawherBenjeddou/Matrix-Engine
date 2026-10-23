#pragma once
#include "Common.h"
///@file Framebuffer.h  Off-screen render target with colour, integer-id and depth attachments.

namespace Matrix::graphics
{
    enum class FramebufferTextureFormat : uint8_t
    {
        None = 0,
        RGBA8,          ///< Standard colour
        RED_INTEGER,    ///< 32-bit signed integer, used for entity picking
        DEPTH24STENCIL8 ///< Depth + stencil
    };

    struct MATRIX_DLL FramebufferTextureSpec
    {
        FramebufferTextureFormat Format = FramebufferTextureFormat::None;
        FramebufferTextureSpec() = default;
        FramebufferTextureSpec(FramebufferTextureFormat format) : Format(format) {}
    };

    struct MATRIX_DLL FramebufferSpec
    {
        uint32_t Width = 1280;
        uint32_t Height = 720;
        std::vector<FramebufferTextureSpec> Attachments;
        uint32_t Samples = 1;
    };

    class MATRIX_DLL Framebuffer
    {
    public:
        explicit Framebuffer(const FramebufferSpec& spec);
        ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        /// (Re)creates all attachments according to the spec.
        void Invalidate();

        void Bind() const;
        void Unbind() const;

        /// Resizes the framebuffer if the size changed. Ignores zero or absurd sizes.
        void Resize(uint32_t width, uint32_t height);

        /// Reads a single integer pixel from an integer colour attachment (used for mouse picking).
        int ReadPixel(uint32_t attachmentIndex, int x, int y) const;

        /// Clears an integer attachment to a value.
        void ClearAttachment(uint32_t attachmentIndex, int value) const;

        uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const;
        uint32_t GetWidth() const { return m_Spec.Width; }
        uint32_t GetHeight() const { return m_Spec.Height; }
        const FramebufferSpec& GetSpec() const { return m_Spec; }

        static constexpr uint32_t MaxSize = 8192;

    private:
        void Release();

        FramebufferSpec m_Spec;
        uint32_t m_RendererID = 0;
        std::vector<FramebufferTextureSpec> m_ColorAttachmentSpecs;
        FramebufferTextureSpec m_DepthAttachmentSpec;
        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };
}
