#pragma once

#include "../image_view.hpp"
#include <mygl/mygl.hpp>

namespace gfx::opengl
{
class image_view_implementation : public detail::image_view_implementation
{
public:
    void     initialize(view_type type, img_format format, const device_image& image, uint32_t base_mip, uint32_t mip_count, uint32_t base_layer, uint32_t layer_count) override;
    std::any api_handle() override;

private:
    mygl::texture _handle = mygl::texture::zero;
};
}