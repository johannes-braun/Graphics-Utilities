#pragma once

#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
class device;

class semaphore
{
public:
    explicit semaphore(device& dev);
    semaphore(semaphore const& other);
    semaphore& operator                   =(semaphore const& other) noexcept;
    semaphore(semaphore&& other) noexcept = default;
    semaphore& operator=(semaphore&& other) noexcept = default;
    ~semaphore()                                     = default;

    const vk::Semaphore& get_semaphore() const noexcept;

private:
    vk::UniqueSemaphore _sem;
};

class fence
{
public:
    explicit fence(device& dev, bool signaled = false);
    fence(fence const& other);
    fence& operator               =(fence const& other) noexcept;
    fence(fence&& other) noexcept = default;
    fence& operator=(fence&& other) noexcept = default;
    ~fence()                                 = default;

    const vk::Fence& get_fence() const noexcept;

private:
    vk::UniqueFence _fence;
};
}    // namespace v1
}    // namespace gfx