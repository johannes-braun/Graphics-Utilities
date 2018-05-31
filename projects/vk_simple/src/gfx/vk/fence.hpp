#pragma once

#include <vulkan/vulkan.h>
#include <memory>

namespace vk
{
    class device;
    class fence
    {
    public:
        fence(const std::shared_ptr<device>& device, VkFenceCreateFlags flags = 0);
        ~fence();
        fence(const fence& other);
        fence& operator=(const fence& other);
        fence(fence&&) = default;
        fence& operator=(fence&&) = default;

        operator VkFence() const noexcept;
        
        void wait(uint64_t timeout = std::numeric_limits<uint64_t>::max()) const noexcept;
        void reset() const noexcept;

    private:
        std::shared_ptr<device> _device;
        VkFence _fence = nullptr;
    };
}