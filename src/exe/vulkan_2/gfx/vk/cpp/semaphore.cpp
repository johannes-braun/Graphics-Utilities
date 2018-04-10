#include "../semaphore.hpp"

namespace gfx::vk
{
    semaphore::semaphore(const std::shared_ptr<device>& device)
        : _device(device)
    {
        VkSemaphoreCreateInfo info;
        info.flags = 0;
        info.pNext = nullptr;
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(*_device, &info, nullptr, &_semaphore);
    }

    semaphore::~semaphore()
    {
        vkDestroySemaphore(*_device, _semaphore, nullptr);
    }

    semaphore::operator VkSemaphore() const noexcept
    {
        return _semaphore;
    }
}