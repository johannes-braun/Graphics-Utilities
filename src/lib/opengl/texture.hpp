#pragma once
#include <jpu/memory>
#include "glad/glad.h"
#include <algorithm>

namespace gl
{
    enum class texture_type
    {
        def_1d = GL_TEXTURE_1D,
        def_2d = GL_TEXTURE_2D, 
        def_3d = GL_TEXTURE_3D, 
        array_1d = GL_TEXTURE_1D_ARRAY, 
        array_2d = GL_TEXTURE_2D_ARRAY, 
        rectangle = GL_TEXTURE_RECTANGLE, 
        cube_map = GL_TEXTURE_CUBE_MAP,
        cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY, 
        buffer = GL_TEXTURE_BUFFER, 
        multisample_2d = GL_TEXTURE_2D_MULTISAMPLE,
        multisample_array_2d = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    };

    class texture : public jpu::ref_count
    {
    public:
        explicit texture(texture_type type);
        ~texture();
        operator bool() const;
        operator unsigned() const;

        void storage_1d(int width, GLenum internal_format, int levels = -1);
        void storage_2d(int width, int height, GLenum internal_format, int levels = -1);
        void storage_3d(int width, int height, int depth, GLenum internal_format, int levels = -1);
        void storage_2d_multisample(int width, int height, int samples, GLenum internal_format,
                                    bool fixed_sample_locations = true);
        void storage_2d_multisample(int width, int height, int depth, int samples, GLenum internal_format,
                                    bool fixed_sample_locations = true);

        void assign_1d(int x, int width, int level, GLenum format, GLenum type, void* pixels) const;
        void assign_1d(GLenum format, GLenum type, void* pixels) const;
        void assign_1d_compressed(int x, int width, int level, GLenum format, GLenum type, void* pixels) const;
        void assign_1d_compressed(GLenum format, GLenum type, void* pixels) const;
        void assign_2d(int x, int y, int width, int height, int level, GLenum format, GLenum type, void* pixels) const;
        void assign_2d(GLenum format, GLenum type, void* pixels) const;
        void assign_2d_compressed(int x, int y, int width, int height, int level, GLenum format, GLenum type, void* pixels) const;
        void assign_2d_compressed(GLenum format, GLenum type, void* pixels) const;
        void assign_3d(int x, int y, int z, int width, int height, int depth, int level, GLenum format, GLenum type,
                       void* pixels) const;
        void assign_3d_compressed(int x, int y, int z, int width, int height, int depth, int level, GLenum format, GLenum type,
                       void* pixels) const;
        void assign_3d(GLenum format, GLenum type, void* pixels) const;
        void assign_3d_compressed(GLenum format, GLenum type, void* pixels) const;

        uint64_t address() const;
        void generate_mipmaps() const;

    private:
        static int max_levels(int width, int height, int depth);

        uint32_t _id;
        texture_type _type;
        int _width{0};
        int _height{0};
        int _depth{0};
    };

    class sampler : public jpu::ref_count
    {
    public:
        sampler();
        ~sampler();
        operator unsigned() const;

        void set(GLenum name, int value) const;
        void set(GLenum name, float value) const;

        uint64_t sample_texture(texture* t) const;

    private:
        uint32_t _id;
    };
}
