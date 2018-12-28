#pragma once

#include "buffer.hpp"
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
namespace vulkan {
class commands
{
public:
    commands()                = delete;
    commands(commands const&) = delete;
    commands& operator=(commands const&) = delete;
    commands(commands&&)                 = default;
    commands& operator=(commands&&) = default;

    [[nodiscard]] auto get_command_buffer() const noexcept -> vk::CommandBuffer const&;

    template<template<typename> typename SBuf, template<typename> typename DBuf, typename T>
    void copy(SBuf<T> const& src, DBuf<T> const& dst, std::optional<u32> count = std::nullopt, std::optional<u32> src_offset = std::nullopt,
              std::optional<u32> dst_offset = std::nullopt)
    {
        u32 const srco = (src_offset ? *src_offset : 0u) * sizeof(T);
        u32 const dsto = (dst_offset ? *dst_offset : 0u) * sizeof(T);
        u32 const size = (count ? *count : std::min(src.size(), dst.size())) * sizeof(T);

        _buf->copyBuffer(src.get_buffer(), dst.get_buffer(), vk::BufferCopy(srco, dsto, size));
    }

private:
    vk::UniqueCommandBuffer _buf;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx