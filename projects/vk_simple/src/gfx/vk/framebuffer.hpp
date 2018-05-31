#pragma once

#include "array_view.hpp"

#include <vulkan/vulkan.h>
#include <memory>

namespace vk
{
    class renderpass;
    class image_view;
    class framebuffer
    {
    public:
        framebuffer(const std::shared_ptr<renderpass>& renderpass, uint32_t width, uint32_t height, uint32_t layers, array_view<std::shared_ptr<image_view>> attachments);
        framebuffer(const std::shared_ptr<renderpass>& renderpass, uint32_t width, uint32_t height, array_view<std::shared_ptr<image_view>> attachments);
        ~framebuffer();
        framebuffer(const framebuffer&) = delete;
        framebuffer& operator=(const framebuffer&) = delete;
        framebuffer(framebuffer&&) = default;
        framebuffer& operator=(framebuffer&&) = default;

        operator VkFramebuffer() const noexcept;

    private:
        std::shared_ptr<renderpass> _renderpass;
        VkFramebuffer _framebuffer;
    };
}