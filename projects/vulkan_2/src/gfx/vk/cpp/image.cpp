#include "../image.hpp"

#include "../device.hpp"
#include <algorithm>

namespace gfx::vk
{
    image::image(const std::shared_ptr<device>& device, VkImageType type, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, uint32_t depth, VkSampleCountFlagBits samples, uint32_t levels, uint32_t layers, VkSharingMode sharing_mode, VkImageTiling tiling, VkImageLayout layout) noexcept
        : _device(device)
    {
        levels = levels==-1 ? uint32_t(std::floor(std::log2(std::max(width, height))) + 1) : levels;

        VkImageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = nullptr;
        info.arrayLayers = layers;
        info.extent = { width, height, depth };
        info.flags = 0;
        info.format = format;
        info.imageType = type;
        info.initialLayout = layout;
        info.mipLevels = levels;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = nullptr;
        info.samples = samples;
        info.sharingMode = sharing_mode;
        info.tiling = tiling;
        info.usage = usage;

        vkCreateImage(*_device, &info, nullptr, &_image);
        _owning = false;
    }

    image::~image()
    {
        if (_owning)
            vkDestroyImage(*_device, _image, nullptr);
    }

    image::operator VkImage() const noexcept
    {
        return _image;
    }

    std::unique_ptr<image_view> image::create_view(VkImageViewType type, VkFormat format, VkImageAspectFlags aspect, uint32_t base_level, uint32_t level_count, uint32_t base_layer, uint32_t layer_count) const noexcept
    {
        constexpr const VkComponentMapping default_swizzle{ VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        return create_view(type, format, aspect, base_level, level_count, base_layer, layer_count, default_swizzle);
    }

    std::unique_ptr<image_view> image::create_view(VkImageViewType type, VkFormat format, VkImageAspectFlags aspect, VkComponentMapping swizzle) const noexcept
    {
        return create_view(type, format, aspect, 0, 1, 0, 1, swizzle);
    }

    std::unique_ptr<image_view> image::create_view(VkImageViewType type, VkFormat format, VkImageAspectFlags aspect, uint32_t base_level, uint32_t level_count, uint32_t base_layer, uint32_t layer_count, VkComponentMapping swizzle) const noexcept
    {
        return std::unique_ptr<image_view>{ new image_view(shared_from_this(), type, format, aspect, base_level, level_count, base_layer, layer_count, swizzle) };
    }

    image_view::~image_view()
    {
        vkDestroyImageView(*(_image->_device), _view, nullptr);
    }

    image_view::operator VkImageView() const noexcept
    {
        return _view;
    }

    image_view::image_view(const std::shared_ptr<const image>& image, VkImageViewType type, VkFormat format, VkImageAspectFlags aspect, uint32_t base_level, uint32_t level_count, uint32_t base_layer, uint32_t layer_count, VkComponentMapping swizzle) noexcept
        : _image(image)
    {
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.components = swizzle;
        info.format = format;
        info.image = *_image;
        info.subresourceRange.aspectMask = aspect;
        info.subresourceRange.baseArrayLayer = base_layer;
        info.subresourceRange.baseMipLevel = base_level;
        info.subresourceRange.layerCount = layer_count;
        info.subresourceRange.levelCount = level_count;
        info.viewType = type;
        vkCreateImageView(*(_image->_device), &info, nullptr, &_view);
    }
}