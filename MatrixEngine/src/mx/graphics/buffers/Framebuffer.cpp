#include "pch.h"
#include "Framebuffer.h"
#include "core/Logging.h"

#include "GL/glew.h"

namespace Matrix::graphics
{
    namespace
    {
        bool IsDepthFormat(FramebufferTextureFormat format)
        {
            return format == FramebufferTextureFormat::DEPTH24STENCIL8;
        }

        void AttachColorTexture(uint32_t id, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
        {
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, internalFormat == GL_R32I ? GL_INT : GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
        }

        void AttachDepthTexture(uint32_t id, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
        {
            glBindTexture(GL_TEXTURE_2D, id);
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, id, 0);
        }
    }

    Framebuffer::Framebuffer(const FramebufferSpec& spec)
        : m_Spec(spec)
    {
        for (const auto& attachment : m_Spec.Attachments)
        {
            if (IsDepthFormat(attachment.Format))
                m_DepthAttachmentSpec = attachment;
            else
                m_ColorAttachmentSpecs.push_back(attachment);
        }
        Invalidate();
    }

    Framebuffer::~Framebuffer()
    {
        Release();
    }

    void Framebuffer::Release()
    {
        if (m_RendererID != 0)
        {
            glDeleteFramebuffers(1, &m_RendererID);
            m_RendererID = 0;
        }
        if (!m_ColorAttachments.empty())
        {
            glDeleteTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());
            m_ColorAttachments.clear();
        }
        if (m_DepthAttachment != 0)
        {
            glDeleteTextures(1, &m_DepthAttachment);
            m_DepthAttachment = 0;
        }
    }

    void Framebuffer::Invalidate()
    {
        Release();

        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        if (!m_ColorAttachmentSpecs.empty())
        {
            m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
            glGenTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());

            for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
            {
                switch (m_ColorAttachmentSpecs[i].Format)
                {
                case FramebufferTextureFormat::RGBA8:
                    AttachColorTexture(m_ColorAttachments[i], GL_RGBA8, GL_RGBA, m_Spec.Width, m_Spec.Height, static_cast<int>(i));
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    AttachColorTexture(m_ColorAttachments[i], GL_R32I, GL_RED_INTEGER, m_Spec.Width, m_Spec.Height, static_cast<int>(i));
                    break;
                default:
                    break;
                }
            }
        }

        if (m_DepthAttachmentSpec.Format != FramebufferTextureFormat::None)
        {
            glGenTextures(1, &m_DepthAttachment);
            AttachDepthTexture(m_DepthAttachment, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Spec.Width, m_Spec.Height);
        }

        if (m_ColorAttachments.size() > 1)
        {
            MX_ASSERT(m_ColorAttachments.size() <= 4, "Too many colour attachments");
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(static_cast<GLsizei>(m_ColorAttachments.size()), buffers);
        }
        else if (m_ColorAttachments.empty())
        {
            glDrawBuffer(GL_NONE);
        }

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
            MX_CORE_ERROR("Framebuffer is incomplete (status 0x{:X})", status);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Spec.Width, m_Spec.Height);
    }

    void Framebuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > MaxSize || height > MaxSize)
            return;
        if (width == m_Spec.Width && height == m_Spec.Height)
            return;
        m_Spec.Width = width;
        m_Spec.Height = height;
        Invalidate();
    }

    int Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) const
    {
        if (attachmentIndex >= m_ColorAttachments.size())
            return -1;
        if (x < 0 || y < 0 || x >= static_cast<int>(m_Spec.Width) || y >= static_cast<int>(m_Spec.Height))
            return -1;
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixel = -1;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
        return pixel;
    }

    void Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value) const
    {
        if (attachmentIndex >= m_ColorAttachments.size())
            return;
        glClearTexImage(m_ColorAttachments[attachmentIndex], 0, GL_RED_INTEGER, GL_INT, &value);
    }

    uint32_t Framebuffer::GetColorAttachmentRendererID(uint32_t index) const
    {
        return index < m_ColorAttachments.size() ? m_ColorAttachments[index] : 0;
    }
}
