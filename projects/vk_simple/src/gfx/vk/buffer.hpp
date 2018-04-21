#pragma once
#include <memory>
#include "image.hpp"
#include "array_view.hpp"
#include "memory.hpp"

namespace gfx::vk
{
    class device;
    class proxy_buffer
    {
    public:
        explicit proxy_buffer(const std::shared_ptr<device>& device, size_t size, VkBufferCreateFlags flags, VkSharingMode sharing_mode, VkBufferUsageFlags usage, array_view<uint32_t> queue_families = {}) noexcept;
        
        ~proxy_buffer();
        proxy_buffer(const proxy_buffer&) = delete;
        proxy_buffer& operator=(const proxy_buffer&) = delete;
        proxy_buffer(proxy_buffer&&) = default;
        proxy_buffer& operator=(proxy_buffer&&) = default;

        void bind_memory(const std::shared_ptr<memory_block>& memory) noexcept;
        VkMemoryRequirements requirements() const noexcept;
        VkBufferMemoryBarrier memory_barrier(size_t size, size_t offset, VkAccessFlags src_access, VkAccessFlags dst_access, uint32_t src_family, uint32_t dst_family) const noexcept;

        operator VkBuffer() const noexcept;

    private:
        VkBuffer _buffer;
        std::shared_ptr<device> _device;
        std::shared_ptr<memory_block> _memory;
    };
}
