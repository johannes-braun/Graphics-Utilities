#pragma once
#include <array>
#include <filesystem>
#include <fstream>
#include <vulkan/vulkan.hpp>

#define USE_VALIDATION_LAYERS 1
#if USE_VALIDATION_LAYERS
constexpr std::array layers{
    "VK_LAYER_LUNARG_standard_validation", 
    "VK_LAYER_LUNARG_core_validation", 
    "VK_LAYER_LUNARG_parameter_validation"
};
#else
constexpr std::array<const char*, 0> layers{};
#endif
constexpr std::array instance_extensions{VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME};

constexpr std::array device_extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME};

struct fam
{
    enum
    {
        graphics = 0,
        compute,
        transfer,
        present
    };
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

vk::UniqueSwapchainKHR create_swapchain(const vk::PhysicalDevice& gpu, const vk::UniqueDevice& device, const vk::UniqueSurfaceKHR& surface,
                                        uint32_t family, const vk::Extent2D& extent);

uint32_t memory_index(const vk::PhysicalDevice& gpu, const vk::MemoryRequirements& req, vk::MemoryPropertyFlags flags);

vk::UniqueShaderModule create_shader(const vk::UniqueDevice& device, const std::filesystem::path& file);
