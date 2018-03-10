#pragma once
#include <jpu/memory.hpp>
#include <mygl/gl.hpp>
#include <stdexcept>

namespace gl
{
    class texture : public jpu::ref_count
    {
    public:
        explicit texture(GLenum type) noexcept;
        ~texture() noexcept;
        operator gl_texture_t() const noexcept;

        void storage_1d(int width, GLenum internal_format, int levels = -1) noexcept;
        void storage_2d(int width, int height, GLenum internal_format, int levels = -1) noexcept;
        void storage_3d(int width, int height, int depth, GLenum internal_format, int levels = -1) noexcept;
        void storage_2d_multisample(int width, int height, int samples, GLenum internal_format,
                                    bool fixed_sample_locations = true) noexcept;
        void storage_2d_multisample(int width, int height, int depth, int samples, GLenum internal_format,
                                    bool fixed_sample_locations = true) noexcept;

        void assign_1d(int x, int width, int level, GLenum format, GLenum type, void* pixels) const noexcept;
        void assign_1d(GLenum format, GLenum type, void* pixels) const noexcept;
        void assign_1d_compressed(int x, int width, int level, GLenum format, GLenum type, void* pixels) const noexcept;
        void assign_1d_compressed(GLenum format, GLenum type, void* pixels) const noexcept;
        void assign_2d(int x, int y, int width, int height, int level, GLenum format, GLenum type, void* pixels) const noexcept;
        void assign_2d(GLenum format, GLenum type, void* pixels) const noexcept;
        void assign_2d_compressed(int x, int y, int width, int height, int level, GLenum format, GLenum type, void* pixels) const noexcept;
        void assign_2d_compressed(GLenum format, GLenum type, void* pixels) const noexcept;
        void assign_3d(int x, int y, int z, int width, int height, int depth, int level, GLenum format, GLenum type,
                       void* pixels) const noexcept;
        void assign_3d_compressed(int x, int y, int z, int width, int height, int depth, int level, GLenum format, GLenum type,
                       void* pixels) const noexcept;
        void assign_3d(GLenum format, GLenum type, void* pixels) const noexcept;
        void assign_3d_compressed(GLenum format, GLenum type, void* pixels) const noexcept;

        uint64_t address() const noexcept;
        void generate_mipmaps() const noexcept;

        void set_buffer(gl_buffer_t buffer, GLenum internal_format) const noexcept;
        void set_buffer(gl_buffer_t buffer, GLenum internal_format, size_t size, size_t offset) const noexcept;

        int width() const noexcept;
        int height() const noexcept;

        void get_texture_data(GLenum format, GLenum type, size_t target_byte_size, void* target, int level = 0) const noexcept;

    private:
        static int max_levels(int width, int height, int depth) noexcept;

        gl_texture_t _id;
        GLenum _type;
        int _width{0};
        int _height{0};
        int _depth{0};
    };

    class sampler : public jpu::ref_count
    {
    public:
        sampler() noexcept;
        ~sampler() noexcept;
        operator gl_sampler_t() const noexcept;

        void set(GLenum name, int value) const noexcept;
        void set(GLenum name, float value) const noexcept;

        uint64_t sample_texture(texture* t) const noexcept;

    private:
        gl_sampler_t _id;
    };

    class image : public jpu::ref_count
    {
    public:
        image(texture* t, int level, bool layered, int layer, GLenum format, GLenum access) noexcept;
        ~image() noexcept;
        operator uint64_t() const noexcept;

    private:
        uint64_t _handle;
    };
}
