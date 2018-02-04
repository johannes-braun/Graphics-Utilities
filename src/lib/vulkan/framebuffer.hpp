#pragma once

#include "logical_device.hpp"
#include "vulkan/texture.hpp"
#include "jpu/memory"

namespace vkn
{
    struct FramebufferCreateInfo
    {
        FramebufferCreateInfo(
            LogicalDevice* device = nullptr,
            const vk::RenderPass renderpass = nullptr,
            const vk::Extent2D extent = {},
            const uint32_t layers = 0,
            vk::ArrayProxy<const jpu::ref_ptr<TextureView>> attachments = {})
            : device(device), renderpass(renderpass), extent(extent), layers(layers), attachments(attachments.begin(), attachments.end())
        {}

        FramebufferCreateInfo& setDevice(LogicalDevice* value) { device = value; return *this; }
        FramebufferCreateInfo& setRenderPass(const vk::RenderPass value) { renderpass = value; return *this; }
        FramebufferCreateInfo& setExtent(const vk::Extent2D value) { extent = value; return *this; }
        FramebufferCreateInfo& setLayers(const uint32_t value) { layers = value; return *this; }
        FramebufferCreateInfo& setAttachments(vk::ArrayProxy<const jpu::ref_ptr<TextureView>> value) { attachments = std::vector<jpu::ref_ptr<TextureView>>(value.begin(), value.end()); return *this; }

        LogicalDevice* device;
        vk::RenderPass renderpass;
        vk::Extent2D extent;
        uint32_t layers;
        std::vector<jpu::ref_ptr<TextureView>> attachments;
    };

    class Framebuffer : ClassInfo<FramebufferCreateInfo, Framebuffer>, public jpu::ref_count
    {
    public:
        explicit Framebuffer(const FramebufferCreateInfo& info);
        ~Framebuffer();

        operator vk::Framebuffer() const;

    private:
        vk::Framebuffer m_framebuffer;
    };
}
