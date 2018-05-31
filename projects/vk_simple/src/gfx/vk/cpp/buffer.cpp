#include "../buffer.hpp"
#include "../device.hpp"

namespace vk
{
    proxy_buffer::proxy_buffer(const std::shared_ptr<device>& device, const size_t size, const VkBufferCreateFlags flags, 
        const VkSharingMode sharing_mode, const VkBufferUsageFlags usage, array_view<uint32_t> queue_families) noexcept
        : _device(device)
    {
        VkBufferCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.size = size;
        info.pQueueFamilyIndices = queue_families.data();
        info.queueFamilyIndexCount = uint32_t(queue_families.size());
        info.sharingMode = sharing_mode;
        info.usage = usage;
        vkCreateBuffer(*_device, &info, nullptr, &_buffer);
    }

    proxy_buffer::~proxy_buffer()
    {
        vkDestroyBuffer(*_device, _buffer, nullptr);
    }

    void proxy_buffer::bind_memory(const std::shared_ptr<memory_block>& memory) noexcept
    {
        _memory = memory;
    }

    VkMemoryRequirements proxy_buffer::requirements() const noexcept
    {
        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(*_device, _buffer, &req);
        return req;
    }

    proxy_buffer::operator VkBuffer() const noexcept
    {
        return _buffer;
    }

    VkBufferMemoryBarrier proxy_buffer::memory_barrier(const size_t size, const size_t offset, const VkAccessFlags src_access, const VkAccessFlags dst_access, const uint32_t src_family, const uint32_t dst_family) const noexcept
    {
        VkBufferMemoryBarrier b;
        b.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        b.pNext = nullptr;
        b.buffer = *this;
        b.size = size;
        b.offset = offset;
        b.srcAccessMask = src_access;
        b.dstAccessMask = dst_access;
        b.srcQueueFamilyIndex = src_family;
        b.dstQueueFamilyIndex = dst_family;
        return b;
    }
}
