#include "fence.hpp"
#include "opengl/fence_opengl.hpp"
#include "../context.hpp"

namespace gfx
{
    inline namespace v1
    {
		std::unique_ptr<detail::fence_implementation> detail::fence_implementation::make()
		{
		    switch (context::current()->options().graphics_api)
		    {
			case gapi::opengl: return std::make_unique<opengl::fence_implementation>();
		    case gapi::vulkan: break;
		    default: break;
		    }
			return nullptr;
		}
    }
}