#include "context_vulkan.hpp"
#include <gfx/log.hpp>

PFN_vkCreateDebugReportCallbackEXT  vkCreateDebugReportCallbackEXT_impl;
PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT_impl;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
    return vkCreateDebugReportCallbackEXT_impl(instance, pCreateInfo, pAllocator, pCallback);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT pCallback,
                                                           const VkAllocationCallbacks* pAllocator)
{
    return vkDestroyDebugReportCallbackEXT_impl(instance, pCallback, pAllocator);
}

namespace gfx::vulkan
{
vk::UniqueInstance create_instance()
{
    vk::ApplicationInfo app_info;
    app_info.apiVersion         = VK_API_VERSION_1_0;
    app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.engineVersion      = VK_MAKE_VERSION(0, 1, 0);
    app_info.pApplicationName   = "Vulkan Multidraw";
    app_info.pEngineName        = "Graphics Utilities";

    vk::InstanceCreateInfo instance_info;
    instance_info.pApplicationInfo = &app_info;

    uint32_t     count;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);

    std::vector<const char*> extensions = {instance_extensions.begin(), instance_extensions.end()};
    extensions.insert(extensions.end(), glfw_extensions, glfw_extensions + count);

    instance_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();

    instance_info.enabledLayerCount   = static_cast<uint32_t>(layers.size());
    instance_info.ppEnabledLayerNames = layers.data();

    return vk::createInstanceUnique(instance_info);
}

vk::UniqueDebugReportCallbackEXT create_debug_callback(const vk::UniqueInstance& instance, vk::DebugReportFlagsEXT flags)
{
    vk::DebugReportCallbackCreateInfoEXT info;
    info.pUserData   = nullptr;
    info.flags       = flags;
    info.pfnCallback = [](VkDebugReportFlagsEXT      f,
                          VkDebugReportObjectTypeEXT ot,
                          uint64_t                   o,
                          size_t                     l,
                          int32_t                    m,
                          const char*                lp,
                          const char*                msg,
                          void*                      ud) -> VkBool32 {
        switch(f)
        {
        case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
            gfx::clogi << msg;
            break;
        case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
            gfx::clogd << msg;
            break;
        case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
            gfx::clogw << msg;
            break;
        case VK_DEBUG_REPORT_ERROR_BIT_EXT:
            gfx::cloge << msg;
            break;
        default:
            break;
        }
        return true;
    };

    // vkCmdDrawIndexedIndirectCountKHR_impl =
    //        reinterpret_cast<decltype(vkCmdDrawIndexedIndirectCountKHR_impl)>(instance->getProcAddr("vkCmdDrawIndexedIndirectCountKHR"));

    vkCreateDebugReportCallbackEXT_impl =
            reinterpret_cast<decltype(vkCreateDebugReportCallbackEXT_impl)>(instance->getProcAddr("vkCreateDebugReportCallbackEXT"));
    vkDestroyDebugReportCallbackEXT_impl =
            reinterpret_cast<decltype(vkDestroyDebugReportCallbackEXT_impl)>(instance->getProcAddr("vkDestroyDebugReportCallbackEXT"));
    return instance->createDebugReportCallbackEXTUnique(info);
}

struct device_infos
{
    vk::UniqueDevice        device;
    std::array<uint32_t, 4> queue_families;
    std::array<uint32_t, 4> queue_indices;
};
device_infos create_device(const vk::UniqueInstance& instance, const vk::PhysicalDevice& gpu, const vk::UniqueSurfaceKHR& surface)
{
    struct queue_infos
    {
        std::array<uint32_t, 4> families;
        std::array<float, 4>    priorities;
        std::array<uint32_t, 4> family_indices;
    } queues;
    std::vector<vk::QueueFamilyProperties> queue_family_properties = gpu.getQueueFamilyProperties();
    for(uint32_t family = 0; family < queue_family_properties.size(); ++family)
    {
        if((queue_family_properties[family].queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics)
            queues.families[fam::graphics] = family;
        if((queue_family_properties[family].queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
            queues.families[fam::compute] = family;
        if((queue_family_properties[family].queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)
            queues.families[fam::transfer] = family;

        const bool supports = gpu.getSurfaceSupportKHR(family, *surface);
        if(glfwGetPhysicalDevicePresentationSupport(static_cast<VkInstance>(*instance), static_cast<VkPhysicalDevice>(gpu), family) &&
           supports)
            queues.families[fam::present] = family;
    }
    queues.priorities[fam::graphics] = 1.f;
    queues.priorities[fam::compute]  = 1.f;
    queues.priorities[fam::transfer] = 0.5f;
    queues.priorities[fam::present]  = 0.8f;

    std::unordered_map<uint32_t, std::vector<float>> queue_filter;
    for(int i = 0; i < 4; ++i)
    {
        auto&& vec               = queue_filter[queues.families[i]];
        queues.family_indices[i] = static_cast<uint32_t>(vec.size());
        vec.push_back(queues.priorities[i]);
    }

    std::vector<vk::DeviceQueueCreateInfo> queue_infos;
    for(const auto& filter : queue_filter)
    {
        vk::DeviceQueueCreateInfo& info = queue_infos.emplace_back();
        info.queueFamilyIndex           = filter.first;
        info.queueCount                 = static_cast<uint32_t>(filter.second.size());
        info.pQueuePriorities           = filter.second.data();
    }
    vk::DeviceCreateInfo device_info;
    device_info.enabledExtensionCount   = static_cast<uint32_t>(device_extensions.size());
    device_info.ppEnabledExtensionNames = device_extensions.data();
    device_info.enabledLayerCount       = static_cast<uint32_t>(layers.size());
    device_info.ppEnabledLayerNames     = layers.data();
    device_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_infos.size());
    device_info.pQueueCreateInfos       = queue_infos.data();

    vk::PhysicalDeviceFeatures features = gpu.getFeatures();
    features.multiDrawIndirect          = true;
    device_info.pEnabledFeatures        = &features;

    device_infos result;
    result.device         = gpu.createDeviceUnique(device_info);
    result.queue_families = queues.families;
    result.queue_indices  = queues.family_indices;
    return result;
}

vk::PhysicalDevice select_gpu(const vk::UniqueInstance& instance) { return instance->enumeratePhysicalDevices()[0]; }

void context_implementation::initialize(GLFWwindow* window)
{
    _instance       = create_instance();
    _debug_callback = create_debug_callback(_instance, vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning);
    _gpu            = select_gpu(_instance);

    vk::SurfaceKHR surf;
    glfwCreateWindowSurface(static_cast<VkInstance>(*_instance), window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surf));
    _surface = vk::UniqueSurfaceKHR(surf, vk::UniqueHandleTraits<vk::SurfaceKHR>::deleter(*_instance));

    auto infos             = create_device(_instance, _gpu, _surface);
    _device                = std::move(infos.device);
    _families              = std::move(infos.queue_families);
    _queues[fam::graphics] = _device->getQueue(_families[fam::graphics], infos.queue_indices[fam::graphics]);
    _queues[fam::compute]  = _device->getQueue(_families[fam::compute], infos.queue_indices[fam::compute]);
    _queues[fam::transfer] = _device->getQueue(_families[fam::transfer], infos.queue_indices[fam::transfer]);
    _queues[fam::present]  = _device->getQueue(_families[fam::present], infos.queue_indices[fam::present]);

    vk::CommandPoolCreateInfo pool_info;
    pool_info.flags              = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    pool_info.queueFamilyIndex    = _families[fam::graphics];
    _command_pools[fam::graphics] = _device->createCommandPoolUnique(pool_info);
    pool_info.queueFamilyIndex    = _families[fam::compute];
    _command_pools[fam::compute]  = _device->createCommandPoolUnique(pool_info);
    pool_info.queueFamilyIndex    = _families[fam::transfer];
    _command_pools[fam::transfer] = _device->createCommandPoolUnique(pool_info);
}

void context_implementation::make_current(GLFWwindow* window) {}
} // namespace gfx::vulkan
