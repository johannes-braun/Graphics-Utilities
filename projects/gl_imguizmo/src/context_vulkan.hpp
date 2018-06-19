#pragma once

#include <vulkan/vulkan.hpp>

#include "context.hpp"
#include "vulkan_alloc.hpp"
#include <unordered_set>

namespace gfx::vulkan
{
#define USE_VALIDATION_LAYERS 1
#if USE_VALIDATION_LAYERS
constexpr std::array layers{"VK_LAYER_LUNARG_standard_validation",
                            "VK_LAYER_LUNARG_core_validation",
                            "VK_LAYER_LUNARG_parameter_validation"};
#else
constexpr std::array<const char*, 0> layers{};
#endif
constexpr std::array instance_extensions{VK_KHR_SURFACE_EXTENSION_NAME,
                                         VK_EXT_DEBUG_REPORT_EXTENSION_NAME};

constexpr std::array device_extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                       VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
                                       VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
                                       VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME};

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
    void initialize(GLFWwindow* window) override;
    ~context_implementation();
    void make_current(GLFWwindow* window) override;

    const vk::Instance&               instance() const noexcept { return _instance.get(); }
    const vk::DebugReportCallbackEXT& debug_callback() const noexcept
    {
        return _debug_callback.get();
    }
    const vk::PhysicalDevice& gpu() const noexcept { return _gpu; }
    const vk::SurfaceKHR&     surface() const noexcept { return _surface.get(); }
    const vk::Device&         device() const noexcept { return _device.get(); }
    const VmaAllocator        allocator() const noexcept { return _allocator; }

    const std::array<vk::Queue, 4>& queues() const noexcept { return _queues; }
    const std::array<uint32_t, 4>&  families() const noexcept { return _families; }
    std::vector<uint32_t>           families(const std::vector<fam::type>& types) const noexcept
    {
        std::unordered_set<uint32_t> fs;
        for(auto& t : types)
            fs.emplace(_families[t]);
        return {fs.begin(), fs.end()};
    }
    const std::array<vk::UniqueCommandPool, 3>& command_pools() const noexcept
    {
        return _command_pools;
    }

    uint32_t memory_index(const vk::MemoryRequirements& req, vk::MemoryPropertyFlags flags)
    {
        const vk::PhysicalDeviceMemoryProperties mem_props = _gpu.getMemoryProperties();
        for(int i = 0; i < int(mem_props.memoryTypeCount); ++i)
        {
            if(req.memoryTypeBits & (1 << i) &&
               (mem_props.memoryTypes[i].propertyFlags & flags) == flags)
            {
                return i;
            }
        }
        return 0;
    }

private:
    vk::UniqueInstance               _instance;
    vk::UniqueDebugReportCallbackEXT _debug_callback;
    vk::PhysicalDevice               _gpu;
    vk::UniqueSurfaceKHR             _surface;
    vk::UniqueDevice                 _device;
    VmaAllocator                     _allocator;

    std::array<vk::Queue, 4>             _queues;
    std::array<uint32_t, 4>              _families{0};
    std::array<vk::UniqueCommandPool, 3> _command_pools;
};
} // namespace gfx::vulkan