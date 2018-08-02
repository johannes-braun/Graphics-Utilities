#pragma once

#include "../general/handle.hpp"
#include "../swapchain.hpp"
#include <vulkan/vulkan.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {
class swapchain_implementation : public detail::swapchain_implementation
{
public:
    uint32_t                         current_image() const noexcept override;
    void                             present() override;
    void                             resize(uint32_t width, uint32_t height) override;
    const std::vector<image_view>&   image_views() const override;
    const std::vector<device_image>& images() const override;
    handle                           api_handle() override;

    ~swapchain_implementation() override;

private:
    bool                           _presented = false;
    VkDevice                       _device    = nullptr;
    movable_handle<VkSwapchainKHR> _swapchain = nullptr;
    std::vector<VkFence>           _render_fences;
    std::vector<VkCommandBuffer>   _primary_command_buffers;
    movable_handle<VkSemaphore>    _present_semaphore = nullptr;
    movable_handle<VkSemaphore>    _render_semaphore  = nullptr;
	VkCommandPool _command_pool = nullptr;
    VkQueue                        _present_queue     = nullptr;
    VkQueue                        _graphics_queue    = nullptr;
    u32                            _current_image     = 0;

    // TODO:
    std::vector<VkImage> _temp_images;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx