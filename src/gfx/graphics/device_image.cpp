#include "host_image.hpp"
#include "opengl/device_image_opengl.hpp"
#include "state_info.hpp"
#include <gfx/context.hpp>

namespace gfx
{
std::unique_ptr<detail::device_image_implementation> detail::device_image_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::device_image_implementation>();
    case gapi::vulkan:
        // return std::make_unique<vulkan::texture_implementation>();
    default: break;
    }
    return nullptr;
}
void device_image::img_reference::operator<<(const host_image& image) const
{
    _img._implementation->fill_from(image, _level, _layer);
}
void device_image::img_reference::operator>>(const host_image& image) const
{
    _img._implementation->fill_to(image, _level, _layer);
}

void device_image::generate_mipmaps()
{
    _implementation->generate_mipmaps();
}

const uint32_t& device_image::dimensions() const noexcept
{
    return _layer_dimensions;
}

const format& device_image::pixel_format() const noexcept
{
    return _format;
}

const extent& device_image::extents() const noexcept
{
    return _extent;
}

const uint32_t& device_image::levels() const noexcept
{
    return _levels;
}

device_image::img_reference::img_reference(uint32_t level, uint32_t layer, device_image& img) : _level(level), _layer(layer), _img(img) {}

device_image::device_image(img_type type, format format, const extent& size, uint32_t levels)
      : _layer_dimensions(static_cast<uint32_t>(type)), _format(format), _extent(size), _levels(levels), _samples(sample_count::x1)
{
    _implementation->initialize(_layer_dimensions, format, size, levels, _samples);
}

device_image::device_image(img_type type, format format, const extent& size, sample_count samples)
      : _layer_dimensions(static_cast<uint32_t>(type)), _format(format), _extent(size), _levels(0), _samples(samples)
{
    _implementation->initialize(_layer_dimensions, format, size, _levels, _samples);
}

device_image::device_image(const host_image& image, uint32_t levels)
      : device_image(img_type::image2d, image.pixel_format(), image.extents(), levels == max_levels ? image.max_levels() : levels)
{
    level(0) << image;
    generate_mipmaps();
}

device_image::img_reference device_image::operator[](uint32_t layer)
{
    return img_reference(0, layer, *this);
}
device_image::img_reference device_image::level(uint32_t level)
{
    return img_reference(level, 0, *this);
}
device_image::img_reference device_image::layer(uint32_t layer)
{
    return img_reference(0, layer, *this);
}
device_image::img_reference device_image::sub_image(uint32_t level, uint32_t layer)
{
    return img_reference(level, layer, *this);
}
}    // namespace gfx