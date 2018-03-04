#include "framebuffer.hpp"
#include "texture.hpp"

namespace gl
{
    framebuffer::render_buffer::render_buffer()
    {
        glCreateRenderbuffers(1, &_id);
    }

    framebuffer::render_buffer::~render_buffer()
    {
        glDeleteRenderbuffers(1, &_id);
    }

    framebuffer::render_buffer::operator unsigned() const
    {
        return _id;
    }

    framebuffer::framebuffer(nullptr_t)
        : _id(0)
    {
    }

    framebuffer::framebuffer()
    {
        glCreateFramebuffers(1, &_id);
    }

    framebuffer::~framebuffer()
    {
        if(_id)
            glDeleteFramebuffers(1, &_id);
    }

    framebuffer::operator unsigned() const
    {
        return _id;
    }

    void framebuffer::use_renderbuffer(const GLenum attachment, const GLenum internal_format, const int width, const int height)
    {
        if(_id == 0)
        {
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, _id, GL_DEBUG_SEVERITY_HIGH, 0, "Cannot attach a renderbuffer to the default framebuffer.");
            return;
        }

        if (_render_buffers.count(attachment) != 0)
            _render_buffers.erase(attachment);

        auto&& buffer = _render_buffers[attachment];
        _attachments.erase(attachment);
        glNamedRenderbufferStorage(buffer, internal_format, width, height);
        glNamedFramebufferRenderbuffer(*this, attachment, GL_RENDERBUFFER, buffer);

        check_complete();
    }

    void framebuffer::use_renderbuffer_multisample(const GLenum attachment, const GLenum internal_format, const int width, const int height,
        const int samples)
    {
        if (_id == 0)
        {
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, _id, GL_DEBUG_SEVERITY_HIGH, 0, "Cannot attach a renderbuffer to the default framebuffer.");
            return;
        }

        if (_render_buffers.count(attachment) != 0)
            _render_buffers.erase(attachment);

        auto&& buffer = _render_buffers[attachment];
        _attachments.erase(attachment);
        glNamedRenderbufferStorageMultisample(buffer, samples, internal_format, width, height);
        glNamedFramebufferRenderbuffer(*this, attachment, GL_RENDERBUFFER, buffer);

        check_complete();
    }

    void framebuffer::attach(const GLenum attachment, texture* texture, const int level)
    {
        if (_id == 0)
        {
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, _id, GL_DEBUG_SEVERITY_HIGH, 0, "Cannot attach a texture to the default framebuffer.");
            return;
        }

        glNamedFramebufferTexture(_id, attachment, *texture, level);
        _attachments[attachment] = texture;
        _render_buffers.erase(attachment);

        check_complete();
    }

    void framebuffer::draw_to_attachments(const std::vector<GLenum>& attachments) const
    {
        glNamedFramebufferDrawBuffers(_id, static_cast<int>(attachments.size()), attachments.data());
    }

    void framebuffer::read_from_attachment(const GLenum attachment) const
    {
        glNamedFramebufferReadBuffer(_id, attachment);
    }

    void framebuffer::bind()
    {
        int val;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &val);
        _last_bound_framebuffer = static_cast<uint32_t>(val);
        glBindFramebuffer(GL_FRAMEBUFFER, _id);

    }

    void framebuffer::unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _last_bound_framebuffer);
    }

    void framebuffer::blit(const framebuffer& other, const blit_rect src, const blit_rect dst, const GLbitfield buffers, const GLenum filter) const
    {
        int read, draw, fbo;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw);
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read);
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other._id);
        glBlitFramebuffer(src.x0, src.y0, src.x1, src.y1, dst.x0, dst.y0, dst.x1, dst.y1, buffers, filter);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, read);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void framebuffer::blit(const framebuffer& other, const blit_rect src_and_dst, const GLbitfield buffers, const GLenum filter) const
    {
        blit(other, src_and_dst, src_and_dst, buffers, filter);
    }

    void framebuffer::clear_color(const int attachment, const glm::vec4& color) const
    {
        glClearNamedFramebufferfv(_id, GL_COLOR, attachment, &color[0]);
    }

    void framebuffer::clear_depth(const float depth) const
    {
        glClearNamedFramebufferfv(_id, GL_DEPTH, 0, &depth);
    }

    void framebuffer::clear_stencil(const int stencil) const
    {
        glClearNamedFramebufferiv(_id, GL_DEPTH, 0, &stencil);
    }

    void framebuffer::check_complete() const
    {
        if (glCheckNamedFramebufferStatus(*this, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, _id, GL_DEBUG_SEVERITY_HIGH, -1,
                "Framebuffer is incomplete!");
    }
}
