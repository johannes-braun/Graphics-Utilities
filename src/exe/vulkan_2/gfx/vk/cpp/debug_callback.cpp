#include "../debug_callback.hpp"

namespace gfx::vk
{
    debug_callback::debug_callback(const std::shared_ptr<instance>& instance, VkDebugReportFlagsEXT flags, callback_function function)
        : _instance(instance), _fun(function)
    {
        VkDebugReportCallbackCreateInfoEXT create_info;
        create_info.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        create_info.pNext       = nullptr;
        create_info.pUserData   = this;
        create_info.flags       = flags;
        create_info.pfnCallback = [](VkDebugReportFlagsEXT f, VkDebugReportObjectTypeEXT ot, uint64_t o, size_t l, int32_t m, const char* lp, const char* msg, void* ud) -> VkBool32 {
            return static_cast<debug_callback*>(ud)->_fun(f, ot, o, l, m, lp, msg);
        };

        VkResult(*create_callback)(VkInstance, VkDebugReportCallbackCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugReportCallbackEXT*) = nullptr;
        create_callback     = reinterpret_cast<decltype(create_callback)>(_instance->proc_address("vkCreateDebugReportCallbackEXT"));
        _destroy_callback   = reinterpret_cast<decltype(_destroy_callback)>(_instance->proc_address("vkDestroyDebugReportCallbackEXT"));
        create_callback(*_instance, &create_info, nullptr, &_callback);
    }

    debug_callback::~debug_callback()
    {
        _destroy_callback(*_instance, _callback, nullptr);
    }
}