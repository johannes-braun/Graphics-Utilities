#pragma once
#include "device_image.hpp"
#include <gfx/api.hpp>

namespace gfx {
inline namespace v1 {
enum class imgv_type
{
    image1d,
    image2d,
    image3d,
    image_cube,
    image1d_array,
    image2d_array,
    image_cube_array
};

namespace detail {
class image_view_implementation
{
public:
    static std::unique_ptr<image_view_implementation> make();

    virtual ~image_view_implementation()                                   = default;
    virtual void     initialize(imgv_type type, format format, const device_image& image, uint32_t base_mip, uint32_t mip_count,
                                uint32_t base_layer, uint32_t layer_count) = 0;
    virtual std::any api_handle()                                          = 0;
};
}    // namespace detail

GFX_api_cast_type(gapi::opengl, image_view, mygl::texture)

    class image_view : public impl::implements<detail::image_view_implementation>
{
public:
    friend struct api_cast_t<gapi::opengl, image_view>;

    image_view(imgv_type type, format format, const device_image& image, uint32_t base_mip, uint32_t mip_count, uint32_t base_layer,
               uint32_t layer_count);
    image_view(imgv_type type, const device_image& image);
    image_view(imgv_type type, const device_image::img_reference& image, uint32_t levels = 1, uint32_t layers = 1);

    GFX_api_cast_op(gapi::opengl, image_view)
};

GFX_api_cast_impl(gapi::opengl, image_view)
}    // namespace v1
}    // namespace gfx