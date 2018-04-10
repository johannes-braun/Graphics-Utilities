#pragma once

#include "array_view.hpp"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

namespace gfx::vk
{
    class physical_device;
    class device;
    class swapchain;
    class surface : public std::enable_shared_from_this<surface>
    {
    public:
        surface(const std::shared_ptr<physical_device>& physical_device, GLFWwindow* window);
        ~surface();
        surface(const surface&) = delete;
        surface& operator=(const surface&) = delete;
        surface(surface&&) = default;
        surface& operator=(surface&&) = default;

        VkSurfaceCapabilitiesKHR capabilities() const noexcept;
        std::vector<VkPresentModeKHR> present_modes() const noexcept;
        std::vector<VkSurfaceFormatKHR> formats() const noexcept;
        operator VkSurfaceKHR() const noexcept;

        std::shared_ptr<swapchain> create_default_swapchain(const std::shared_ptr<device>& device, array_view<uint32_t> queue_family_indices, uint32_t min_img_count) const noexcept;

    private:
        VkSurfaceKHR _surface;
        std::shared_ptr<physical_device> _physical_device;
    };
}