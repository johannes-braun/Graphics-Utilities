#include "context_vulkan.hpp"
#include "init_struct.hpp"
#include <gfx/log.hpp>
#define VMA_IMPLEMENTATION
#include <vulkan/vk_mem_alloc.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {
context_implementation::~context_implementation()
{
    if (_allocator) vmaDestroyAllocator(_allocator);
    if (_surface) vkDestroySurfaceKHR(_instance, _surface, nullptr);
    if (_debug_callback) _vkDestroyDebugReportCallbackEXT(_instance, _debug_callback, nullptr);
    if (_device) {
        for (auto& cp : _command_pools) vkDestroyCommandPool(_device, cp, nullptr);
        vkDestroyDevice(_device, nullptr);
    }
    if (_instance) vkDestroyInstance(_instance, nullptr);
}

void context_implementation::initialize(GLFWwindow* window, const context_options& options)
{
    init_instance(options);
    if (options.debug) init_debug_callback();

    if (options.use_window) glfwCreateWindowSurface(_instance, window, nullptr, &_surface);

    u32 gpu_count = 0;
    vkEnumeratePhysicalDevices(_instance, &gpu_count, nullptr);
    std::vector<VkPhysicalDevice> gpus(gpu_count);
    vkEnumeratePhysicalDevices(_instance, &gpu_count, gpus.data());
    _gpu = gpus[0];
    init_devices();

    init<VmaAllocatorCreateInfo> alloc_create_info;
    alloc_create_info.device         = _device;
    alloc_create_info.physicalDevice = _gpu;
    alloc_create_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    vmaCreateAllocator(&alloc_create_info, &_allocator);

    init<VkCommandPoolCreateInfo> pool_info{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = _queue_families[fam::graphics];
    vkCreateCommandPool(_device, &pool_info, nullptr, &_command_pools[fam::graphics]);
    pool_info.queueFamilyIndex = _queue_families[fam::compute];
    vkCreateCommandPool(_device, &pool_info, nullptr, &_command_pools[fam::compute]);
    pool_info.queueFamilyIndex = _queue_families[fam::transfer];
    vkCreateCommandPool(_device, &pool_info, nullptr, &_command_pools[fam::transfer]);

	init<VkDescriptorPoolCreateInfo> dpi{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	dpi.maxSets = 1024;

    std::array<VkDescriptorPoolSize, 4> sizes{
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 512u },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1024u },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 32u },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1024u }
	};

	dpi.pPoolSizes = sizes.data();
	dpi.poolSizeCount = static_cast<u32>(sizes.size());
	vkCreateDescriptorPool(_device, &dpi, nullptr, &_descriptor_pool);
}

void context_implementation::init_instance(const context_options& opt)
{
    init<VkApplicationInfo> app_info{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion         = VK_API_VERSION_1_0;
    app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.engineVersion      = VK_MAKE_VERSION(0, 1, 0);
    app_info.pApplicationName   = opt.window_title.c_str();
    app_info.pEngineName        = "Graphics Utilities";

    init<VkInstanceCreateInfo> instance_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instance_info.pApplicationInfo = &app_info;

    uint32_t     count;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);

    std::vector<const char*> extensions = {std::begin(instance_extensions), std::end(instance_extensions)};
    if (opt.use_window) {
        extensions.insert(extensions.end(), glfw_extensions, glfw_extensions + count);
    }

    instance_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();

    if (opt.debug) {
        instance_info.enabledLayerCount   = static_cast<uint32_t>(std::size(layers));
        instance_info.ppEnabledLayerNames = std::data(layers);
    }
    vkCreateInstance(&instance_info, nullptr, &_instance);
}

void context_implementation::init_debug_callback()
{
    init<VkDebugReportCallbackCreateInfoEXT> debug_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debug_info.pUserData = nullptr;
    debug_info.flags     = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT
                       | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    debug_info.pfnCallback = [](VkDebugReportFlagsEXT f, VkDebugReportObjectTypeEXT ot, uint64_t o, size_t l, int32_t m, const char* lp,
                                const char* msg, void* ud) -> VkBool32 {
        switch (f)
        {
        case VK_DEBUG_REPORT_INFORMATION_BIT_EXT: ilog << msg; break;
        case VK_DEBUG_REPORT_DEBUG_BIT_EXT: dlog << msg; break;
        case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT: wlog << msg; break;
        case VK_DEBUG_REPORT_ERROR_BIT_EXT: elog << msg; break;
        default: break;
        }
        return true;
    };

    _vkCreateDebugReportCallbackEXT =
        reinterpret_cast<decltype(_vkCreateDebugReportCallbackEXT)>(vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT"));
    _vkDestroyDebugReportCallbackEXT =
        reinterpret_cast<decltype(_vkDestroyDebugReportCallbackEXT)>(vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT"));

    _vkCreateDebugReportCallbackEXT(_instance, &debug_info, nullptr, &_debug_callback);
}

void context_implementation::init_devices()
{
    struct queue_infos
    {
        std::array<uint32_t, 4> families;
        std::array<float, 4>    priorities;
        std::array<uint32_t, 4> family_indices;
    } queues;

    u32 count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_family_properties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &count, queue_family_properties.data());

    queues.families[fam::present] = 0;
    for (uint32_t family = 0; family < queue_family_properties.size(); ++family) {
        if ((queue_family_properties[family].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
            queues.families[fam::graphics] = family;
        if ((queue_family_properties[family].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT)
            queues.families[fam::compute] = family;
        if ((queue_family_properties[family].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT)
            queues.families[fam::transfer] = family;

        if (_surface) {
            VkBool32 supports = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(_gpu, family, _surface, &supports);
            if (glfwGetPhysicalDevicePresentationSupport(_instance, _gpu, family) && supports) queues.families[fam::present] = family;
        }
    }
    queues.priorities[fam::graphics] = 1.f;
    queues.priorities[fam::compute]  = 1.f;
    queues.priorities[fam::transfer] = 0.5f;
    queues.priorities[fam::present]  = 0.8f;

    std::unordered_map<uint32_t, std::vector<float>> queue_filter;
    for (int i = 0; i < 4; ++i) {
        auto&& vec               = queue_filter[queues.families[i]];
        queues.family_indices[i] = static_cast<uint32_t>(vec.size());
        vec.push_back(queues.priorities[i]);
    }

    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    for (const auto& filter : queue_filter) {
        auto& info            = queue_infos.emplace_back(init<VkDeviceQueueCreateInfo>(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO));
        info.queueFamilyIndex = filter.first;
        info.queueCount       = static_cast<uint32_t>(filter.second.size());
        info.pQueuePriorities = filter.second.data();
    }
    init<VkDeviceCreateInfo> device_info{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_info.enabledExtensionCount   = static_cast<uint32_t>(std::size(device_extensions));
    device_info.ppEnabledExtensionNames = std::data(device_extensions);
    device_info.enabledLayerCount       = static_cast<uint32_t>(std::size(layers));
    device_info.ppEnabledLayerNames     = std::data(layers);
    device_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_infos.size());
    device_info.pQueueCreateInfos       = queue_infos.data();

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(_gpu, &features);
    features.multiDrawIndirect   = true;
    device_info.pEnabledFeatures = &features;

    vkCreateDevice(_gpu, &device_info, nullptr, &_device);

    _queue_families = queues.families;
    _queue_indices  = queues.family_indices;

    vkGetDeviceQueue(_device, _queue_families[fam::graphics], _queue_indices[fam::graphics], &_queues[fam::graphics]);
    vkGetDeviceQueue(_device, _queue_families[fam::compute], _queue_indices[fam::compute], &_queues[fam::compute]);
    vkGetDeviceQueue(_device, _queue_families[fam::transfer], _queue_indices[fam::transfer], &_queues[fam::transfer]);
    vkGetDeviceQueue(_device, _queue_families[fam::present], _queue_indices[fam::present], &_queues[fam::present]);
}

void context_implementation::make_current(GLFWwindow* window)
{
    // Nothing.
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx