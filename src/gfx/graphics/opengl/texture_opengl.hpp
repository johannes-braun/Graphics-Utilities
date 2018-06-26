#pragma once

#include "texture.hpp"
#include <mygl/mygl.hpp>

namespace gfx::opengl
{
class texture_implementation : public detail::texture_implementation
{
    gli::gl         gli_gl{gli::gl::PROFILE_GL33};
    mygl::texture   _handle;
    gli::gl::format _gl_format;
    gli::format     _format;
    gli::extent3d   _extent;
    texture_type    _type;

public:
    void upload(const gli::texture& image, uint32_t layer) override
    {
        assert(image.extent(0) == _extent);
        gli::gl::format transformed = gli_gl.translate(
                image.format(), gli::swizzles(gli::SWIZZLE_RED, gli::SWIZZLE_GREEN, gli::SWIZZLE_BLUE, gli::SWIZZLE_ALPHA));
        switch(_type)
        {
        case texture_type::tex1d:
            glTextureSubImage2D(_handle,
                                0,
                                0,
                                layer,
                                _extent.x,
                                1,
                                static_cast<GLenum>(transformed.External),
                                static_cast<GLenum>(transformed.Type),
                                image.data());
            break;
        case texture_type::tex2d:
            glTextureSubImage3D(_handle,
                                0,
                                0,
                                0,
                                layer,
                                _extent.x,
                                _extent.y,
                                1,
                                static_cast<GLenum>(transformed.External),
                                static_cast<GLenum>(transformed.Type),
                                image.data());
            break;
        case texture_type::tex3d:
            glTextureSubImage3D(_handle,
                                0,
                                0,
                                0,
                                0,
                                _extent.x,
                                _extent.y,
                                _extent.z,
                                static_cast<GLenum>(transformed.External),
                                static_cast<GLenum>(transformed.Type),
                                image.data());
            break;
        default:
            break;
        }
    }

    void resize(texture_type type, gli::extent3d size, gli::format format, uint32_t levels) override
    {
        _format    = format;
        _gl_format = gli_gl.translate(format, gli::swizzles(gli::SWIZZLE_RED, gli::SWIZZLE_GREEN, gli::SWIZZLE_BLUE, gli::SWIZZLE_ALPHA));

        switch(type)
        {
        case texture_type::tex1d:
            glCreateTextures(GL_TEXTURE_1D_ARRAY, 1, &_handle);
            glTextureStorage2D(_handle, levels, static_cast<GLenum>(_gl_format.Internal), size.x, size.y);
            break;
        case texture_type::tex2d:
            glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &_handle);
            glTextureStorage3D(_handle, levels, static_cast<GLenum>(_gl_format.Internal), size.x, size.y, size.z);
            break;
        case texture_type::tex3d:
            glCreateTextures(GL_TEXTURE_3D, 1, &_handle);
            glTextureStorage3D(_handle, levels, static_cast<GLenum>(_gl_format.Internal), size.x, size.y, size.z);
            break;
        default:
            break;
        }
    }
};
}