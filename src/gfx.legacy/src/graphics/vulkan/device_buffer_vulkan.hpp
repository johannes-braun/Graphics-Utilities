#pragma once

#include <graphics/device_buffer.hpp>
#include "../general/handle.hpp"
#include "vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {
class device_buffer_implementation : public detail::device_buffer_implementation
{
public:
    void     allocate(size_type size) override;
    void     copy(const std::any& source, const std::any& target, difference_type src_offset, difference_type dst_offset, size_type size,
                  fence* f) override;
    void     update(difference_type offset, size_type size, const std::byte* data) override;
    std::any api_handle() override;

    void update_flags(buffer_usage_flags usage) override;
    ~device_buffer_implementation() override;

private:
    VkDevice           _device   = nullptr;
    VkBufferUsageFlags _usage_vk = 0;

    movable_handle<VkBuffer>      _buffer     = nullptr;
    movable_handle<VmaAllocation> _allocation = nullptr;
    VmaAllocator                  _allocator  = nullptr;

    int                          _current_cmd  = 0;
    VkCommandPool                _command_pool = nullptr;
    std::vector<VkCommandBuffer> _transfer_commands;
    std::vector<VkFence>         _transfer_fences;
    VkQueue                      _transfer_queue = nullptr;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx