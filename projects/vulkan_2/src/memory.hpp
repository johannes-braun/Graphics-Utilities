#pragma once

#include <vulkan/vulkan.hpp>

namespace vkx
{
    class block
    {
    public:
        size_t size() const noexcept;
        size_t offset() const noexcept;
        bool valid() const noexcept;
        bool mapped() const noexcept;
        void* data() const noexcept;
        vk::DeviceMemory memory() const noexcept;
        operator vk::DeviceMemory() const noexcept;
        
    private:
        block(size_t size, size_t offset, vk::DeviceMemory memory, void* mapped_data);
        void mark_free();

        size_t _size                    = 0;
        size_t _offset                  = 0;
        vk::DeviceMemory _device_memory = nullptr;
        void* _mapped_data              = nullptr;
    };

    class block_allocator
    {
    public:
        std::shared_ptr<block> allocate(const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags properties);
        void deallocate(const std::shared_ptr<block>& block);

    private:

    };
}