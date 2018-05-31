#include "../fence.hpp"
#include "../device.hpp"
#include <cassert>

namespace vk
{
    fence::fence(const std::shared_ptr<device>& device, VkFenceCreateFlags flags)
        : _device(device)
    {
        VkFenceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;

        vkCreateFence(*_device, &info, nullptr, &_fence);
    }

    fence::~fence()
    {
        vkDestroyFence(*_device, _fence, nullptr);
    }

    fence::fence(const fence& other)
    {
        operator=(other);
    }

    fence& fence::operator=(const fence& other)
    {
        _device = other._device;
        if (_fence)
            vkDestroyFence(*_device, _fence, nullptr);

        VkFenceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = vkGetFenceStatus(*_device, other._fence) == VK_SUCCESS ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        
        vkCreateFence(*_device, &info, nullptr, &_fence);
        return *this;
    }

    fence::operator VkFence() const noexcept
    {
        return _fence;
    }

    void fence::wait(uint64_t timeout) const noexcept
    {
        vkWaitForFences(*_device, 1, &_fence, true, timeout);
    }

    void fence::reset() const noexcept
    {
        vkResetFences(*_device, 1, &_fence);
    }
}