#include "../texture.hpp"
#include <algorithm>

namespace gl
{
    namespace v2
    {
        int max_levels(int width, int height, int depth)
        {
            return 1 + static_cast<uint32_t>(std::log2(std::max(width, std::max(height, depth))));
        }

        texture::texture(GLenum type) noexcept
            : _type(type)
        {
        }

        texture::texture(GLenum type, int width, GLenum internal_format, int levels) noexcept
            : texture(type)
        {
            _internal_format = internal_format;
            _height = 1;
            _depth = 1;
            _levels = levels == -1 ? max_levels(width, _height, _depth) : levels;
            resize(width);
        }

        texture::texture(GLenum type, int width, int height, GLenum internal_format, int levels) noexcept
            : texture(type)
        {
            _internal_format = internal_format;
            _depth = 1;
            _levels = levels == -1 ? max_levels(width, height, _depth) : levels;
            resize(width, height);
        }

        texture::texture(GLenum type, int width, int height, int depth, GLenum internal_format, int levels) noexcept
            : texture(type)
        {
            _internal_format = internal_format;
            _levels = levels == -1 ? max_levels(width, height, depth) : levels;
            resize(width, height, depth);
        }

        texture::texture(GLenum type, int width, int height, samples samples, GLenum internal_format, bool fixed_locations) noexcept
            : texture(type)
        {
            _internal_format = internal_format;
            _fixed_sample_locations = fixed_locations;
            resize(width, height, samples);
        }

        texture::texture(GLenum type, int width, int height, int depth, samples samples, GLenum internal_format, bool fixed_locations) noexcept
            : texture(type)
        {
            _internal_format = internal_format;
            _fixed_sample_locations = fixed_locations;
            resize(width, height, depth, samples);
        }

        texture::texture(const texture& other) noexcept
        {
            operator=(std::forward<const texture&>(other));
        }

        texture::texture(texture&& other) noexcept
        {
            operator=(std::forward<texture&&>(other));
        }

        texture& texture::operator=(const texture& other) noexcept
        {
            if (_id != gl_texture_t::zero)
                glDeleteTextures(1, &_id);
            _type = other._type;
            _internal_format = other._internal_format;
            _levels = other._levels;
            _fixed_sample_locations = other._fixed_sample_locations;
            _handle = other._handle;
            glCreateTextures(_type, 1, &_id);

            switch (_type)
            {
            case GL_TEXTURE_1D:
                resize(other._width);
                break;
            case GL_TEXTURE_1D_ARRAY:
            case GL_TEXTURE_2D:
            case GL_TEXTURE_CUBE_MAP:
            case GL_TEXTURE_RECTANGLE:
                resize(other._width, other._height);
                break;
            case GL_TEXTURE_CUBE_MAP_ARRAY:
            case GL_TEXTURE_2D_ARRAY:
            case GL_TEXTURE_3D:
                resize(other._width, other._height, other._depth);
                break;
            case GL_TEXTURE_2D_MULTISAMPLE:
                resize(other._width, other._height, other._samples);
                break;
            case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                resize(other._width, other._height, other._depth, other._samples);
            default: break;
            }

            for(int level = 0; level < max_levels(other._width, other._height, other._depth); ++level)
                glCopyImageSubData(uint32_t(other._id), other._type, level, 0, 0, 0, 
                    uint32_t(_id), _type, level, 0, 0, 0, 
                    other._width, other._height, other._depth);
            return *this;
        }

        texture& texture::operator=(texture&& other) noexcept
        {
            if (_id != gl_texture_t::zero)
                glDeleteTextures(1, &_id);
            _id = other._id;
            _type = other._type;
            _internal_format = other._internal_format;
            _levels = other._levels;
            _width = other._width;
            _height = other._height;
            _depth = other._depth;
            _fixed_sample_locations = other._fixed_sample_locations;
            _samples = other._samples;
            _handle = other._handle;

            other._id = gl_texture_t::zero;
            return *this;
        }

        texture::~texture()
        {
            if (_id != gl_texture_t::zero)
                glDeleteTextures(1, &_id);
        }

        void texture::resize(int width)
        {
            if (width == _width)
                return;
            _width = width;
            if (_id != gl_texture_t::zero)
                glDeleteTextures(1, &_id);
            glCreateTextures(_type, 1, &_id);
            glTextureStorage1D(_id, _levels, _internal_format, _width);
            init_handle();
        }

        void texture::resize(int width, int height)
        {
            if (width == _width && height == _height)
                return;
            _width = width;
            _height = height;
            if (_id != gl_texture_t::zero)
                glDeleteTextures(1, &_id);
            glCreateTextures(_type, 1, &_id);
            glTextureStorage2D(_id, _levels, _internal_format, _width, _height);
            init_handle();
        }

        void texture::resize(int width, int height, int depth)
        {
            if (width == _width && height == _height && depth == _depth)
                return;
            _width = width;
            _height = height;
            _depth = depth;
            if (_id != gl_texture_t::zero)
                glDeleteTextures(1, &_id);
            glCreateTextures(_type, 1, &_id);
            glTextureStorage3D(_id, _levels, _internal_format, _width, _height, _depth);
            init_handle();
        }

        void texture::resize(int width, int height, samples samples)
        {
            if (width == _width && height == _height && samples == _samples)
                return;
            _width = width;
            _height = height;
            _samples = samples;
            if (_id != gl_texture_t::zero)
                glDeleteTextures(1, &_id);
            glCreateTextures(_type, 1, &_id);
            glTextureStorage2DMultisample(_id, 1 << uint32_t(samples), _internal_format, _width, _height, _fixed_sample_locations);
            init_handle();
        }

        void texture::resize(int width, int height, int depth, samples samples)
        {
            if (width == _width && height == _height && depth == _depth && samples == _samples)
                return;
            _width = width;
            _height = height;
            _depth = depth;
            _samples = samples;
            if (_id != gl_texture_t::zero)
                glDeleteTextures(1, &_id);
            glCreateTextures(_type, 1, &_id);
            glTextureStorage3DMultisample(_id, 1 << uint32_t(samples), _internal_format, _width, _height, _depth, _fixed_sample_locations);
            init_handle();
        }

        void texture::assign(int x, int width, GLenum format, GLenum type, const void* pixels)
        {
            assign(0, x, width, format, type, pixels);
        }

        void texture::assign(int x, int y, int width, int height, GLenum format, GLenum type, const void* pixels)
        {
            assign(0, x, y, width, height, format, type, pixels);
        }

        void texture::assign(int x, int y, int z, int width, int height, int depth, GLenum format, GLenum type, const void* pixels)
        {
            assign(0, x, y, z, width, height, depth, format, type, pixels);
        }

        void texture::assign(int level, int x, int width, GLenum format, GLenum type, const void* pixels)
        {
            glTextureSubImage1D(_id, level, x, width, format, type, pixels);
        }

        void texture::assign(int level, int x, int y, int width, int height, GLenum format, GLenum type, const void* pixels)
        {
            glTextureSubImage2D(_id, level, x, y, width, height, format, type, pixels);
        }

        void texture::assign(int level, int x, int y, int z, int width, int height, int depth, GLenum format, GLenum type, const void* pixels)
        {
            glTextureSubImage3D(_id, level, x, y, z, width, height, depth, format, type, pixels);
        }

        void texture::assign(GLenum format, GLenum type, const void* pixels)
        {
            assign(0, format, type, pixels);
        }

        void texture::assign(int level, GLenum format, GLenum type, const void* pixels)
        {
            assert((_type == GL_TEXTURE_1D ||
                _type == GL_TEXTURE_2D ||
                _type == GL_TEXTURE_3D ||
                _type == GL_TEXTURE_RECTANGLE) && "Detected unsupported texture type for this function.");

            switch (_type)
            {
            case GL_TEXTURE_1D:
                assign(level, 0, _width, format, type, pixels);
                break;
            case GL_TEXTURE_2D:
                assign(level, 0, 0, _width, _height, format, type, pixels);
                break;
            case GL_TEXTURE_RECTANGLE:
                assign(level, 0, 0, _width, _height, format, type, pixels);
                break;
            case GL_TEXTURE_3D:
                assign(level, 0, 0, 0, _width, _height, _depth, format, type, pixels);
                break;
            default: break;
            }
        }

        void texture::get_data(GLenum format, GLenum type, size_t size, void* target) const noexcept
        {
            get_data(0, format, type, size, target);
        }

        void texture::get_data(int level, GLenum format, GLenum type, size_t size, void* target) const noexcept
        {
            glGetTextureImage(_id, level, format, type, static_cast<int>(size), target);
        }

        void texture::set_buffer(gl_buffer_t buffer, GLenum internal_format) const noexcept
        {
            glTextureBuffer(_id, internal_format, buffer);
        }

        void texture::set_buffer(gl_buffer_t buffer, GLenum internal_format, size_t size, size_t offset) const noexcept
        {
            glTextureBufferRange(_id, internal_format, buffer, offset, size);
        }

        void texture::generate_mipmaps() const noexcept
        {
            glGenerateTextureMipmap(_id);
        }

        int texture::width() const noexcept
        {
            return _width;
        }

        int texture::height() const noexcept
        {
            return _height;
        }

        int texture::depth() const noexcept
        {
            return _depth;
        }

        uint64_t texture::handle() const noexcept
        {
            return _handle;
        }

        texture::operator gl_texture_t() const noexcept
        {
            return _id;
        }
    
        void texture::init_handle() noexcept
        {
            if (!glGetTextureSamplerHandleARB)
                _handle = 0;
            else
            {
                _handle = glGetTextureHandleARB(_id);
                if (!glIsTextureHandleResidentARB(_handle))
                    glMakeTextureHandleResidentARB(_handle);
            }
        }
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

    uint64_t sampler::sample(const v2::texture& t) const noexcept
    {
        if (!glGetTextureSamplerHandleARB)
            return 0;
        const auto handle = glGetTextureSamplerHandleARB(t, _id);
        if (!glIsTextureHandleResidentARB(handle))
            glMakeTextureHandleResidentARB(handle);
        return handle;
    }
    
    sampler::operator gl_sampler_t() const noexcept
    {
        return _id;
    }
    
    image::image(const v2::texture& t, const GLenum format, const GLenum access) noexcept
        : image(t, 0, format, access)
    {

    }

    image::image(const v2::texture& t, const int level, const GLenum format, const GLenum access) noexcept
        : _handle(glGetImageHandleARB(t, level, false, 0, format))
    {
        glMakeImageHandleResidentARB(_handle, access);
    }

    image::image(const v2::texture& t, const int level, const int layer, const GLenum format, const GLenum access) noexcept
        : _handle(glGetImageHandleARB(t, level, true, layer, format))
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
}
