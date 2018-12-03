#pragma once

#include <graphics/host_buffer.hpp>
#if defined(NOMINMAX)
#define GFX_REDEFINE_NOMINMAX
#undef NOMINMAX
#endif // NOMINMAX
#include <vk_mem_alloc.h>
#if defined(GFX_REDEFINE_NOMINMAX)
#define NOMINMAX
#undef GFX_REDEFINE_NOMINMAX
#endif // GFX_REDEFINE_NOMINMAX
#include <vulkan/vulkan.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {
class host_buffer_implementation : public detail::host_buffer_implementation
{
public:
    constexpr static VkBufferUsageFlags flags =
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT
        | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
        | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

    allocation allocate(size_type size) override;
    void       deallocate(const allocation& alloc) override;
    std::any   api_handle() override;

    ~host_buffer_implementation() override;
private:
	movable_handle<uint32_t> _alloc_dealloc = 0;
    VkBuffer     _last_buffer = nullptr;
    VmaAllocator _allocator   = nullptr;
    VkDevice     _device      = nullptr;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx