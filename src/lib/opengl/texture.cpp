#include "texture.hpp"

namespace gl
{
    texture::texture(texture_type type): _type(type)
    {
        glCreateTextures(static_cast<GLenum>(type), 1, &_id);
    }

    texture::~texture()
    {
        glDeleteTextures(1, &_id);
    }

    texture::operator bool() const
    {
        return _id != 0;
    }

    texture::operator unsigned() const
    {
        return _id;
    }

    void texture::storage_1d(int width, GLenum internal_format, int levels)
    {
        _width = width;
        glTextureStorage1D(_id, levels == -1 ? max_levels(width, 1, 1) : levels, internal_format, width);
    }

    void texture::storage_2d(int width, int height, GLenum internal_format, int levels)
    {
        _width = width;
        _height = height;
        glTextureStorage2D(_id, levels == -1 ? max_levels(width, height, 1) : levels, internal_format, width, height);
    }

    void texture::storage_3d(int width, int height, int depth, GLenum internal_format, int levels)
    {
        _width = width;
        _height = height;
        _depth = depth;
        glTextureStorage3D(_id, levels == -1 ? max_levels(width, height, depth) : levels, internal_format, width,
                           height, depth);
    }

    void texture::storage_2d_multisample(int width, int height, int samples, GLenum internal_format,
                                         bool fixed_sample_locations)
    {
        _width = width;
        _height = height;
        glTextureStorage2DMultisample(_id, samples, internal_format, width, height, fixed_sample_locations);
    }

    void texture::storage_2d_multisample(int width, int height, int depth, int samples, GLenum internal_format,
                                         bool fixed_sample_locations)
    {
        _width = width;
        _height = height;
        _depth = depth;
        glTextureStorage3DMultisample(_id, samples, internal_format, width, height, depth, fixed_sample_locations);
    }

    void texture::assign_1d(int x, int width, int level, GLenum format, GLenum type, void* pixels) const
    {
        glTextureSubImage1D(_id, level, x, width, format, type, pixels);
    }

    void texture::assign_1d(GLenum format, GLenum type, void* pixels) const
    {
        assign_1d(0, _width, 0, format, type, pixels);
    }

    void texture::assign_1d_compressed(int x, int width, int level, GLenum format, GLenum type, void* pixels) const
    {
        glCompressedTextureSubImage1D(_id, level, x, width, format, type, pixels);
    }

    void texture::assign_1d_compressed(GLenum format, GLenum type, void* pixels) const
    {
        assign_1d_compressed(0, _width, 0, format, type, pixels);
    }

    void texture::assign_2d_compressed(int x, int y, int width, int height, int level, GLenum format, GLenum type,
        void* pixels) const
    {
        glCompressedTextureSubImage2D(_id, level, x, y, width, height, format, type, pixels);
    }

    void texture::assign_2d_compressed(GLenum format, GLenum type, void* pixels) const
    {
        assign_2d_compressed(0, 0, _width, _height, 0, format, type, pixels);
    }

    void texture::assign_2d(int x, int y, int width, int height, int level, GLenum format, GLenum type,
                            void* pixels) const
    {
        glTextureSubImage2D(_id, level, x, y, width, height, format, type, pixels);
    }

    void texture::assign_2d(GLenum format, GLenum type, void* pixels) const
    {
        assign_2d(0, 0, _width, _height, 0, format, type, pixels);
    }

    void texture::assign_3d(int x, int y, int z, int width, int height, int depth, int level, GLenum format,
                            GLenum type, void* pixels) const
    {
        glTextureSubImage3D(_id, level, x, y, z, width, height, depth, format, type, pixels);
    }

    void texture::assign_3d_compressed(int x, int y, int z, int width, int height, int depth, int level, GLenum format,
                            GLenum type, void* pixels) const
    {
        glCompressedTextureSubImage3D(_id, level, x, y, z, width, height, depth, format, type, pixels);
    }

    void texture::assign_3d_compressed(GLenum format, GLenum type, void* pixels) const
    {
        assign_3d_compressed(0, 0, 0, _width, _height, _depth, 0, format, type, pixels);
    }

    void texture::assign_3d(GLenum format, GLenum type, void* pixels) const
    {
        assign_3d(0, 0, 0, _width, _height, _depth, 0, format, type, pixels);
    }

    uint64_t texture::address() const
    {
        if(!glGetTextureSamplerHandleARB)
            return 0;
        const auto handle = glGetTextureHandleARB(_id);
        if (!glIsTextureHandleResidentARB(handle))
            glMakeTextureHandleResidentARB(handle);
        return handle;
    }

    void texture::generate_mipmaps() const
    {
        glGenerateTextureMipmap(_id);
    }

    int texture::max_levels(int width, int height, int depth)
    {
        return 1 + static_cast<uint32_t>(std::log2(std::max(width, std::max(height, depth))));
    }

    sampler::sampler()
    {
        glCreateSamplers(1, &_id);
    }

    sampler::~sampler()
    {
        glDeleteSamplers(1, &_id);
    }

    void sampler::set(GLenum name, int value) const
    {
        glSamplerParameteri(_id, name, value);
    }

    void sampler::set(GLenum name, float value) const
    {
        glSamplerParameterf(_id, name, value);
    }

    uint64_t sampler::sample_texture(texture* t) const
    {
        if(!glGetTextureSamplerHandleARB)
            return 0;
        const auto handle = glGetTextureSamplerHandleARB(*t, _id);
        if (!glIsTextureHandleResidentARB(handle))
            glMakeTextureHandleResidentARB(handle);
        return handle;
    }

    sampler::operator unsigned() const
    {
        return _id;
    }
}
