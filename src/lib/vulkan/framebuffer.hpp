#pragma once

#include "device.hpp"
#include "texture.hpp"
#include "jpu/memory"

namespace vkn
{
    class framebuffer : public jpu::ref_count
    {
    public:
        framebuffer(device* device, vk::RenderPass pass, uint32_t width, uint32_t height, uint32_t layers = 1);
        framebuffer(device* device, vk::RenderPass pass, vk::Extent2D extent, uint32_t layers = 1);
        ~framebuffer();

        void set_attachments(std::vector<jpu::ref_ptr<texture_view>> attachments);
        void finalize();

        operator vk::Framebuffer() const;

    private:
        vk::Framebuffer _framebuffer;
        device * _device;
        vk::RenderPass _pass;
        vk::Extent2D _extent;
        uint32_t _layers;
        std::vector<jpu::ref_ptr<texture_view>> _attachments;
    };
}
