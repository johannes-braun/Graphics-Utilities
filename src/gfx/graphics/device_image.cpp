#include "host_image.hpp"
#include "opengl/device_image_opengl.hpp"
#include <gfx/context.hpp>

namespace gfx
{
std::unique_ptr<detail::device_image_implementation> detail::make_device_image_implementation()
{
    switch(context::current()->options().graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::device_image_implementation>();
    case gapi::vulkan:
        // return std::make_unique<vulkan::texture_implementation>();
    default:
        break;
    }
    return nullptr;
}
void device_image::img_reference::operator<<(const host_image& image) const { _img._implementation->fill_from(image, _level, _layer); }

void device_image::generate_mipmaps()
{ _implementation->generate_mipmaps(); }

device_image::img_reference::img_reference(uint32_t level, uint32_t layer, device_image& img)
        : _level(level)
        , _layer(layer)
        , _img(img)
{
}

device_image::device_image(uint32_t layer_dimensions, format format, const extent& size, uint32_t levels)
        : _implementation(detail::make_device_image_implementation())
{
    _implementation->initialize(layer_dimensions, format, size, levels);
}

device_image::img_reference device_image::operator[](uint32_t layer) { return img_reference(0, layer, *this); }
device_image::img_reference               device_image::level(uint32_t level) { return img_reference(level, 0, *this); }
device_image::img_reference               device_image::layer(uint32_t layer) { return img_reference(0, layer, *this); }
device_image::img_reference device_image::sub_image(uint32_t level, uint32_t layer) { return img_reference(level, layer, *this); }
} // namespace gfx