#pragma once
#include <array>
#include <vulkan/vulkan.hpp>

constexpr std::array<const char*, 3> layers{"VK_LAYER_LUNARG_standard_validation", "VK_LAYER_LUNARG_core_validation", "VK_LAYER_LUNARG_parameter_validation"};
constexpr std::array<const char*, 2> instance_extensions{"VK_KHR_surface", "VK_EXT_debug_report"};
constexpr std::array<const char*, 3> device_extensions{"VK_KHR_swapchain", "VK_KHR_push_descriptor", "VK_KHR_shader_draw_parameters"};

enum family
{
    graphics = 0,
    compute,
    transfer,
    present
};

vk::UniqueInstance               create_instance();
vk::UniqueDebugReportCallbackEXT create_debug_callback(const vk::UniqueInstance& instance, vk::DebugReportFlagsEXT flags);

struct device_infos
{
    vk::UniqueDevice        device;
    std::array<uint32_t, 4> queue_families;
    std::array<uint32_t, 4> queue_indices;
};
device_infos create_device(const vk::UniqueInstance& instance, const vk::PhysicalDevice& gpu, const vk::UniqueSurfaceKHR& surface);

vk::UniqueSwapchainKHR create_swapchain(const vk::PhysicalDevice& gpu, const vk::UniqueDevice& device, const vk::UniqueSurfaceKHR& surface, uint32_t family, const vk::Extent2D& extent);

uint32_t memory_index(const vk::PhysicalDevice& gpu, const vk::MemoryRequirements& req, vk::MemoryPropertyFlags flags);