#include "vulkan/sync.hpp"
#include "vulkan/device.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {
semaphore::semaphore(device& dev) : _sem(dev.get_device().createSemaphoreUnique({})) {}

semaphore::semaphore(const semaphore& other) : _sem(other._sem.getOwner().createSemaphoreUnique({})) {}

semaphore& semaphore::operator=(const semaphore& other) noexcept
{
    _sem = other._sem.getOwner().createSemaphoreUnique({});
    return *this;
}

const vk::Semaphore& semaphore::get_semaphore() const noexcept
{
    return _sem.get();
}

fence::fence(device& dev, bool signaled)
      : _fence(dev.get_device().createFenceUnique({signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags {}}))
{}

fence::fence(const fence& other)
      : _fence(other._fence.getOwner().createFenceUnique({other._fence.getOwner().getFenceStatus(other._fence.get()) == vk::Result::eSuccess
                                                              ? vk::FenceCreateFlagBits::eSignaled
                                                              : vk::FenceCreateFlags {}}))
{}

fence& fence::operator=(const fence& other) noexcept
{
    _fence = other._fence.getOwner().createFenceUnique({other._fence.getOwner().getFenceStatus(other._fence.get()) == vk::Result::eSuccess
                                                            ? vk::FenceCreateFlagBits::eSignaled
                                                            : vk::FenceCreateFlags {}});
    return *this;
}

const vk::Fence& fence::get_fence() const noexcept
{
    return _fence.get();
}

event::event(device& dev) : _event(dev.get_device().createEventUnique(vk::EventCreateInfo())) {}

event::event(event const& other) : _event(other._event.getOwner().createEventUnique(vk::EventCreateInfo())) {}

event& event::operator=(event const& other) noexcept
{
    _event = other._event.getOwner().createEventUnique(vk::EventCreateInfo());
    return *this;
}

auto event::get_event() const noexcept -> const vk::Event&
{
    return _event.get();
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx