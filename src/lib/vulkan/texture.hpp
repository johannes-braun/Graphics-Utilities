#pragma once

#include <filesystem>
#include <vulkan/vulkan.hpp>
#include "vulkan/memory.hpp"
#include <jpu/memory>

namespace vkn
{
    class LogicalDevice;

    struct TextureCreateInfo
    {
        TextureCreateInfo(LogicalDevice* device = nullptr)
            : device(device) {}

        TextureCreateInfo& setDevice(LogicalDevice* value) { device = value; return *this; }

        LogicalDevice* device;
    };

    enum class TextureDataType
    {
        eFloat = 0,
        eUByte
    };

    class TextureView;

    class Texture : ClassInfo<TextureCreateInfo, Texture>, public jpu::ref_count
    {
        friend TextureView;
    public:
        explicit Texture(const TextureCreateInfo& info);
        ~Texture();

        void loadFile(const std::experimental::filesystem::path& file, const TextureDataType type,
            vk::QueueFlagBits transfer_queue_type = vk::QueueFlagBits::eTransfer);

        void loadEmpty(vk::ImageCreateInfo info);

        void generateMipmaps() const;
        void generateMipmaps(const vk::CommandBuffer command_buffer) const;

        vk::ImageView createImageView(vk::ImageSubresourceRange resource_range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 0, 0, 1)) const;

        vk::Format format() const;
        const vk::Extent3D& extent() const;
        uint32_t levels() const;
        vk::Image image() const;

    private:
        vk::Format m_format{};

        vk::Extent3D m_extent{};
        uint32_t m_mip_levels{ 0 };

        vk::Image m_image{ nullptr };
        MemoryBlock m_memory{ nullptr };
    };

    struct TextureViewCreateInfo
    {
        TextureViewCreateInfo(Texture* texture = nullptr, const vk::ImageSubresourceRange resource_range = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 0, 0, 1))
            : texture(texture), resource_range(resource_range) {}

        TextureViewCreateInfo& setTexture(Texture* value) { texture = value; return *this; }
        TextureViewCreateInfo& setResourceRange(const vk::ImageSubresourceRange value) { resource_range = value; return *this; }

        Texture* texture;
        vk::ImageSubresourceRange resource_range;
    };

    class TextureView : ClassInfo<TextureViewCreateInfo, TextureView>, public jpu::ref_count
    {
    public:
        explicit TextureView(const TextureViewCreateInfo& info);
        explicit TextureView(LogicalDevice* device, vk::ImageViewCreateInfo create_info);
        ~TextureView();

        operator vk::ImageView() const;

    private:
        vk::ImageView m_image_view;
    };
}
