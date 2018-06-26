#include "texture.hpp"
#include <gfx/context.hpp>
#include "opengl/texture_opengl.hpp"

namespace gfx
{
std::unique_ptr<detail::texture_implementation> detail::make_texture_implementation()
{
    switch(context::current()->options().graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::texture_implementation>();
    case gapi::vulkan:
        // return std::make_unique<vulkan::texture_implementation>();
    default:
        break;
    }
    return nullptr;
}
}