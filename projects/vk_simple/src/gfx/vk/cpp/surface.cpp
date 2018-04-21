#include "../surface.hpp"

#include "../physical_device.hpp"
#include "../instance.hpp"
#include "../swapchain.hpp"
#include "../device.hpp"

namespace gfx::vk
{
    surface::surface(const std::shared_ptr<physical_device>& physical_device, GLFWwindow* window)
        : _physical_device(physical_device) {
        glfwCreateWindowSurface(*(_physical_device->_instance), window, nullptr, &_surface);
    }

    surface::~surface()
    {
        vkDestroySurfaceKHR(*(_physical_device->_instance), _surface, nullptr);
    }

    VkSurfaceCapabilitiesKHR surface::capabilities() const noexcept
    {
        VkSurfaceCapabilitiesKHR caps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*_physical_device, _surface, &caps);
        return caps;
    }

    std::vector<VkPresentModeKHR> surface::present_modes() const noexcept
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(*_physical_device, _surface, &count, nullptr);
        std::vector<VkPresentModeKHR> modes(count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(*_physical_device, _surface, &count, modes.data());
        return modes;
    }

    std::vector<VkSurfaceFormatKHR> surface::formats() const noexcept
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(*_physical_device, _surface, &count, nullptr);
        std::vector<VkSurfaceFormatKHR> fmt(count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(*_physical_device, _surface, &count, fmt.data());
        return fmt;
    }

    surface::operator VkSurfaceKHR() const noexcept { return _surface; }

    std::shared_ptr<swapchain> surface::create_default_swapchain(const std::shared_ptr<device>& device, array_view<uint32_t> queue_family_indices, uint32_t min_img_count) const noexcept
    {
        const auto caps = capabilities();

        swapchain_create_info info;
        info.clipped                = true;
        info.composite_alpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.color_space            = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        info.extent                 = caps.currentExtent;
        info.format                 = VK_FORMAT_B8G8R8A8_UNORM;
        info.image_sharing_mode     = VK_SHARING_MODE_EXCLUSIVE;
        info.image_usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.image_array_layers     = 1;
        info.min_image_count        = min_img_count;
        info.present_mode           = VK_PRESENT_MODE_MAILBOX_KHR;
        info.pre_transform          = caps.currentTransform;
        info.queue_family_indices   = { queue_family_indices.begin(), queue_family_indices.end() };
        info.surface                = shared_from_this();

        return std::make_shared<swapchain>(device, info);
    }
}