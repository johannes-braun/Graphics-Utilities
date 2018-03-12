#include "../texture.hpp"
#include <algorithm>

namespace gl
{
    texture::texture(GLenum type) noexcept : _type(type)
    {
        glCreateTextures(_type, 1, &_id);
    }

    texture::~texture() noexcept
    {
        glDeleteTextures(1, &_id);
    }

    texture::operator gl_texture_t() const noexcept
    {
        return _id;
    }

    void texture::storage_1d(const int width, const GLenum internal_format, const int levels) noexcept
    {
        _width = width;
        glTextureStorage1D(_id, levels == -1 ? max_levels(width, 1, 1) : levels, internal_format, width);
    }

    void texture::storage_2d(const int width, const int height, const GLenum internal_format, const int levels) noexcept
    {
        _width = width;
        _height = height;
        glTextureStorage2D(_id, levels == -1 ? max_levels(width, height, 1) : levels, internal_format, width, height);
    }

    void texture::storage_3d(const int width, const int height, const int depth, const GLenum internal_format, const int levels) noexcept
    {
        _width = width;
        _height = height;
        _depth = depth;
        glTextureStorage3D(_id, levels == -1 ? max_levels(width, height, depth) : levels, internal_format, width,
                           height, depth);
    }

    void texture::storage_2d_multisample(const int width, int height, const int samples, const GLenum internal_format,
                                         const bool fixed_sample_locations) noexcept
    {
        _width = width;
        _height = height;
        glTextureStorage2DMultisample(_id, samples, internal_format, width, height, fixed_sample_locations);
    }

    void texture::storage_2d_multisample(const int width, const int height, const int depth, const int samples, const GLenum internal_format,
                                         const bool fixed_sample_locations) noexcept
    {
        _width = width;
        _height = height;
        _depth = depth;
        glTextureStorage3DMultisample(_id, samples, internal_format, width, height, depth, fixed_sample_locations);
    }

    void texture::assign_1d(const int x, const int width, const int level, const GLenum format, const GLenum type, void* pixels) const noexcept
    {
        glTextureSubImage1D(_id, level, x, width, format, type, pixels);
    }

    void texture::assign_1d(const GLenum format, const GLenum type, void* pixels) const noexcept
    {
        assign_1d(0, _width, 0, format, type, pixels);
    }

    void texture::assign_1d_compressed(const int x, const int width, const int level, const GLenum format, const GLenum type, void* pixels) const noexcept
    {
        glCompressedTextureSubImage1D(_id, level, x, width, format, type, pixels);
    }

    void texture::assign_1d_compressed(const GLenum format, const GLenum type, void* pixels) const noexcept
    {
        assign_1d_compressed(0, _width, 0, format, type, pixels);
    }

    void texture::assign_2d_compressed(const int x, const int y, const int width, const int height, const int level, const GLenum format, const GLenum type,
                                       void* pixels) const noexcept
    {
        glCompressedTextureSubImage2D(_id, level, x, y, width, height, format, type, pixels);
    }

    void texture::assign_2d_compressed(const GLenum format, const GLenum type, void* pixels) const noexcept
    {
        assign_2d_compressed(0, 0, _width, _height, 0, format, type, pixels);
    }

    void texture::assign_2d(const int x, const int y, const int width, const int height, const int level, const GLenum format, const GLenum type,
                            void* pixels) const noexcept
    {
        glTextureSubImage2D(_id, level, x, y, width, height, format, type, pixels);
    }

    void texture::assign_2d(const GLenum format, const GLenum type, void* pixels) const noexcept
    {
        assign_2d(0, 0, _width, _height, 0, format, type, pixels);
    }

    void texture::assign_3d(const int x, const int y, const int z, const int width, const int height, const int depth, const int level, const GLenum format,
                            const GLenum type, void* pixels) const noexcept
    {
        glTextureSubImage3D(_id, level, x, y, z, width, height, depth, format, type, pixels);
    }

    void texture::assign_3d_compressed(const int x, const int y, const int z, const int width, const int height, const int depth, const int level, const GLenum format,
        const GLenum type, void* pixels) const noexcept
    {
        glCompressedTextureSubImage3D(_id, level, x, y, z, width, height, depth, format, type, pixels);
    }

    void texture::assign_3d_compressed(const GLenum format, const GLenum type, void* pixels) const noexcept
    {
        assign_3d_compressed(0, 0, 0, _width, _height, _depth, 0, format, type, pixels);
    }

    void texture::assign_3d(const GLenum format, const GLenum type, void* pixels) const noexcept
    {
        assign_3d(0, 0, 0, _width, _height, _depth, 0, format, type, pixels);
    }

    uint64_t texture::address() const noexcept
    {
        if (!glGetTextureSamplerHandleARB)
            return 0;
        const auto handle = glGetTextureHandleARB(_id);
        if (!glIsTextureHandleResidentARB(handle))
            glMakeTextureHandleResidentARB(handle);
        return handle;
    }

    void texture::generate_mipmaps() const noexcept
    {
        glGenerateTextureMipmap(_id);
    }

    void texture::set_buffer(const gl_buffer_t buffer, const GLenum internal_format) const noexcept
    {
        glTextureBuffer(_id, internal_format, buffer);
    }

    void texture::set_buffer(const gl_buffer_t buffer, const GLenum internal_format, const size_t size, const size_t offset) const noexcept
    {
        glTextureBufferRange(_id, internal_format, buffer, size, offset);
    }

    int texture::width() const noexcept
    {
        return _width;
    }

    int texture::height() const noexcept
    {
        return _height;
    }

    void texture::get_texture_data(const GLenum format, const GLenum type, const size_t target_byte_size, void* target, const int level) const noexcept
    {
        glGetTextureImage(_id, level, format, type, static_cast<int>(target_byte_size), target);
    }

    int texture::max_levels(const int width, const int height, const int depth) noexcept
    {
        return 1 + static_cast<uint32_t>(std::log2(std::max(width, std::max(height, depth))));
    }

    sampler::sampler() noexcept
    {
        glCreateSamplers(1, &_id);

        // Default settings.
        set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        set(GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
        set(GL_TEXTURE_CUBE_MAP_SEAMLESS, true);
        set(GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
        set(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        set(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    }


    sampler::sampler(const sampler& other) noexcept
    {
        glCreateSamplers(1, &_id);
        operator=(std::forward<const sampler&>(other));
    }

    sampler::sampler(sampler&& other) noexcept
    {
        glCreateSamplers(1, &_id);
        operator=(std::forward<sampler&&>(other));
    }

    sampler& sampler::operator=(const sampler& other) noexcept
    {
        for (const auto paramf : _float_parameters)
        {
            float p;
            glGetSamplerParameterfv(other._id, paramf, &p);
            set(paramf, p);
        }
        for (const auto paramf : _int_parameters)
        {
            int p;
            glGetSamplerParameteriv(other._id, paramf, &p);
            set(paramf, p);
        }
        for (const auto paramf : _float_array_parameters)
        {
            float p[4];
            glGetSamplerParameterfv(other._id, paramf, p);
            glSamplerParameterfv(_id, paramf, p);
        }
        return *this;
    }

    sampler& sampler::operator=(sampler&& other) noexcept
    {
        _id = other._id;
        other._id = gl_sampler_t::zero;
        return *this;
    }

    sampler::~sampler() noexcept
    {
        if(_id != gl_sampler_t::zero)
            glDeleteSamplers(1, &_id);
    }

    void sampler::set(const GLenum name, const int value) const noexcept
    {
        glSamplerParameteri(_id, name, value);
    }

    void sampler::set(const GLenum name, const float value) const noexcept
    {
        glSamplerParameterf(_id, name, value);
    }

    uint64_t sampler::sample_texture(texture* t) const noexcept
    {
        if (!glGetTextureSamplerHandleARB)
            return 0;
        const auto handle = glGetTextureSamplerHandleARB(*t, _id);
        if (!glIsTextureHandleResidentARB(handle))
            glMakeTextureHandleResidentARB(handle);
        return handle;
    }

    image::image(texture* t, const int level, const bool layered, const int layer, const GLenum format, const GLenum access) noexcept
        : _handle(glGetImageHandleARB(*t, level, layered, layer, format))
    {
        glMakeImageHandleResidentARB(_handle, access);
    }

    image::~image() noexcept
    {
        glMakeImageHandleNonResidentARB(_handle);
    }

    image::operator uint64_t() const noexcept
    {
        return _handle;
    }

    sampler::operator gl_sampler_t() const noexcept
    {
        return _id;
    }
}
