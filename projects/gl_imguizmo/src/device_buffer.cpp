#include "device_buffer.hpp"
#include "context.hpp"

#include "device_buffer_opengl.hpp"
#include "device_buffer_vulkan.hpp"

namespace gfx
{
std::unique_ptr<detail::device_buffer_implementation> detail::make_device_buffer_implementation()
{
    switch(context::current()->graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::device_buffer_implementation>();
    case gapi::vulkan:
        return std::make_unique<vulkan::device_buffer_implementation>();
    default:
        break;
    }
    return nullptr;
}
} // namespace gfx
