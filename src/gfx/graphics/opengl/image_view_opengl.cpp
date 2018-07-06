#define GFX_EXPOSE_APIS
#include "device_image_opengl.hpp"
#include "image_view_opengl.hpp"
#include "formats.hpp"

namespace gfx::opengl
{
void image_view_implementation::initialize(imgv_type type, format format, const device_image& image, uint32_t base_mip,
                                           uint32_t mip_count, uint32_t base_layer, uint32_t layer_count)
{
    if(glIsTexture(_handle))
        glDeleteTextures(1, &_handle);

    GLenum target = GLenum(0);
    switch(type)
    {
    case imgv_type::image_1d:
        target = GL_TEXTURE_1D;
        break;
    case imgv_type::image_2d:
        target = GL_TEXTURE_2D;
        break;
    case imgv_type::image_3d:
        target = GL_TEXTURE_3D;
        break;
    case imgv_type::image_cube:
        target = GL_TEXTURE_CUBE_MAP;
        break;
    case imgv_type::image_1d_array:
        target = GL_TEXTURE_1D_ARRAY;
        break;
    case imgv_type::image_2d_array:
        target = GL_TEXTURE_2D_ARRAY;
        break;
    case imgv_type::image_cube_array:
        target = GL_TEXTURE_CUBE_MAP_ARRAY;
        break;
    default:
        break;
    }

    glGenTextures(1, &_handle);
    glTextureView(_handle,
                  target,
                  api_cast<gapi::opengl>(image),
                  format_from(format).internal_format,
                  base_mip,
                  mip_count,
                  base_layer,
                  layer_count);
}

std::any image_view_implementation::api_handle() { return _handle; }
}
