#pragma once
#include "vulkan/vulkan.hpp"
#include "devices.hpp"
#include <numeric>
#include <map>
#include <jpu/memory>
#include "create_info.hpp"

namespace vkn
{
    class LogicalDevice;
}

namespace vkn
{

    struct MemoryPoolCreateInfo
    {
        MemoryPoolCreateInfo(LogicalDevice* device = nullptr, const vk::DeviceSize default_chunk_size = 1 << 25)
            : device(device), default_chunk_size(default_chunk_size) {}

        MemoryPoolCreateInfo& setDevice(LogicalDevice* value) { device = value; return *this; }
        MemoryPoolCreateInfo& setDefaultChunkSize(const vk::DeviceSize value) { default_chunk_size = value; return *this; }

        LogicalDevice* device;
        vk::DeviceSize default_chunk_size;
    };

	struct Block
	{
		vk::DeviceSize size = 0;
		vk::DeviceSize offset = 0;
		vk::DeviceMemory memory = nullptr;
		bool free = true;
        void* data = nullptr;
	};

	struct Chunk
	{
		vk::DeviceSize size = 0;
		vk::DeviceMemory memory = nullptr;
		std::list<std::unique_ptr<Block>> blocks;
        void* base_data = nullptr;
	};

	using MemoryBlock = const Block*;

	class MemoryPool : ClassInfo<MemoryPoolCreateInfo, MemoryPool>, public jpu::ref_count
	{
	public:
		explicit MemoryPool(const MemoryPoolCreateInfo& info);
		~MemoryPool();

		MemoryBlock allocate(vk::DeviceSize size, uint32_t filter, vk::MemoryPropertyFlags flags);
	    uint32_t typeIndex(uint32_t filter, vk::MemoryPropertyFlags flags) const;

		void free(MemoryBlock block);
		void freeAll();

        uint32_t allocatedChunkCount() const;
        const std::map<uint32_t, std::list<Chunk>>& allocatedChunks() const;

	private:
		static Block* partitionBlock(vk::MemoryAllocateInfo info, Chunk& chunk, std::list<std::unique_ptr<Block>>::iterator block_position);
		void freeBlock(std::list<Chunk>& chunk_list, std::list<Chunk>::iterator c, std::list<std::unique_ptr<Block>>::iterator block_position) const;

		std::map<uint32_t, std::list<Chunk>> m_chunks;
	};
}
