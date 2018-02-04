#pragma once

#include <array>
#include <string_view>
#include <vector>

namespace vkn::detail
{
    constexpr std::array<const char*, 3> layers
    {
        "VK_LAYER_LUNARG_standard_validation",
        "VK_LAYER_LUNARG_core_validation",
        "VK_LAYER_LUNARG_parameter_validation"
    };

    constexpr std::array<const char*, 2> device_extensions
    {
        "VK_KHR_swapchain",
        "VK_KHR_push_descriptor"
    };

    const std::vector<const char*>& instance_extensions();
    const std::vector<const char*>& instance_layers();
}
