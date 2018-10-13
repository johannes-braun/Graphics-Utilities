#include "sync.hpp"
#include "device.hpp"

namespace gfx
{
    inline namespace v1
    {

    semaphore::semaphore(device& dev)
        : _sem(dev.dev().createSemaphoreUnique({}))
    {
    }

    const vk::Semaphore& semaphore::sem() const noexcept
    {
        return _sem.get();
    }

    fence::fence(device& dev, bool signaled)
        : _fence(dev.dev().createFenceUnique({signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags{}}))
    {
    }

    const vk::Fence& fence::fen() const noexcept
    {
        return _fence.get();
    }
    }
}