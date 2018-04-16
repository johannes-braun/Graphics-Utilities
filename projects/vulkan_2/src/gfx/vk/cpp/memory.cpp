#include "../memory.hpp"
#include "../device.hpp"
#include "../physical_device.hpp"
#include <algorithm>

namespace gfx::vk
{
    memory_block::memory_block(chunk& parent, const size_t size, const size_t offset, VkDeviceMemory memory, void* mapped_data)
        : _parent_chunk(parent), _size(size), _offset(offset), _device_memory(memory), _mapped_data(mapped_data)
    {

    }

    memory_block::~memory_block()
    {
        _parent_chunk.notify_freed();
    }

    size_t memory_block::size() const noexcept
    {
        return _size;
    }

    size_t memory_block::offset() const noexcept
    {
        return _offset;
    }

    bool memory_block::valid() const noexcept
    {
        return _device_memory != nullptr;
    }

    bool memory_block::mapped() const noexcept
    {
        return _mapped_data != nullptr;
    }

    void* memory_block::data() const noexcept
    {
        return _mapped_data;
    }

    void memory_block::mark_free()
    {
        _device_memory = nullptr;
    }

    VkDeviceMemory memory_block::memory() const noexcept
    {
        return _device_memory;
    }

    memory_block::operator VkDeviceMemory() const noexcept
    {
        return memory();
    }

    block_allocator::block_allocator(const std::shared_ptr<device>& device, const size_t chunk_size)
        : _device(device), _alloc_size(chunk_size)
    {
        
    }

    chunk::chunk(const std::shared_ptr<device>& device, const VkMemoryAllocateInfo& info, VkMemoryPropertyFlags properties)
        : _device(device), _size(info.allocationSize), _offset(0)
    {
        vkAllocateMemory(*_device, &info, nullptr, &_device_memory);

        if ((properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
            vkMapMemory(*_device, _device_memory, 0, info.allocationSize, 0, &_mapped_data);

        _blocks.emplace_back();
    }

    chunk::~chunk()
    {
        if(_mapped_data) vkUnmapMemory(*_device, _device_memory);
        vkFreeMemory(*_device, _device_memory, nullptr);
    }

    std::shared_ptr<memory_block> chunk::try_allocate(const VkMemoryAllocateInfo& info, const VkMemoryRequirements& requirements)
    {
        for(auto it = _blocks.begin(); it != _blocks.end(); ++it)
        {
            if(it->expired())
            {
                size_t offset = it == _blocks.begin() ? 0 : [&]() {
                    auto ptr = std::prev(it)->lock();
                    return ptr->offset() + ptr->size();
                }();
                offset = offset + ((offset + requirements.alignment) % requirements.alignment);

                const size_t size = it == std::prev(_blocks.end()) ? _size - offset : std::next(it)->lock()->offset() - offset;
                if (size >= info.allocationSize)
                {
                    std::shared_ptr<memory_block> ptr(new memory_block(*this, info.allocationSize, offset, _device_memory, static_cast<uint8_t*>(_mapped_data)+offset));
                    it = _blocks.insert(it, ptr);
                    return ptr;
                }
            }
        }
        return nullptr;
    }

    void chunk::notify_freed()
    {
        for(auto it = _blocks.begin(); it != _blocks.end(); ++it)
        {
            while(it->expired())
            {
                const auto next = std::next(it);
                if (next == _blocks.end())
                    break;
                if (next->expired())
                    it = _blocks.erase(it);
                else 
                    ++it;
            }
        }
    }

    std::shared_ptr<memory_block> block_allocator::allocate(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties mem_props;
        vkGetPhysicalDeviceMemoryProperties(*_device->_physical_device, &mem_props);

        uint32_t memory_type = 0;
        for(int i=0; i<int(mem_props.memoryTypeCount); ++i)
        {
            if(requirements.memoryTypeBits & (1<<i) && mem_props.memoryTypes[i].propertyFlags & properties)
            {
                memory_type = i;
                break;
            }
        }

        VkMemoryAllocateInfo info;
        info.allocationSize = requirements.size;
        info.memoryTypeIndex = memory_type;
        info.pNext = nullptr;
        info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

        std::vector<std::unique_ptr<chunk>>& type_chunks = _chunks[properties];
        for(std::unique_ptr<chunk>& chunk_ptr : type_chunks)
        {
            if (const auto block_ptr = chunk_ptr->try_allocate(info, requirements))
                return block_ptr;
        }

        // No suitable chunk found. Create a new one.
        VkMemoryAllocateInfo chunk_info;
        chunk_info.allocationSize = _alloc_size;
        chunk_info.memoryTypeIndex = memory_type;
        chunk_info.pNext = nullptr;
        chunk_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        return type_chunks.emplace_back(new chunk(_device, chunk_info, properties))->try_allocate(info, requirements);
    }

    multi_block_allocator::multi_block_allocator(const std::shared_ptr<device>& device)
        : _small_allocator(device, 1<<10), _medium_allocator(device, 1<<20), _large_allocator(device, 1<<25)
    {
        
    }

    std::shared_ptr<memory_block> multi_block_allocator::allocate(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags properties)
    {
        if (std::clamp<size_t>(requirements.size, 0, 1<<10) == requirements.size)
            return _small_allocator.allocate(requirements, properties);
        if (std::clamp<size_t>(requirements.size, 1<<10, 1<<20) == requirements.size)
            return _medium_allocator.allocate(requirements, properties);
        if (std::clamp<size_t>(requirements.size, 1<<20, 1<<25) == requirements.size)
            return _large_allocator.allocate(requirements, properties);
        return nullptr;
    }
}
