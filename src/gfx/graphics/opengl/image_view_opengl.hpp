#pragma once

#include "../image_view.hpp"
#include <any>
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
class image_view_implementation : public detail::image_view_implementation
{
public:
    ~image_view_implementation();
    void initialize(imgv_type type, format format, const device_image& image, uint32_t base_mip, uint32_t mip_count, uint32_t base_layer,
                    uint32_t layer_count) override;
    std::any api_handle() override;

private:
    mygl::texture _handle = mygl::texture::zero;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx