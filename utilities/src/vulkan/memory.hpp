#pragma once
#include <vulkan/vulkan.hpp>
#include <jpu/memory.hpp>
#include <map>
#include <list>

namespace vkn
{
    class device;
}

namespace vkn
{
    struct memory_block
    {
        size_t size = 0;
        size_t offset = 0;
        vk::DeviceMemory memory = nullptr;
        bool free = true;
        void* data = nullptr;
    };

    struct memory_chunk
    {
        size_t size = 0;
        vk::DeviceMemory memory = nullptr;
        std::list<std::unique_ptr<memory_block>> blocks;
        void* data = nullptr;
    };

    class memory_pool : public jpu::ref_count
    {
    public:
        using size_type = uint64_t;

        explicit memory_pool(device* device, size_type chunk_size = 1 << 25);
        ~memory_pool();

        const memory_block* allocate(size_type size, uint32_t filter, vk::MemoryPropertyFlags flags);
        void free(const memory_block* block);
        void clear();

        uint32_t memory_type(uint32_t filter, vk::MemoryPropertyFlags flags) const;
        
        uint32_t chunk_count() const;
        const std::map<uint32_t, std::list<memory_chunk>>& chunks() const;

    private:
        static memory_block* partition(vk::MemoryAllocateInfo info, memory_chunk& chunk, std::list<std::unique_ptr<memory_block>>::iterator block_position);
        void free_block(std::list<memory_chunk>& chunk_list, std::list<memory_chunk>::iterator c, std::list<std::unique_ptr<memory_block>>::iterator block_position) const;

        std::map<uint32_t, std::list<memory_chunk>> _chunks;
        device* _device;
        size_type _chunk_size;
    };
}
