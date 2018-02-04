#include "texture.hpp"
#include <any>
#include "vulkan/command_execute.hpp"
#include "vulkan/logical_device.hpp"
#include "stb/stb_image.h"

namespace vkn
{
    Texture::Texture(const TextureCreateInfo& info)
        : ClassInfo(info)
    {
        m_info.device->inc_ref();
    }

    Texture::~Texture()
    {
        if (m_memory) m_info.device->memory()->free(m_memory);
        if (m_image) m_info.device->destroyImage(m_image);
        m_info.device->dec_ref();
    }

    void Texture::loadFile(const std::experimental::filesystem::path& file, const TextureDataType type,
        const vk::QueueFlagBits transfer_queue_type)
    {
        auto&& transfer_queue = m_info.device->queue(transfer_queue_type).queue;
        auto&& transfer_pool = m_info.device->commandPool(transfer_queue_type);

        uint8_t* data_ptr = nullptr;
        int width, height, channels;
        switch (type)
        {
        case TextureDataType::eFloat:
            data_ptr = reinterpret_cast<uint8_t*>(stbi_loadf(file.string().c_str(), &width, &height, &channels, 4));
            break;
        case TextureDataType::eUByte:
            data_ptr = stbi_load(file.string().c_str(), &width, &height, &channels, 4);
            break;
        }

        m_extent = vk::Extent3D(width, height, 1);
        m_mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height))) + 1);

        const auto type_size = type == TextureDataType::eFloat ? sizeof(float) : sizeof(uint8_t);
        m_format = type == TextureDataType::eFloat ? vk::Format::eR32G32B32A32Sfloat : vk::Format::eR8G8B8A8Unorm;
        const vk::DeviceSize texture_data_size = width * height*type_size * 4;

        const auto staging_buffer = m_info.device->createBuffer(vk::BufferCreateInfo({}, texture_data_size, vk::BufferUsageFlagBits::eTransferSrc));
        const auto staging_buffer_req = m_info.device->getBufferMemoryRequirements(staging_buffer);
        const auto staging_memory = m_info.device->memory()->allocate(staging_buffer_req.size, staging_buffer_req.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);
        m_info.device->bindBufferMemory(staging_buffer, staging_memory->memory, staging_memory->offset);

        const vk::MappedMemoryRange memory_range(staging_memory->memory, staging_memory->offset, staging_memory->size);
        memcpy(staging_memory->data, data_ptr, texture_data_size);
        free(data_ptr);
        m_info.device->flushMappedMemoryRanges(memory_range);

        vk::ImageCreateInfo image_info;
        image_info.setArrayLayers(1)
            .setImageType(vk::ImageType::e2D)
            .setFormat(m_format)
            .setMipLevels(m_mip_levels)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setTiling(vk::ImageTiling::eOptimal)
            .setUsage(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setExtent(m_extent);
        m_image = m_info.device->createImage(image_info);
        const auto texture_requirements = m_info.device->getImageMemoryRequirements(m_image);
        m_memory = m_info.device->memory()->allocate(texture_requirements.size, texture_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
        m_info.device->bindImageMemory(m_image, m_memory->memory, m_memory->offset);

        auto transfer_buffer = m_info.device->allocateCommandBuffers(vk::CommandBufferAllocateInfo(transfer_pool, vk::CommandBufferLevel::ePrimary, 1))[0];
        transfer_buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        {
            const vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
            command::transformImageLayout(transfer_buffer, m_image, range,
                vk::AccessFlags(), vk::AccessFlagBits::eTransferWrite,
                vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
                vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer);

            vk::BufferImageCopy copy;
            copy.setImageSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1))
                .setImageExtent(m_extent);
            transfer_buffer.copyBufferToImage(staging_buffer, m_image, vk::ImageLayout::eTransferDstOptimal, copy);

            command::transformImageLayout(transfer_buffer, m_image, range,
                vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
                vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllGraphics);
        }
        transfer_buffer.end();

        vk::SubmitInfo texture_copy_submit;
        texture_copy_submit.setCommandBufferCount(1)
            .setPCommandBuffers(&transfer_buffer);
        transfer_queue.submit(texture_copy_submit, nullptr);
        transfer_queue.waitIdle();

        m_info.device->freeCommandBuffers(transfer_pool, transfer_buffer);
        m_info.device->memory()->free(staging_memory);
        m_info.device->destroyBuffer(staging_buffer);
    }

    void Texture::loadEmpty(const vk::ImageCreateInfo info)
    {
        m_extent = info.extent;
        m_format = info.format;
        m_mip_levels = info.mipLevels;

        m_image = m_info.device->createImage(info);
        const auto texture_requirements = m_info.device->getImageMemoryRequirements(m_image);
        m_memory = m_info.device->memory()->allocate(texture_requirements.size, texture_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
        m_info.device->bindImageMemory(m_image, m_memory->memory, m_memory->offset);
    }

    void Texture::generateMipmaps() const
    {
        m_info.device->oneTimeCommand(vk::QueueFlagBits::eTransfer, [&](vk::CommandBuffer command_buffer) {
            generateMipmaps(command_buffer);
        });
    }

    void Texture::generateMipmaps(const vk::CommandBuffer command_buffer) const
    {
        const vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        command::transformImageLayout(command_buffer, m_image, range,
            vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eTransferRead,
            vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eTransferSrcOptimal,
            vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eTransfer);

        for (uint32_t level = 1; level < m_mip_levels; ++level)
        {
            const vk::ImageSubresourceRange mip_range(vk::ImageAspectFlagBits::eColor, level, 1, 0, 1);

            vk::ImageBlit image_blit;
            image_blit.setSrcOffsets({ vk::Offset3D{}, vk::Offset3D{ static_cast<int32_t>(m_extent.width >> (level - 1)), static_cast<int32_t>(m_extent.height >> (level - 1)), 1 } })
                .setSrcSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, level - 1, 0, 1))
                .setDstOffsets({ vk::Offset3D{}, vk::Offset3D{ static_cast<int32_t>(m_extent.width >> level), static_cast<int32_t>(m_extent.height >> level), 1 } })
                .setDstSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, level, 0, 1));

            command::transformImageLayout(command_buffer, m_image, mip_range,
                vk::AccessFlags(), vk::AccessFlagBits::eTransferWrite,
                vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer);

            command_buffer.blitImage(m_image, vk::ImageLayout::eTransferSrcOptimal, m_image, vk::ImageLayout::eTransferDstOptimal, { image_blit }, vk::Filter::eLinear);

            command::transformImageLayout(command_buffer, m_image, mip_range,
                vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eTransferRead,
                vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal,
                vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer);
        }

        command::transformImageLayout(command_buffer, m_image,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, m_mip_levels, 0, 1),
            vk::AccessFlagBits::eTransferRead, vk::AccessFlagBits::eShaderRead,
            vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllGraphics);
    }

    vk::ImageView Texture::createImageView(vk::ImageSubresourceRange resource_range) const
    {
        if (resource_range.levelCount == 0)
            resource_range.setLevelCount(m_mip_levels);

        return m_info.device->createImageView(vk::ImageViewCreateInfo({}, m_image, vk::ImageViewType::e2D, m_format, {}, resource_range));
    }

    vk::Format Texture::format() const
    {
        return m_format;
    }

    const vk::Extent3D& Texture::extent() const
    {
        return m_extent;
    }

    uint32_t Texture::levels() const
    {
        return m_mip_levels;
    }

    vk::Image Texture::image() const
    {
        return m_image;
    }

    TextureView::TextureView(const TextureViewCreateInfo& info) : ClassInfo(info),
        m_image_view(m_info.texture->createImageView(m_info.resource_range))
    {
        m_info.texture->inc_ref();
    }

    TextureView::TextureView(LogicalDevice* device, const vk::ImageViewCreateInfo create_info)
        : ClassInfo(TextureViewCreateInfo(new Texture(device), create_info.subresourceRange)),
        m_image_view(m_info.texture->m_info.device->createImageView(create_info))
    {
        m_info.texture->inc_ref();
    }

    TextureView::~TextureView()
    {
        m_info.texture->m_info.device->destroyImageView(m_image_view);
        m_info.texture->dec_ref();
    }

    TextureView::operator vk::ImageView() const
    {
        return m_image_view;
    }
}
