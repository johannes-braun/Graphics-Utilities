#pragma once

#include "context.hpp"
#include "host_buffer_opengl.hpp"

namespace gfx
{
std::unique_ptr<detail::host_buffer_implementation> detail::make_host_buffer_implementation()
{
    switch(context::current()->graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::host_buffer_implementation>();
    case gapi::vulkan:
        break;
    default:
        break;
    }
    return nullptr;
}
} // namespace gfx