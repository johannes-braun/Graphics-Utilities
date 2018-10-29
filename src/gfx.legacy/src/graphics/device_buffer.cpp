#include <context.hpp>
#include <graphics/device_buffer.hpp>

#include "opengl/device_buffer_opengl.hpp"
#include "vulkan/device_buffer_vulkan.hpp"

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::device_buffer_implementation> detail::device_buffer_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::device_buffer_implementation>();
    case gapi::vulkan: return std::make_unique<vulkan::device_buffer_implementation>();
    default: break;
    }
    return nullptr;
}
}    // namespace v1
}    // namespace gfx
