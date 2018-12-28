// Compile vma_allocator implementation in here.
#define VMA_IMPLEMENTATION
#include "vulkan/allocator.hpp"


namespace gfx {
inline namespace v1 {
namespace vulkan {
namespace detail {
void allocator_base::deleter::operator()(type* alloc) const noexcept
{
    vmaDestroyAllocator(alloc);
}
}    // namespace detail
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx