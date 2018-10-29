#pragma once

#include "../general/handle.hpp"
#include "init_struct.hpp"
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <context.hpp>

namespace gfx {
inline namespace v1 {
namespace vulkan {
constexpr const char* layers[]{"VK_LAYER_LUNARG_standard_validation"};
constexpr const char* instance_extensions[]{VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};
constexpr const char* device_extensions[]{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
                                          VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME, VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME};

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
	bool on_run(bool open) override;

    VkSurfaceKHR                        surface() const { return _surface; }
    const std::array<uint32_t, 4>&      queue_families() const { return _queue_families; }
    const std::array<uint32_t, 4>&      queue_indices() const { return _queue_indices; }
    const std::array<VkQueue, 4>&       queues() const { return _queues; }
    const std::array<VkCommandPool, 4>& command_pools() const { return _command_pools; }
    VkDevice                            device() const { return _device; }
    VkPhysicalDevice                    gpu() const { return _gpu; }
    VmaAllocator                        allocator() const { return _allocator; }
	VkDescriptorPool                    descriptor_pool() const { return _descriptor_pool; }
	const VkPhysicalDeviceProperties& gpu_properties() const noexcept { return _gpu_properties; }

	std::array<std::vector<VkSemaphore>, 4>          final_wait_semaphores;
	std::array<std::vector<VkPipelineStageFlags>, 4> final_wait_stages;

private:
    void init_instance(const context_options& opt);
    void init_debug_callback();
    void init_devices();

    PFN_vkCreateDebugReportCallbackEXT       _vkCreateDebugReportCallbackEXT  = nullptr;
    PFN_vkDestroyDebugReportCallbackEXT      _vkDestroyDebugReportCallbackEXT = nullptr;
    movable_handle<VkInstance>               _instance;
    movable_handle<VkDebugReportCallbackEXT> _debug_callback;
    movable_handle<VkSurfaceKHR>             _surface;
	movable_handle<VkDescriptorPool>         _descriptor_pool;
    std::array<uint32_t, 4>                  _queue_families{0};
    std::array<uint32_t, 4>                  _queue_indices{0};
    std::array<VkQueue, 4>                   _queues{0};
    std::array<VkCommandPool, 4>             _command_pools{0};
    movable_handle<VkDevice>                 _device;
    VkPhysicalDevice                         _gpu = nullptr;
    movable_handle<VmaAllocator>             _allocator;
	init<VkPhysicalDeviceProperties>         _gpu_properties;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx