#pragma once

#include "../general/handle.hpp"
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <gfx/context.hpp>

namespace gfx {
inline namespace v1 {
namespace vulkan {
constexpr const char* layers[]{"VK_LAYER_LUNARG_standard_validation", "VK_LAYER_LUNARG_core_validation",
                               "VK_LAYER_LUNARG_parameter_validation"};
constexpr const char* instance_extensions[]{VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
constexpr const char* device_extensions[]{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
                                          VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME};

struct fam
{
    enum type
    {
        graphics = 0,
        compute,
        transfer,
        present
    };
};

class context_implementation : public detail::context_implementation
{
public:
	~context_implementation() override;

    void initialize(GLFWwindow* window, const context_options& options) override;
    void make_current(GLFWwindow* window) override;

    VkSurfaceKHR                        surface() { return _surface; }
    const std::array<uint32_t, 4>&      queue_families() { return _queue_families; }
    const std::array<uint32_t, 4>&      queue_indices() { return _queue_indices; }
    const std::array<VkQueue, 4>&       queues() { return _queues; }
    const std::array<VkCommandPool, 4>& command_pools() { return _command_pools; }
    VkDevice                            device() { return _device; }
    VkPhysicalDevice                    gpu() { return _gpu; }
    VmaAllocator                        allocator() { return _allocator; }

private:
    void init_instance(const context_options& opt);
    void init_debug_callback();
    void init_devices();

    PFN_vkCreateDebugReportCallbackEXT       _vkCreateDebugReportCallbackEXT = nullptr;
    PFN_vkDestroyDebugReportCallbackEXT      _vkDestroyDebugReportCallbackEXT = nullptr;
    movable_handle<VkInstance>               _instance;
    movable_handle<VkDebugReportCallbackEXT> _debug_callback;
    movable_handle<VkSurfaceKHR>             _surface;
    std::array<uint32_t, 4>                  _queue_families{};
    std::array<uint32_t, 4>                  _queue_indices{};
    std::array<VkQueue, 4>                   _queues{};
    std::array<VkCommandPool, 4>             _command_pools{};
    movable_handle<VkDevice>                 _device;
    VkPhysicalDevice                         _gpu = nullptr;
    movable_handle<VmaAllocator>             _allocator;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx