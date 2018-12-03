// Compile vma_allocator implementation in here.
#define VMA_IMPLEMENTATION
#include "allocator.hpp"


namespace gfx {
inline namespace v1 {
namespace detail {
void allocator_base::deleter::operator()(type* alloc) const noexcept
{
    vmaDestroyAllocator(alloc);
}
}    // namespace detail
}    // namespace v1
}    // namespace gfx