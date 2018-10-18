#pragma once

#include "buffer.hpp"
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
class commands
{
public:
    const vk::CommandBuffer& cmd() const noexcept;

    template<template<typename> typename SBuf, template<typename> typename DBuf, typename T>
    void copy(const SBuf<T>& src, const DBuf<T>& dst, std::optional<u32> count = std::nullopt,
              std::optional<u32> src_offset = std::nullopt, std::optional<u32> dst_offset = std::nullopt)
    {
        const u32 srco = (src_offset ? *src_offset : 0u) * sizeof(T);
        const u32 dsto = (dst_offset ? *dst_offset : 0u) * sizeof(T);
        const u32 size = (count ? *count : std::min(src.size(), dst.size())) * sizeof(T);

		_buf->copyBuffer(src.get_buffer(), dst.get_buffer(), vk::BufferCopy(srco, dsto, size));
    }

private:
    vk::UniqueCommandBuffer _buf;
};
}    // namespace v1
}    // namespace gfx