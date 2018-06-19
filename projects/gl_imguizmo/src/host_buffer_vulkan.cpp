#include "context_vulkan.hpp"
#include "host_buffer_vulkan.hpp"

namespace gfx::vulkan
{
host_buffer_implementation::~host_buffer_implementation()
{
    if(_allocation)
        vmaFreeMemory(_allocator, _allocation);
}

std::byte* host_buffer_implementation::grow(const std::byte* old_data, const size_type old_size,
                                            const size_type new_capacity)
{
    vk::BufferCreateInfo create_info;

    auto& ctx  = context::current();
    auto  impl = static_cast<context_implementation*>(
            std::any_cast<detail::context_implementation*>(ctx->implementation()));

    std::vector<uint32_t> family_indices = impl->families({fam::transfer});
    create_info.pQueueFamilyIndices      = std::data(family_indices);
    create_info.queueFamilyIndexCount    = static_cast<uint32_t>(std::size(family_indices));
    create_info.sharingMode              = vk::SharingMode::eExclusive;
    create_info.size                     = new_capacity;
    create_info.usage =
            vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;

    _allocator = impl->allocator();

    VmaAllocation           new_allocation{0};
    VmaAllocationCreateInfo alloc_info{0};
    VmaAllocationInfo       alloc_result_info{0};
    alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    
    vk::UniqueBuffer       new_buffer = impl->device().createBufferUnique(create_info);
    vk::MemoryRequirements req = impl->device().getBufferMemoryRequirements(new_buffer.get());
    auto                   res = vmaAllocateMemory(_allocator,
                                 reinterpret_cast<VkMemoryRequirements*>(&req),
                                 &alloc_info,
                                 &new_allocation,
                                 &alloc_result_info);
    impl->device().bindBufferMemory(new_buffer.get(), alloc_result_info.deviceMemory, alloc_result_info.offset);

    std::byte* new_data = static_cast<std::byte*>(alloc_result_info.pMappedData);

    memcpy(new_data, old_data, std::min(old_size, new_capacity));

    if(_allocation)
        vmaFreeMemory(_allocator, _allocation);
    _allocation = new_allocation;
    _buffer     = std::move(new_buffer);

    return new_data;
}

} // namespace gfx
