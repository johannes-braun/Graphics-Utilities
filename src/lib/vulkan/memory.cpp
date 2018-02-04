#include "memory.hpp"
#include "vulkan/logical_device.hpp"

namespace vkn
{
    MemoryPool::MemoryPool(const MemoryPoolCreateInfo& info)
        : ClassInfo(info)
    {
    }

    uint32_t MemoryPool::typeIndex(const uint32_t filter, const vk::MemoryPropertyFlags flags) const
    {
		const auto memory_properties = m_info.device->gpu().getMemoryProperties();
		for (auto i = 0; i < static_cast<int>(memory_properties.memoryTypeCount); ++i)
			if (filter & (1 << i) && memory_properties.memoryTypes[i].propertyFlags & flags)
				return i;

        throw std::runtime_error("Could not find a suitable memory type index");
    }

	MemoryPool::~MemoryPool()
	{
		freeAll();
	}

	MemoryBlock MemoryPool::allocate(const vk::DeviceSize size, const uint32_t filter, const vk::MemoryPropertyFlags flags)
	{
        vk::MemoryAllocateInfo info(static_cast<uint32_t>(std::ceil(size / 1024.f) * 1024.f), typeIndex(filter, flags));
        bool should_map = (flags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible;

		if (m_chunks.count(info.memoryTypeIndex) != 0 && !m_chunks[info.memoryTypeIndex].empty())
		{
			auto&& chunk_list = m_chunks[info.memoryTypeIndex];

			for (auto&& chunk : chunk_list)
			{
				if (auto it = std::find_if(chunk.blocks.begin(), chunk.blocks.end(), [&info](const auto& block) { return block->free && block->size >= info.allocationSize; }); it != chunk.blocks.end())
				{
					auto block = partitionBlock(info, chunk, it);
                    if(should_map)
                        block->data = static_cast<uint8_t*>(chunk.base_data) + block->offset;
                    return block;
				}
			}
		}

		// No free or suitable chunk found. creating a new one.
		Chunk& chunk = m_chunks[info.memoryTypeIndex].emplace_back();
		chunk.size = std::max(m_info.default_chunk_size, info.allocationSize);
		chunk.memory = m_info.device->allocateMemory(vk::MemoryAllocateInfo(chunk.size, info.memoryTypeIndex));
        if(should_map)
            chunk.base_data = m_info.device->mapMemory(chunk.memory, 0, chunk.size);

		Block* block = chunk.blocks.emplace_back(std::make_unique<Block>()).get();
		block->size = chunk.size;
		block->free = true;
		block->memory = chunk.memory;
		block->offset = 0;

		auto partitioned_block = partitionBlock(info, chunk, chunk.blocks.begin());
        if(should_map)
            partitioned_block->data = static_cast<uint8_t*>(chunk.base_data) + partitioned_block->offset;
        return partitioned_block;
	}

	void MemoryPool::free(const MemoryBlock block)
	{
		for (auto&& list : m_chunks)
			for (auto chunk =list.second.begin(); chunk != list.second.end(); ++chunk)
				for (auto it = chunk->blocks.begin(); it != chunk->blocks.end(); ++it)
					if ((*it).get() == block)
                    {
						freeBlock(list.second, chunk, it);
                        return;
                    }
	}

	void MemoryPool::freeAll()
	{
		for (auto&& chunk_list : m_chunks)
			for (auto&& chunk : chunk_list.second)
				m_info.device->freeMemory(chunk.memory);

		m_chunks.clear();
	}

    uint32_t MemoryPool::allocatedChunkCount() const
    {
        return static_cast<uint32_t>(m_chunks.size());
    }

    const std::map<uint32_t, std::list<Chunk>>& MemoryPool::allocatedChunks() const
    {
        return m_chunks;
    }

    Block* MemoryPool::partitionBlock(const vk::MemoryAllocateInfo info, Chunk& chunk,
		const std::list<std::unique_ptr<Block>>::iterator block_position)
	{
        if(info.allocationSize == block_position->get()->size)
        {
            block_position->get()->free = false;
            return block_position->get();
        }
		const auto new_block_it = chunk.blocks.emplace(block_position, std::make_unique<Block>());
		const auto old_block_it = std::next(new_block_it);

		Block* new_block = (*new_block_it).get();
		Block* old_block = (*old_block_it).get();

		new_block->size = info.allocationSize;
		new_block->free = false;
		new_block->memory = chunk.memory;
		new_block->offset = old_block->offset;

		old_block->size -= info.allocationSize;
		old_block->offset += info.allocationSize;
		return new_block;
	}

	void MemoryPool::freeBlock(std::list<Chunk>& chunk_list, std::list<Chunk>::iterator c, std::list<std::unique_ptr<Block>>::iterator block_position) const
	{
        block_position->get()->free = true;
        auto&& chunk = *c;

        // Optimize adjacent free blocks
        if(block_position != chunk.blocks.begin())
        {
            while(block_position->get()->free || block_position->get()->size == 0)
            {
                if(block_position == chunk.blocks.begin())
                    break;

                block_position = std::prev(block_position);
            }
        } 
        else if(chunk.size != m_info.default_chunk_size)
        {
            m_info.device->freeMemory(chunk.memory);
            chunk_list.erase(c);
            return;
        }

        if(!block_position->get()->free)
            block_position = std::next(block_position);

        for(auto it = std::next(block_position); it != chunk.blocks.end() && (it->get()->free || it->get()->size == 0);)
        {
            // offset is same as before.
            const auto prev = std::prev(it);
            prev->get()->size += it->get()->size;
            it = chunk.blocks.erase(it);
        }
        std::prev(chunk.blocks.end())->get()->free = true;
	}
}
