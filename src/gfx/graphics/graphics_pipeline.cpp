#include "graphics_pipeline.hpp"
#include "../context.hpp"
#include "opengl/graphics_pipeline_opengl.hpp"

namespace gfx
{
    namespace v2
    {
    std::unique_ptr<detail::graphics_pipeline_implementation> detail::graphics_pipeline_implementation::make()
    {
        switch(context::current()->options().graphics_api)
        {
		case gapi::opengl: return std::make_unique<opengl::graphics_pipeline_implementation>();
        case gapi::vulkan: break;
        default: ;
        }
		return nullptr;
    }

    graphics_pipeline::graphics_pipeline(const pipeline_state& state)
    {
        implementation()->initialize(state);
    }
    }
}