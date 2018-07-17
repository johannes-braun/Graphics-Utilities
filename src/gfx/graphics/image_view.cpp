#include "image_view.hpp"
#include <gfx/context.hpp>
#include "opengl/image_view_opengl.hpp"

namespace gfx
{
std::unique_ptr<detail::image_view_implementation> detail::image_view_implementation::make()
{
    switch(context::current()->options().graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::image_view_implementation>();
    case gapi::vulkan: break;
    default:
        break;
    }
    return nullptr;
}

image_view::image_view(imgv_type type, format         format, const device_image& image, uint32_t base_mip, uint32_t mip_count,
                       uint32_t  base_layer, uint32_t layer_count)
{
    implementation()->initialize(type, format, image, base_mip, mip_count, base_layer, layer_count);
}
}
