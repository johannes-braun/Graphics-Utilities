#pragma once

#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
namespace vulkan {
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

    [[nodiscard]] auto get_semaphore() const noexcept -> const vk::Semaphore&;

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

    [[nodiscard]] auto get_fence() const noexcept -> const vk::Fence&;

private:
    vk::UniqueFence _fence;
};

class event
{
public:
    explicit event(device& dev);
    event(event const& other);
    event& operator               =(event const& other) noexcept;
    event(event&& other) noexcept = default;
    event& operator=(event&& other) noexcept = default;
    ~event()                                 = default;

    [[nodiscard]] auto get_event() const noexcept -> const vk::Event&;

private:
    vk::UniqueEvent _event;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx