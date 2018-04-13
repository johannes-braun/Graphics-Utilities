#include "../memory.hpp"
#include "../device.hpp"

namespace vkn
{
    memory_pool::memory_pool(device* device, const size_type chunk_size)
        : _device(device), _chunk_size(chunk_size)
    {

    }

    memory_pool::~memory_pool()
    {
        clear();
    }

    const memory_block* memory_pool::allocate(const size_type size, const uint32_t filter, const vk::MemoryPropertyFlags flags)
    {
        vk::MemoryAllocateInfo info(static_cast<uint32_t>(std::ceil(size / 1024.f) * 1024.f), memory_type(filter, flags));
        const bool should_map = (flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible;

        if (_chunks.count(info.memoryTypeIndex) != 0 && !_chunks[info.memoryTypeIndex].empty())
        {
            auto&& chunk_list = _chunks[info.memoryTypeIndex];

            for (auto&& chunk : chunk_list)
            {
                if (const auto it = std::find_if(chunk.blocks.begin(), chunk.blocks.end(), [&info](const auto& block)
                {
                    return block->free && block->size >= info.allocationSize;
                }); it != chunk.blocks.end())
                {
                    const auto block = partition(info, chunk, it);
                    if (should_map)
                        block->data = static_cast<uint8_t*>(chunk.data) + block->offset;
                    return block;
                }
            }
        }

        // No free or suitable chunk found. creating a new one.
        memory_chunk& chunk = _chunks[info.memoryTypeIndex].emplace_back();
        chunk.size = std::max(_chunk_size, info.allocationSize);
        chunk.memory = _device->allocateMemory(vk::MemoryAllocateInfo(chunk.size, info.memoryTypeIndex));
        if (should_map)
            chunk.data = _device->mapMemory(chunk.memory, 0, chunk.size);

        memory_block* block = chunk.blocks.emplace_back(std::make_unique<memory_block>()).get();
        block->size = chunk.size;
        block->free = true;
        block->memory = chunk.memory;
        block->offset = 0;

        const auto partitioned_block = partition(info, chunk, chunk.blocks.begin());
        if (should_map)
            partitioned_block->data = static_cast<uint8_t*>(chunk.data) + partitioned_block->offset;
        return partitioned_block;
    }

    void memory_pool::free(const memory_block* block)
    {
        for (auto&& list : _chunks)
            for (auto chunk = list.second.begin(); chunk != list.second.end(); ++chunk)
                for (auto it = chunk->blocks.begin(); it != chunk->blocks.end(); ++it)
                    if ((*it).get() == block)
                    {
                        free_block(list.second, chunk, it);
                        return;
                    }
    }

    void memory_pool::clear()
    {
        for (auto&& chunk_list : _chunks)
            for (auto&& chunk : chunk_list.second)
                _device->freeMemory(chunk.memory);

        _chunks.clear();
    }

    uint32_t memory_pool::memory_type(const uint32_t filter, const vk::MemoryPropertyFlags flags) const
    {
        const auto memory_properties = _device->gpu().getMemoryProperties();
        for (auto i = 0; i < static_cast<int>(memory_properties.memoryTypeCount); ++i)
            if (filter & (1 << i) && memory_properties.memoryTypes[i].propertyFlags & flags)
                return i;

        throw std::runtime_error("Could not find a suitable memory type index");
    }

    uint32_t memory_pool::chunk_count() const
    {
        return static_cast<uint32_t>(_chunks.size());
    }

    const std::map<uint32_t, std::list<memory_chunk>>& memory_pool::chunks() const
    {
        return _chunks;
    }

    memory_block* memory_pool::partition(const vk::MemoryAllocateInfo info, memory_chunk& chunk,
        const std::list<std::unique_ptr<memory_block>>::iterator block_position)
    {
        if (info.allocationSize == block_position->get()->size)
        {
            block_position->get()->free = false;
            return block_position->get();
        }
        const auto new_block_it = chunk.blocks.emplace(block_position, std::make_unique<memory_block>());
        const auto old_block_it = std::next(new_block_it);

        memory_block* new_block = (*new_block_it).get();
        memory_block* old_block = (*old_block_it).get();

        new_block->size = info.allocationSize;
        new_block->free = false;
        new_block->memory = chunk.memory;
        new_block->offset = old_block->offset;

        old_block->size -= info.allocationSize;
        old_block->offset += info.allocationSize;
        return new_block;
    }

    void memory_pool::free_block(std::list<memory_chunk>& chunk_list, const std::list<memory_chunk>::iterator c,
        std::list<std::unique_ptr<memory_block>>::iterator block_position) const
    {
        block_position->get()->free = true;
        auto&& chunk = *c;

        // Optimize adjacent free blocks
        if (block_position != chunk.blocks.begin())
        {
            while (block_position->get()->free || block_position->get()->size == 0)
            {
                if (block_position == chunk.blocks.begin())
                    break;

                block_position = std::prev(block_position);
            }
        }
        else if (chunk.size != _chunk_size)
        {
            _device->freeMemory(chunk.memory);
            chunk_list.erase(c);
            return;
        }

        if (!block_position->get()->free)
            block_position = std::next(block_position);

        for (auto it = std::next(block_position); it != chunk.blocks.end() && (it->get()->free || it->get()->size == 0);)
        {
            // offset is same as before.
            const auto prev = std::prev(it);
            prev->get()->size += it->get()->size;
            it = chunk.blocks.erase(it);
        }
        std::prev(chunk.blocks.end())->get()->free = true;
    }
}
