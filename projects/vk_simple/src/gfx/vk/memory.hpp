#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <unordered_map>

namespace vk
{
    class device;

    class chunk;
    class memory_block : public std::enable_shared_from_this<memory_block>
    {
    public:
        friend chunk;
        ~memory_block();

        size_t size() const noexcept;
        size_t offset() const noexcept;
        bool valid() const noexcept;
        bool mapped() const noexcept;
        void* data() const noexcept;
        VkDeviceMemory memory() const noexcept;
        operator VkDeviceMemory() const noexcept;
        
    private:
        memory_block(chunk& parent, size_t size, size_t offset, VkDeviceMemory memory, void* mapped_data);
        void mark_free();

        chunk& _parent_chunk;
        size_t _size                    = 0;
        size_t _offset                  = 0;
        VkDeviceMemory _device_memory   = nullptr;
        void* _mapped_data              = nullptr;
    };

    class chunk
    {
    public:
        chunk(const std::shared_ptr<device>& device, const VkMemoryAllocateInfo& info, VkMemoryPropertyFlags properties);
        ~chunk();
        std::shared_ptr<memory_block> try_allocate(const VkMemoryAllocateInfo& info, const VkMemoryRequirements& requirements);
        void notify_freed();

    private:
        std::shared_ptr<device> _device;
        std::vector<std::weak_ptr<memory_block>> _blocks;
        size_t _size                    = 0;
        size_t _offset                  = 0;
        VkDeviceMemory _device_memory   = nullptr;
        void* _mapped_data              = nullptr;
    };

    class block_allocator
    {
    public:
        block_allocator(const std::shared_ptr<device>& device, size_t chunk_size);
        std::shared_ptr<memory_block> allocate(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags properties);

    private:
        std::shared_ptr<device> _device;
        size_t _alloc_size;
        std::unordered_map<VkMemoryPropertyFlags, std::vector<std::unique_ptr<chunk>>> _chunks;
    };

    class multi_block_allocator
    {
    public:
        explicit multi_block_allocator(const std::shared_ptr<device>& device);
        std::shared_ptr<memory_block> allocate(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags properties);

    private:
        block_allocator _small_allocator;
        block_allocator _medium_allocator;
        block_allocator _large_allocator;
    };
}
