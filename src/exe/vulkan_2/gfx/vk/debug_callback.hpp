#pragma once

#include "instance.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <functional>
#include <memory>

namespace gfx::vk
{
    class debug_callback
    {
    public:
        using callback_function = std::function<bool(VkDebugReportFlagsEXT, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char*, const char*)>;
        debug_callback(const std::shared_ptr<instance>& instance, VkDebugReportFlagsEXT flags, callback_function function);
        ~debug_callback();

    private:
        std::shared_ptr<instance> _instance;
        callback_function _fun;
        VkDebugReportCallbackEXT _callback;
        VkResult(*_destroy_callback)(VkInstance, VkDebugReportCallbackEXT, const VkAllocationCallbacks*) = nullptr;
    };
}