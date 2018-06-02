#include "init.hpp"
#include <GLFW/glfw3.h>
#include <gfx/log.hpp>
#include <vulkan/vulkan_win32.h>

//PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR_impl;
//VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirectCountKHR(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
//{
//    vkCmdDrawIndexedIndirectCountKHR_impl(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
//}

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
            tlog_i("VK Debug") << msg;
            break;
        case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
            tlog_d("VK Debug") << msg;
            break;
        case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
            tlog_w("VK Debug") << msg;
            break;
        case VK_DEBUG_REPORT_ERROR_BIT_EXT:
            tlog_e("VK Debug") << msg;
            break;
        default:
            break;
        }
        return true;
    };

    //vkCmdDrawIndexedIndirectCountKHR_impl =
    //        reinterpret_cast<decltype(vkCmdDrawIndexedIndirectCountKHR_impl)>(instance->getProcAddr("vkCmdDrawIndexedIndirectCountKHR"));

    vkCreateDebugReportCallbackEXT_impl =
            reinterpret_cast<decltype(vkCreateDebugReportCallbackEXT_impl)>(instance->getProcAddr("vkCreateDebugReportCallbackEXT"));
    vkDestroyDebugReportCallbackEXT_impl =
            reinterpret_cast<decltype(vkDestroyDebugReportCallbackEXT_impl)>(instance->getProcAddr("vkDestroyDebugReportCallbackEXT"));
    return instance->createDebugReportCallbackEXTUnique(info);
}

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
            queues.families[graphics] = family;
        if((queue_family_properties[family].queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
            queues.families[compute] = family;
        if((queue_family_properties[family].queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)
            queues.families[transfer] = family;

        const bool supports = gpu.getSurfaceSupportKHR(family, *surface);
        if(glfwGetPhysicalDevicePresentationSupport(static_cast<VkInstance>(*instance), static_cast<VkPhysicalDevice>(gpu), family) &&
           supports)
            queues.families[present] = family;
    }
    queues.priorities[graphics] = 1.f;
    queues.priorities[compute]  = 1.f;
    queues.priorities[transfer] = 0.5f;
    queues.priorities[present]  = 0.8f;

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

vk::UniqueSwapchainKHR create_swapchain(const vk::PhysicalDevice& gpu, const vk::UniqueDevice& device, const vk::UniqueSurfaceKHR& surface,
                                        uint32_t family, const vk::Extent2D& extent)
{
    vk::SwapchainCreateInfoKHR swapchain_info;
    swapchain_info.clipped               = true;
    swapchain_info.compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchain_info.imageArrayLayers      = 1;
    swapchain_info.imageSharingMode      = vk::SharingMode::eExclusive;
    swapchain_info.imageUsage            = vk::ImageUsageFlagBits::eColorAttachment;
    swapchain_info.pQueueFamilyIndices   = &family;
    swapchain_info.queueFamilyIndexCount = 1;

    const vk::SurfaceCapabilitiesKHR capabilities = gpu.getSurfaceCapabilitiesKHR(surface.get());
    swapchain_info.surface                        = surface.get();
    swapchain_info.imageExtent                    = extent;
    swapchain_info.minImageCount                  = capabilities.minImageCount;
    swapchain_info.preTransform                   = capabilities.currentTransform;

    const std::vector<vk::SurfaceFormatKHR> formats = gpu.getSurfaceFormatsKHR(surface.get());
    if(const auto it = std::find_if(formats.begin(),
                                    formats.end(),
                                    [](const vk::SurfaceFormatKHR& fmt) {
                                        return fmt.format == vk::Format::eB8G8R8A8Unorm &&
                                               fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
                                    });
       it == formats.end())
    {
        log_e << "Did not find bgra8 format with srgb-nonlinear color space.";
    }
    else
    {
        swapchain_info.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        swapchain_info.imageFormat     = vk::Format::eB8G8R8A8Unorm;
    }
    const std::vector<vk::PresentModeKHR> present_modes = gpu.getSurfacePresentModesKHR(surface.get());
    if(const auto it = std::find_if(present_modes.begin(),
                                    present_modes.end(),
                                    [](const vk::PresentModeKHR& mode) { return mode == vk::PresentModeKHR::eMailbox; });
       it == present_modes.end())
    {
        log_e << "Did not find mailbox present mode.";
    }
    else
        swapchain_info.presentMode = vk::PresentModeKHR::eMailbox;
    return device->createSwapchainKHRUnique(swapchain_info);
}

uint32_t memory_index(const vk::PhysicalDevice& gpu, const vk::MemoryRequirements& req, vk::MemoryPropertyFlags flags)
{
    const vk::PhysicalDeviceMemoryProperties mem_props = gpu.getMemoryProperties();
    for(int i = 0; i < int(mem_props.memoryTypeCount); ++i)
    {
        if(req.memoryTypeBits & (1 << i) && (mem_props.memoryTypes[i].propertyFlags & flags) == flags)
        {
            return i;
        }
    }
    return 0;
}
