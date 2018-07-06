#define GFX_EXPOSE_APIS
#include "../state_info.hpp"
#include "device_image_opengl.hpp"
#include "formats.hpp"
#include <any>
#include <mygl/mygl.hpp>

namespace gfx::opengl
{
std::any device_image_implementation::api_handle() { return _handle; }
void     device_image_implementation::generate_mipmaps() { glGenerateTextureMipmap(_handle); }

void device_image_implementation::initialize(uint32_t layer_dimensions, format format, const extent& size, uint32_t levels,
                                             sample_count samples)
{
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    _format          = format;
    auto x           = format_from(format);
    _internal_format = x.internal_format;
    _external_format = x.external_format;
    _external_type   = x.external_type;
    _extent          = size;
    _levels          = levels;

    switch(layer_dimensions)
    {
    case 1:
    {
        _type = GL_TEXTURE_1D_ARRAY;
        if(glCreateTextures)
        {
            glCreateTextures(_type, 1, &_handle);
            glTextureStorage2D(_handle, _levels, _internal_format, _extent.width, _extent.height);
        }
        else
        {
            glGenTextures(1, &_handle);
            glBindTexture(_type, _handle);
            glTexStorage2D(_type, _levels, _internal_format, _extent.width, _extent.height);
            glBindTexture(_type, mygl::texture::zero);
        }
    }
    break;
    case 2:
    {
        _type = samples == sample_count::x1 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        if(glCreateTextures)
        {
            glCreateTextures(_type, 1, &_handle);
            if(_type == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
                glTextureStorage3DMultisample(
                        _handle, static_cast<int>(samples), _internal_format, _extent.width, _extent.height, _extent.depth, true);
            else
                glTextureStorage3D(_handle, _levels, _internal_format, _extent.width, _extent.height, _extent.depth);
        }
        else
        {
            glGenTextures(1, &_handle);
            glBindTexture(_type, _handle);

            if(_type == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
                glTexStorage3DMultisample(
                        _type, static_cast<int>(samples), _internal_format, _extent.width, _extent.height, _extent.depth, true);
            else
                glTexStorage3D(_type, _levels, _internal_format, _extent.width, _extent.height, _extent.depth);
            glBindTexture(_type, mygl::texture::zero);
        }
    }
    break;
    case 3:
    {
        _type = GL_TEXTURE_3D;
        if(glCreateTextures)
        {
            glCreateTextures(_type, 1, &_handle);
            glTextureStorage3D(_handle, _levels, _internal_format, _extent.width, _extent.height, _extent.depth);
        }
        else
        {
            glGenTextures(1, &_handle);
            glBindTexture(_type, _handle);
            glTexStorage3D(_type, _levels, _internal_format, _extent.width, _extent.height, _extent.depth);
            glBindTexture(_type, mygl::texture::zero);
        }
    }
    break;
    }
}
void device_image_implementation::fill_from(const host_image& image, uint32_t level, uint32_t layer)
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, api_cast<gapi::opengl>(image.storage()));
    switch(_type)
    {
    case GL_TEXTURE_3D:
        if(glTextureSubImage3D)
            glTextureSubImage3D(
                    _handle, level, 0, 0, 0, _extent.width, _extent.height, _extent.depth, _external_format, _external_type, nullptr);
        else
        {
            glBindTexture(_type, _handle);
            glTexSubImage3D(_type, level, 0, 0, 0, _extent.width, _extent.height, _extent.depth, _external_format, _external_type, nullptr);
            glBindTexture(_type, mygl::texture::zero);
        }
    case GL_TEXTURE_2D_ARRAY:
        if(glTextureSubImage3D)
            glTextureSubImage3D(_handle, level, 0, 0, layer, _extent.width, _extent.height, 1, _external_format, _external_type, nullptr);
        else
        {
            glBindTexture(_type, _handle);
            glTexSubImage3D(_type, level, 0, 0, layer, _extent.width, _extent.height, 1, _external_format, _external_type, nullptr);
            glBindTexture(_type, mygl::texture::zero);
        }
    case GL_TEXTURE_1D_ARRAY:
        if(glTextureSubImage3D)
            glTextureSubImage2D(_handle, level, 0, layer, _extent.width, 1, _external_format, _external_type, nullptr);
        else
        {
            glBindTexture(_type, _handle);
            glTexSubImage2D(_type, level, 0, layer, _extent.width, 1, _external_format, _external_type, nullptr);
            glBindTexture(_type, mygl::texture::zero);
        }
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mygl::buffer::zero);
    glFinish();
}

void device_image_implementation::fill_to(const host_image& image, uint32_t level, uint32_t layer)
{
    glBindBuffer(GL_PIXEL_PACK_BUFFER, api_cast<gapi::opengl>(image.storage()));
    glGetTextureSubImage(_handle,
                         level,
                         0,
                         0,
                         0,
                         _extent.width,
                         _extent.height,
                         _extent.depth,
                         _external_format,
                         _external_type,
                         format_element_size(_format) * _extent.count(),
                         nullptr);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, mygl::buffer::zero);
    glFinish();
}
}
