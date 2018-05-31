#pragma once

#include <vulkan/vulkan.h>
#include <memory>

namespace vk
{
    class swapchain;
    class device;
    class image_view;
    class image : public std::enable_shared_from_this<image>
    {
    public:
        friend swapchain;
        friend image_view;

        image(const std::shared_ptr<device>& device, VkImageType type, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, uint32_t depth, VkSampleCountFlagBits samples, uint32_t levels = -1, uint32_t layers = 1, VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED) noexcept;
        
        ~image();
        image(const image&) = delete;
        image& operator=(const image&) = delete;
        image(image&&) = default;
        image& operator=(image&&) = default;

        std::unique_ptr<image_view> create_view(VkImageViewType type, VkFormat format, VkImageAspectFlags aspect, uint32_t base_level = 0, uint32_t level_count = 1, uint32_t base_layer = 0, uint32_t layer_count = 1) const noexcept;
        std::unique_ptr<image_view> create_view(VkImageViewType type, VkFormat format, VkImageAspectFlags aspect, VkComponentMapping swizzle) const noexcept;
        std::unique_ptr<image_view> create_view(VkImageViewType type, VkFormat format, VkImageAspectFlags aspect, uint32_t base_level, uint32_t level_count, uint32_t base_layer, uint32_t layer_count, VkComponentMapping swizzle) const noexcept;
        operator VkImage() const noexcept;

    private:
        image() = default;
        VkImage _image;
        std::shared_ptr<device> _device;
        bool _owning = true;
    };

    class image_view
    {
    public:
        friend image;
        ~image_view();
        image_view(const image_view&) = delete;
        image_view& operator=(const image_view&) = delete;
        image_view(image_view&&) = default;
        image_view& operator=(image_view&&) = default;

        operator VkImageView() const noexcept;

    private:
        image_view(const std::shared_ptr<const image>& image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspect, uint32_t base_level, uint32_t level_count, uint32_t base_layer, uint32_t layer_count, VkComponentMapping swizzle) noexcept;

        VkImageView _view;
        std::shared_ptr<const image> _image;
    };
}