#pragma once

#include <mygl/mygl.hpp>

#include <glm/glm.hpp>
#include <array>

namespace gl
{
    class texture;
    struct pixel
    {
        friend texture;
        pixel() = delete;
        pixel(const pixel&) = delete;
        pixel& operator=(const pixel&) = delete;
        pixel& operator=(pixel&&) = delete;
        ~pixel();

        void set(const glm::vec4& color) noexcept;
        const glm::vec4& get() const noexcept;
        glm::vec4& get() noexcept;

        operator const glm::vec4&() const noexcept;
        operator glm::vec4&() noexcept;
        pixel& operator=(const glm::vec4& color) noexcept;

    private:
        pixel(const texture* parent, glm::ivec3 position, int level) noexcept;
        pixel(pixel&& other) = default;

        glm::vec4 _color;
        glm::ivec3 _position;
        int _level;
        const texture* _parent;
    };

    enum class samples
    {
        x1 = 0,
        x2,
        x4,
        x8,
        x16,
        x32,
        x64
    };

    class texture
    {
    public:
        friend pixel;

        explicit texture(GLenum type) noexcept;
        explicit texture(GLenum type, int width, GLenum internal_format, int levels = -1) noexcept;
        explicit texture(GLenum type, int width, int height, GLenum internal_format, int levels = -1) noexcept;
        explicit texture(GLenum type, int width, int height, int depth, GLenum internal_format, int levels = -1) noexcept;
        explicit texture(GLenum type, int width, int height, samples samples, GLenum internal_format, bool fixed_locations = true) noexcept;
        explicit texture(GLenum type, int width, int height, int depth, samples samples, GLenum internal_format, bool fixed_locations = true) noexcept;

        texture(const texture& other) noexcept;
        texture(texture&& other) noexcept;
        texture& operator=(const texture& other) noexcept;
        texture& operator=(texture&& other) noexcept;
        ~texture();

        void resize(int width);
        void resize(int width, int height);
        void resize(int width, int height, int depth);
        void resize(int width, int height, samples samples);
        void resize(int width, int height, int depth, samples samples);

        void assign(int x, int width, GLenum format, GLenum type, const void* pixels) noexcept;
        void assign(int x, int y, int width, int height, GLenum format, GLenum type, const void* pixels) noexcept;
        void assign(int x, int y, int z, int width, int height, int depth, GLenum format, GLenum type, const void* pixels) noexcept;
        void assign(int level, int x, int width, GLenum format, GLenum type, const void* pixels) noexcept;
        void assign(int level, int x, int y, int width, int height, GLenum format, GLenum type, const void* pixels) noexcept;
        void assign(int level, int x, int y, int z, int width, int height, int depth, GLenum format, GLenum type, const void* pixels) noexcept;
        void assign(GLenum format, GLenum type, const void* pixels) noexcept;
        void assign(int level, GLenum format, GLenum type, const void* pixels) noexcept;

        pixel at(int x, int level) const;
        pixel at(int x, int y, int level) const;
        pixel at(int x, int y, int z, int level) const;

        void get_data(GLenum format, GLenum type, size_t size, void* target) const noexcept;
        void get_data(int level, GLenum format, GLenum type, size_t size, void* target) const noexcept;
        void set_buffer(mygl::buffer buffer, GLenum internal_format) const noexcept;
        void set_buffer(mygl::buffer buffer, GLenum internal_format, size_t size, size_t offset) const noexcept;
        void generate_mipmaps() const noexcept;

        int width() const noexcept;
        int height() const noexcept;
        int depth() const noexcept;
        int levels() const noexcept;
        GLenum internal_format() const noexcept;
        uint64_t handle() const noexcept;

        operator mygl::texture() const noexcept;
        mygl::texture id() const noexcept;

        void bind(uint32_t binding) const noexcept;
        void bind_image(uint32_t binding) const noexcept;
        void bind_image(uint32_t binding, GLenum access) const noexcept;
        void bind_image(uint32_t binding, GLenum format, GLenum access) const noexcept;
        void bind_image(uint32_t binding, int level, GLenum format, GLenum access) const noexcept;
        void bind_image(uint32_t binding, int level, int layer, GLenum format, GLenum access) const noexcept;

    private:
        void init_handle() noexcept;

        mygl::texture _id = mygl::texture::zero;
        GLenum _type;
        GLenum _internal_format;
        int _levels{ 1 };
        int _width{ 1 };
        int _height{ 1 };
        int _depth{ 1 };
        bool _fixed_sample_locations = true;
        samples _samples = samples::x1;
        uint64_t _handle = 0;
    };

    class sampler
    {
    public:
        sampler() noexcept;
        ~sampler() noexcept;
        sampler(const sampler& other) noexcept;
        sampler(sampler&& other) noexcept;
        sampler& operator=(const sampler& other) noexcept;
        sampler& operator=(sampler&& other) noexcept;
        operator mygl::sampler() const noexcept;
        mygl::sampler id() const noexcept;

        void set(GLenum name, int value) const noexcept;
        void set(GLenum name, float value) const noexcept;
        void set(GLenum name, GLenum value) const noexcept { set(name, int(value)); }

        uint64_t sample(mygl::texture t) const noexcept;
        void bind(uint32_t binding) const noexcept;

    private:
        constexpr const static std::array<GLenum, 3> _float_parameters{
            GL_TEXTURE_MIN_LOD,
            GL_TEXTURE_MAX_LOD,
            GL_TEXTURE_MAX_ANISOTROPY
        };

        constexpr const static std::array<GLenum, 8> _int_parameters{
            GL_TEXTURE_WRAP_S,
            GL_TEXTURE_WRAP_T,
            GL_TEXTURE_WRAP_R,
            GL_TEXTURE_MIN_FILTER,
            GL_TEXTURE_MAG_FILTER,
            GL_TEXTURE_COMPARE_MODE,
            GL_TEXTURE_COMPARE_FUNC,
            GL_TEXTURE_CUBE_MAP_SEAMLESS
        };

        constexpr const static std::array<GLenum, 1> _float_array_parameters{
            GL_TEXTURE_BORDER_COLOR
        };

        mygl::sampler _id;
    };

    class image
    {
    public:
        image(const texture& texture, GLenum format, GLenum access) noexcept;
        image(const texture& texture, int level, GLenum format, GLenum access) noexcept;
        image(const texture& texture, int level, int layer, GLenum format, GLenum access) noexcept;
        ~image() noexcept;

        uint64_t handle() const noexcept;
        void bind(uint32_t binding) const noexcept;

    private:
        uint64_t _handle;
        mygl::texture _texture;
        int _level;
        bool _layered;
        int _layer;
        GLenum _format;
        GLenum _access;
    };
}
