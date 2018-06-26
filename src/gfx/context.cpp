#include <gfx/context.hpp>
#include "graphics/opengl/context_opengl.hpp"
//#include "context_vulkan.hpp"

namespace gfx::detail
{
std::unique_ptr<context_implementation> make_context_implementation(const gapi api) {
    switch(api)
    {
    case gapi::opengl:
        return std::make_unique<opengl::context_implementation>();
    case gapi::vulkan:
        //return std::make_unique<vulkan::context_implementation>();
    default: break;
    }
    return nullptr;
}
}
