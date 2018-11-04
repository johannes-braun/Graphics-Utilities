#pragma once

#include "graphics/types.hpp"
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {

class device;

template<template<typename> typename Buffer, typename T>
vk::DescriptorBufferInfo info_for(const Buffer<T>& buf, u32 count, u32 offset = 0)
{
    return vk::DescriptorBufferInfo(buf.get_buffer(), offset * sizeof(T), count * sizeof(T));
}

template<template<typename> typename Buffer, typename T>
vk::DescriptorBufferInfo info_for(const Buffer<T>& buf)
{
    return info_for(buf, buf.size(), 0);
}

}    // namespace v1
}    // namespace gfx