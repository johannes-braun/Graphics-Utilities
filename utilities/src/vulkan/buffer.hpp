#pragma once

#include "device.hpp"
#include <vulkan/vulkan.hpp>
#include <jpu/memory.hpp>

namespace vkn
{
    class buffer : public jpu::ref_count
    {
    public:

        template<typename Container, typename = std::void_t<typename Container::value_type,
            decltype(std::declval<Container>().data()),
            decltype(std::declval<Container>().size())>
        >
        buffer(device* device, const Container& c, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_flags);

        buffer(device* device, size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memory_flags);
        ~buffer();

        void copy_to(buffer* other, size_t size = ~0, size_t src_offset = 0, size_t dst_offset = 0, vk::CommandBuffer cbuf = nullptr) const;

        void* data();
        const void* data() const;
        size_t size() const;
        operator vk::Buffer() const;

        template<typename Container, typename = std::void_t<typename Container::value_type, decltype(std::declval<Container>().data()), decltype(std::declval<Container>().size())>>
        void assign(const Container& c, size_t offset = 0);


    private:
        device * _device;
        vk::BufferUsageFlags _usage;
        vk::Buffer _buffer;
        vk::MemoryPropertyFlags _memory_flags;
        vk::MemoryRequirements _requirements;
        const memory_block* _memory_block;
    };
}

#include "impl/buffer.inl"