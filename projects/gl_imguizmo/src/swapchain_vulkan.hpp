#pragma once

#include "swapchain.hpp"
#include <vulkan/vulkan.hpp>

namespace gfx::vulkan
{
class swapchain_implementation : public detail::swapchain_implementation
{
public:
    swapchain_implementation();
    uint32_t current_image() const noexcept override;
    void     present() override;

private:
    vk::Device                           _device;
    vk::UniqueSwapchainKHR               _swapchain;
    std::vector<vk::Image>               _images;
    std::vector<vk::UniqueCommandBuffer> _primary_command_buffers;
    std::vector<vk::UniqueFence> _render_fences;
    vk::UniqueSemaphore                  _present_semaphore;
    vk::UniqueSemaphore                  _render_semaphore;
    vk::Queue                            _present_queue;
    vk::Queue                            _graphics_queue;

    int32_t _current_image = -1;
};
} // namespace gfx::vulkan