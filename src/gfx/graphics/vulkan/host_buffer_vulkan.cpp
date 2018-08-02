#include "context_vulkan.hpp"
#include "host_buffer_vulkan.hpp"
#include <unordered_set>

namespace gfx {
inline namespace v1 {
namespace vulkan {
host_buffer_implementation::allocation host_buffer_implementation::allocate(size_type size)
{
    VkBufferCreateInfo create_info{0};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    auto& ctx         = context::current();
    auto  impl        = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
	_device = impl->device();

    std::unordered_set<uint32_t> family_indices{impl->queue_families()[fam::transfer], impl->queue_families()[fam::graphics],
                                                impl->queue_families()[fam::compute]};
    std::vector<uint32_t>        fam(family_indices.begin(), family_indices.end());
    create_info.pQueueFamilyIndices   = std::data(fam);
    create_info.queueFamilyIndexCount = static_cast<uint32_t>(std::size(fam));
    create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    create_info.size                  = size;
    create_info.usage                 = flags;

    _allocator = impl->allocator();

    VmaAllocation           new_allocation{0};
    VmaAllocationCreateInfo alloc_info{0};
    VmaAllocationInfo       alloc_result_info{0};
    alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VkBuffer new_buffer;
	vkCreateBuffer(impl->device(), &create_info, nullptr, &new_buffer);
	VkMemoryRequirements req{ 0 };
    vkGetBufferMemoryRequirements(impl->device(), new_buffer, &req);
    auto                   res =
        vmaAllocateMemory(_allocator, &req, &alloc_info, &new_allocation, &alloc_result_info);
	vkBindBufferMemory(impl->device(), new_buffer, alloc_result_info.deviceMemory, alloc_result_info.offset);

	_last_buffer = new_buffer;

	allocation a;
	a.data = static_cast<std::byte*>(alloc_result_info.pMappedData);
	a.handle = std::make_pair(new_buffer, new_allocation);
	return a;
}

void host_buffer_implementation::deallocate(const allocation& alloc)
{
	if (alloc.handle.has_value()) {
        auto&&[buffer, last_alloc] = std::any_cast<std::pair<VkBuffer, VmaAllocation>>(alloc.handle);
		vmaFreeMemory(_allocator, last_alloc);
		vkDestroyBuffer(_device, buffer, nullptr);
    }
}

std::any host_buffer_implementation::api_handle()
{
    return _last_buffer;
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx