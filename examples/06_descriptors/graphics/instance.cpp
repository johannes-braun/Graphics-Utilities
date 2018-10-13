#include "instance.hpp"
#include "info.hpp"
#include "extensions.hpp"

#include <gfx/log.hpp>

namespace gfx
{
    inline namespace v1
    {
		instance::instance(std::string_view app_name, version_t app_version, bool debug, bool surface_support)
			: _capabilities{ debug, surface_support }
		{
			vk::ApplicationInfo app_info;
			app_info.apiVersion = version_t(1, 1, 0);
			app_info.applicationVersion = app_version;
			app_info.engineVersion = detail::engine_version;
			app_info.pEngineName = detail::engine_name.data();
			app_info.pApplicationName = app_name.data();

			std::vector<const char*> extensions;
			if (surface_support)
			{
				extensions.push_back("VK_KHR_surface");
				extensions.push_back("VK_KHR_win32_surface");
			}
			if (debug) extensions.push_back("VK_EXT_debug_utils");

			std::vector<const char*> layers;
			if (debug) layers.push_back("VK_LAYER_LUNARG_standard_validation");

			const vk::InstanceCreateInfo instance_info({}, &app_info, static_cast<u32>(layers.size()), layers.data(),
				static_cast<u32>(extensions.size()), extensions.data());
			_instance = vk::createInstanceUnique(instance_info);
			_dispatcher.load(_instance.get());

			if (debug)
			{
				using sev = vk::DebugUtilsMessageSeverityFlagBitsEXT;
				const vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_info(
					{}, sev::eError | sev::eWarning | sev::eInfo, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
					[](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
						const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32 {
					switch (sev(messageSeverity))
					{
					case sev::eError: gfx::elog << pCallbackData->pMessage; break;
					case sev::eWarning: gfx::wlog << pCallbackData->pMessage; break;
					case sev::eInfo: gfx::ilog << pCallbackData->pMessage; break;
					case sev::eVerbose: gfx::dlog << pCallbackData->pMessage; break;
					}
					return false;
				});
				_debug_messenger = _instance->createDebugUtilsMessengerEXTUnique(debug_messenger_info, nullptr, _dispatcher);
			}
		}

    const extension_dispatch& instance::dispatcher() const noexcept
    {
        return _dispatcher;
    }

    bool instance::is_debug() const noexcept
    {
        return _capabilities.debug;
    }

    bool instance::is_surface_supported() const noexcept
    {
        return _capabilities.surface_support;
    }
    }
}