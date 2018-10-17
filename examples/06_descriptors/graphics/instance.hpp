#pragma once

#include <vulkan/vulkan.hpp>
#include "version.hpp"
#include "extensions.hpp"

namespace gfx
{
	inline namespace v1
	{
		class instance
		{
		public:
			explicit instance(std::string_view app_name, version_t app_version, bool debug, bool surface_support);
			~instance() = default;

			vk::Instance          inst() const noexcept { return _instance.get(); }
		    const extension_dispatch& dispatcher() const noexcept;

		    bool is_debug() const noexcept;
		    bool is_surface_supported() const noexcept;

		private:
			vk::UniqueInstance                                           _instance;
			vk::UniqueHandle<vk::DebugUtilsMessengerEXT, extension_dispatch> _debug_messenger;
			extension_dispatch                                               _dispatcher;
			vk::UniqueHandle<vk::DebugReportCallbackEXT, extension_dispatch> _debug_callback;

			struct
			{
				bool debug = false;
				bool surface_support = false;
			} _capabilities;
		};

	}
}