#pragma once

#include "structure.hpp"

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace vk
{
    class surface;
    class device;
    class swapchain;
    class fence;
    class semaphore;
    class image;

    struct swapchain_create_info
    {
    private:
        friend swapchain;
        VkSwapchainCreateFlagsKHR       _flags              = 0;
    public:
        std::shared_ptr<const surface>  surface             = nullptr;
        VkFormat                        format              = VK_FORMAT_B8G8R8A8_UNORM;
        VkColorSpaceKHR                 color_space         = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        VkExtent2D                      extent              = { 1, 1 };
        VkPresentModeKHR                present_mode        = VK_PRESENT_MODE_MAILBOX_KHR;

        uint32_t                        min_image_count     = 4;
        uint32_t                        image_array_layers  = 1;
        VkImageUsageFlags               image_usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        VkSharingMode                   image_sharing_mode  = VK_SHARING_MODE_EXCLUSIVE;

        std::vector<uint32_t>           queue_family_indices {};

        VkSurfaceTransformFlagBitsKHR   pre_transform      = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        VkCompositeAlphaFlagBitsKHR     composite_alpha    = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        VkBool32                        clipped            = true;
        std::shared_ptr<swapchain>      old_swapchain      = nullptr;
    };

    class swapchain
    {
    public:
        swapchain(const std::shared_ptr<device>& device, const swapchain_create_info& create_info);
        ~swapchain();
        swapchain(const swapchain&) = delete;
        swapchain& operator=(const swapchain&) = delete;
        swapchain(swapchain&&) = default;
        swapchain& operator=(swapchain&&) = default;

        operator VkSwapchainKHR() const noexcept;
        std::vector<std::shared_ptr<image>> images() const noexcept;
        std::pair<VkResult, uint32_t> next_image(const std::shared_ptr<semaphore>& semaphore = nullptr, const std::shared_ptr<fence>& fence = nullptr, uint64_t timeout = std::numeric_limits<uint64_t>::max()) const noexcept;

    private:
        std::vector<std::shared_ptr<image>> _images;
        std::shared_ptr<device> _device;
        VkSwapchainKHR _swapchain;
    };
}