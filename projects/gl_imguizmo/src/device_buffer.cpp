#include "device_buffer.hpp"
#include "context.hpp"

#include "device_buffer_opengl.hpp"

namespace gfx
{
std::unique_ptr<detail::device_buffer_implementation> detail::make_device_buffer_implementation()
{
    switch(context::current()->graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::device_buffer_implementation>();
    case gapi::vulkan:
        break;
    default:
        break;
    }
    return nullptr;
}
} // namespace gfx
