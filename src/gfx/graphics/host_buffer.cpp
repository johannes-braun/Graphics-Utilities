#include "opengl/host_buffer_opengl.hpp"
#include "vulkan/host_buffer_vulkan.hpp"
#include <gfx/context.hpp>
#include <gfx/graphics/host_buffer.hpp>

namespace gfx {
inline namespace v1 {
std::unique_ptr<detail::host_buffer_implementation> detail::host_buffer_implementation::make()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::host_buffer_implementation>();
    case gapi::vulkan: return std::make_unique<vulkan::host_buffer_implementation>();
    default: break;
    }
    return nullptr;
}
}    // namespace v1
}    // namespace gfx