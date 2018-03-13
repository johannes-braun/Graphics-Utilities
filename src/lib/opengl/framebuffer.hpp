#pragma once

#include "gl.hpp"

#include <memory>
#include <vector>
#include <jpu/memory.hpp>
#include <map>
#include <glm/detail/type_vec4.hpp>

namespace gl
{
    namespace v2
    {
        class texture;
    }

    class framebuffer : public jpu::ref_count
    {
        struct render_buffer
        {
            render_buffer() noexcept;
            ~render_buffer() noexcept;
            operator gl_renderbuffer_t() const noexcept;
        private:
            gl_renderbuffer_t _id;
        };

    public:
        static framebuffer default_fbo() noexcept { return nullptr; }
        framebuffer(nullptr_t) noexcept;
        framebuffer() noexcept;
        ~framebuffer() noexcept;
        operator gl_framebuffer_t() const noexcept;

        void use_renderbuffer(GLenum attachment, GLenum internal_format, int width, int height);
        void use_renderbuffer_multisample(GLenum attachment, GLenum internal_format, int width, int height, int samples);
        void attach(GLenum attachment, std::shared_ptr<v2::texture> texture, int level = 0);

        void draw_to_attachments(const std::vector<GLenum>& attachments) const noexcept;
        void read_from_attachment(GLenum attachment) const noexcept;

        void bind() const noexcept;
        void unbind() const noexcept;

        struct blit_rect { int x0, y0, x1, y1; };
        void blit(const framebuffer& other, blit_rect src, blit_rect dst, GLbitfield buffers, GLenum filter) const noexcept;
        void blit(const framebuffer& other, blit_rect src_and_dst, GLbitfield buffers, GLenum filter) const noexcept;

        void clear_color(int attachment, const glm::vec4& color) const noexcept;
        void clear_depth(float depth) const noexcept;
        void clear_stencil(int stencil) const noexcept;

    private:
        void check_complete() const noexcept;

        gl_framebuffer_t _id;
        std::map<GLenum, std::shared_ptr<v2::texture>> _attachments;
        std::map<GLenum, render_buffer> _render_buffers;
    };
}
