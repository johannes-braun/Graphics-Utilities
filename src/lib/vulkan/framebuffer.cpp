#include "framebuffer.hpp"

namespace vkn
{
    framebuffer::
        framebuffer(device* device, vk::RenderPass pass, uint32_t width, uint32_t height, uint32_t layers)
        : framebuffer(device, pass, vk::Extent2D(width, height), layers)
    {}

    framebuffer::framebuffer(device* device, vk::RenderPass pass, vk::Extent2D extent, uint32_t layers) : _device(device),
        _pass(pass),
        _extent(extent),
        _layers(layers)
    {
        _device->inc_ref();
    }

    framebuffer::~framebuffer()
    {
        if (_framebuffer) _device->destroyFramebuffer(_framebuffer);
        _device->dec_ref();
    }

    void framebuffer::set_attachments(std::vector<jpu::ref_ptr<TextureView>> attachments)
    {
        _attachments = attachments;
    }

    void framebuffer::finalize()
    {
        std::vector<vk::ImageView> attachments;
        attachments.reserve(_attachments.size());
        for (const auto attachment : _attachments)
            attachments.push_back(*attachment);

        vk::FramebufferCreateInfo create_info;
        create_info.setAttachmentCount(static_cast<uint32_t>(std::size(attachments)))
            .setPAttachments(std::data(attachments))
            .setWidth(_extent.width)
            .setHeight(_extent.height)
            .setLayers(_layers)
            .setRenderPass(_pass);
        _framebuffer = _device->createFramebuffer(create_info);
    }

    framebuffer::operator vk::Framebuffer() const
    {
        return _framebuffer;
    }
}
