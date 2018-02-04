#include "framebuffer.hpp"

namespace vkn
{
    Framebuffer::Framebuffer(const FramebufferCreateInfo& info): ClassInfo(info)
    {
        m_info.device->inc_ref();

        std::vector<vk::ImageView> attachments;
        attachments.reserve(m_info.attachments.size());
        for(const auto attachment : m_info.attachments)
            attachments.push_back(*attachment);

        vk::FramebufferCreateInfo create_info;
        create_info.setAttachmentCount(static_cast<uint32_t>(std::size(attachments)))
            .setPAttachments(std::data(attachments))
            .setWidth(m_info.extent.width)
            .setHeight(m_info.extent.height)
            .setLayers(m_info.layers)
            .setRenderPass(m_info.renderpass);
        m_framebuffer = m_info.device->createFramebuffer(create_info);
    }

    Framebuffer::~Framebuffer()
    {
        m_info.device->destroyFramebuffer(m_framebuffer);
        m_info.device->dec_ref();
    }

    Framebuffer::operator vk::Framebuffer() const
    {
        return m_framebuffer;
    }
}
