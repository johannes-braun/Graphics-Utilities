#include "../texture.hpp"
#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace gl
{
void pixel::set(const glm::vec4& color) noexcept { _color = color; }

const glm::vec4& pixel::get() const noexcept { return _color; }

glm::vec4& pixel::get() noexcept { return _color; }

pixel::operator const glm::vec4&() const noexcept { return get(); }

pixel::operator glm::vec4&() noexcept { return get(); }

pixel& pixel::operator=(const glm::vec4& color) noexcept
{
    set(color);
    return *this;
}

pixel::pixel(const texture* parent, glm::ivec3 position, int level) noexcept
        : _parent(parent)
        , _position(position)
        , _level(level)
{
    assert(_parent->_type != GL_TEXTURE_2D_MULTISAMPLE && _parent->_type != GL_TEXTURE_2D_MULTISAMPLE_ARRAY &&
           "Cannot fetch pixel from multisample textures.");
    glGetTextureSubImage(
            *_parent, _level, _position.x, _position.y, _position.z, 1, 1, 1, GL_RGBA, GL_FLOAT, sizeof(glm::vec4), &_color[0]);
}

pixel::~pixel()
{
    switch(_parent->_type)
    {
    case GL_TEXTURE_1D:
        glTextureSubImage1D(*_parent, _level, _position.x, 1, GL_RGBA, GL_FLOAT, &_color[0]);
        break;
    case GL_TEXTURE_1D_ARRAY:
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_RECTANGLE:
        glTextureSubImage2D(*_parent, _level, _position.x, _position.y, 1, 1, GL_RGBA, GL_FLOAT, &_color[0]);
        break;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_3D:
        glTextureSubImage3D(*_parent, _level, _position.x, _position.y, _position.z, 1, 1, 1, GL_RGBA, GL_FLOAT, &_color[0]);
        break;
    default:
        break;
    }
}

int max_levels(int width, int height, int depth) { return 1 + static_cast<uint32_t>(std::log2(std::max(width, std::max(height, depth)))); }

texture::texture(GLenum type) noexcept
        : _type(type)
{
}

texture::texture(GLenum type, int width, GLenum internal_format, int levels) noexcept
        : texture(type)
{
    _internal_format = internal_format;
    _height          = 1;
    _depth           = 1;
    _levels          = levels == -1 ? max_levels(width, _height, _depth) : levels;
    resize(width);
}

texture::texture(GLenum type, int width, int height, GLenum internal_format, int levels) noexcept
        : texture(type)
{
    _internal_format = internal_format;
    _depth           = 1;
    _levels          = levels == -1 ? max_levels(width, height, _depth) : levels;
    resize(width, height);
}

texture::texture(GLenum type, int width, int height, int depth, GLenum internal_format, int levels) noexcept
        : texture(type)
{
    _internal_format = internal_format;
    _levels          = levels == -1 ? max_levels(width, height, depth) : levels;
    resize(width, height, depth);
}

texture::texture(GLenum type, int width, int height, samples samples, GLenum internal_format, bool fixed_locations) noexcept
        : texture(type)
{
    _internal_format        = internal_format;
    _fixed_sample_locations = fixed_locations;
    resize(width, height, samples);
}

texture::texture(GLenum type, int width, int height, int depth, samples samples, GLenum internal_format, bool fixed_locations) noexcept
        : texture(type)
{
    _internal_format        = internal_format;
    _fixed_sample_locations = fixed_locations;
    resize(width, height, depth, samples);
}

texture::texture(const texture& other) noexcept { operator=(std::forward<const texture&>(other)); }

texture::texture(texture&& other) noexcept { operator=(std::forward<texture&&>(other)); }

texture& texture::operator=(const texture& other) noexcept
{
    if(_id != mygl::texture::zero)
        glDeleteTextures(1, &_id);
    _type                   = other._type;
    _internal_format        = other._internal_format;
    _levels                 = other._levels;
    _fixed_sample_locations = other._fixed_sample_locations;
    _handle                 = other._handle;

    if(other._id == mygl::texture::zero)
    {
        _width   = other._width;
        _height  = other._height;
        _depth   = other._depth;
        _samples = other._samples;
        _id      = mygl::texture::zero;
        return *this;
    }
    else
        glCreateTextures(_type, 1, &_id);

    switch(_type)
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
    default:
        break;
    }

    for(int level = 0; level < max_levels(other._width, other._height, other._depth); ++level)
        glCopyImageSubData(uint32_t(other._id),
                           other._type,
                           level,
                           0,
                           0,
                           0,
                           uint32_t(_id),
                           _type,
                           level,
                           0,
                           0,
                           0,
                           other._width,
                           other._height,
                           other._depth);
    return *this;
}

texture& texture::operator=(texture&& other) noexcept
{
    if(_id != mygl::texture::zero)
        glDeleteTextures(1, &_id);
    _id                     = other._id;
    _type                   = other._type;
    _internal_format        = other._internal_format;
    _levels                 = other._levels;
    _width                  = other._width;
    _height                 = other._height;
    _depth                  = other._depth;
    _fixed_sample_locations = other._fixed_sample_locations;
    _samples                = other._samples;
    _handle                 = other._handle;

    other._id = mygl::texture::zero;
    return *this;
}

texture::~texture()
{
    if(_id != mygl::texture::zero)
        glDeleteTextures(1, &_id);
}

void texture::resize(int width)
{
    if(width == _width)
        return;
    _width = width;
    if(_id != mygl::texture::zero)
        glDeleteTextures(1, &_id);
    glCreateTextures(_type, 1, &_id);
    glTextureStorage1D(_id, _levels, _internal_format, _width);
    init_handle();
}

void texture::resize(int width, int height)
{
    if(width == _width && height == _height)
        return;
    _width  = width;
    _height = height;
    if(_id != mygl::texture::zero)
        glDeleteTextures(1, &_id);
    glCreateTextures(_type, 1, &_id);
    glTextureStorage2D(_id, _levels, _internal_format, _width, _height);
    init_handle();
}

void texture::resize(int width, int height, int depth)
{
    if(width == _width && height == _height && depth == _depth)
        return;
    _width  = width;
    _height = height;
    _depth  = depth;
    if(_id != mygl::texture::zero)
        glDeleteTextures(1, &_id);
    glCreateTextures(_type, 1, &_id);
    glTextureStorage3D(_id, _levels, _internal_format, _width, _height, _depth);
    init_handle();
}

void texture::resize(int width, int height, samples samples)
{
    if(width == _width && height == _height && samples == _samples)
        return;
    _width   = width;
    _height  = height;
    _samples = samples;
    if(_id != mygl::texture::zero)
        glDeleteTextures(1, &_id);
    glCreateTextures(_type, 1, &_id);
    glTextureStorage2DMultisample(_id, 1 << uint32_t(samples), _internal_format, _width, _height, _fixed_sample_locations);
    init_handle();
}

void texture::resize(int width, int height, int depth, samples samples)
{
    if(width == _width && height == _height && depth == _depth && samples == _samples)
        return;
    _width   = width;
    _height  = height;
    _depth   = depth;
    _samples = samples;
    if(_id != mygl::texture::zero)
        glDeleteTextures(1, &_id);
    glCreateTextures(_type, 1, &_id);
    glTextureStorage3DMultisample(_id, 1 << uint32_t(samples), _internal_format, _width, _height, _depth, _fixed_sample_locations);
    init_handle();
}

void texture::assign(int x, int width, GLenum format, GLenum type, const void* pixels) noexcept
{
    assign(0, x, width, format, type, pixels);
}

void texture::assign(int x, int y, int width, int height, GLenum format, GLenum type, const void* pixels) noexcept
{
    assign(0, x, y, width, height, format, type, pixels);
}

void texture::assign(int x, int y, int z, int width, int height, int depth, GLenum format, GLenum type, const void* pixels) noexcept
{
    assign(0, x, y, z, width, height, depth, format, type, pixels);
}

void texture::assign(int level, int x, int width, GLenum format, GLenum type, const void* pixels) noexcept
{
    glTextureSubImage1D(_id, level, x, width, format, type, pixels);
}

void texture::assign(int level, int x, int y, int width, int height, GLenum format, GLenum type, const void* pixels) noexcept
{
    glTextureSubImage2D(_id, level, x, y, width, height, format, type, pixels);
}

void texture::assign(int level, int x, int y, int z, int width, int height, int depth, GLenum format, GLenum type,
                     const void* pixels) noexcept
{
    glTextureSubImage3D(_id, level, x, y, z, width, height, depth, format, type, pixels);
}

void texture::assign(GLenum format, GLenum type, const void* pixels) noexcept { assign(0, format, type, pixels); }

void texture::assign(int level, GLenum format, GLenum type, const void* pixels) noexcept
{
    assert((_type == GL_TEXTURE_1D || _type == GL_TEXTURE_2D || _type == GL_TEXTURE_3D || _type == GL_TEXTURE_RECTANGLE) &&
           "Detected unsupported texture type for this function.");

    switch(_type)
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
    default:
        break;
    }
}

void texture::get_data(GLenum format, GLenum type, size_t size, void* target) const noexcept { get_data(0, format, type, size, target); }

void texture::get_data(int level, GLenum format, GLenum type, size_t size, void* target) const noexcept
{
    glGetTextureImage(_id, level, format, type, static_cast<int>(size), target);
}

void texture::set_buffer(mygl::buffer buffer, GLenum internal_format) const noexcept { glTextureBuffer(_id, internal_format, buffer); }

void texture::set_buffer(mygl::buffer buffer, GLenum internal_format, size_t size, size_t offset) const noexcept
{
    glTextureBufferRange(_id, internal_format, buffer, offset, size);
}

void texture::generate_mipmaps() const noexcept { glGenerateTextureMipmap(_id); }

int texture::width() const noexcept { return _width; }

int texture::height() const noexcept { return _height; }

int texture::depth() const noexcept { return _depth; }

int texture::levels() const noexcept { return _levels; }

uint64_t texture::handle() const noexcept { return _handle; }

GLenum texture::internal_format() const noexcept { return _internal_format; }

texture::operator mygl::texture() const noexcept { return _id; }

mygl::texture texture::id() const noexcept { return _id; }

void texture::bind(uint32_t binding) const noexcept { glBindTextureUnit(binding, _id); }

void texture::bind_image(uint32_t binding) const noexcept { bind_image(binding, GL_READ_WRITE); }

void texture::bind_image(uint32_t binding, GLenum access) const noexcept { bind_image(binding, 0, _internal_format, access); }

void texture::bind_image(uint32_t binding, GLenum format, GLenum access) const noexcept { bind_image(binding, 0, format, access); }

void texture::bind_image(uint32_t binding, int level, GLenum format, GLenum access) const noexcept
{
    glBindImageTexture(binding, _id, level, _depth == 1 ? false : true, 0, access, format);
}

void texture::bind_image(uint32_t binding, int level, int layer, GLenum format, GLenum access) const noexcept
{
    glBindImageTexture(binding, _id, level, true, layer, access, format);
}

pixel texture::at(int x, int level) const { return pixel(this, {x, 0, 0}, level); }

pixel texture::at(int x, int y, int level) const { return pixel(this, {x, y, 0}, level); }

pixel texture::at(int x, int y, int z, int level) const { return pixel(this, {x, y, z}, level); }

void texture::init_handle() noexcept
{
    if(!glGetTextureSamplerHandleARB)
        _handle = 0;
    else
    {
        _handle = glGetTextureHandleARB(_id);
        if(!glIsTextureHandleResidentARB(_handle))
            glMakeTextureHandleResidentARB(_handle);
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

sampler::sampler(const sampler& other) noexcept { operator=(std::forward<const sampler&>(other)); }

sampler::sampler(sampler&& other) noexcept { operator=(std::forward<sampler&&>(other)); }

sampler& sampler::operator=(const sampler& other) noexcept
{
    if(_id != mygl::sampler::zero)
        glDeleteSamplers(1, &_id);
    if(other._id == mygl::sampler::zero)
    {
        _id = mygl::sampler::zero;
        return *this;
    }
    else
        glCreateSamplers(1, &_id);
    for(const auto paramf : _float_parameters)
    {
        float p;
        glGetSamplerParameterfv(other._id, paramf, &p);
        set(paramf, p);
    }
    for(const auto paramf : _int_parameters)
    {
        int p;
        glGetSamplerParameteriv(other._id, paramf, &p);
        set(paramf, p);
    }
    for(const auto paramf : _float_array_parameters)
    {
        float p[4];
        glGetSamplerParameterfv(other._id, paramf, p);
        glSamplerParameterfv(_id, paramf, p);
    }
    return *this;
}

sampler& sampler::operator=(sampler&& other) noexcept
{
    if(_id != mygl::sampler::zero)
        glDeleteSamplers(1, &_id);
    _id       = other._id;
    other._id = mygl::sampler::zero;
    return *this;
}

sampler::~sampler() noexcept
{
    if(_id != mygl::sampler::zero)
        glDeleteSamplers(1, &_id);
}

void sampler::set(const GLenum name, const int value) const noexcept { glSamplerParameteri(_id, name, value); }

void sampler::set(const GLenum name, const float value) const noexcept { glSamplerParameterf(_id, name, value); }

uint64_t sampler::sample(mygl::texture t) const noexcept
{
    if(!glGetTextureSamplerHandleARB)
        return 0;
    const auto handle = glGetTextureSamplerHandleARB(t, _id);
    if(!glIsTextureHandleResidentARB(handle))
        glMakeTextureHandleResidentARB(handle);
    return handle;
}

sampler::operator mygl::sampler() const noexcept { return _id; }

mygl::sampler sampler::id() const noexcept { return _id; }

void sampler::bind(uint32_t binding) const noexcept { glBindSampler(binding, _id); }

image::image(const texture& t, const GLenum format, const GLenum access) noexcept
        : image(t, 0, format, access)
{
}

image::image(const texture& t, const int level, const GLenum format, const GLenum access) noexcept
        : _texture(t)
        , _level(level)
        , _layered(false)
        , _layer(0)
        , _format(format)
        , _access(access)
{
    _handle = (t == mygl::texture::zero ? 0 : glGetImageHandleARB(t, level, false, 0, format));
    if(_handle)
        glMakeImageHandleResidentARB(_handle, access);
}

image::image(const texture& t, const int level, const int layer, const GLenum format, const GLenum access) noexcept
        : _handle(glGetImageHandleARB(t, level, true, layer, format))
        , _texture(t)
        , _level(level)
        , _layered(true)
        , _layer(layer)
        , _format(format)
        , _access(access)
{
    _handle = (t == mygl::texture::zero ? 0 : glGetImageHandleARB(t, level, true, layer, format));
    if(_handle)
        glMakeImageHandleResidentARB(_handle, access);
}

void image::bind(uint32_t binding) const noexcept { glBindImageTexture(binding, _texture, _level, _layered, _layer, _access, _format); }

image::~image() noexcept
{
    if(_handle)
        glMakeImageHandleNonResidentARB(_handle);
}

uint64_t image::handle() const noexcept { return _handle; }
}
