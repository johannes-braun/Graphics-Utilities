#pragma once

#include <graphics/image_view.hpp>
#include "../general/handle.hpp"
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
    handle api_handle() override;

private:
    movable_handle<mygl::texture> _handle = mygl::texture{ 0 };
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx