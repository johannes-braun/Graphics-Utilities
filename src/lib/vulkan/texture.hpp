#pragma once

#include <filesystem>
#include <vulkan/vulkan.hpp>
#include "memory.hpp"
#include <jpu/memory>

namespace vkn
{
    class device;

    enum class texture_data_type
    {
        eFloat = 0,
        eUByte
    };

    class texture_view;

    class texture : public jpu::ref_count
    {
        friend texture_view;
    public:
        explicit texture(device* device);
        ~texture();

        void assign_2d(uint32_t width, uint32_t height, uint32_t channels, size_t type_size, vk::Format format, void* data, vk::QueueFlagBits transfer_queue_type = vk::QueueFlagBits::eTransfer);

        void loadEmpty(vk::ImageCreateInfo info);

        void generateMipmaps() const;
        void generateMipmaps(const vk::CommandBuffer command_buffer) const;

        vk::ImageView createImageView(vk::ImageSubresourceRange resource_range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 0, 0, 1)) const;

        vk::Format format() const;
        const vk::Extent3D& extent() const;
        uint32_t levels() const;
        vk::Image image() const;

    private:
        device* _device;
        vk::Format _format{};

        vk::Extent3D _extent{};
        uint32_t _mip_levels{ 0 };

        vk::Image _image{ nullptr };
        const memory_block* _memory{ nullptr };
    };

    class texture_view : public jpu::ref_count
    {
    public:
        explicit texture_view(texture* texture, vk::ImageSubresourceRange resource_range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 0, 0, 1));
        explicit texture_view(device* device, vk::ImageViewCreateInfo create_info);
        ~texture_view();

        operator vk::ImageView() const;

    private:
        texture* _texture;
        vk::ImageSubresourceRange _resource_range;
        vk::ImageView _image_view;
    };
}
