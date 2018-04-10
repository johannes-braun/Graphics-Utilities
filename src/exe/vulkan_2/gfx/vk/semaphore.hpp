#pragma once

#include "device.hpp"

#include <vulkan/vulkan.h>
#include <memory>

namespace gfx::vk
{
    class semaphore
    {
    public:
        semaphore(const std::shared_ptr<device>& device);
        ~semaphore();
        semaphore(const semaphore&) = delete;
        semaphore& operator=(const semaphore&) = delete;
        semaphore(semaphore&&) = default;
        semaphore& operator=(semaphore&&) = default;
        
        operator VkSemaphore() const noexcept;

    private:
        VkSemaphore _semaphore;
        std::shared_ptr<device> _device;
    };
}