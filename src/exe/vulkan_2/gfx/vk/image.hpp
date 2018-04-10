#pragma once

#include <vulkan/vulkan.h>
#include <memory>

namespace gfx::vk
{
    class swapchain;
    class device;
    class image
    {
    public:
        friend swapchain;
        ~image();

    private:
        image() = default;
        VkImage _image;
        std::shared_ptr<device> _device;
        bool _owning = true;
    };
}