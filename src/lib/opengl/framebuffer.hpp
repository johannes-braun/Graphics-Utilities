#pragma once

#include <mygl/gl.hpp>
#include <vector>
#include <jpu/memory>
#include <map>
#include <glm/detail/type_vec4.hpp>

namespace gl
{
    class texture;

    class framebuffer : public jpu::ref_count
    {
        struct render_buffer
        {
            render_buffer();
            ~render_buffer();
            operator gl_renderbuffer_t() const;
        private:
            gl_renderbuffer_t _id;
        };

    public:
        static framebuffer default_fbo() { return nullptr; }
        framebuffer(nullptr_t);
        framebuffer();
        ~framebuffer();
        operator gl_framebuffer_t() const;

        void use_renderbuffer(GLenum attachment, GLenum internal_format, int width, int height);
        void use_renderbuffer_multisample(GLenum attachment, GLenum internal_format, int width, int height, int samples);
        void attach(GLenum attachment, texture* texture, int level = 0);

        void draw_to_attachments(const std::vector<GLenum>& attachments) const;
        void read_from_attachment(GLenum attachment) const;

        void bind();
        void unbind() const;

        struct blit_rect { int x0, y0, x1, y1; };
        void blit(const framebuffer& other, blit_rect src, blit_rect dst, uint32_t buffers, GLenum filter) const;
        void blit(const framebuffer& other, blit_rect src_and_dst, uint32_t buffers, GLenum filter) const;

        void clear_color(int attachment, const glm::vec4& color) const;
        void clear_depth(float depth) const;
        void clear_stencil(int stencil) const;

    private:
        void check_complete() const;

        gl_framebuffer_t _id;
        std::map<GLenum, jpu::ref_ptr<texture>> _attachments;
        std::map<GLenum, render_buffer> _render_buffers;
        gl_framebuffer_t _last_bound_framebuffer { 0 };
    };
}
