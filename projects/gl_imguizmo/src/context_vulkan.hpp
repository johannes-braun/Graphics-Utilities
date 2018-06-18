#pragma once

#include <vulkan/vulkan.hpp>

#include "context.hpp"

namespace gfx::vulkan
{
#define USE_VALIDATION_LAYERS 1
#if USE_VALIDATION_LAYERS
constexpr std::array layers{
        "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_LUNARG_core_validation", "VK_LAYER_LUNARG_parameter_validation"};
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

class context_implementation : public detail::context_implementation
{
public:
    void initialize(GLFWwindow* window) override;
    void make_current(GLFWwindow* window) override;

    const vk::Instance&                 instance() const noexcept { return _instance.get(); }
    const vk::DebugReportCallbackEXT&   debug_callback() const noexcept { return _debug_callback.get(); }
    const vk::PhysicalDevice&           gpu() const noexcept { return _gpu; }
    const vk::SurfaceKHR&               surface() const noexcept { return _surface.get(); }
    const vk::Device&                   device() const noexcept { return _device.get(); }

    const std::array<vk::Queue, 4>&             queues() const noexcept { return _queues; }
    const std::array<uint32_t, 4>&              families() const noexcept { return _families; }
    const std::array<vk::UniqueCommandPool, 3>& command_pools() const noexcept { return _command_pools; }

private:
    vk::UniqueInstance               _instance;
    vk::UniqueDebugReportCallbackEXT _debug_callback;
    vk::PhysicalDevice               _gpu;
    vk::UniqueSurfaceKHR             _surface;
    vk::UniqueDevice                 _device;

    std::array<vk::Queue, 4>             _queues;
    std::array<uint32_t, 4>              _families{0};
    std::array<vk::UniqueCommandPool, 3> _command_pools;
};
} // namespace gfx::vulkan