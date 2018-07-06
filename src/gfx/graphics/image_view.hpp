#pragma once
#include "device_image.hpp"
#include <gfx/api.hpp>

namespace gfx
{
enum class imgv_type
{
    image_1d,
    image_2d,
    image_3d,
    image_cube,
    image_1d_array,
    image_2d_array,
    image_cube_array
};

namespace detail
{
    class image_view_implementation
    {
    public:
        virtual ~image_view_implementation()                                   = default;
        virtual void     initialize(imgv_type type, format format, const device_image& image, uint32_t base_mip, uint32_t mip_count,
                                    uint32_t base_layer, uint32_t layer_count) = 0;
        virtual std::any api_handle()                                          = 0;
    };

    std::unique_ptr<image_view_implementation> make_image_view_implementation();
}

GFX_api_cast_type(gapi::opengl, image_view, mygl::texture)

class image_view
{
public:
    friend struct api_cast_t<gapi::opengl, image_view>;

    image_view(imgv_type type, format format, const device_image& image, uint32_t base_mip, uint32_t mip_count, uint32_t base_layer,
               uint32_t layer_count)
            : _implementation(detail::make_image_view_implementation())
    {
        _implementation->initialize(type, format, image, base_mip, mip_count, base_layer, layer_count);
    }

GFX_api_cast_op(gapi::opengl, image_view)

private:
    std::unique_ptr<detail::image_view_implementation> _implementation;
};

GFX_api_cast_impl(gapi::opengl, image_view)
}