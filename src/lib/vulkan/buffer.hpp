#pragma once

#include "device.hpp"
#include <vulkan/vulkan.hpp>
#include <jpu/memory>

namespace vkn
{
    class buffer : public jpu::ref_count
    {
    public:

        template<typename Container, typename = std::void_t<typename Container::value_type,
            decltype(std::declval<Container>().data()),
            decltype(std::declval<Container>().size())>>
            buffer(device* device, const Container& c, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_flags);

        buffer(device* device, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_flags);
        ~buffer();

        // other, size, src_offset, dst_offset
        void copy_to(buffer* other, size_t size = ~0, size_t src_offset = 0, size_t dst_offset = 0, vk::CommandBuffer cbuf = nullptr);

        void* data();
        const void* data() const;
        size_t size() const;
        operator vk::Buffer() const;

        template<typename Container, typename = std::void_t<typename Container::value_type, decltype(std::declval<Container>().data()), decltype(std::declval<Container>().size())>>
        void assign(const Container& c, size_t offset = 0);


    private:
        device* _device;
        vk::BufferUsageFlags _usage;
        vk::Buffer _buffer;
        vk::MemoryPropertyFlags _memory_flags;
        vk::MemoryRequirements _requirements;
        const memory_block* _memory_block;
    };

    template<typename Container, typename>
    buffer::buffer(device* device, const Container& c, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_flags)
        : buffer(device, c.size() * sizeof(typename Container::value_type), usage, memory_flags)
    {
        assert((memory_flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible);
        assign(c);
    }

    template<typename Container, typename>
    void buffer::assign(const Container& c, size_t offset)
    {
        if (!data())
            throw std::invalid_argument("Cannot assign to buffer with host-invisible memory.");
        memcpy(static_cast<uint8_t*>(data()) + offset, c.data(), c.size() * sizeof(typename Container::value_type));
    }
}