#include "command_list.hpp"
#include "opengl/command_list_opengl.hpp"
#include "vulkan/command_list_vulkan.hpp"
#include <gfx/context.hpp>

namespace gfx
{
	namespace v2
	{
		std::unique_ptr<detail::commands_implementation> detail::commands_implementation::make()
		{
			switch (context::current()->options().graphics_api)
			{
			case gapi::opengl: return std::make_unique<opengl::commands_implementation>();
			case gapi::vulkan: return std::make_unique<vulkan::commands_implementation>();
			}
			return nullptr;
		}
	}
}