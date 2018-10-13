#include "extensions.hpp"

namespace gfx
{
    inline namespace v1
    {
		void extension_dispatch::load(vk::Instance instance)
		{
#define load_fun(Name) Name = reinterpret_cast<PFN_##Name>(instance.getProcAddr(#Name))

			load_fun(vkCreateDebugUtilsMessengerEXT);
			load_fun(vkDestroyDebugUtilsMessengerEXT);

#undef load_fun
		}

		void extension_dispatch::load(vk::Device device)
		{
#define load_fun(Name) Name = reinterpret_cast<PFN_##Name>(device.getProcAddr(#Name))

			load_fun(vkCreateSwapchainKHR);
			load_fun(vkDestroySwapchainKHR);

#undef load_fun
		}
    }
}