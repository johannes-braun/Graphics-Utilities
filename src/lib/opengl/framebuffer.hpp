#pragma once

#include "gl.hpp"
#include "texture.hpp"

#include <variant>
#include <memory>
#include <vector>
#include <map>
#include <cassert>

namespace gl
{
    class framebuffer;

    class renderbuffer
    {
    public:
        renderbuffer(GLenum internal_format, int width, int height) noexcept;
        renderbuffer(GLenum internal_format, int width, int height, samples s) noexcept;
        renderbuffer(const renderbuffer& other) noexcept;
        renderbuffer(renderbuffer&& other) noexcept;
        renderbuffer& operator=(const renderbuffer& other) noexcept;
        renderbuffer& operator=(renderbuffer&& other) noexcept;
        ~renderbuffer() noexcept;

        operator gl_renderbuffer_t() const noexcept;

        int width() const noexcept;
        int height() const noexcept;

    private:
        gl_renderbuffer_t _id = gl_renderbuffer_t::zero;
        int _width{ 1 };
        int _height{ 1 };
        bool _multisampled = false;
        GLenum _internal_format;
        samples _samples = samples::x1;
    };

    class attachment
    {
    public:
        friend framebuffer;
        using all_types = std::variant<std::nullptr_t, std::shared_ptr<texture>, std::shared_ptr<renderbuffer>>;

        attachment(const attachment& other) = delete;
        attachment(attachment&& other) = default;
        attachment& operator=(const attachment& other) = delete;
        attachment& operator=(attachment&& other) = delete;

        std::nullptr_t set(std::nullptr_t);
        std::shared_ptr<renderbuffer>& set(const std::shared_ptr<renderbuffer>& r);
        std::shared_ptr<texture>& set(const std::shared_ptr<texture>& t);
        std::shared_ptr<texture>& set(const std::shared_ptr<texture>& t, int level);
        std::shared_ptr<texture>& set(const std::shared_ptr<texture>& t, int level, int layer);
        std::shared_ptr<texture>& operator=(const std::shared_ptr<texture>& t);
        std::shared_ptr<renderbuffer>& operator=(const std::shared_ptr<renderbuffer>& r);
        std::nullptr_t operator=(std::nullptr_t);

        operator const std::shared_ptr<texture>&() const;
        operator const std::shared_ptr<renderbuffer>&() const;

        int width() const noexcept;
        int height() const noexcept;

    private:
        enum class attachment_type
        {
            texture = 0,
            renderbuffer,
            empty
        };

        attachment(const framebuffer& framebuffer, GLenum type);

        gl_framebuffer_t _framebuffer = gl_framebuffer_t::zero;
        attachment_type _attachment_type = attachment_type::empty;
        GLenum _type;
        all_types _storage;
        int _level{ 0 };
        int _layer{ 0 };
        bool _layered{ false };
    };

    class framebuffer
    {
    public:
        static framebuffer& zero();

        framebuffer(std::nullptr_t);
        framebuffer() noexcept;
        framebuffer(const framebuffer& other) noexcept;
        framebuffer(framebuffer&& other) noexcept;
        framebuffer& operator=(const framebuffer& other) noexcept;
        framebuffer& operator=(framebuffer&& other) noexcept;
        ~framebuffer() noexcept;

        attachment& at(GLenum type);
        const attachment& at(GLenum type) const;
        attachment& operator[](GLenum type);
        const attachment& operator[](GLenum type) const;
        operator gl_framebuffer_t() const noexcept;

        GLenum status() const noexcept;
        attachment& emplace(GLenum type, const std::shared_ptr<texture>& a);
        attachment& emplace(GLenum type, const std::shared_ptr<renderbuffer>& a);
        void bind() const noexcept;

        template<typename T> using base_t = std::decay_t<std::remove_const_t<T>>;
        template<typename T, typename VT> using value_type_check_t = std::enable_if_t<std::is_same_v<typename base_t<T>::value_type, float>>;
        template<typename T, typename = std::void_t<value_type_check_t<T, float>, decltype(std::declval<base_t<T>>()[0])>>
        void clear(int color_attachment, const T& color) const
        {
            assert(color[0] == color[0] && color[3] == color[3] && "Length access check failed.");
            clear(color_attachment, &color[0]);
        }

        void clear(int color_attachment, const std::initializer_list<float>& color) const;
        void clear(int color_attachment, const float* color) const;
        void clear(float depth, int stencil) const noexcept;

        void set_readbuffer(GLenum attachment) noexcept;
        void set_drawbuffer(GLenum attachment) noexcept;
        template<typename Container, typename = std::void_t<decltype(std::data(std::declval<Container>())), std::enable_if_t<!std::is_same_v<Container, std::initializer_list<GLenum>> && std::is_same_v<typename Container::value_type, GLenum>>>>
        void set_drawbuffers(Container attachments) noexcept;
        void set_drawbuffers(std::initializer_list<GLenum> attachments) noexcept;
        void blit(const framebuffer& other, GLbitfield mask, GLenum filter = GL_NEAREST) const noexcept;
        void blit(const framebuffer& other, int x, int y, int width, int height, GLbitfield mask, GLenum filter = GL_NEAREST) const noexcept;
        void blit(const framebuffer& other, int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y, int dst_width, int dst_height, GLbitfield mask, GLenum filter = GL_NEAREST) const noexcept;

    private:
        void status_assert() const noexcept;

        gl_framebuffer_t _id = gl_framebuffer_t::zero;
        GLenum _readbuffer;
        std::vector<GLenum> _drawbuffers;
        std::map<GLenum, attachment> _attachments;
    };
}

#include "impl/framebuffer.inl"