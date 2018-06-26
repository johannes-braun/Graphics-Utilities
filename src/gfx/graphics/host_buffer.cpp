#pragma once

#include <gfx/context.hpp>
#include <gfx/graphics/host_buffer.hpp>
#include "opengl/host_buffer_opengl.hpp"

//#include "host_buffer_vulkan.hpp"

namespace gfx
{
std::unique_ptr<detail::host_buffer_implementation> detail::make_host_buffer_implementation()
{
    switch(context::current()->options().graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::host_buffer_implementation>();
    case gapi::vulkan:
        //return std::make_unique<vulkan::host_buffer_implementation>();
    default:
        break;
    }
    return nullptr;
}
} // namespace gfx