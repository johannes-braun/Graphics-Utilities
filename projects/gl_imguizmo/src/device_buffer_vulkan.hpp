#pragma once

#include "device_buffer.hpp"
#include "vulkan_alloc.hpp"
#include <vulkan/vulkan.hpp>

namespace gfx::vulkan
{
class device_buffer_implementation : public detail::device_buffer_implementation
{
public:
    ~device_buffer_implementation();
    void     update_flags(buffer_usage_flags usage) override;
    void     allocate(size_type size) override;
    void     copy(const std::any& source, const std::any& target, difference_type src_offset,
                  difference_type dst_offset, size_type size) override;
    void     update(difference_type offset, size_type size, const std::byte* data) override;
    std::any api_handle() override;

private:
    vk::Device           _device;
    vk::BufferUsageFlags _usage_vk;

    VkBuffer      _buffer     = nullptr;
    VmaAllocation _allocation = nullptr;
    VmaAllocator  _allocator;

    vk::UniqueCommandBuffer _transfer_command;
    vk::Queue               _transfer_queue;
    vk::Fence               _transfer_fence;
};
}