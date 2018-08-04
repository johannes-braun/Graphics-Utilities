#include "image_view.hpp"
#include "opengl/image_view_opengl.hpp"
#include "vulkan/image_view_vulkan.hpp"
#include <gfx/context.hpp>

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::image_view_implementation> detail::image_view_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::image_view_implementation>();
	case gapi::vulkan: return std::make_unique<vulkan::image_view_implementation>();
    default: break;
    }
    return nullptr;
}

image_view::image_view(imgv_type type, format format, const device_image& image, uint32_t base_mip, uint32_t mip_count, uint32_t base_layer,
                       uint32_t layer_count)
{
    implementation()->initialize(type, format, image, base_mip, mip_count, base_layer, layer_count);
}

image_view::image_view(imgv_type type, const device_image& image)
      : image_view(type, image.pixel_format(), image, 0, image.levels(), 0,
                   image.dimensions() < 3 ? image.extents().vec[image.dimensions()] : 1)
{}

    image_view::image_view(imgv_type type, const device_image::img_reference& image, uint32_t levels, uint32_t layers)
		: image_view(type, image._img.pixel_format(), image._img, image._level, levels, image._layer, layers)
{
}
}    // namespace v1
}    // namespace gfx
