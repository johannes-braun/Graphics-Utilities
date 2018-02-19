#pragma once

namespace vkn
{
    template<typename Container, typename>
    buffer::buffer(device* device, const Container& c, const vk::BufferUsageFlags usage, const vk::MemoryPropertyFlags memory_flags)
        : buffer(device, c.size() * sizeof(typename Container::value_type), usage, memory_flags)
    {
        assert((memory_flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible);
        assign(c);
    }

    template<typename Container, typename>
    void buffer::assign(const Container& c, const size_t offset)
    {
        if (!data())
            throw std::invalid_argument("Cannot assign to buffer with host-invisible memory.");
        memcpy(static_cast<uint8_t*>(data()) + offset, c.data(), c.size() * sizeof(typename Container::value_type));
    }
}