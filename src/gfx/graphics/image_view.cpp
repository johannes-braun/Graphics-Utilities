#include "image_view.hpp"
#include <gfx/context.hpp>
#include "opengl/image_view_opengl.hpp"

namespace gfx
{
std::unique_ptr<detail::image_view_implementation> detail::make_image_view_implementation()
{
    switch(context::current()->options().graphics_api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::image_view_implementation>();
    case gapi::vulkan: break;
    default:
        break;
    }
    return nullptr;
}
}
