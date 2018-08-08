#define GFX_EXPOSE_APIS
#include "device_image_opengl.hpp"
#include "formats.hpp"
#include "image_view_opengl.hpp"
#include "graphics_pipeline_opengl.hpp"

namespace gfx {
inline namespace v1 {
namespace opengl {
image_view_implementation::~image_view_implementation()
{
    if (glIsTexture(_handle)) glDeleteTextures(1, &_handle);
}

void image_view_implementation::initialize(imgv_type type, format format, const device_image& image, uint32_t base_mip, uint32_t mip_count,
                                           uint32_t base_layer, uint32_t layer_count)
{
    if (glIsTexture(_handle)) glDeleteTextures(1, &_handle);

    GLenum target = GLenum(0);
    switch (type)
    {
    case imgv_type::image1d: target = GL_TEXTURE_1D; break;
    case imgv_type::image2d: target = image.samples() == sample_count::x1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE; break;
    case imgv_type::image3d: target = GL_TEXTURE_3D; break;
    case imgv_type::image_cube: target = GL_TEXTURE_CUBE_MAP; break;
    case imgv_type::image1d_array: target = GL_TEXTURE_1D_ARRAY; break;
    case imgv_type::image2d_array:
        target = image.samples() == sample_count::x1 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        break;
    case imgv_type::image_cube_array: target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
    default: break;
    }

    glGenTextures(1, &_handle);
    glTextureView(_handle, target, handle_cast<mygl::texture>(image), format_from(format).internal_format, base_mip, mip_count, base_layer,
                  layer_count);
}

handle image_view_implementation::api_handle()
{
    return _handle.get();
}
}    // namespace opengl
}    // namespace v1
}    // namespace gfx