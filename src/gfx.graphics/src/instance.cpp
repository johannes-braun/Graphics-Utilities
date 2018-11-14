#include "instance.hpp"
#include "extensions.hpp"
#include "info.hpp"

#include <gfx.core/log.hpp>
#include <unordered_set>

namespace gfx {
inline namespace v1 {
instance::instance(std::string_view app_name, version_t app_version, bool debug, bool surface_support, vk::ArrayProxy<const char* const> additional_extensions)
      : _app_name(app_name), _app_version(app_version), _capabilities{debug, surface_support}
{
	initialize(app_name, app_version, debug, surface_support, additional_extensions);
}
void instance::initialize(std::string_view app_name, version_t app_version, bool debug, bool surface_support, vk::ArrayProxy<const char* const> additional_extensions)
{
    vk::ApplicationInfo app_info;
    app_info.apiVersion         = version_t(1, 1, 0);
    app_info.applicationVersion = app_version;
    app_info.engineVersion      = detail::engine_version;
    app_info.pEngineName        = detail::engine_name.data();
    app_info.pApplicationName   = app_name.data();

	std::unordered_set<std::string_view> extensions(additional_extensions.begin(), additional_extensions.end());
    if (surface_support)
    {
		extensions.emplace("VK_KHR_surface");
		extensions.emplace("VK_KHR_win32_surface");
    }
    if (debug)
    {
		extensions.emplace("VK_EXT_debug_utils");
		extensions.emplace("VK_EXT_debug_report");
    }
	_extensions.clear();
	_extensions.reserve(extensions.size());
	for (const auto& s : extensions)
		_extensions.push_back(s.data());

    std::vector<const char*> layers;
    if (debug) layers.push_back("VK_LAYER_LUNARG_standard_validation");

    const vk::InstanceCreateInfo instance_info({}, &app_info, static_cast<u32>(layers.size()), layers.data(),
                                               static_cast<u32>(_extensions.size()), _extensions.data());
    _instance = vk::createInstanceUnique(instance_info);
    _dispatcher.load(_instance.get());

    if (debug)
    {
        using dcf = vk::DebugReportFlagBitsEXT;
        const vk::DebugReportCallbackCreateInfoEXT callback_info(
            dcf::eWarning | dcf::eDebug | dcf::eInformation | dcf::ePerformanceWarning | dcf::eError,
            [](VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode,
               const char* pLayerPrefix, const char* pMessage, void* pUserData) -> VkBool32 {
                switch (dcf(flags))
                {
                case dcf::eError: gfx::elog << pMessage; break;
                case dcf::eWarning: gfx::wlog << pMessage; break;
                case dcf::ePerformanceWarning: gfx::wlog << pMessage; break;
                case dcf::eInformation: gfx::ilog << pMessage; break;
                case dcf::eDebug: gfx::dlog << pMessage; break;
                }
                return true;
            });
        _debug_callback = _instance->createDebugReportCallbackEXTUnique(callback_info, nullptr, _dispatcher);

        // using sev = vk::DebugUtilsMessageSeverityFlagBitsEXT;
        // const vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_info(
        //    {}, sev::eError | sev::eWarning | sev::eInfo, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
        //    [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
        //       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32 {
        //        switch (sev(messageSeverity))
        //        {
        //        case sev::eError: gfx::elog << pCallbackData->pMessage; break;
        //        case sev::eWarning: gfx::wlog << pCallbackData->pMessage; break;
        //        case sev::eInfo: gfx::ilog << pCallbackData->pMessage; break;
        //        case sev::eVerbose: gfx::dlog << pCallbackData->pMessage; break;
        //        }
        //        return true;
        //    });
        //_debug_messenger = _instance->createDebugUtilsMessengerEXTUnique(debug_messenger_info, nullptr, _dispatcher);
    }
}

instance::instance(const instance& other)
      : instance(other._app_name, other._app_version, other._capabilities.debug, other._capabilities.surface_support)
{}

instance::instance(instance&& other) noexcept
{
    _app_name        = std::move(other._app_name);
    _app_version     = std::move(other._app_version);
    _capabilities    = std::move(other._capabilities);
    _debug_callback  = std::move(other._debug_callback);
    _debug_messenger = std::move(other._debug_messenger);
    _dispatcher      = std::move(other._dispatcher);
    _instance        = std::move(other._instance);

    other._app_version                  = {0, 0, 0};
    other._capabilities.debug           = false;
    other._capabilities.surface_support = false;
}

instance& instance::operator=(const instance& other)
{
	initialize(other._app_name, other._app_version, other._capabilities.debug, other._capabilities.surface_support, other._extensions);
	return *this;
}

instance& instance::operator=(instance&& other) noexcept
{
    _app_name        = std::move(other._app_name);
    _app_version     = std::move(other._app_version);
    _capabilities    = std::move(other._capabilities);
    _debug_callback  = std::move(other._debug_callback);
    _debug_messenger = std::move(other._debug_messenger);
    _dispatcher      = std::move(other._dispatcher);
    _instance        = std::move(other._instance);
	_extensions		 = std::move(other._extensions);

    other._app_version                  = {0, 0, 0};
    other._capabilities.debug           = false;
    other._capabilities.surface_support = false;
    return *this;
}

const vk::Instance& instance::get_instance() const noexcept
{
    return _instance.get();
}

const extension_dispatch& instance::get_dispatcher() const noexcept
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
}    // namespace v1
}    // namespace gfx