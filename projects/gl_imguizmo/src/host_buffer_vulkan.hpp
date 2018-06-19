#pragma once
#include "host_buffer.hpp"
#include "vulkan_alloc.hpp"
#include <vulkan/vulkan.hpp>

namespace gfx::vulkan
{
class host_buffer_implementation : public detail::host_buffer_implementation
{
public:
    ~host_buffer_implementation();
    std::byte* grow(const std::byte* old_data, size_type old_size, size_type new_capacity) override;

    vk::Buffer buffer() const { return _buffer.get(); }

private:
    vk::UniqueBuffer _buffer;
    VmaAllocation    _allocation = nullptr;
    VmaAllocator     _allocator  = nullptr;
};
} // namespace gfx
