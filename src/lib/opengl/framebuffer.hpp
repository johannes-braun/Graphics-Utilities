#pragma once

#include <glad/glad.h>
#include <vector>
#include "jpu/memory"
#include <map>

namespace gl
{
    class texture;

    class framebuffer : public jpu::ref_count
    {
        struct render_buffer
        {
            render_buffer() { glCreateRenderbuffers(1, &_id); }
            ~render_buffer() { glDeleteRenderbuffers(1, &_id); }
            operator unsigned() const { return _id; }
        private:
            uint32_t _id;
        };

    public:
        framebuffer();
        ~framebuffer();
        operator bool() const;
        operator unsigned() const;

        void use_renderbuffer(GLenum attachment, GLenum internal_format, int width, int height);
        void use_renderbuffer_multisample(GLenum attachment, GLenum internal_format, int width, int height, int samples);
        void attach(GLenum attachment, texture* texture, int level = 0);

        void draw_to_attachments(const std::vector<GLenum>& attachments) const;
        void read_from_attachment(GLenum attachment) const;

        void bind();
        void unbind();

        struct blit_rect { int x0, y0, x1, y1; };
        void blit(framebuffer* other, blit_rect src, blit_rect dst, GLbitfield buffers, GLenum filter);

    private:
        void check_complete() const;

        uint32_t _id;
        std::map<GLenum, jpu::ref_ptr<texture>> _attachments;
        std::map<GLenum, render_buffer> _render_buffers;
        uint32_t _last_bound_framebuffer { 0 };
    };
}
