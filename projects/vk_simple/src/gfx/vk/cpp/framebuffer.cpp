#include "../framebuffer.hpp"
#include "../image.hpp"
#include "../renderpass.hpp"
#include "../device.hpp"

#include <vector>

namespace vk
{
    framebuffer::framebuffer(const std::shared_ptr<renderpass>& renderpass, uint32_t width, uint32_t height, uint32_t layers, array_view<std::shared_ptr<image_view>> attachments)
        : _renderpass(renderpass)
    {
        std::vector<VkImageView> att(attachments.size());
        for (int i=0; i<att.size(); ++i) att[i] = *(attachments.data()[i]);

        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.attachmentCount = uint32_t(att.size());
        info.pAttachments = att.data();
        info.width = width;
        info.height = height;
        info.layers = layers;
        info.renderPass = *_renderpass;
        vkCreateFramebuffer(*(_renderpass->_device), &info, nullptr, &_framebuffer);
    }

    framebuffer::framebuffer(const std::shared_ptr<renderpass>& renderpass, uint32_t width, uint32_t height, array_view<std::shared_ptr<image_view>> attachments)
        : framebuffer(renderpass, width, height, 1, attachments)
    {
    }

    framebuffer::~framebuffer()
    {
        vkDestroyFramebuffer(*(_renderpass->_device), _framebuffer, nullptr);
    }

    framebuffer::operator VkFramebuffer() const noexcept
    {
        return _framebuffer;
    }
}