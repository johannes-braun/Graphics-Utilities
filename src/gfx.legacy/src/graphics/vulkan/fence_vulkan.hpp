#pragma once

#include <graphics/fence.hpp>
#include "../general/handle.hpp"
#include <vulkan/vulkan.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {

class fence_implementation : public detail::fence_implementation
{
public:
    ~fence_implementation() override;

    void     wait(std::chrono::nanoseconds timeout) override;
    std::any api_handle() override;

private:
    VkDevice                _device;
    movable_handle<VkFence> _fence;
};

}    // namespace vulkan
}    // namespace v1
}    // namespace gfx
