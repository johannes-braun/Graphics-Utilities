#pragma once

#if defined(NOMINMAX)
#undef NOMINMAX
#define GFX_REDEFINE_NOMINMAX
#endif

// Keep this separate to not generate warnings in vk_mem_alloc
#include <memory>
#include <vk_mem_alloc.h>

#if defined(GFX_REDEFINE_NOMINMAX)
#undef GFX_REDEFINE_NOMINMAX
#define NOMINMAX
#endif

namespace gfx {
inline namespace v1 {
namespace vulkan {
namespace detail {
struct allocator_base
{
    using type = VmaAllocator_T;
    struct deleter
    {
        void operator()(type* alloc) const noexcept;
    };
};
}    // namespace detail

using allocator        = VmaAllocator;
using unique_allocator = std::unique_ptr<detail::allocator_base::type, detail::allocator_base::deleter>;
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx