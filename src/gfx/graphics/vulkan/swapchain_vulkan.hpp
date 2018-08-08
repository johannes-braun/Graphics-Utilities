#pragma once

#include "../general/handle.hpp"
#include "../swapchain.hpp"
#include <vulkan/vulkan.h>
#include "context_vulkan.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {
class swapchain_implementation : public detail::swapchain_implementation
{
public:
	~swapchain_implementation() override;

    uint32_t                         current_image() const noexcept override;
    void                             present() override;
    void                             resize(uint32_t width, uint32_t height) override;
    const std::vector<image_view>&   image_views() const override;
    const std::vector<device_image>& images() const override;
    handle                           api_handle() override;
    
private:
	context_implementation* _ctx_impl = nullptr;
    VkDevice                       _device            = nullptr;
    movable_handle<VkSwapchainKHR> _swapchain         = nullptr;
    movable_handle<VkSemaphore>    _present_semaphore = nullptr;
    movable_handle<VkSemaphore>    _render_semaphore  = nullptr;
    VkCommandPool                  _command_pool      = nullptr;
    VkQueue                        _present_queue     = nullptr;
    VkQueue                        _graphics_queue    = nullptr;
    bool                           _presented         = false;
    u32                            _current_image     = 0;
    std::vector<VkFence>           _render_fences;
    std::vector<VkCommandBuffer>   _primary_command_buffers;

    // TODO:
    std::vector<VkImage> _temp_images;
	std::vector<image_view> _image_views;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx