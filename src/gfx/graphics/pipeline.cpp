#include "opengl/pipeline_opengl.hpp"
#include "pipeline.hpp"

namespace gfx
{
std::unique_ptr<detail::graphics_pipeline_implementation> detail::make_graphics_pipeline_implementation()
{
    switch (context::current()->options().graphics_api)
    {
    case gapi::opengl: return std::make_unique<opengl::graphics_pipeline_implementation>();
    case gapi::vulkan:
    default: break;
    }
    return nullptr;
}
}